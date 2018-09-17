/** ****************************************************************************
 * @copyright Copyright (c) XXX
 * All rights reserved.
 *
 * @file    lorawan_lpm_port.c
 * @brief   lower power management port
 * @version V1.0.0
 * @author  liucp
 * @date    20180408
 * @warning none
 * @note <b>history:</b>
 * 1. create file
 *
 */

/*******************************************************************************
 * INCLUDES
 */
#include "lorawan_port.h"
#include "hw_msp.h"
#include "dbg_port.h"

/*******************************************************************************
 * DEBUG SWITCH MACROS
 */


/*******************************************************************************
 * MACROS
 */



/*******************************************************************************
 * TYPEDEFS
 */

/**
 * \defgroup GROUP_LOCAL_FUNCTIONS DRIVER:BOARD:LOCAL_FUNCTIONS
 */


/*******************************************************************************
 * CONSTANTS
 */
/*******************************************************************************
 * LOCAL FUNCTIONS DECLEAR
 */
/**
 * \brief enter stop mode
 * \return none
 */
static void enter_stop_mode(void);

/**
 * \brief exit stop mode
 * \return none
 */
static void exit_stop_mode(void);

/**
 * \brief enter sleep mode
 * \return none
 */
static void enter_sleep_mode(void);

/**
 * \brief get battery level
 * \return none
 */
static uint8_t get_battery_level(void);

/**
 * \brief get unique id
 * \return none
 */
static void get_unique_id(uint8_t *id);

/**
 * \brief get random seed
 * \return none
 */
static uint32_t get_random_seed(void);

/**
 * @fn     get_mft_id
 * @brief  get manufactory id
 * @param  None
 * @rtn    id
 */
static uint32_t get_mft_id(void);

/**
 * @fn     get_mft_model
 * @brief  get manufactory model
 * @param  None
 * @rtn    model
 */
static uint32_t get_mft_model(void);

/**
 * @fn     get_mft_rev
 * @brief  get manufactory revision
 * @param  None
 * @rtn    rev
 */
static uint32_t get_mft_rev(void);

/**
 * @fn     get_mft_sn
 * @brief  get manufactory sn
 * @param  None
 * @rtn    sn
 */
static uint32_t get_mft_sn(void);

/**
 * @fn     set_mft_baud
 * @brief  set manufactory console's baudrate
 * @param  baudrate of the console
 * @rtn    true indicate success, false indicate baudrate not supported
 */
static bool set_mft_baud(uint32_t baud);

/**
 * @fn     get_mft_baud
 * @brief  get manufactory console's baudrate
 * @param  None
 * @rtn    console baudrate
 */
static uint32_t get_mft_baud(void);

/*******************************************************************************
 * GLOBAL VARIABLES
 */
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

/*******************************************************************************
 * STATIC VARIABLES
 */




/*******************************************************************************
 * EXTERNAL VARIABLES
 */


/*******************************************************************************
 *  GLOBAL FUNCTIONS IMPLEMENTATION
 */


/*******************************************************************************
 * LOCAL FUNCTIONS IMPLEMENTATION
 */
void enter_stop_mode(void)
{
    wwdg_feed_timer_off( );
    HW_EnterStopMode();
}

void exit_stop_mode(void)
{
    HW_ExitStopMode();
    wwdg_feed_timer_on( );
}

void enter_sleep_mode(void)
{
    wwdg_feed_check_timer( );
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

static uint32_t get_mft_id(void)
{
    return HW_Get_MFT_ID();
}

static uint32_t get_mft_model(void)
{
    return HW_Get_MFT_Model();
}

static uint32_t get_mft_rev(void)
{
    return HW_Get_MFT_Rev();
}

static uint32_t get_mft_sn(void)
{
    return HW_Get_MFT_SN();
}

static bool set_mft_baud(uint32_t baud)
{
    return HW_Set_MFT_Baud(baud);
}

static uint32_t get_mft_baud(void)
{
    return HW_Get_MFT_Baud();
}


