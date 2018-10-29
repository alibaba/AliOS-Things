/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include "board.h"

#define AOS_START_STACK 1536

ktask_t *g_aos_init;
static kinit_t kinit;

extern int aos_framework_init(void);
extern void board_init(void);
extern void numicro_soc_init(void);
extern void hw_start_hal(void);

static void var_init()
{
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
}

uint32_t uid_hash_value = 0xffffffff ;

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

    printf("FMC User config: 0:%08x, 1:%08x, CBS:%d\n", au32Config[0], au32Config[1], ((au32Config[0]&0xC0)>>6) );
	if ( (au32Config[0]&0xC0) != 0x0 )	//Boot from LD-IAP mode?
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
    
exit_numicro_userconf_check_rewrite:   

    /* Disable FMC ISP function */
    FMC_Close();
    /* Lock protected registers */
    SYS_LockReg();
}

static void sys_init(void)
{
    /* Call initialize functions in every board implementation. */
	numicro_soc_init();

	numicro_userconf_check_rewrite();

    hw_start_hal();

    board_init();
    
    var_init();
    
    aos_kernel_init(&kinit);
}

int main(void)
{
    /* Specify multiple heap region and size for AliOS-things. */
    aos_heap_set();

    /* Do krhino initialization */
    aos_init();

    /* Create Task for system initialization and demo. */
    krhino_task_dyn_create ( &g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)sys_init, 1 );

    /* Start scheduling & execute first task. */
    aos_start();

    return 0;
}
