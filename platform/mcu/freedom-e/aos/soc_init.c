#include "k_api.h"
#include "hal/soc/uart.h"
#include <platform.h>
#include "encoding.h"
#include <stdarg.h>
#include <stdio.h>

uart_dev_t   uart_0;

#define OS_WAIT_FOREVER		0xffffffffU

#define HAL_WAIT_FOREVER    OS_WAIT_FOREVER

extern uint32_t g_debug_uart_port ;

#if defined (__CC_ARM) && defined(__MICROLIB)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#elif defined(__ICCARM__)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#else
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#endif /* defined (__CC_ARM) && defined(__MICROLIB) */


void _putc(char c) {
  while ((int32_t) UART0_REG(UART_REG_TXFIFO) < 0);
  UART0_REG(UART_REG_TXFIFO) = c;
}


void set_timer() {
  static uint64_t then = 0;
  
  volatile uint64_t * mtime       = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIME);
  volatile uint64_t * mtimecmp    = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIMECMP);
  if(then != 0)  {
    //next timer irq is 1 second from previous
    then += 1*RTC_FREQ/RHINO_CONFIG_TICKS_PER_SECOND;
    //then += 1*RTC_FREQ;
  } else{ //first time setting the timer
    uint64_t now = *mtime;
    then = now + 1*RTC_FREQ/RHINO_CONFIG_TICKS_PER_SECOND;
    //then = now + 1*RTC_FREQ;
  }
  *mtimecmp = then;

  set_csr(mie, MIP_MTIP);
}

/*Entry Point for Machine Timer Interrupt Handler*/
/*called from bsp/env/ventry.s          */
void handle_m_time_interrupt(){
  static uint32_t onoff=1;

  clear_csr(mie, MIP_MTIP);

  //increment ecall_countdown
  //ecall_countdown++;

  // Set Green LED
  if(onoff)	{
	  GPIO_REG(GPIO_OUTPUT_VAL)  |=  (0x1 << GREEN_LED_OFFSET) ;
	  onoff=0;
  }else	{
	  GPIO_REG(GPIO_OUTPUT_VAL)  &=  ~((0x1 << GREEN_LED_OFFSET)) ;
	  onoff=1;
  }
  krhino_tick_proc();

  set_timer();
  //re-enable button1 irq
  //set_csr(mie, MIP_MLIP(LOCAL_INT_BTN_1));

}


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  if (ch == '\n') {
    //hal_uart_send(&console_uart, (void *)"\r", 1, 30000);
    hal_uart_send(&uart_0, (void *)"\r", 1, 30000);
  }
  hal_uart_send(&uart_0, &ch, 1, 30000);
  return ch;
}

/**
  * @brief  Retargets the C library scanf function to the USART.
  * @param  None
  * @retval None
  */
GETCHAR_PROTOTYPE
{
  /* Place your implementation of fgetc here */
  /* e.g. readwrite a character to the USART2 and Loop until the end of transmission */
  uint8_t ch = -1; //EOF;
  int32_t ret = -1;
  
  uint32_t recv_size;
  ret = hal_uart_recv_II(&uart_0, &ch, 1, &recv_size, HAL_WAIT_FOREVER);

  if (ret == 0) {
      return ch;
  } else {
      return -1;
  }
}


