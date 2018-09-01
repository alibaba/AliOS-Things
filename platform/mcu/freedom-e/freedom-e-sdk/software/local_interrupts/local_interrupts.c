// See LICENSE for license details.

#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include <string.h>
#include "encoding.h"
#include <unistd.h>

#ifndef _SIFIVE_COREPLEXIP_ARTY_H
#error 'local_interrupts' demo only supported for Coreplex IP Eval Kits
#endif

// Global Variable used to show
// software interrupts.
volatile uint32_t g_debouncing;

void debounce();

// Structures for registering different interrupt handlers
// for different parts of the application.
typedef void (*interrupt_function_ptr_t) (void);

// This function enables some of the local interrupts sources
// used in this demo -- just those for the buttons and
// Switch 3.

void enable_local_interrupts(){

  // lowest priority
  set_csr(mie, MIP_MLIP(LOCAL_INT_SW_3 ));
  set_csr(mie, MIP_MLIP(LOCAL_INT_BTN_0));
  set_csr(mie, MIP_MLIP(LOCAL_INT_BTN_1));
  set_csr(mie, MIP_MLIP(LOCAL_INT_BTN_2));
  set_csr(mie, MIP_MLIP(LOCAL_INT_BTN_3));
  // highest priority
  
}

void disable_local_interrupts() {

  // lowest priority
  clear_csr(mie, MIP_MLIP(LOCAL_INT_SW_3 ));
  clear_csr(mie, MIP_MLIP(LOCAL_INT_BTN_0));
  clear_csr(mie, MIP_MLIP(LOCAL_INT_BTN_1));
  clear_csr(mie, MIP_MLIP(LOCAL_INT_BTN_2));
  clear_csr(mie, MIP_MLIP(LOCAL_INT_BTN_3));
  // highest priority

}

/*Entry Point for Machine Software Interrupt Handler*/
void msi_isr() {

  const char msi_msg[] = "    Debouncing: (this message due to Software Interrupt))\n\n";
  write (STDOUT_FILENO, msi_msg, strlen(msi_msg));  

  //clear the  SW interrupt
  CLINT_REG(CLINT_MSIP) = 0;

}

/*Entry Point for Machine Timer Interrupt Handler*/
void mti_isr(){

  // Disable the timer interrupt. The Debounce logic sets it.
  clear_csr(mie, MIP_MTIP);

  // Enable all the local interrupts
  enable_local_interrupts();
}


const char * instructions_msg = " \
\n\
                         SiFive, Inc\n\
 E31/E51 Coreplex IP Eval Kit 'local_interrupts' demo.\n\
\n\
The Buttons 0-3 and Switch 3 are enabled as local\n\
interrupts sources. A .5 s 'debounce' timer is used \n\
between these interrupts. Software interrupts are\n\
used to print a message while debouncing.\n\
Note the priority of the interrupts sources.\n\
\n";

void print_instructions() {

  write (STDERR_FILENO, instructions_msg, strlen(instructions_msg));

}

void button_0_isr(void) {

  // Toggle Red LED
  const char button_0_msg[] = "Button 0 was pressed. Toggle Red.\n";
  write (STDOUT_FILENO, button_0_msg, strlen(button_0_msg));  
  GPIO_REG(GPIO_OUTPUT_VAL) = GPIO_REG(GPIO_OUTPUT_VAL) ^ (0x1 << RED_LED_OFFSET);
  debounce();  
};

void button_1_isr(void) {

  // Toggle Green LED
  const char button_1_msg[] = "Button 1 was pressed. Toggle Green.\n";
  write (STDOUT_FILENO, button_1_msg, strlen(button_1_msg));  
  GPIO_REG(GPIO_OUTPUT_VAL) = GPIO_REG(GPIO_OUTPUT_VAL) ^ (0x1 << GREEN_LED_OFFSET);
  debounce();
};


void button_2_isr(void) {

  // Toggle Blue LED
  const char button_2_msg[] = "Button 2 was pressed. Toggle Blue.\n";
  write (STDOUT_FILENO, button_2_msg, strlen(button_2_msg));  
  GPIO_REG(GPIO_OUTPUT_VAL) = GPIO_REG(GPIO_OUTPUT_VAL) ^ (0x1 << BLUE_LED_OFFSET);
  debounce();
  
};

void button_3_isr(void) {
  const char button_3_msg[] = "Button 3 was pressed! (No LEDs change).\n";
  write (STDOUT_FILENO, button_3_msg, strlen(button_3_msg));
  debounce();
}

void switch_3_isr(void) {
  const char sw_3_msg[] = "Switch 3 is on! But buttons have higher priority.\n";
  write (STDOUT_FILENO, sw_3_msg, strlen(sw_3_msg));
  debounce();
}

void debounce(int local_interrupt_num) {

  // Disable the most recent interrupt.
  // Don't enable it again until the timer goes off,
  // in .5 second.
  
  // Set the machine timer to go off in .5 seconds.
  // If the timer was already set to go off, this "cancels"
  // the current one.
  
  volatile uint64_t * mtime       = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIME);
  volatile uint64_t * mtimecmp    = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIMECMP);
  uint64_t now = *mtime;
  uint64_t then = now + .5*RTC_FREQ;
  *mtimecmp = then;

  disable_local_interrupts();
  g_debouncing = 1;

  // Enable the Machine-Timer bit in MIE
  set_csr(mie, MIP_MTIP);
  
}

// See bsp/env/<BOARD>/init.c for how this
// interrupt vector is used.

interrupt_function_ptr_t localISR[32]; 

int main(int argc, char **argv)
{
  
  // Configure LEDs as outputs.
  GPIO_REG(GPIO_INPUT_EN)    &= ~((0x1<< RED_LED_OFFSET) | (0x1<< GREEN_LED_OFFSET) | (0x1 << BLUE_LED_OFFSET)) ;
  GPIO_REG(GPIO_OUTPUT_EN)   |=  ((0x1<< RED_LED_OFFSET)| (0x1<< GREEN_LED_OFFSET) | (0x1 << BLUE_LED_OFFSET)) ;
  GPIO_REG(GPIO_OUTPUT_VAL)  |=   (0x1 << BLUE_LED_OFFSET) ;
  GPIO_REG(GPIO_OUTPUT_VAL)  &=  ~((0x1<< RED_LED_OFFSET) | (0x1<< GREEN_LED_OFFSET)) ;

  // The Buttons and Switches which are used as local interrupt sources
  // do not go through the GPIO peripheral, so they do not need to
  // be configured as inputs.
  
  // Disable the timer & local interrupts until setup is done (they're
  // not reset by default)
  
  // Disable all interrupts. MIE is not reset.
  write_csr(mie, 0);

  for (int isr = 0; isr < 32; isr++){
    localISR[isr] = 0;
  }
  
  localISR[IRQ_M_SOFT]  = msi_isr;
  localISR[IRQ_M_TIMER] = mti_isr;
  localISR[IRQ_M_LOCAL + LOCAL_INT_SW_3]  = switch_3_isr;
  localISR[IRQ_M_LOCAL + LOCAL_INT_BTN_0] = button_0_isr;
  localISR[IRQ_M_LOCAL + LOCAL_INT_BTN_1] = button_1_isr;
  localISR[IRQ_M_LOCAL + LOCAL_INT_BTN_2] = button_2_isr;
  localISR[IRQ_M_LOCAL + LOCAL_INT_BTN_3] = button_3_isr;

  print_instructions();
  
  enable_local_interrupts();

  g_debouncing = 0;

  // Enable SW interrupts as well in this demo.
  set_csr(mie, MIP_MSIP);
  
  // Enable all global interrupts
  set_csr(mstatus, MSTATUS_MIE);

  volatile int foo = 1;
  while(foo){
    if (g_debouncing){
      //Trigger a SW interrupt
      CLINT_REG(CLINT_MSIP) = 1;
      g_debouncing = 0;
    }
  }

  return 0;

}
