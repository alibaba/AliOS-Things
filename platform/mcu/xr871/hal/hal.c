/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/unistd.h>
#include <sys/errno.h>
#include <string.h>

#include "aos/hal/uart.h"
#include "aos/hal/wdg.h"
#include "aos/hal/sd.h"
#include "aos/hal/flash.h"

#include "network/hal/wifi.h"
//#include "hal/ota.h"

#include "sys/interrupt.h"

#include "board.h"

extern int errno;
wdg_dev_t  wdg;
uart_dev_t uart_0;
sd_dev_t sd_dev;

extern hal_wifi_module_t sim_aos_wifi_xr871;
//extern hal_ota_module_t xr871_ota_module;

void hal_init(void)
{
    // init uart
    uart_0.port                = STDIO_UART;
    uart_0.config.baud_rate    = STDIO_UART_BUADRATE;
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.parity       = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;
    printf("uart_0.port:%d\n",uart_0.port);
    hal_uart_init(&uart_0);
}

void hal_init_post(void)
{
    // init wifi
	hal_wifi_register_module(&sim_aos_wifi_xr871);
	hal_wifi_init();
	
// 	hal_ota_register_module(&xr871_ota_module);
}

void hal_boot(hal_partition_t partition)
{
    uint32_t addr;
    hal_logic_partition_t  partition_info;
    hal_logic_partition_t *p_partition_info;

    intc_deinit();

    p_partition_info = &partition_info;
    memset(p_partition_info, 0, sizeof(hal_logic_partition_t));
    hal_flash_info_get(partition, p_partition_info);
    addr = p_partition_info->partition_start_addr;
    __asm volatile ("BX %0" : : "r" (addr) );
}

void hal_reboot(void)
{
	HAL_WDG_Reboot();
}

