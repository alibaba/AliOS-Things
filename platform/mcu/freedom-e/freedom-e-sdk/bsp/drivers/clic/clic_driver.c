// See LICENSE for license details.

#include "sifive/devices/clic.h"
#include "clic/clic_driver.h"
#include "platform.h"
#include "encoding.h"
#include <string.h>


void volatile_memzero(uint8_t * base, unsigned int size) {
  volatile uint8_t * ptr;
  for (ptr = base; ptr < (base + size); ptr++){
    *ptr = 0;
  }
}

// Note that there are no assertions or bounds checking on these
// parameter values.
void clic_init (
                clic_instance_t * this_clic,
                uintptr_t hart_addr,
                interrupt_function_ptr_t* vect_table,
                interrupt_function_ptr_t default_handler,
                uint32_t num_irq,
                uint32_t num_config_bits
                )
{
  this_clic->hart_addr=  hart_addr;
  this_clic->vect_table= vect_table;
  this_clic->num_config_bits= num_config_bits;

  //initialize vector table
  for(int i=0;i++;i<num_irq)  {
    this_clic->vect_table[i] = default_handler;
  }

  //set base vectors
  write_csr(mtvt, vect_table);


  //clear all interrupt enables and pending
  volatile_memzero((uint8_t*)(this_clic->hart_addr+CLIC_INTIE), num_irq);
  volatile_memzero((uint8_t*)(this_clic->hart_addr+CLIC_INTIP), num_irq);

  //clear nlbits and nvbits; all interrupts trap to level 15
  *(volatile uint8_t*)(this_clic->hart_addr+CLIC_CFG)=0;

}

void clic_install_handler (clic_instance_t * this_clic, uint32_t source, interrupt_function_ptr_t handler) {
    this_clic->vect_table[source] = handler;
}

void clic_enable_interrupt (clic_instance_t * this_clic, uint32_t source) {
    *(volatile uint8_t*)(this_clic->hart_addr+CLIC_INTIE+source) = 1;
}

void clic_disable_interrupt (clic_instance_t * this_clic, uint32_t source){
  *(volatile uint8_t*)(this_clic->hart_addr+CLIC_INTIE+source) = 0;
}

void clic_set_pending(clic_instance_t * this_clic, uint32_t source){
  *(volatile uint8_t*)(this_clic->hart_addr+CLIC_INTIP+source) = 1;
}

void clic_clear_pending(clic_instance_t * this_clic, uint32_t source){
  *(volatile uint8_t*)(this_clic->hart_addr+CLIC_INTIP+source) = 0;
}

void clic_set_intcfg (clic_instance_t * this_clic, uint32_t source, uint32_t intcfg){
  *(volatile uint8_t*)(this_clic->hart_addr+CLIC_INTCFG+source) = intcfg;
}

uint8_t clic_get_intcfg  (clic_instance_t * this_clic, uint32_t source){
  return *(volatile uint8_t*)(this_clic->hart_addr+CLIC_INTCFG+source);
}

void clic_set_cliccfg (clic_instance_t * this_clic, uint32_t cfg){
  *(volatile uint8_t*)(this_clic->hart_addr+CLIC_CFG) = cfg;
}

uint8_t clic_get_cliccfg  (clic_instance_t * this_clic){
  return *(volatile uint8_t*)(this_clic->hart_addr+CLIC_CFG);
}

//sets an interrupt level based encoding of nmbits, nlbits
uint8_t clic_set_int_level( clic_instance_t * this_clic, uint32_t source, uint8_t level) {
  //extract nlbits
  uint8_t nlbits = clic_get_cliccfg(this_clic);
  nlbits = (nlbits >>1) & 0x7;

  //shift level right to mask off unused bits
  level = level>>((this_clic->num_config_bits)-nlbits); //plus this_clic->nmbits which is always 0 for now.
  //shift level into correct bit position
  level = level << (8-this_clic->num_config_bits) + (this_clic->num_config_bits - nlbits);
 
  //write to clicintcfg
  uint8_t current_intcfg = clic_get_intcfg(this_clic, source);
  clic_set_intcfg(this_clic, source, (current_intcfg | level));

  return level;
}

//gets an interrupt level based encoding of nmbits, nlbits
uint8_t clic_get_int_level( clic_instance_t * this_clic, uint32_t source) {
  uint8_t level;
  level = clic_get_intcfg(this_clic, source);

  //extract nlbits
  uint8_t nlbits = clic_get_cliccfg(this_clic);
  nlbits = (nlbits >>1) & 0x7;

  //shift level
  level = level >> (8-(this_clic->num_config_bits));

  //shift level right to mask off priority bits
  level = level>>(this_clic->num_config_bits-nlbits); //this_clic->nmbits which is always 0 for now.

  return level;
}

//sets an interrupt priority based encoding of nmbits, nlbits
uint8_t clic_set_int_priority( clic_instance_t * this_clic, uint32_t source, uint8_t priority) {
  //priority bits = num_config_bits - nlbits
  //extract nlbits
  uint8_t nlbits = clic_get_cliccfg(this_clic);
  nlbits = (nlbits >>1) & 0x7;

  uint8_t priority_bits = this_clic->num_config_bits-nlbits;
  if(priority_bits = 0) {
    //no bits to set
    return 0;
  }
  //mask off unused bits
  priority = priority >> (8-priority_bits);
  //shift into the correct bit position
  priority = priority << (8-(this_clic->num_config_bits));

  //write to clicintcfg
  uint8_t current_intcfg = clic_get_intcfg(this_clic, source);
  clic_set_intcfg(this_clic, source, (current_intcfg | priority));
  return current_intcfg;
}

//gets an interrupt priority based encoding of nmbits, nlbits
uint8_t clic_get_int_priority( clic_instance_t * this_clic, uint32_t source) {
  uint8_t priority;
  priority = clic_get_intcfg(this_clic, source);

  //extract nlbits
  uint8_t nlbits = clic_get_cliccfg(this_clic);
  nlbits = (nlbits >>1) & 0x7;

  //shift left to mask off level bits
  priority = priority << nlbits;

  //shift priority
  priority = priority >> (8-((this_clic->num_config_bits)+nlbits));

 return priority;
}


