// See LICENSE for license details.

#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include <string.h>
#include "plic/plic_driver.h"
#include "encoding.h"
#include <unistd.h>
#include "stdatomic.h"

void reset_demo (void);

// Structures for registering different interrupt handlers
// for different parts of the application.
typedef void (*function_ptr_t) (void);
void no_interrupt_handler (void) {};
function_ptr_t g_ext_interrupt_handlers[PLIC_NUM_INTERRUPTS];


// Instance data for the PLIC.
plic_instance_t g_plic;


/**
* use mtime to wait for a specified number of ticks.
* async determins if this is a busy wait, or if
* an irq is scheduled
*/
void mtime_wait( uint64_t ticks, uint32_t async)
{

    volatile uint64_t * mtime  = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIME);
    uint64_t now = *mtime;
    
    //if async, schedule irq
    if(async) {
      volatile uint64_t * mtimecmp = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIMECMP);
      uint64_t then = now + ticks; 
      *mtimecmp = then;
      set_csr(mie, MIP_MTIP);

    } else  {
      //else busy wait
      uint64_t then = now + ticks;
      while(*mtime<then) {}

    }

}

/*Entry Point for PLIC Interrupt Handler*/
void handle_m_ext_interrupt(){
  plic_source int_num  = PLIC_claim_interrupt(&g_plic);
  if ((int_num >=1 ) && (int_num < PLIC_NUM_INTERRUPTS)) {
    g_ext_interrupt_handlers[int_num]();
  }
  else {
    exit(1 + (uintptr_t) int_num);
  }
  PLIC_complete_interrupt(&g_plic, int_num);
}

//global countdown timer
static uint32_t countdown = 10;
/*Entry Point for Machine Timer Interrupt Handler*/
void handle_m_time_interrupt(){

  //cleare the timer irq
  clear_csr(mie, MIP_MTIP);

  //schedule next timer irq for 1 second
  mtime_wait(1*RTC_FREQ, 1);

  //flash a led
  GPIO_REG(GPIO_OUTPUT_VAL) ^=	(0x1 << BLUE_LED_OFFSET);
  
  //print the count
  printf("watchdog reset in %d\n", countdown--);
}


const char * instructions_msg = " \
\n\
                SIFIVE, INC.\n\
\n\
         5555555555555555555555555\n\
        5555                   5555\n\
       5555                     5555\n\
      5555                       5555\n\
     5555       5555555555555555555555\n\
    5555       555555555555555555555555\n\
   5555                             5555\n\
  5555                               5555\n\
 5555                                 5555\n\
5555555555555555555555555555          55555\n\
 55555           555555555           55555\n\
   55555           55555           55555\n\
     55555           5           55555\n\
       55555                   55555\n\
         55555               55555\n\
           55555           55555\n\
             55555       55555\n\
               55555   55555\n\
                 555555555\n\
                   55555\n\
                     5\n\
\n\
This application demonstrates the functionality of\n\
the watchdog timer present in the HiFive1 AON peripheral.\n\
\n\
The Watchdog timer will expire in 10 seconds resulting in\n\
a software reset.\n\
\n\
Press a key to prevent a reset\n\
";

void print_instructions() {

  write (STDOUT_FILENO, instructions_msg, strlen(instructions_msg));

}


/**
* Flash the red led for a second, then set up
* blue for blinking during mtime irq
*/
void led_init() {


  // Set up the GPIOs such that the LED GPIO
  // can be used as both Inputs and Outputs.
  GPIO_REG(GPIO_INPUT_EN)    &= ~((0x1<< RED_LED_OFFSET) | (0x1<< GREEN_LED_OFFSET) | (0x1 << BLUE_LED_OFFSET)) ;
  GPIO_REG(GPIO_OUTPUT_EN)   |=  ((0x1<< RED_LED_OFFSET)| (0x1<< GREEN_LED_OFFSET)| (0x1<< BLUE_LED_OFFSET));
  GPIO_REG(GPIO_OUTPUT_VAL)  |=  ((0x1<< RED_LED_OFFSET) | (0x1<< GREEN_LED_OFFSET) | (0x1<< BLUE_LED_OFFSET));

  //flash red led to indicate reset
  GPIO_REG(GPIO_OUTPUT_VAL)  &=   ~(0x1 << RED_LED_OFFSET) ;
  mtime_wait(1*RTC_FREQ,0);
  GPIO_REG(GPIO_OUTPUT_VAL)  |=   (0x1 << RED_LED_OFFSET) ;
}

/**
* initialize the watchdog to reset in
* 5 seconds
**/
void watchdog_init()  {


//reset in 10 seconds
AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
AON_REG(AON_WDOGCMP) = 11;
//wdogconfig: : wdogrsten | enablealways | reset to 0 | max scale
AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
AON_REG(AON_WDOGCFG) |= (AON_WDOGCFG_RSTEN | AON_WDOGCFG_ENALWAYS |\
                        AON_WDOGCFG_ZEROCMP | AON_WDOGCFG_SCALE) ;

}

void reset_demo ()  {

  // Disable the machine & timer interrupts until setup is done.
  clear_csr(mie, MIP_MEIP);
  clear_csr(mie, MIP_MTIP);

  //enable uart input
  UART0_REG(UART_REG_RXCTRL) = UART_RXEN;

  /**************************************************************************
   * Set up the PLIC
   *************************************************************************/
  PLIC_init(&g_plic,
	    PLIC_CTRL_ADDR,
	    PLIC_NUM_INTERRUPTS,
	    PLIC_NUM_PRIORITIES);

  for (int ii = 0; ii < PLIC_NUM_INTERRUPTS; ii ++){
    g_ext_interrupt_handlers[ii] = no_interrupt_handler;
  }


  led_init();
  print_instructions();
  watchdog_init();
  
  // Enable the Machine-External bit in MIE
  set_csr(mie, MIP_MEIP);
  // Enable interrupts in general.
  set_csr(mstatus, MSTATUS_MIE);
}

int main(int argc, char **argv)
{

  reset_demo();
  //schedule a 1 second timer 
  mtime_wait(1*RTC_FREQ,1);

 
  while (1){
    char c;
    if(((int32_t) UART0_REG(UART_REG_RXFIFO)) > 0){
        //flash green led to indicate Kick
        GPIO_REG(GPIO_OUTPUT_VAL)  &=   ~(0x1 << GREEN_LED_OFFSET) ;  

        printf("Feeding Watchdog.\nHiFive1 will reset in 10 seconds.\n");
        countdown = 10;
        AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
        AON_REG(AON_WDOGFEED) = AON_WDOGFEED_VALUE;

        //busy wait a bit so the user sees the led blink
        mtime_wait(5000, 0);
        //turn off led
        GPIO_REG(GPIO_OUTPUT_VAL)  |=   (0x1 << GREEN_LED_OFFSET) ;
    }
  }

  return 0;

}
