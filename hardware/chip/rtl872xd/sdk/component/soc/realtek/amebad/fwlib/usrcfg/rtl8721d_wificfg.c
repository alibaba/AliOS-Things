/**
  ******************************************************************************
  * @file    rtl8721d_wificfg.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the wifi configration 
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#include "ameba_soc.h"

WIFICFG_TypeDef wifi_config =
{
	.wifi_app_ctrl_tdma = FALSE,
	.wifi_ultra_low_power = FALSE, /* default is FALSE */
	.km4_cache_enable = TRUE, /* default is TRUE */
	.km0_dslp_force_reinit = TRUE, /*default is TURE*/
};

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
