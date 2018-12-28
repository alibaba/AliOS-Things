/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/* Includes ------------------------------------------------------------------*/
#include "lorawan_port.h"
#include "hw_msp.h"
#include "rtc-board.h"
#include "gpio-board.h"
#include "system/timer.h"
#include "sx1276.h"

/**
 * @fn     enter_stop_mode
 * @brief  enter stop mode
 * @param  None
 * @rtn    None
 */
static void enter_stop_mode()
{
    HW_EnterStopMode();
}

/**
 * @fn     exit_stop_mode
 * @brief  exit from stop mode
 * @param  None
 * @rtn    None
 */
static void exit_stop_mode()
{
    HW_ExitStopMode();
}

/**
 * @fn     enter_sleep_mode
 * @brief  enter sleep mode
 * @param  None
 * @rtn    None
 */
static void enter_sleep_mode()
{
    HW_EnterSleepMode();
}

static uint8_t get_battery_level(void)
{
    return HW_GetBatteryLevel();
}

static void get_unique_id(uint8_t *id)
{
    HW_GetUniqueId(id);
}

static uint32_t get_random_seed(void)
{
    return HW_GetRandomSeed();
}


/**
 * @fn     get_mft_id
 * @brief  get manufactory id
 * @param  None
 * @rtn    id
 */
static uint32_t get_mft_id(void)
{
    return HW_Get_MFT_ID();
}

/**
 * @fn     get_mft_model
 * @brief  get manufactory model
 * @param  None
 * @rtn    model
 */
static uint32_t get_mft_model(void)
{
    return HW_Get_MFT_Model();
}

/**
 * @fn     get_mft_rev
 * @brief  get manufactory revision
 * @param  None
 * @rtn    rev
 */
static uint32_t get_mft_rev(void)
{
    return HW_Get_MFT_Rev();
}

/**
 * @fn     get_mft_sn
 * @brief  get manufactory sn
 * @param  None
 * @rtn    sn
 */
static uint32_t get_mft_sn(void)
{
    return HW_Get_MFT_SN();
}

/**
 * @fn     set_mft_baud
 * @brief  set manufactory console's baudrate
 * @param  baudrate of the console
 * @rtn    true indicate success, false indicate baudrate not supported
 */
static bool set_mft_baud(uint32_t baud)
{
    return HW_Set_MFT_Baud(baud);
}

/**
 * @fn     get_mft_baud
 * @brief  get manufactory console's baudrate
 * @param  None
 * @rtn    console baudrate
 */
static uint32_t get_mft_baud(void)
{
    return HW_Get_MFT_Baud();
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
