// See LICENSE for license details.

#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include "encoding.h"
#include <platform.h>

#ifndef _SIFIVE_COREPLEXIP_ARTY_H
#error 'coreplexip_welcome' demo only supported for Coreplex IP Eval Kits
#endif

static const char sifive_msg[] = "\n\r\
\n\r\
                SIFIVE, INC.\n\r\
\n\r\
         5555555555555555555555555\n\r\
        5555                   5555\n\r\
       5555                     5555\n\r\
      5555                       5555\n\r\
     5555       5555555555555555555555\n\r\
    5555       555555555555555555555555\n\r\
   5555                             5555\n\r\
  5555                               5555\n\r\
 5555                                 5555\n\r\
5555555555555555555555555555          55555\n\r\
 55555           555555555           55555\n\r\
   55555           55555           55555\n\r\
     55555           5           55555\n\r\
       55555                   55555\n\r\
         55555               55555\n\r\
           55555           55555\n\r\
             55555       55555\n\r\
               55555   55555\n\r\
                 555555555\n\r\
                   55555\n\r\
                     5\n\r\
\n\r\
";

#if __riscv_xlen == 32
  static const char welcome_msg[] = "\n\r\
\n\r\
Welcome to the E31 Coreplex IP FPGA Evaluation Kit!\n\r\
\n\r";
#else
static const char welcome_msg[] = "\n\r\
\n\r\
Welcome to the E51 Coreplex IP FPGA Evaluation Kit!\n\r\
\n\r";
#endif

static void _putc(char c) {
  while ((int32_t) UART0_REG(UART_REG_TXFIFO) < 0);
  UART0_REG(UART_REG_TXFIFO) = c;
}

int _getc(char * c){
  int32_t val = (int32_t) UART0_REG(UART_REG_RXFIFO);
  if (val > 0) {
    *c =  val & 0xFF;
    return 1;
  }
  return 0;
}

static void _puts(const char * s) {
  while (*s != '\0'){
    _putc(*s++);
  }
}

int main (void){

  // 115200 Baud Rate at (65 / 2) MHz
  UART0_REG(UART_REG_DIV) = 282;
  UART0_REG(UART_REG_TXCTRL) = UART_TXEN;
  UART0_REG(UART_REG_RXCTRL) = UART_RXEN;

  // Wait a bit because we were changing the GPIOs
  volatile int i=0;
  while(i < 10000){i++;}

  _puts(sifive_msg);

  _puts(welcome_msg);
  
  uint16_t r=0x3F;
  uint16_t g=0;
  uint16_t b=0;
  // Set up RGB PWM
  
  PWM0_REG(PWM_CFG)   = 0;
  PWM0_REG(PWM_CFG)   = (PWM_CFG_ENALWAYS) | (PWM_CFG_ZEROCMP) | (PWM_CFG_DEGLITCH);
  PWM0_REG(PWM_COUNT) = 0;
  
  // The LEDs are intentionally left somewhat dim. 
  PWM0_REG(PWM_CMP0)  = 0xFE;

  while(1){
    volatile uint64_t *  now = (volatile uint64_t*)(CLINT_CTRL_ADDR + CLINT_MTIME);
    volatile uint64_t then = *now + 400;
    while (*now < then) { }

    if(r > 0 && b == 0){
      r--;
      g++;
    }
    if(g > 0 && r == 0){
      g--;
      b++;
    }
    if(b > 0 && g == 0){
      r++;
      b--;
    }
    
    PWM0_REG(PWM_CMP1)  = 0xFF - (r >> 2);
    PWM0_REG(PWM_CMP2)  = 0xFF - (g >> 2);
    PWM0_REG(PWM_CMP3)  = 0xFF - (b >> 2);
    
  }// While (1)
}
  
