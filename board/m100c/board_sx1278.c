/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "hw.h"
#include "radio.h"
#include "delay.h"
#include "timer.h"
#include "sx1276.h"
#include "port_mcu.h"

#define BOARD_WAKEUP_TIME 5

void SX1276SetXO( uint8_t state );

uint32_t SX1276GetWakeTime( void );

void SX1276IoIrqInit( DioIrqHandler **irqHandlers );

void SX1276_SetAntSw( uint8_t opMode );

uint8_t SX1276GetPaSelect( uint32_t channel );

void SX1276SetAntSwLowPower( bool status );

void SX1276SetRfTxPower( int8_t power );

void SX1276SetAntSw(uint8_t opMode);

static LoRaBoardCallback_t BoardCallbacks = { SX1276SetXO,
                                              SX1276GetWakeTime,
                                              SX1276IoIrqInit,
                                              SX1276SetRfTxPower,
                                              SX1276SetAntSwLowPower,
                                              SX1276SetAntSw
                                            };
DioIrqHandler **g_irq_handlers;

uint32_t SX1276GetWakeTime(void)
{
    return BOARD_WAKEUP_TIME;
}

void SX1276SetXO(uint8_t state)
{
}

static void gpio_int_dispatcher(uint32_t iflags)
{
    volatile uint32_t index;
    volatile uint8_t  handler_index;
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

    if (index >= 9 && index <= 11) {
        handler_index = index - 9;
    } else if (index >= 13 && index <= 14) {
        handler_index = index = 10;
    } else {
        return;
    }
    if (g_irq_handlers[handler_index]) {
        g_irq_handlers[handler_index]();
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

void SX1276IoInit(void)
{
    SX1276BoardInit(&BoardCallbacks);

    // DIO 0
    GPIO_PinModeSet(gpioPortC, 9, gpioModeInputPull, 0);
    GPIO_IntConfig(gpioPortC, 9, true, false, true);
    // DIO 1
    GPIO_PinModeSet(gpioPortC, 10, gpioModeInputPull, 0);
    GPIO_IntConfig(gpioPortC, 10, true, false, true);
    // DIO 2
    GPIO_PinModeSet(gpioPortC, 11, gpioModeInputPull, 0);
    GPIO_IntConfig(gpioPortC, 11, true, false, true);
    // DIO 3
    GPIO_PinModeSet(gpioPortC, 13, gpioModeInputPull, 0);
    GPIO_IntConfig(gpioPortC, 13, true, false, true);
    // DIO 4
    GPIO_PinModeSet(gpioPortC, 14, gpioModeInputPull, 0);
    GPIO_IntConfig(gpioPortC, 14, true, false, true);
}

void SX1276IoIrqInit(DioIrqHandler **irq_handlers)
{
    NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
    NVIC_EnableIRQ(GPIO_EVEN_IRQn);
    NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
    NVIC_EnableIRQ(GPIO_ODD_IRQn);
    g_irq_handlers = irq_handlers;
}

void SX1276IoDeInit( void )
{
}

void SX1276SetRfTxPower(int8_t power)
{
    uint8_t paConfig = 0;
    uint8_t paDac = 0;

    paConfig = SX1276Read(REG_PACONFIG);
    paDac = SX1276Read(REG_PADAC);

    paConfig = (paConfig & RF_PACONFIG_PASELECT_MASK) | SX1276GetPaSelect(SX1276.Settings.Channel);
    paConfig = (paConfig & RF_PACONFIG_MAX_POWER_MASK) | 0x70;

    if ((paConfig & RF_PACONFIG_PASELECT_PABOOST) == RF_PACONFIG_PASELECT_PABOOST) {
        if (power > 17) {
            paDac = (paDac & RF_PADAC_20DBM_MASK) | RF_PADAC_20DBM_ON;
        } else {
            paDac = (paDac & RF_PADAC_20DBM_MASK) | RF_PADAC_20DBM_OFF;
        }
        if ((paDac & RF_PADAC_20DBM_ON) == RF_PADAC_20DBM_ON) {
            if (power < 5) {
                power = 5;
            }
            if (power > 20) {
                power = 20;
            }
            paConfig = (paConfig & RF_PACONFIG_OUTPUTPOWER_MASK) | (uint8_t)((uint16_t)(power - 5) & 0x0F);
        } else {
            if (power < 2) {
                power = 2;
            }
            if (power > 17) {
                power = 17;
            }
            paConfig = (paConfig & RF_PACONFIG_OUTPUTPOWER_MASK) | (uint8_t)((uint16_t)(power - 2) & 0x0F);
        }
    } else {
        if (power < -1) {
            power = -1;
        }
        if (power > 14) {
            power = 14;
        }
        paConfig = (paConfig & RF_PACONFIG_OUTPUTPOWER_MASK) | (uint8_t)((uint16_t)(power + 1) & 0x0F);
    }
    SX1276Write(REG_PACONFIG, paConfig);
    SX1276Write(REG_PADAC, paDac);
}

uint8_t SX1276GetPaSelect(uint32_t channel)
{
    return RF_PACONFIG_PASELECT_PABOOST;
}

static void SX1276AntSwInit(void)
{
    // PC8: RF_SWITCH_EN
    GPIO_PinModeSet(gpioPortC, 8, gpioModePushPull, 1);

    // PC15: RF_SWITCH_PW
    GPIO_PinModeSet(gpioPortC, 15, gpioModePushPull, 1);
}

static void SX1276AntSwDeInit(void)
{
    // PC15: RF_SWITCH_PW
    GPIO_PinModeSet(gpioPortC, 15, gpioModePushPull, 0);

    // PC8: RF_SWITCH_EN
    GPIO_PinModeSet(gpioPortC, 8, gpioModePushPull, 0);
}

void SX1276SetAntSwLowPower(bool status)
{
    if (status == false) {
        SX1276AntSwInit();
    }  else {
        SX1276AntSwDeInit();
    }
}

void SX1276SetAntSw(uint8_t opMode)
{
    uint8_t paConfig = SX1276Read(REG_PACONFIG);

    switch (opMode) {
        case RFLR_OPMODE_TRANSMITTER:
            GPIO_PinOutClear(gpioPortC, 8);
            SX1276.RxTx = 1;
            break;
        case RFLR_OPMODE_RECEIVER:
        case RFLR_OPMODE_RECEIVER_SINGLE:
        case RFLR_OPMODE_CAD:
        default:
            GPIO_PinOutSet(gpioPortC, 8);
            SX1276.RxTx = 0;
            break;
    }
}

bool SX1276CheckRfFrequency( uint32_t frequency )
{
    return true;
}

const struct Radio_s Radio = {
    SX1276IoInit,
    SX1276IoDeInit,
    SX1276Init,
    SX1276GetStatus,
    SX1276SetModem,
    SX1276SetChannel,
    SX1276IsChannelFree,
    SX1276Random,
    SX1276SetRxConfig,
    SX1276SetTxConfig,
    SX1276CheckRfFrequency,
    SX1276GetTimeOnAir,
    SX1276Send,
    SX1276SetSleep,
    SX1276SetStby,
    SX1276SetRx,
    SX1276StartCad,
    SX1276SetTxContinuousWave,
    SX1276ReadRssi,
    SX1276Write,
    SX1276Read,
    SX1276WriteBuffer,
    SX1276ReadBuffer,
    SX1276SetSyncWord,
    SX1276SetMaxPayloadLength,
    SX1276GetRadioWakeUpTime
};
