#ifndef _GPIO_UART_H_
#define _GPIO_UART_H_

#define CONFIG_GPIO_SIMU_UART_RX 1
#define CONFIG_GPIO_SIMU_UART_TX 1


#define SIMU_UART_GPIONUM             18
#define SIMU_UART_GPIO_RX             19
#define SIMU_UART_GPIO_TEST           15

extern void gpio_isr(void);
extern void gu_delay(uint32_t ) ;
#ifdef CONFIG_GPIO_SIMU_UART_TX
extern void gpio_uart_send_init(void);
extern int guart_fputc(int , FILE *);
#endif

#ifdef CONFIG_GPIO_SIMU_UART_RX
extern void gpio_uart_recv_init(void);
#endif

#endif // _GPIO_UART_H_
// eof

