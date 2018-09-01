// See LICENSE for license details.

#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include <string.h>
#include "plic/plic_driver.h"
#include "encoding.h"
#include <unistd.h>

#ifndef _SIFIVE_COREPLEXIP_ARTY_H
#error 'global_interrupts' demo only supported for Coreplex IP Eval Kits
#endif

// Global Instance data for the PLIC
// for use by the PLIC Driver.
plic_instance_t g_plic;

// Flag for state
int g_switch1Wins;

// Debounce counter (PWM can't go slow enough)
int g_debounce;

void debounce();

// Structures for registering different interrupt handlers
// for different parts of the application.
typedef void (*interrupt_function_ptr_t) (void);

// See bsp/env/<BOARD>/init.c for how this
// interrupt vector is used.
interrupt_function_ptr_t localISR[32]; 

interrupt_function_ptr_t g_ext_interrupt_handlers[PLIC_NUM_INTERRUPTS];

void set_timer() {
  
  volatile uint64_t * mtime       = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIME);
  volatile uint64_t * mtimecmp    = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIMECMP);
  uint64_t now = *mtime;
  uint64_t then = now + 10*RTC_FREQ;
  *mtimecmp = then;

}

/*Entry Point for Machine Timer Interrupt Handler*/
void mti_isr(){
 
  if (g_switch1Wins) {
    printf("#### Giving Switch 1 Priority for 10 seconds ####\n");
    // All other things being equal, lower IDs have
    // higher priority. We have already set
    // Switch 1 to priority 2
    // in the setup, so by giving these equal priority Switch 1 will win.
    PLIC_set_priority(&g_plic, INT_EXT_DEVICE_SW_2, 2);
  } else {
    printf("**** Giving Switch 2 Priority for 10 seconds ****\n");
    // By setting Switch 2 a higher integer priority, it will win over switch 1.
    PLIC_set_priority(&g_plic, INT_EXT_DEVICE_SW_2, 3);
  }    
  g_switch1Wins ^= 0x1;

  set_timer();

}

/*Entry Point for PLIC Interrupt Handler*/
void mei_isr(){
  plic_source int_num  = PLIC_claim_interrupt(&g_plic);
  if ((int_num >=1 ) && (int_num < PLIC_NUM_INTERRUPTS)) {
    g_ext_interrupt_handlers[int_num]();
  }
  else {
    exit(1 + (uintptr_t) int_num);
  }
  PLIC_complete_interrupt(&g_plic, int_num);
}

const char * instructions_msg = " \
\n\
                SIFIVE, INC.\n\
E31/E51 Coreplex IP Eval Kit 'global_interrupts' demo.\n\
\n\
Switches  1 and 2 are enabled as External Global Interrupts \n\
(they don't go through the PLIC). You an observe priorities.\n\
Priorities invert every few seconds, which is driven by the \n\
PWM0 global interrupt.                                      \n\
\n";

void print_instructions() {

  write (STDOUT_FILENO, instructions_msg, strlen(instructions_msg));

}

void invalid_global_isr() {
  printf("Unexpected global interrupt!\n");
}

void invalid_local_isr() {
  printf ("Unexpected local interrupt!\n");
}

void switch_1_handler() {

  printf("Switch 1 is on! Even if Switch 2 is on, Switch 1 must have higher priority right now.\n");
  
  // Set Green LED
  GPIO_REG(GPIO_OUTPUT_VAL)  |=  (0x1 << GREEN_LED_OFFSET) ;
  GPIO_REG(GPIO_OUTPUT_VAL)  &=  ~((0x1<< RED_LED_OFFSET));

  debounce();
  
}

void switch_2_handler() {
  printf("Switch 2 is on! Even if Switch 1 is on, Switch 2 must have higher priority right now.\n");

  // Set RED LED
  GPIO_REG(GPIO_OUTPUT_VAL)  &=  ~(0x1 << GREEN_LED_OFFSET) ;
  GPIO_REG(GPIO_OUTPUT_VAL)  |=   (0x1<< RED_LED_OFFSET);

  debounce();
}

// We use PWM 0 as a 
// timer interrupt for debouncing.

void pwm_0_handler() {


  if (g_debounce == 0) {
    printf("    Done debouncing.\n");
    
    //Lower the threshold s.t. the switches can hit.
    PLIC_set_threshold(&g_plic, 1);
    
    // Clear the PWM interrupt
    PWM0_REG(PWM_CFG) = 0;
    
  } else {
    // Keep waiting
    g_debounce --;
    // This clears out the interrupt and re-arms the timer.
    PWM0_REG(PWM_CFG) = ((PWM_CFG_ONESHOT) | (PWM_CFG_ZEROCMP)| 0x7 | (PWM_CFG_STICKY));
    
  }

}

void debounce(int local_interrupt_num) {

  printf("    Starting a debounce.\n");

  g_debounce = 600;
  
  // This clears out the interrupt and re-arms the timer.
  PWM0_REG(PWM_CFG) = ((PWM_CFG_ONESHOT) | (PWM_CFG_ZEROCMP)| 0x7 | (PWM_CFG_STICKY));
  
  // Set the threshold high enough that the
  // switches won't cause the interrupt to fire,
  // only the PWM or timer interrupts.
  PLIC_set_threshold(&g_plic, 4);

}

int main(int argc, char **argv)
{

  for (int gisr = 0; gisr < PLIC_NUM_INTERRUPTS; gisr++){
    g_ext_interrupt_handlers[PLIC_NUM_INTERRUPTS] = invalid_global_isr;
  }
  g_ext_interrupt_handlers[PWM0_INT_BASE + 0] = pwm_0_handler;
  g_ext_interrupt_handlers[INT_EXT_DEVICE_SW_1] = switch_1_handler;
  g_ext_interrupt_handlers[INT_EXT_DEVICE_SW_2] = switch_2_handler;
  
  for (int lisr = 0; lisr < 32; lisr++){
    localISR[lisr] = invalid_local_isr;
  }
  
  localISR[IRQ_M_TIMER] = mti_isr;
  localISR[IRQ_M_EXT]   = mei_isr;

  print_instructions();
  
  // Set up RGB LEDs for a visual.
  
  GPIO_REG(GPIO_OUTPUT_EN)   |=  ((0x1<< RED_LED_OFFSET)| (0x1<< GREEN_LED_OFFSET));
  GPIO_REG(GPIO_OUTPUT_VAL)  |=  (0x1 << GREEN_LED_OFFSET) ;
  GPIO_REG(GPIO_OUTPUT_VAL)  &=  ~(0x1<< RED_LED_OFFSET);
				   
  /**************************************************************************
   * Set up the PLIC
   *
   *************************************************************************/
  PLIC_init(&g_plic,
	    PLIC_CTRL_ADDR,
	    PLIC_NUM_INTERRUPTS,
	    PLIC_NUM_PRIORITIES);

  /**************************************************************************
   * Give Switch 1 and Switch 2 Equal priority of 2.
   *
   *************************************************************************/

  PLIC_enable_interrupt (&g_plic, PWM0_INT_BASE + 0);
  PLIC_enable_interrupt (&g_plic, INT_EXT_DEVICE_SW_1);
  PLIC_enable_interrupt (&g_plic, INT_EXT_DEVICE_SW_2);

  // PWM always beats the switches, because we use it
  // as a debouncer, and we lower the threshold
  // to do so.

  PWM0_REG(PWM_CFG) = 0;

  // Make sure people aren't blinded by LEDs connected here.
  PWM0_REG(PWM_CMP0)  = 0xFE;
  PWM0_REG(PWM_CMP1)  = 0xFF;
  PWM0_REG(PWM_CMP2)  = 0xFF;
  PWM0_REG(PWM_CMP3)  = 0xFF;
  PLIC_set_priority(&g_plic, PWM0_INT_BASE + 0   , 5);

  // Start the switches out at the same priority. Switch1
  // would win.
  PLIC_set_priority(&g_plic, INT_EXT_DEVICE_SW_1, 2);
  PLIC_set_priority(&g_plic, INT_EXT_DEVICE_SW_2, 2);

  // Set up machine timer interrupt. Every few seconds it
  // will invert the switch priorities.
  set_timer();

  // Enable timer interrupts.
  set_csr(mie, MIP_MTIP);

  // Enable Global (PLIC) interrupts.
  set_csr(mie, MIP_MEIP);

  g_switch1Wins = 1;
  
  // Enable all interrupts
  set_csr(mstatus, MSTATUS_MIE);
  
  while(1){
    asm volatile ("wfi");
  }
  
  return 0;

}
