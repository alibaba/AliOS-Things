// See LICENSE for license details.

// This is the 'bootloader' which ships on HiFive1 boards,
// at SPI Flash location 0x20000000.
// HiFive1 boards have code burned into their OTP which ends with
// a jump to 0x20000000. The Freedom E SDK and the Arduino IDE
// linker scripts cause user programs to be compiled and
// flashed at 0x20400000. Therefore, this code ultimately jumps
// to 0x20400000, while attempting to leave the machine state as close
// to its reset state as possible.
//
// When compiled in the freedom-e-sdk environment, this code will
// be compiled as a user program at 0x20400000, so will continuously jump to itself.
// It is provided in the freedom-e-sdk mostly for reference purposes, so
// users know what is running on their board. Replacing the bootloader
// at 0x20000000 is not currently supported by the SDK.
//
// These are the instructions for the user of this program, from the
// HiFive1 Getting Started Guide:
// 
// This program is designed to allow quick boot, but
// also a "safe" reboot option if a "bad" program
// is flashed into the HiFive1's SPI Flash. A "bad" program
// is one which makes it impossible for the programmer
// to communicate with the HiFive1. For example, a program which
// disables FE310's active clock, or which puts the FE310 to sleep
// with no way of waking it up. Bad programs can always be restarted using
// the RESET button, and using the "safe" bootloader can be halted
// before they perform any unsafe behavior.
//
// To activate "normal" boot mode, press the RESET button on
// the HiFive1. After approximately 1s, the green LED will flash
// for 1/2 second, then the user program will execute.
//
// To activate "safe" boot mode, press the RESET button. When
// the green LED flashes, immediately press the RESET button again.
// After 1 second, the red LED will blink. The user program will not
// execute, and the programmer can connect. To exit "safe" boot mode,
// press the RESET button a final time.

#include <stdint.h>
#include "platform.h"
#include "encoding.h"

#ifndef _SIFIVE_HIFIVE1_H
#error "double_tap_dontboot is designed to run on HiFive1 and/or E300 Arty Dev Kit."
#endif


#define BACKUP15_MAGIC 0xD027B007

#define FINAL_ADDRESS 0x20400000

#define RED_LED 22
#define GREEN_LED 19

int main(void)
{
  uint64_t now;
  uint64_t then;
  
  // Restore the default mtvec (which may have been set by initialization
  // code, depending on the environment in which this C code is compiled).
  // By default, this would cause an infinite loop upon exception, which is
  // also "safe" behavior and the debugger can connect.
  
  write_csr(mtvec, 0x0);
  
  // How did we get here? We only want to execute this code
  // on resets (vs waking up from sleep).
  if ((AON_REG(AON_PMUCAUSE) & AON_PMUCAUSE_WAKEUPCAUSE) ==
      AON_WAKEUPCAUSE_RESET) {

    if (AON_REG(AON_BACKUP15) == BACKUP15_MAGIC) {
      // Reset was "double-tapped".

      // Re-arm the reset double-tap
      AON_REG(AON_BACKUP15) = 0;

      // PWM Red LED

      GPIO_REG(GPIO_IOF_EN)     |=   (1 << RED_LED);
      GPIO_REG(GPIO_OUTPUT_XOR) &=  ~(1 << RED_LED);
      GPIO_REG(GPIO_IOF_SEL)    |=   (1 << RED_LED);

      GPIO_REG(GPIO_OUTPUT_VAL) &=  ~(1 << GREEN_LED);
      GPIO_REG(GPIO_OUTPUT_XOR) &=  ~(1 << GREEN_LED);
      GPIO_REG(GPIO_OUTPUT_EN)  &=  ~(1 << GREEN_LED);

      PWM1_REG(PWM_CFG)   = 0;
      PWM1_REG(PWM_COUNT) = 0;
      PWM1_REG(PWM_CMP0)  = 0xFF;
      PWM1_REG(PWM_CMP3)  = 0xFF;
      PWM1_REG(PWM_CFG)   = PWM_CFG_ENALWAYS;

      int pwm_val = 255;
	
      // Wait for debugger or another RESET press.
      while(1){

	// Make the PWM a fade. This is preferable to just a PWM blink
	// because it makes it clear that the processor is actually
	// running this code, not just the PWM hardware.

	now = *((volatile uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIME));
	then = now + 32768/500;
	while (*((volatile uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIME)) < then) {
	  asm volatile ("");
	}
	pwm_val = (pwm_val == 0) ? 255 : (pwm_val -1);
	PWM1_REG(PWM_CMP3) = pwm_val << 4;
      }
           
    } // If Magic
      
      // Turn on Green LED to indicate time-to-double-tap
      // LEDs are Active-Low
    GPIO_REG(GPIO_OUTPUT_VAL)     |=  (1 << GREEN_LED);
    GPIO_REG(GPIO_OUTPUT_XOR)     |=  (1 << GREEN_LED);
    GPIO_REG(GPIO_OUTPUT_EN)      |=  (1 << GREEN_LED);

    // Re-arm the reset double-tap
    uint32_t save = AON_REG(AON_BACKUP15);
      
    AON_REG(AON_BACKUP15) = BACKUP15_MAGIC;

    // Wait 500 ms. If reset is tapped at this point,
    // we will execute the "magic" loop above.
    now = *((volatile uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIME));
    then = now + 32768/2;
    while (*((volatile uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIME)) < then) {
      asm volatile ("");
    }
      
    // Re-arm the reset double-tap
    AON_REG(AON_BACKUP15) = save;
   
  }
  
  // Restore the GPIO Registers to their default
  GPIO_REG(GPIO_OUTPUT_VAL)     = 0;
  GPIO_REG(GPIO_OUTPUT_XOR)     = 0;
  GPIO_REG(GPIO_OUTPUT_EN)      = 0;

  // Jump to "user code" in SPI Flash.
  void (*pgm_start)(void) = (void*) FINAL_ADDRESS;
  pgm_start();

  // This value is meaningless, but
  // since this code should never be reached,
  // make it non-zero.
  return (1234567);
}
