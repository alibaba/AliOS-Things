/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#include "aos/init.h"

#include "board.h"

#include "network/hal/wifi.h"

extern void hw_start_hal(void);

#ifndef AOS_BINS
extern int application_start(int argc, char **argv);
#endif

uint32_t uid_hash_value = 0xffffffff;

/* main task stask size(byte) */
#define OS_MAIN_TASK_STACK 0x200
#define OS_MAIN_TASK_PRI 32

static ktask_t g_main_task;
cpu_stack_t g_main_task_buf[OS_MAIN_TASK_STACK];
static kinit_t kinit = {0, NULL, 1};

static void numicro_userconf_check_rewrite (void)
{
    uint32_t au32Config[2];
        uint32_t    i, u32Data;            /* variables */

    SYS_UnlockReg();

    FMC_Open();

    if (FMC_GetBootSource() == 0)
        printf("[APROM]\n");           /* debug message */
    else
        printf("[LDROM]\n");           /* debug message */

    u32Data = FMC_ReadCID();           /* Get company ID, should be 0xDA. */
    printf("  Company ID ............................ [0x%08x]\n", u32Data);   /* information message */
    uid_hash_value ^= u32Data;

    u32Data = FMC_ReadPID();           /* Get product ID. */
    printf("  Product ID ............................ [0x%08x]\n", u32Data);   /* information message */
    uid_hash_value ^= u32Data;

    for (i = 0; i < 3; i++)            /* Get 96-bits UID. */
    {
        u32Data = FMC_ReadUID(i);
        uid_hash_value ^= u32Data;
        printf("  Unique ID %d ........................... [0x%08x]\n", i, u32Data);  /* information message */
    }

    for (i = 0; i < 4; i++)            /* Get 4 words UCID. */
    {
        u32Data = FMC_ReadUCID(i);
        uid_hash_value ^= u32Data;
        printf("  Unique Customer ID %d .................. [0x%08x]\n", i, u32Data);  /* information message */
    }
    printf("uid_hash_value=0x%08x\r\n", uid_hash_value);

    if (FMC_ReadConfig(au32Config, 2) < 0)
        goto exit_numicro_userconf_check_rewrite;

    #if 0
    //Disable LDROM booting.
    printf("FMC User config: 0:%08x, 1:%08x, CBS:%d\n", au32Config[0], au32Config[1], ((au32Config[0]&0xC0)>>6) );
    if ( (au32Config[0]&0xC0) != 0x0 )  //Boot from LD-IAP mode?
    {
        FMC_ENABLE_CFG_UPDATE();

        au32Config[0] = (au32Config[0] & ~(0xC0)) ;

        if (FMC_WriteConfig(au32Config, 2) < 0)
        {
            printf("Error: FMC_WriteConfig!\n");
            goto exit_numicro_userconf_check_rewrite;
        }

        memset(au32Config, 0xff ,sizeof(au32Config));

        FMC_ReadConfig(au32Config, 2);

        if ( (au32Config[0]&0xC0) != 0x0 )
        {
            printf("Error: Program Config Failed - 0:%08x, 1:%08x\n", au32Config[0], au32Config[1]);
            goto exit_numicro_userconf_check_rewrite;
        }

        printf("do chip reset\n");
        /* Reset Chip to reload new CONFIG value */
        SYS->IPRST0 = SYS_IPRST0_CHIPRST_Msk;
    }
    #endif

exit_numicro_userconf_check_rewrite:

    /* Disable FMC ISP function */
    FMC_Close();
    /* Lock protected registers */
    SYS_LockReg();
}

static void sys_init(void)
{
    /* user code start*/

    /*insert driver to enable irq for example: starting to run tick time.
     drivers to trigger irq is forbidden before aos_start, which will start core schedule.
    */
    /* Configure the Systick interrupt time */
    SysTick_Config ( SystemCoreClock / RHINO_CONFIG_TICKS_PER_SECOND);

    /* Standard I/O initialization */
    stduart_init();

    numicro_userconf_check_rewrite();

    board_cli_init();

#if defined(DEV_SAL_MK3060)
    extern hal_wifi_module_t aos_wifi_module_mk3060;
    hal_wifi_register_module(&aos_wifi_module_mk3060);
    hal_wifi_init();
#endif

#if defined(WITH_SAL) || defined(CONFIG_AOS_LWIP)
//    hal_ota_register_module(&numicro_ota_module);
#endif

    hw_start_hal();

#ifdef CONFIG_AOS_LWIP
    extern hal_wifi_module_t numicro_eth_m487;
    extern int lwip_tcpip_init(void);
    hal_wifi_register_module(&numicro_eth_m487);
    lwip_tcpip_init();
#endif
    /*user_trigger_irq();*/  //for example

    /*aos components init including middleware and protocol and so on !*/
    aos_components_init(&kinit);

    #ifndef AOS_BINS
    application_start(0, NULL);  /* jump to app/example entry */
    #endif
}


int main(void)
{
    /*irq initialized is approved here.But irq triggering is forbidden, which will enter CPU scheduling.
    Put them in sys_init which will be called after aos_start.
    Irq for task schedule should be enabled here, such as PendSV for cortex-M4.
    */
    board_init();   //including aos_heap_set();  flash_partition_init();

    /*kernel init, malloc can use after this!*/
    krhino_init();

    /*main task to run */
    krhino_task_create(&g_main_task, "main_task", 0,
                       OS_MAIN_TASK_PRI, 0,
                       g_main_task_buf, OS_MAIN_TASK_STACK,
                       (task_entry_t)sys_init, 1);

    /*kernel start schedule!*/
    krhino_start();

    /*never run here*/
    return 0;
}

