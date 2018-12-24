/**
 ******************************************************************************
 * @file    mico_opt.h
 * @author  William Xu
 * @version V1.0.0
 * @date    18-Apr-2018
 * @brief   This file provide default configurations.
 ******************************************************************************
 *
 * Copyright (c) 2009-2018 MXCHIP Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

#ifndef __MX_OPT_H__
#define __MX_OPT_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *                            AliCloud SDS service
 ******************************************************************************/

#if !defined ALISDS_ATTR_VAL_MAX_LEN
#define ALISDS_ATTR_VAL_MAX_LEN              (1024)     /**< Max possible attr value length, reduce to save memory */
#endif

#if !defined ALISDS_ATTR_NAME_MAX_LEN
#define ALISDS_ATTR_NAME_MAX_LEN             (64)       /**< Max possible attr name length, reduce to save memory */
#endif

/******************************************************************************
 *                            AT command parser
 ******************************************************************************/

#if !defined AT_BUFFER_SIZE
#define AT_BUFFER_SIZE                    (512)         /**< Max buffer used for AT command analyser, no used for recv raw data */
#endif

/******************************************************************************
 *                            MX HAL
 ******************************************************************************/

#if !defined MX_SERIAL_RX_BUF_SIZE
#define MX_SERIAL_RX_BUF_SIZE               (1024)  /**< AT command receiver serial port input buffer size */
#endif


/******************************************************************************
 *                             MX Debug Enabler
 ******************************************************************************/

#if !defined MX_DEBUG_MIN_LEVEL
#define MX_DEBUG_MIN_LEVEL                    MX_DEBUG_LEVEL_ALL
#endif

#if !defined MX_DEBUG_TYPES_ON
#define MX_DEBUG_TYPES_ON                     MX_DEBUG_ON
#endif


/******************************************************************************
 *                            Debug and Log
 ******************************************************************************/

#if !defined CONFIG_APP_DEBUG
#define CONFIG_APP_DEBUG                       MX_DEBUG_ON
#endif

#if !defined CONFIG_CLOUD_DEBUG
#define CONFIG_CLOUD_DEBUG                     MX_DEBUG_ON
#endif


#ifdef __cplusplus
} /*extern "C" */
#endif

#endif //__MX_OPT_H
