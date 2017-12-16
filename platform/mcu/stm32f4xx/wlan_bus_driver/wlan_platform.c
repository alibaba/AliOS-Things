/**
 ******************************************************************************
 * @file    wlan_platform.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file provide functions called by MICO to wlan RF module
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_logging.h"
#include "wlan_platform_common.h"

/* Used to give a 32k clock to EMW1062 wifi rf module */
OSStatus host_platform_init_wlan_powersave_clock( void )
{
#if defined ( MICO_USE_WIFI_32K_CLOCK_MCO ) && defined ( MICO_USE_WIFI_32K_PIN )
    platform_gpio_set_alternate_function( wifi_control_pins[WIFI_PIN_32K_CLK].port, wifi_control_pins[WIFI_PIN_32K_CLK].pin_number, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_AF_MCO );

    /* enable LSE output on MCO1 */
    RCC_MCO1Config( RCC_MCO1Source_LSE, RCC_MCO1Div_1 );
    return kNoErr;
#elif defined ( MICO_USE_WIFI_32K_PIN )
    return host_platform_deinit_wlan_powersave_clock( );
#else
    return kNoErr;
#endif
}



