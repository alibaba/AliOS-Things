/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "hw.h"
#include "radio.h"
#include "delay.h"
#include "system/timer.h"
#include "sx1276.h"
#include "port_mcu.h"
#include "lorawan_port.h"
#include "hw_conf.h"

#define BOARD_WAKEUP_TIME           5

#define BOARD_TCXO_WAKEUP_TIME      0

void SX1276SetXO( uint8_t state );

uint32_t SX1276GetWakeTime( void );

void SX1276IoIrqInit( DioIrqHandler **irqHandlers );

void SX1276_SetAntSw( uint8_t opMode );

uint8_t SX1276GetPaSelect( uint32_t channel );

void SX1276SetAntSwLowPower( bool status );

void SX1276SetRfTxPower( int8_t power );

void SX1276SetAntSw(uint8_t opMode);

uint32_t SX1276GetWakeTime(void)
{
    return BOARD_WAKEUP_TIME;
}

void SX1276SetXO(uint8_t state)
{
}

void SX1276IoInit(void)
{
    GpioInit(&SX1276.DIO0, RADIO_DIO_0, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioInit(&SX1276.DIO1, RADIO_DIO_1, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioInit(&SX1276.DIO2, RADIO_DIO_2, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioInit(&SX1276.DIO3, RADIO_DIO_3, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioInit(&SX1276.DIO4, RADIO_DIO_4, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
}

extern DioIrqHandler **g_irq_handlers;


void SX1276IoIrqInit(DioIrqHandler **irq_handlers)
{
    GpioSetInterrupt(&SX1276.DIO0, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irq_handlers[0]);
    GpioSetInterrupt(&SX1276.DIO1, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irq_handlers[1]);
    GpioSetInterrupt(&SX1276.DIO2, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irq_handlers[2]);
    GpioSetInterrupt(&SX1276.DIO3, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irq_handlers[3]);
    GpioSetInterrupt(&SX1276.DIO4, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irq_handlers[4]);
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

Gpio_t   g_antSwitchEn;
Gpio_t   g_antSwitchPw;

static void SX1276AntSwInit(void)
{
    // PC8: RF_SWITCH_EN
    GpioInit(&g_antSwitchEn, RADIO_ANT_SWITCH_EN, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1);

    // PC15: RF_SWITCH_PW
    GpioInit(&g_antSwitchPw, RADIO_ANT_SWITCH_PW, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1);
}

static void SX1276AntSwDeInit(void)
{
    // PC8: RF_SWITCH_EN
    GpioInit(&g_antSwitchEn, RADIO_ANT_SWITCH_EN, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);

    // PC15: RF_SWITCH_PW
    GpioInit(&g_antSwitchPw, RADIO_ANT_SWITCH_PW, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
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
            break;
        case RFLR_OPMODE_RECEIVER:
        case RFLR_OPMODE_RECEIVER_SINGLE:
        case RFLR_OPMODE_CAD:
        default:
            GPIO_PinOutSet(gpioPortC, 8);
            break;
    }
}

bool SX1276CheckRfFrequency( uint32_t frequency )
{
    return true;
}

void SX1276Reset( void )
{
    // Set RESET pin to 0
    //aos_lrwan_radio_ctrl.radio_reset();
    GpioInit(&SX1276.Reset, RADIO_RESET, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);

    // Wait 1 ms
    DelayMs( 1 );

    // Configure RESET as input
    //aos_lrwan_radio_ctrl.radio_reset_cfg_input();
    GpioInit(&SX1276.Reset, RADIO_RESET, PIN_INPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1);

    // Wait 6 ms
    DelayMs( 6 );
}

uint32_t SX1276GetBoardTcxoWakeupTime( void )
{
    return BOARD_TCXO_WAKEUP_TIME;
}

const struct Radio_s Radio = {
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
    SX1276SetMaxPayloadLength,
    SX1276SetPublicNetwork,
    SX1276GetWakeupTime
};
