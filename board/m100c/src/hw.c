/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include "em_device.h"
#include "em_cmu.h"
#include "em_rtc.h"
#include "em_common.h"
#include "em_gpio.h"
#include "em_chip.h"

#include "spi-board.h"
#include "rtc-board.h"
#include "debug.h"
#include "radio.h"
#include "sx1276.h"
#include "hw_conf.h"

extern const struct Radio_s Radio;

void HW_Init(void)
{
    // PF2 <-> NRESET
    GpioInit(&SX1276.Reset, RADIO_RESET, PIN_INPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1);

    // sx1278 SPI
    SpiInit(&SX1276.Spi, SPI_1, RADIO_MOSI, RADIO_MISO, RADIO_SCLK, RADIO_NSS);

    // rtc init
    rtc_setup();

    SX1276IoInit();
}

void HW_DeInit(void)
{
}

void HW_GetUniqueId(uint8_t *id)
{
    id[7] = 1;
    id[6] = 2;
    id[5] = 3;
    id[4] = 4;
    id[3] = 5;
    id[2] = 6;
    id[1] = 7;
    id[0] = 8;
}

uint8_t HW_GetBatteryLevel(void)
{
    return 0xff;
}

void HW_EnterStopMode(void)
{
}

uint32_t HW_GetRandomSeed(void)
{
    return 0x12345678;
}

void HW_ExitStopMode(void)
{
}

void HW_EnterSleepMode(void)
{
}

static void HW_IoInit(void)
{
}

static void HW_IoDeInit(void)
{
}

void HW_Reset( void )
{
    NVIC_SystemReset();
}
