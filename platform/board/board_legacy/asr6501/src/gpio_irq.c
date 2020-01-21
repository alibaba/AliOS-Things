/*******************************************************************************
	Gpio_irq.c
	Register gpio isr and distinguish different GPIO irq type
*******************************************************************************/
#include <project.h>
#include "gpio.h"
#include <stdio.h> 

#define DIO1__INTR (0x40040410)
#define DIO1__MASK (0x40u)

static GpioIoeIrqHandler GpioIrqHandlerCallback[IOE_PINS] = {0};
void RegisterGpioCallback(Gpio_t *obj, GpioIoeIrqHandler GpioIrqCallback)
{
	if (GpioIrqCallback != NULL) {
		switch(obj->pin) {
			case DIO1_PIN:
				GpioIrqHandlerCallback[DIO1_PIN] = GpioIrqCallback;
				break;
			case SETB_PIN:
			case RESET_PIN:
			case BUSY_PIN:
			case DIO2_PIN:
			case DIO3_PIN:
			case SETA_PIN:
			case OTHER_PIN:
			default:
				break;
		}
	}
	return ;
}

void GpioIsrEntry (void)
{
	uint32 data = 0;

	data = CY_GET_REG32(pin_wakeup__INTR);
	if(data & pin_wakeup__MASK){
		CY_SET_REG32(pin_wakeup__INTR, pin_wakeup__MASK);
		return;
	}

	data = CY_GET_REG32(DIO1__INTR);
	if(data & DIO1__MASK){
		CY_SET_REG32(DIO1__INTR, DIO1__MASK);
		GpioIrqHandlerCallback[DIO1_PIN]();
		return;
	}

    data = CY_GET_REG32(UART_1_rx_wake__INTR);
    if (data & UART_1_rx_wake__MASK) {
        CY_SET_REG32(UART_1_rx_wake__INTR, UART_1_rx_wake__MASK);
        return;
    }  
}

