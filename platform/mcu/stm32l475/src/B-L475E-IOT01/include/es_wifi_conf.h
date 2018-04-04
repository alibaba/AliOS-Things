/**
  ******************************************************************************
  * @file    Cloud/AWS/Inc/es_wifi_conf.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    12-April-2017
  * @brief   ES-WIFI configuration.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#ifndef __ES_WIFI_CONF_H
#define __ES_WIFI_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif  

#define RTOS_AOS
   
#ifdef RTOS_AOS
#include "aos/kernel.h"
#include "k_api.h"


#define LOCK_SPI()           	aos_mutex_lock(&spi_mutex, RHINO_WAIT_FOREVER)
#define UNLOCK_SPI()         	aos_mutex_unlock(&spi_mutex)
#define SEM_SIGNAL(a)     	aos_sem_signal(&a)
#define SEM_WAIT(a,timeout)  	aos_sem_wait(&a,timeout)
#define RTOS_FREE_SEM_MUTEX	rtos_create_resource
#define RTOS_CREATE_SEM_MUTEX	rtos_create_resource

static aos_mutex_t es_wifi_mutex;
static aos_mutex_t spi_mutex;
static aos_sem_t spi_rx_sem;
static aos_sem_t spi_tx_sem;
static aos_sem_t cmddata_rdy_rising_sem;

static	void	rtos_create_resource(void);
static	void	rtos_free_resource(void);


static	void	rtos_create_resource(void)
{
  aos_mutex_new(&spi_mutex);
  aos_sem_new(&spi_rx_sem, 0);   
  aos_sem_new(&spi_tx_sem, 0);   
  aos_sem_new(&cmddata_rdy_rising_sem, 0);
}

static	void	rtos_free_resource(void)
{
  aos_mutex_free(&spi_mutex);
  aos_sem_free(&spi_tx_sem);
  aos_sem_free(&spi_rx_sem);
  aos_sem_free(&cmddata_rdy_rising_sem);
}

#else
#define LOCK_SPI()
#define UNLOCK_SPI()
#define SEM_SIGNAL(a)     
#define RTOS_FREE_SEM_MUTEX()
#define RTOS_CREATE_SEM_MUTEX()
#endif

#define ES_WIFI_MAX_SSID_NAME_SIZE                  32
#define ES_WIFI_MAX_PSWD_NAME_SIZE                  32
#define ES_WIFI_PRODUCT_ID_SIZE                     32
#define ES_WIFI_PRODUCT_NAME_SIZE                   32
#define ES_WIFI_FW_REV_SIZE                         24
#define ES_WIFI_API_REV_SIZE                        16
#define ES_WIFI_STACK_REV_SIZE                      16
#define ES_WIFI_RTOS_REV_SIZE                       16

#define ES_WIFI_DATA_SIZE                           1400
#define ES_WIFI_MAX_DETECTED_AP                     10
   
#define ES_WIFI_TIMEOUT                             0xFFFF
                                                    
#define ES_WIFI_USE_PING                            1
#define ES_WIFI_USE_AWS                             0
#define ES_WIFI_USE_FIRMWAREUPDATE                  0
#define ES_WIFI_USE_WPS                             0
                                                    
#define ES_WIFI_USE_SPI                             0    
#define ES_WIFI_USE_UART                            (!ES_WIFI_USE_SPI)   
   


#ifdef __cplusplus
}
#endif
#endif //__ES_WIFI_CONF_H

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
