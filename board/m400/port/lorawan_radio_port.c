/** ****************************************************************************
 * @copyright Copyright (c) XXX
 * All rights reserved.
 *
 * @file    lorawan_radio_port.c
 * @brief   radio port for lorawan
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
#include "hw_gpio.h"
#include "hw_spi.h"


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
 * \brief radio reset
 * \return none
 */
static void radio_reset(void);

/**
 * @brief  Configure radio reset pin as input
 * @return none
 */
static void radio_reset_cfg_input(void);

/**
 * @brief  enable radio data tx/rx
 * @return none
 */
static void radio_rw_en(void);

/**
 * @brief  disable radio data tx/rx
 * @return none
 */
void radio_rw_dis(void);

/**
 * @brief  write and read radio data
 * @param  [in] tx_data - data to tx
 * @return data rx
 */
static uint16_t radio_rw(uint16_t tx_data);


/*******************************************************************************
 * GLOBAL VARIABLES
 */
/* LoRaWan radio control */
hal_lrwan_radio_ctrl_t aos_lrwan_radio_ctrl = {
    .radio_reset = radio_reset,
    .radio_reset_cfg_input = radio_reset_cfg_input,
    .radio_rw_en = radio_rw_en,
    .radio_rw_dis = radio_rw_dis,
    .radio_rw = radio_rw,
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

void radio_reset()
{
    GPIO_InitTypeDef initStruct={0};

    initStruct.Mode =GPIO_MODE_OUTPUT_PP;
    initStruct.Pull = GPIO_NOPULL;
    initStruct.Speed = GPIO_SPEED_HIGH;

    // Set RESET pin to 0
    HW_GPIO_Init( RADIO_RESET_PORT, RADIO_RESET_PIN, &initStruct);
    HW_GPIO_Write( RADIO_RESET_PORT, RADIO_RESET_PIN, 0 );
}

void radio_reset_cfg_input()
{
    GPIO_InitTypeDef initStruct={0};

    initStruct.Mode =GPIO_MODE_OUTPUT_PP;
    initStruct.Pull = GPIO_NOPULL;
    initStruct.Speed = GPIO_SPEED_HIGH;

    // Configure RESET as input
    initStruct.Mode = GPIO_NOPULL;
    HW_GPIO_Init( RADIO_RESET_PORT, RADIO_RESET_PIN, &initStruct);
}

void radio_rw_en()
{
	//NSS = 0;
    HW_GPIO_Write( RADIO_NSS_PORT, RADIO_NSS_PIN, 0 );
}

void radio_rw_dis()
{
	//NSS = 1;
    HW_GPIO_Write( RADIO_NSS_PORT, RADIO_NSS_PIN, 1 );
}

uint16_t radio_rw(uint16_t tx_data)
{
	return HW_SPI_InOut(tx_data);
}
