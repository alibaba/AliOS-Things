/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <hal/wifi.h>
#include <k_api.h>
#include "ulog/ulog.h"

#include "NuMicro.h"



void hal_reboot(void)
{
    SYS_UnlockReg();                   /* Unlock register lock protect */	
    FMC_Open();										/* Enable FMC ISP function */
    FMC_SetVectorPageAddr(0x100000);
    FMC_SET_LDROM_BOOT();
//    SYS_ResetCPU();

    NVIC_SystemReset();
}


uint32_t uid_hash_value = 0xffffffff ;

void numicro_userconf_check_rewrite (void)
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
    #endif
    
exit_numicro_userconf_check_rewrite:   

    /* Disable FMC ISP function */
    FMC_Close();
    /* Lock protected registers */
    SYS_LockReg();
}


extern k_mm_region_t g_mm_region[1];
void hw_start_hal(void)
{    
	numicro_userconf_check_rewrite();
	
#if defined (__CC_ARM) && defined(__MICROLIB)
    printf("[%s] ARMCC\n", __func__ );
#elif defined(__ICCARM__)
    printf("[%s] IAR\n", __func__ );
#else
    printf("[%s] GCC\n", __func__ );
#endif
    printf("Heap start address: 0x%08x\n", g_mm_region[0].start );
    printf("Heap size: %d KB\n", g_mm_region[0].len / 1024 );
    //test_numicro_ota_set_boot();
    
#if defined(DEV_SAL_MK3060)
	extern hal_wifi_module_t aos_wifi_module_mk3060;
	hal_wifi_register_module(&aos_wifi_module_mk3060);	
	hal_wifi_init();
#endif

#if defined(DEV_SAL_ESP8266)
	extern hal_wifi_module_t aos_wifi_module_esp8266;
	#ifdef ESP8266_USE_HARD_RESET
	extern gpio_dev_t* esp8266_reset_pin;
	esp8266_reset_pin = &board_gpio_table[21];
	#endif
	hal_wifi_register_module(&aos_wifi_module_esp8266);	
	hal_wifi_init();
#endif

#if defined(WITH_LWIP)    
    extern hal_wifi_module_t numicro_eth_m487;
    extern int lwip_tcpip_init(void);
    hal_wifi_register_module(&numicro_eth_m487);
    lwip_tcpip_init();
#endif

#if defined(WITH_SAL) || defined(WITH_LWIP)
//    hal_ota_register_module(&numicro_ota_module);
#endif
    
}
