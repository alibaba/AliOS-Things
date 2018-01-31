/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __ESP_COMMON_H__
#define __ESP_COMMON_H__

/** \mainpage ESP8266_RTOS_SDK
 *
 *    - Misc APIs : misc APIs
 *    - WiFi APIs : WiFi related APIs
   *    - SoftAP APIs : ESP8266 Soft-AP APIs
   *    - Station APIs : ESP8266 station APIs
   *    - Common APIs : WiFi common APIs
   *    - Force Sleep APIs : WiFi Force Sleep APIs
   *    - Rate Control APIs : WiFi Rate Control APIs
   *    - User IE APIs : WiFi User IE APIs
   *    - Sniffer APIs : WiFi sniffer APIs
   *    - WPS APIs : WiFi WPS APIs
   *    - Smartconfig APIs : SmartConfig APIs
   *    - AirKiss APIs : AirKiss APIs
 *    - Spiffs APIs : Spiffs APIs
 *    - SSC APIs : Simple Serial Command APIs
 *    - System APIs : System APIs
   *    - Boot APIs : Boot mode APIs
   *    - Upgrade APIs : Firmware upgrade (FOTA) APIs
 *    - Software timer APIs : Software timer APIs
 *    - Network Espconn APIs : Network espconn APIs
 *    - ESP-NOW APIs : ESP-NOW APIs
 *    - Mesh APIs : Mesh APIs
 *    - Driver APIs : Driver APIs
   *    - PWM Driver APIs : PWM driver APIs
   *    - UART Driver APIs : UART driver APIs
   *    - GPIO Driver APIs : GPIO driver APIs
   *    - SPI Driver APIs : SPI Flash APIs
   *    - Hardware timer APIs : Hardware timer APIs
 *
  * void user_init(void) is the entrance function of the application.
  * @attention 1. It is recommended that users set the timer to the periodic mode
  *               for periodic checks. 
  * @attention (1). In freeRTOS timer or os_timer, do not delay by while(1) or
  *               in the manner that will block the thread.
  * @attention (2). The timer callback should not occupy CPU more than 15ms.
  * @attention (3). os_timer_t should not define a local variable, it has to be global varialbe
  *               or memory got by malloc.
  *
  * @attention 2. Since esp_iot_rtos_sdk_v1.0.4, functions are stored in CACHE by
  *               default, need not be added ICACHE_FLASH_ATTR any more. The interrupt
  *               functions can also be stored in CACHE. If users want to store some
  *               frequently called functions in RAM, please add IRAM_ATTR before
  *               functions' name.
  *
  * @attention 3. Network programming use socket, please do not bind to the same port.  
  * @attention (1). If users want to create 3 or more than 3 TCP connections, please add 
  *               "TCP_WND = 2 x TCP_MSS;" in "user_init".
  *
  * @attention 4. Priority of the RTOS SDK is 15. xTaskCreate is an interface of
  *               freeRTOS. For details of the freeRTOS and APIs of the system, 
  *               please visit http://www.freertos.org
  * @attention (1). When using xTaskCreate to create a task, the task stack range is [176, 512].
  * @attention (2). If an array whose length is over 60 bytes is used in a task, 
  *                 it is suggested that users use malloc and free rather than local
  *                 variable to allocate array. Large local variables could lead to
  *                 task stack overflow.
  * @attention (3). The RTOS SDK takes some priorities. Priority of the pp task is
  *                 13; priority of precise timer(ms) thread is 12; priority of the
  *                 TCP/IP task is 10; priority of the freeRTOS timer is 2; priority of
  *                 the idle task is 0.
  * @attention (4). Users can use tasks with priorities from 1 to 9. 
  * @attention (5). Do not revise FreeRTOSConfig.h, configurations are decided by source code
  *                 inside the RTOS SDK, users can not change it.
 */

#include "c_types.h"
#include "esp_libc.h"
#include "esp_misc.h"
#include "esp_wifi.h"
#include "esp_softap.h"
#include "esp_sta.h"
#include "esp_system.h"
#include "esp_timer.h"
//#include "esp_ssc.h"
//#include "esp_spiffs.h"
//#include "esp_wpa2.h"

#include "esp8266/esp8266.h"

//#include "smartconfig.h"
//#include "spi_flash.h"
//#include "pwm.h"

#endif
