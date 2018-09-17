/**
 * @file    wm_include.h
 *
 * @brief   the configuration file of sdk
 *
 * @author  winnermicro
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef __WM_INCLUDE_H__
#define __WM_INCLUDE_H__

/**
 * @mainpage winnermicro venus sdk
 *
 * @section intro_sec winnermicro venus sdk APIs.
 *
 * This library provides easy to use APIs to use by hiding all details.
 *
 * @section usage_sec How to use the sdk
 *
 * @subsection source how to code
 *
 * void UserMain(void) is the entrance function of the application:
 * @code
 * void UserMain(void)
 * {
 *     printf("\n user task\n");
 *
 * #if DEMO_CONSOLE
 *	   CreateDemoTask();
 * #endif
 *
 *     //user's task
 * }
 * @endcode
 *
 * @subsection compile How to compile
 *
 * To build with the sdk you can use the keil tools.
 * Opening "Tools\Keil\Project\WM_SDK.uvproj" to compile.
 *
 * @section install_sec How to download the firmware
 *
 * @subsection fw_flash Download the "WM_SDK.img" image
 *
 * Go to write image to flash.
 *
 * @subsection fw_ram Download the "WM_SDK_UART.img" image
 *
 * Go to download image to ram.
 * @code
 * Press "ESC" and the reset key until the com0 output: "cc..."
 * @endcode
 *
 * \n
 * @attention 1. It is recommended that users set the timer to the periodic mode
 *               for periodic checks.
 * @attention (1). In freeRTOS timer or os_timer, do not delay by while(1) or
 *               in the manner that will block the thread.
 * @attention (2). The timer callback should not occupy CPU more than 15ms.
 * @attention (3). os_timer_t should not define a local variable, it has to be global varialbe
 *               or memory got by malloc.
 *
 * @attention 2. Functions are stored in CACHE by default, need not ICACHE_FLASH_ATTR any more.
 *                The interrupt functions can also be stored in CACHE. If users want to store some
 *                frequently called functions in RAM, please add IRAM_ATTR before functions' name.
 *
 * @attention 3. Priority of the RTOS SDK is 15. xTaskCreate is an interface of
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

#include <stdio.h>
#include <stdlib.h>
#include "wm_type_def.h"
#include "wm_uart.h"
#include "wm_gpio.h"
//#include "wm_flash.h"
#include "wm_hostspi.h"
#include "wm_socket.h"
#include "wm_sockets.h"
#include "wm_wifi.h"
#include "wm_hspi.h"
#include "wm_pwm.h"
#include "wm_params.h"
#include "wm_osal.h"
#include "wm_netif.h"
#include "wm_efuse.h"
#include "wm_mem.h"
#include "wm_regs.h"


#endif
