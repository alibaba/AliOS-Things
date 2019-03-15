/**
 ******************************************************************************
 * @file    sht2x.h
 * @author  William Xu
 * @version V1.0.0
 * @date    9-Apr-2018
 * @brief   SHT2x sensor driver api header file
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


#ifndef __SHT2x_H__
#define __SHT2x_H__

#ifdef __cplusplus
 extern "C" {
#endif

/** \addtogroup drivers */
/** @{*/

/** \addtogroup SHT2x */
/** @{*/


/**
 * @brief   Initialize I2C bus and SHT2x
 *
 * @return  none
 */ 
uint8_t SHT2x_Init(void); 

/**
 * @brief   Procedure a software reset
 *
 * @return  none
 */ 
uint8_t SHT2x_SoftReset(void);

/**
 * @brief   Read temperature data form SHT2x, this operation require some time
 *
 * @return  none
 */ 
float SHT2x_GetTempPoll(void);

/**
 * @brief   Read humidity data form SHT2x, this operation require some time
 *
 * @return  none
 */
float SHT2x_GetHumiPoll(void);

/** @}*/
/** @}*/

#ifdef __cplusplus
}
#endif

#endif

