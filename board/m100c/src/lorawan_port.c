/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include "em_device.h"
#include "em_cmu.h"
#include "em_rtc.h"
#include "em_common.h"
#include "em_gpio.h"

#include "lorawan_port.h"
#include "system/timer.h"
#include "debug.h"

#include <aos/hal/uart.h>

static void enter_stop_mode(void)
{
}

static void exit_stop_mode(void)
{
}

static void enter_sleep_mode(void)
{
    /* enter EM2 Mode for low power */
    EMU_EnterEM2(false);
}

static uint8_t get_battery_level(void)
{
    return 0;
}

static void get_unique_id(uint8_t *id)
{
    *id = 0;
}

static uint32_t get_random_seed(void)
{
    return 0;
}

static uint32_t get_mft_id(void)
{
    return 0x1234;
}

static uint32_t get_mft_model(void)
{
    return 0x4321;
}

static uint32_t get_mft_rev(void)
{
    return 0x0001;
}

static uint32_t get_mft_sn(void)
{
    return 0xFFFF;
}

static bool set_mft_baud(uint32_t baud)
{
    return false;
}

static uint32_t get_mft_baud(void)
{
    return 115200;
}

extern uart_dev_t uart_1;
int linkwan_serial_output(uint8_t *buffer, int len)
{
    hal_uart_send(&uart_1, buffer, len, 1000);
    return len;
}

/* the struct is for changing the device working mode */
hal_lrwan_dev_chg_mode_t aos_lrwan_chg_mode = {
    .enter_stop_mode  = enter_stop_mode,
    .exit_stop_mode   = exit_stop_mode,
    .enter_sleep_mode = enter_sleep_mode,
};

hal_lrwan_sys_t aos_lrwan_sys = {
    .get_battery_level = get_battery_level,
    .get_unique_id = get_unique_id,
    .get_random_seed = get_random_seed,
};

/* LoraWan manufactory interface*/
hal_manufactory_itf_t aos_mft_itf = {
    .get_mft_id = get_mft_id,
    .get_mft_model = get_mft_model,
    .get_mft_rev = get_mft_rev,
    .get_mft_sn = get_mft_sn,
    .set_mft_baud = set_mft_baud,
    .get_mft_baud = get_mft_baud,
};



