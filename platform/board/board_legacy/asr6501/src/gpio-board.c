/*!
 * \file      gpio-board.c
 *
 * \brief     Target board GPIO driver implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include <project.h>
//#include "asr_project.h"
#include "utilities.h"
#include "board-config.h"
#include "hw_rtc.h"
#include "gpio-board.h"
#include "debug.h"

void hal_gpio_write(int pin, int value)
{
    switch (pin)
    {
        case RESET_PIN: SPI_NRESET_Write(value); break;
        case DIO1_PIN: dio1_Write(value); break;
        case ANTPOW_PIN: antpow_Write(value); break;
        case NSS_PIN: nss_Write(value); break;
        case BUSY_PIN: SPI_BUSY_Write(value); break;
        default:
            DBG_PRINTF("unsupport pin %d\r\n", pin);
    }
}

int hal_gpio_read(int pin)
{
    switch (pin)
    {
        case RESET_PIN: return SPI_NRESET_Read();
        case DIO1_PIN: return dio1_Read();
        case ANTPOW_PIN: return antpow_Read();
        case NSS_PIN: return nss_Read();
        case BUSY_PIN: return SPI_BUSY_Read();
        default:
            DBG_PRINTF("unsupport pin %d\r\n", pin);
            return 0;
    }
    return 0;
}

void GpioMcuInit( Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value )
{
    obj->pin = pin;

    if( pin == NC )
    {
        return;
    }

    obj->pin = pin;
    // Sets initial output value

    GpioMcuWrite( obj, value );

}

void GpioMcuSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler )
{

    static uint8 g_lora_irq_init = 0;
    RegisterGpioCallback(obj, irqHandler);
    if (g_lora_irq_init == 0) {
        //extern void GpioIsrEntry (void);
        //global_irq_StartEx(GpioIsrEntry);
        g_lora_irq_init = 1;
    }
}

void GpioMcuRemoveInterrupt( Gpio_t *obj )
{
    //if (obj->pin == DIO1_PIN)
    //    global_irq_Stop();
}

void GpioMcuWrite( Gpio_t *obj, uint32_t value )
{
    if(obj == NULL)
    {
        DBG_PRINTF("write gpio faild!\r\n");
        return;
    }

    // Check if pin is not connected
    if( obj->pin == NC )
    {
        return;
    }

    hal_gpio_write(obj->pin, value);
}

void GpioMcuToggle( Gpio_t *obj )
{
    if(obj == NULL)
    {
        DBG_PRINTF("toggle gpio faild!\r\n");
        return;
    }

    // Check if pin is not connected
    if( obj->pin == NC )
    {
        return;
    }
    hal_gpio_write(obj->pin, hal_gpio_read(obj->pin) == 0 ? 1 : 0);
}

uint32_t GpioMcuRead( Gpio_t *obj )
{
    if( obj == NULL )
    {
        DBG_PRINTF("gpio read faild!\r\n");
        return 0;
    }
    // Check if pin is not connected
    if( obj->pin == NC )
    {
        return 0;
    }

    return hal_gpio_read(obj->pin);
}