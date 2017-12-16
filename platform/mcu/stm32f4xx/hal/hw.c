/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <k_api.h>
#include <aos/log.h>
#include <hal/soc/soc.h>
#include <hal/soc/timer.h>
#include <hal/base.h>
#include <hal/wifi.h>
#include <hal/ota.h>
#include "board.h"

#define TAG "hw"

#define us2tick(us) \
    ((us * RHINO_CONFIG_TICKS_PER_SECOND + 999999) / 1000000)

uart_dev_t uart_0;
extern hal_wifi_module_t sim_aos_wifi_mico;
void hal_reboot(void)
{
    platform_mcu_reset();
}

static void _timer_cb(void *timer, void *arg)
{
    hal_timer_t *tmr = arg;
    tmr->cb(tmr->arg);
}

void hal_timer_init(hal_timer_t *tmr, unsigned int period, unsigned char auto_reload, unsigned char ch, hal_timer_cb_t cb, void *arg)
{
    (void)ch;
    memset(tmr, 0, sizeof(*tmr));
    tmr->cb = cb;
    tmr->arg = arg;
    if (auto_reload > 0u) {
        krhino_timer_dyn_create((ktimer_t **)&tmr->priv, "hwtmr", _timer_cb,
                                us2tick(period), us2tick(period), tmr, 0);
    }
    else {
        krhino_timer_dyn_create((ktimer_t **)&tmr->priv, "hwtmr", _timer_cb,
                                us2tick(period), 0, tmr, 0);
    }
}

int32_t hal_timer_start(hal_timer_t *tmr)
{
    return krhino_timer_start(tmr->priv);
}


void hal_timer_stop(hal_timer_t *tmr)
{
    krhino_timer_stop(tmr->priv);
    krhino_timer_dyn_del(tmr->priv);
    tmr->priv = NULL;
}

//extern hal_wifi_module_t sim_aos_wifi_stm23l475;
//extern struct hal_ota_module_s stm32l475_ota_module;
void hw_start_hal(void)
{
    
    //hal_wifi_register_module(&sim_aos_wifi_stm23l475);
    //hal_ota_register_module(&stm32l475_ota_module);
    uart_0.port                = STDIO_UART;
    uart_0.config.baud_rate    = STDIO_UART_BUADRATE;
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.parity       = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;

    hal_uart_init(&uart_0);
    printf("start-----------hal\n");
    hal_wifi_register_module(&sim_aos_wifi_mico);
    hal_wifi_init(); 
}


int stdio_hardfault( char* data, uint32_t size )
{
  hal_uart_send(&uart_0, data, size, 1000);
  return 0;
}

