// See LICENSE file for licence details

#ifndef PLIC_DRIVER_H
#define PLIC_DRIVER_H


__BEGIN_DECLS

#include "platform.h"

typedef void (*interrupt_function_ptr_t) (void);

typedef struct __clic_instance_t
{
  uintptr_t hart_addr;
  interrupt_function_ptr_t* vect_table;
  uint32_t num_config_bits;
  uint32_t num_sources;  
} clic_instance_t;

// Note that there are no assertions or bounds checking on these
// parameter values.
void clic_init (clic_instance_t * this_clic, uintptr_t hart_addr, interrupt_function_ptr_t* vect_table, interrupt_function_ptr_t default_handler, uint32_t num_irq,uint32_t num_config_bits);
void clic_install_handler (clic_instance_t * this_clic, uint32_t source, interrupt_function_ptr_t handler);
void clic_enable_interrupt (clic_instance_t * this_clic, uint32_t source);
void clic_disable_interrupt (clic_instance_t * this_clic, uint32_t source);
void clic_set_pending(clic_instance_t * this_clic, uint32_t source);
void clic_clear_pending(clic_instance_t * this_clic, uint32_t source);
void clic_set_intcfg (clic_instance_t * this_clic, uint32_t source, uint32_t intcfg);
uint8_t clic_get_intcfg (clic_instance_t * this_clic, uint32_t source);
void clic_set_cliccfg (clic_instance_t * this_clic, uint32_t cfg);
uint8_t clic_get_cliccfg  (clic_instance_t * this_clic);
//sets an interrupt level based encoding of nmbits, nlbits
uint8_t clic_set_int_level( clic_instance_t * this_clic, uint32_t source, uint8_t level);
//get an interrupt level based encoding of nmbits, nlbits
uint8_t clic_get_int_level( clic_instance_t * this_clic, uint32_t source);
//sets an interrupt priority based encoding of nmbits, nlbits
uint8_t clic_set_int_priority( clic_instance_t * this_clic, uint32_t source, uint8_t priority);
//sets an interrupt priority based encoding of nmbits, nlbits
uint8_t clic_get_int_priority( clic_instance_t * this_clic, uint32_t source);

__END_DECLS

#endif
