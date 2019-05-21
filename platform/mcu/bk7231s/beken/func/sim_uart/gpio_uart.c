#include <stdio.h>
#include "include.h"
#include "arm_arch.h"

#include "uart_pub.h"
#include "uart.h"

#include "drv_model_pub.h"
#include "sys_ctrl_pub.h"
#include "mem_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"
#include "gpio_uart.h"


#include "mem_pub.h"
#include "intc_pub.h"

#include "icu_pub.h"
#include "gpio_pub.h"

void gu_delay(uint32_t usec)
{
    volatile uint32_t loops;

    while (usec--)
    {
        loops = 40;
        while (loops--);
    }
}

#ifdef CONFIG_GPIO_SIMU_UART_TX
void gpio_uart_send_init(void)
{
    UINT32 param;
	
    param = GPIO_CFG_PARAM(SIMU_UART_GPIONUM, GMODE_OUTPUT);
	sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
}

void gpio_uart_send_byte(unsigned char *buff, unsigned int len)
{
    volatile unsigned char c, n, loops;
    UINT32 param;

    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();

    param = GPIO_CFG_PARAM(SIMU_UART_GPIONUM, GMODE_OUTPUT);
	sddev_control(GPIO_DEV_NAME, CMD_GPIO_OUTPUT, &param);
    gu_delay(60);

    while(len--)
    {
        param = GPIO_CFG_PARAM(SIMU_UART_GPIONUM, GMODE_INPUT_PULLDOWN);
		sddev_control(GPIO_DEV_NAME, CMD_GPIO_OUTPUT, &param);
        gu_delay(33);
        loops = 10;
        while (loops--);
        c = *buff++;
        n = 8;
        while(n--)
        {
            param = GPIO_CFG_PARAM(SIMU_UART_GPIONUM, (c & 0x01));
			sddev_control(GPIO_DEV_NAME, CMD_GPIO_OUTPUT, &param);
            gu_delay(33);
            loops = 6;
            while (loops--);
            c >>= 1;
        }
        param = GPIO_CFG_PARAM(SIMU_UART_GPIONUM, GMODE_OUTPUT);
		sddev_control(GPIO_DEV_NAME, CMD_GPIO_OUTPUT, &param);
        gu_delay(33);
        loops = 6;
        while (loops--);
    }
    GLOBAL_INT_RESTORE();
}

int guart_fputc(int ch, FILE *f)
{
    gpio_uart_send_byte((unsigned char *)&ch, 1);

    return ch;
}

#endif

#ifdef CONFIG_GPIO_SIMU_UART_TX
void GPIO_Simu_Isr(unsigned char ucChannel)
{
    UINT32 gpiosts ;
    UINT8 c = 0, n, loops;

    if(ucChannel == SIMU_UART_GPIO_RX)
    {
        gu_delay(21);
        loops = 5;
        while (loops--);
        for(n = 0; n < 8; n++)
        {
            c >>= 1;
            if(bk_gpio_input(SIMU_UART_GPIO_RX))
                c |= 0x80;

            gu_delay(33);
            loops = 20;
            while (loops--);
            loops++;
        }
    }
}

void gpio_uart_recv_init(void)
{
    UINT32 param;
	GPIO_INT_ST int_param;
	
    param = GPIO_CFG_PARAM(SIMU_UART_GPIO_RX, GMODE_INPUT_PULLUP);
	sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);

    intc_service_register(IRQ_GPIO, PRI_IRQ_GPIO, gpio_isr);
	
    param = IRQ_GPIO_BIT;
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);

	int_param.id = SIMU_UART_GPIO_RX;
	int_param.mode = GMODE_INPUT_PULLDOWN;
	int_param.phandler = GPIO_Simu_Isr;
	sddev_control(GPIO_DEV_NAME, CMD_GPIO_INT_ENABLE, &int_param);
}
#endif
// eof

