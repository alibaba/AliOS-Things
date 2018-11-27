/**
 *
 * \file
 *
 * \brief This module contains SAME54 BSP APIs implementation.
 *
 * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
#ifndef _SAM_SDIO_H_
#define _SAM_SDIO_H_

#include "compiler.h"
#include "../include/nm_bus_wrapper.h"

#ifdef CONF_WILC_USE_SDIO 
/**
*	@fn		nmi_sdio_cmd52
*	@brief	SDIO Command 52
*	@return	M2M_SUCCESS in case of success and <0 in case of failure
*/ 
int8_t nmi_sdio_cmd52(tstrNmSdioCmd52* cmd);

/**
*	@fn		nmi_sdio_cmd53
*	@brief	SDIO Command 53
*	@return	M2M_SUCCESS in case of success and <0 in case of failure
*/ 
int8_t nmi_sdio_cmd53(tstrNmSdioCmd53* cmd);

/**
*	@fn		sam4s_sdio_init
*	@brief	Sam4s SDIO Init
*	@return	M2M_SUCCESS in case of success and <0 in case of failure
*/ 
int8_t same54_sdio_init(void);

#endif /* CONF_WILC_USE_SDIO */
#endif /* _SAM_SDIO_H_ */