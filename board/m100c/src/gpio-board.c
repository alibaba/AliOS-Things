#include "system/gpio.h"
#include "em_gpio.h"
#include "sx1276.h"
#include <stdint.h>
#include <stdlib.h>

static GpioIrqHandler *g_irq_handlers[16] = {NULL};

void GpioMcuInit( Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value )
{
    obj->pin = pin;

    if( pin == NC )
    {
        return;
    }

    obj->pinIndex = ( 0x01 << ( obj->pin & 0x0F ) );

    if( ( obj->pin & 0xF0 ) == 0x00 ) {
        obj->port = (void *)gpioPortA;
    } else if( ( obj->pin & 0xF0 ) == 0x10 ) {
        obj->port = (void *)gpioPortB;
    } else if( ( obj->pin & 0xF0 ) == 0x20 ) {
        obj->port = (void *)gpioPortC;
    } else if( ( obj->pin & 0xF0 ) == 0x30 ) {
        obj->port = (void *)gpioPortD;
    } else if( ( obj->pin & 0xF0 ) == 0x40 ) {
        obj->port = (void *)gpioPortE;
    } else {
        return;
    }

    if (mode == PIN_INPUT) {
        if (config = PIN_PUSH_PULL) {
            if (type == PIN_PULL_UP) {
                GPIO_PinModeSet((GPIO_Port_TypeDef)obj->port,
                                (obj->pin & 0x0F),
                                gpioModeInputPull,
                                1);
            } else {
                GPIO_PinModeSet((GPIO_Port_TypeDef)obj->port,
                                (obj->pin & 0x0F),
                                gpioModeInputPull,
                                0);
            }
        } else {
            GPIO_PinModeSet((GPIO_Port_TypeDef)obj->port,
                            (obj->pin & 0x0F),
                            gpioModeInput,
                            value);
        }
    } else if (mode == PIN_OUTPUT) {
        GPIO_PinModeSet((GPIO_Port_TypeDef)obj->port,
                        (obj->pin & 0x0F),
                        gpioModePushPull,
                        value);
    } else {
        return;
    }
}

void GpioMcuSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler )
{
    bool  risingEdge;
    bool  fallingEdge;

    if (irqMode == IRQ_RISING_EDGE) {
        risingEdge = true;
        fallingEdge = false;
    } else {
        risingEdge = false;
        fallingEdge = true;
    }

    GPIO_IntConfig((GPIO_Port_TypeDef)obj->port,
                    (obj->pin & 0x0F),
                    risingEdge,
                    fallingEdge,
                    true);

    if ((obj->pin & 0x0F) & 0x0000aaaa) {
        NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
        NVIC_EnableIRQ(GPIO_ODD_IRQn);

    } else {
        NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
        NVIC_EnableIRQ(GPIO_EVEN_IRQn);
    }

    g_irq_handlers[(obj->pin) & 0x0F] = irqHandler;
}

void GpioMcuRemoveInterrupt( Gpio_t *obj )
{

}

void GpioMcuWrite( Gpio_t *obj, uint32_t value )
{
    if (value == 0) {
        GPIO_PinOutClear((GPIO_Port_TypeDef)obj->port, ( obj->pin & 0x0F ));
    } else {
        GPIO_PinOutSet((GPIO_Port_TypeDef)obj->port, ( obj->pin & 0x0F ));
    }
}

void GpioMcuToggle( Gpio_t *obj )
{

}

uint32_t GpioMcuRead( Gpio_t *obj )
{
    return GPIO_PinInGet((GPIO_Port_TypeDef)obj->port, ( obj->pin & 0x0F ));
}

static void gpio_int_dispatcher(uint32_t iflags)
{
    volatile uint32_t index;
    volatile uint32_t value;

    if (g_irq_handlers == NULL) {
        return;
    }

    value = iflags;

    for(index = 0; index < 32; index++) {
        if ((value & (0x01 << index)) != 0) {
            break;
        }
    }

    if (index >= 32) {
        return;
    }

    if (g_irq_handlers[index]) {
        g_irq_handlers[index]();
    }
}


void GPIO_EVEN_IRQHandler(void)
{
    uint32_t iflags;

    iflags = GPIO_IntGetEnabled() & 0x00005555;

    GPIO_IntClear(iflags);

    gpio_int_dispatcher(iflags);
}

void GPIO_ODD_IRQHandler(void)
{
    uint32_t iflags;

    iflags = GPIO_IntGetEnabled() & 0x0000aaaa;

    GPIO_IntClear(iflags);

    gpio_int_dispatcher(iflags);
}

