/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Generic SX1276mb1mas driver implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
 /*******************************************************************************
  * @file    eml3047.h
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    01-June-2017
  * @brief   driver LoRa module murata cmwx1zzabz-091
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EML3047_H__
#define __EML3047_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#define BOARD_WAKEUP_TIME  5 //TCXO





#define RF_MID_BAND_THRESH                          525000000

/* Exported functions ------------------------------------------------------- */ 

/*!
 * \brief Initializes the radio I/Os pins interface
 */
void SX1276IoInit( void );


/*!
 * \brief De-initializes the radio I/Os pins interface. 
 *
 * \remark Useful when going in MCU lowpower modes
 */
void SX1276IoDeInit( void );

/*!
 * \brief Checks if the given RF frequency is supported by the hardware
 *
 * \param [IN] frequency RF frequency to be checked
 * \retval isSupported [true: supported, false: unsupported]
 */
bool SX1276CheckRfFrequency( uint32_t frequency );

/*!
 * Radio hardware and global parameters
 */

#ifdef __cplusplus
}
#endif

#endif /* __MLM32L07X01_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
