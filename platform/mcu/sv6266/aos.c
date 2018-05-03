/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>

#include "sys/xip.h"
#include "sys/dbgcon.h"
#include "osal.h"
#include "wifinetstack.h"
#include "idmanage/pbuf.h"
#include "security/drv_security.h"
#include "phy/drv_phy.h"
#include "soc_defs.h"
#include "ieee80211_mgmt.h"
#include "ieee80211_mac.h"
#include "sta_func.h"
#include "wifi_api.h"
#include "netstack.h"
#include "netstack_def.h"
#include "wifi_api.h"

#define AOS_START_STACK 4096

ktask_t *g_radio_init;
static void ssvradio_init_task(void *pdata)
{
    PBUF_Init();
    NETSTACK_RADIO.init();    
    drv_sec_init();
    netstack_init(NULL);
    OS_TaskDelete(NULL);
}

ktask_t *g_aos_init;
static kinit_t kinit;
static void var_init()
{
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
}

extern void hw_start_hal(void);
extern void board_init();
static void system_init(void)
{
    board_init();
    hw_start_hal();
    var_init();
    aos_kernel_init(&kinit);
}

static void app_start(void)
{
    xip_init();
    xip_enter();
    flash_init();
    
    dbgcon_init(UART_SPR_BAUD_921600);
    
    OS_Init();
    OS_MemInit();
    OS_PsramInit();
    
    OS_TaskCreate(ssvradio_init_task, "ssvradio_init", 512, NULL, 1, NULL);
    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)system_init, 1);
    
    OS_StartScheduler();
}

void APP_Init(void)
{
    app_start();
}
