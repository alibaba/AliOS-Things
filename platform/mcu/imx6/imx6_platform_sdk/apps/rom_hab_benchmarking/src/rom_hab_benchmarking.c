/*
 * Copyright (c) 2011-2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "sdk.h"
#include "platform_init.h"
#include "hab_defines.h"
#include "core/mmu.h"
#include "core/cortex_a9.h"
#include "timer/epit.h"
#include "timer/gpt.h"
#include "timer/timer.h"
#include "registers/regsarmglobaltimer.h"
#include "registers/regsepit.h"

/* These defines could be customized */
#define NUM_OF_BOOT_DEVICES 3 /* Primary and recovery */

typedef unsigned char (*device_init_f)(void);
typedef unsigned char (*device_read_data_f)(uint32_t*, uint32_t, uint32_t);

typedef enum pu_irom_log_entry {
    ROM_LOG_BOOTMODE_INTERNAL_FUSE      = 0x00010000,
    ROM_LOG_BOOTMODE_SERIAL             = 0x00010001,
    ROM_LOG_BOOTMODE_INTERNAL           = 0x00010002,
    ROM_LOG_BOOTMODE_TEST               = 0x00010003,
    ROM_LOG_SEC_CONFIG_FAB              = 0x00020000,
    ROM_LOG_SEC_CONFIG_RETURN           = 0x00020033,
    ROM_LOG_SEC_CONFIG_OPEN             = 0x000200F0,
    ROM_LOG_SEC_CONFIG_CLOSED           = 0x000200CC,
    ROM_LOG_DIR_BT_DIS_VALUE0           = 0x00030000,
    ROM_LOG_DIR_BT_DIS_VALUE1           = 0x00030001,
    ROM_LOG_BT_FUSE_SEL_VALUE0          = 0x00040000,
    ROM_LOG_BT_FUSE_SEL_VALUE1          = 0x00040001,
    ROM_LOG_PRIM_IMAGE_SELECT           = 0x00050000,
    ROM_LOG_SEC_IMAGE_SELECT            = 0x00050001,
    ROM_LOG_PRIM_BOOTDEVICE_NAND        = 0x00060000,
    ROM_LOG_PRIM_BOOTDEVICE_USDHC       = 0x00060001,
    ROM_LOG_PRIM_BOOTDEVICE_SATA        = 0x00060002,
    ROM_LOG_PRIM_BOOTDEVICE_I2C         = 0x00060003,
    ROM_LOG_PRIM_BOOTDEVICE_ECSPI       = 0x00060004,
    ROM_LOG_PRIM_BOOTDEVICE_NOR         = 0x00060005,
    ROM_LOG_PRIM_BOOTDEVICE_ONENAND     = 0x00060006,
    ROM_LOG_REC_BOOTDEVICE_I2C          = 0x00061003,
    ROM_LOG_REC_BOOTDEVICE_ECSPI        = 0x00061004,
    ROM_LOG_REC_BOOTDEVICE_NONE         = 0x00061FFF,
    ROM_LOG_MFG_BOOTDEVICE_USDHC        = 0x00062001,
    ROM_LOG_DEVICE_INIT_CALL            = 0x00070000,
    ROM_LOG_DEVICE_INIT_PASS            = 0x000700F0,
    ROM_LOG_DEVICE_INIT_FAIL            = 0x00070033,
    ROM_LOG_DEVICE_READ_DATA_CALL       = 0x00080000,
    ROM_LOG_DEVICE_READ_DATA_PASS       = 0x000800F0,
    ROM_LOG_DEVICE_READ_DATA_FAIL       = 0x00080033,
    ROM_LOG_AUTHENTICATION_STATUS       = 0x00090000,
    ROM_LOG_PLUGIN_IMAGE_CALL           = 0x000A0000,
    ROM_LOG_PLUGIN_IMAGE_PASS           = 0x000A00F0,
    ROM_LOG_PLUGIN_IMAGE_FAIL           = 0x000A0033,
    ROM_LOG_PROGRAM_IMAGE_CALL          = 0x000B0000,
    ROM_LOG_SDP_ENTRY                   = 0x000C0000,
    ROM_LOG_SDP_IMAGE_CALL              = 0x000D0000,              
} pu_irom_log_entry_t;

typedef struct
{
    uint32_t src_sbmr1;
} HAPI_CLOCK_SRC_SBMR_SHADOW_REG_T;
enum BOOT_DEVICE_TYPE {
    PRIMARY_DEVICE=0,
    RECOVERY_DEVICE=1
};

typedef struct {
    uint32_t start;            /* Start address of the image */
    uint32_t size;             /* Size of the image */
    uint32_t plugin;           /* Plugin flag */
} BOOT_DATA_T;

#define PLUGIN_IMAGE(boot_data) (((BOOT_DATA_T*)boot_data)->plugin)

typedef struct {
    device_init_f device_init;            /* pointer to device_init function. May be NULL. */
    device_read_data_f device_read_data;  /* pointer to device_read_data function. May be NULL. */
    uint32_t initial_image_size;            /* size of initial image downloaded to temporary location. */
    uint32_t *initial_image_address;        /* temporary location of initial image. */
    uint32_t image_offset;                  /* image offset from start of boot device.
                                             The offset is given in device_page_size units. */
    uint32_t device_page_size;              /* device page size */
    uint32_t ivt_offset;                    /* image vector table offset from start of boot image. */
    unsigned char use_secondary_image;          /* flag that indicates if boot device supports secondary image
                                             using secondary image table (SECONDARY_IMG_TBL_T). */
    pu_irom_log_entry_t device_log_entry; /* log entry of selected device */
} BOOT_DRIVER_DATA_T;

typedef struct {
    BOOT_DRIVER_DATA_T boot_driver_data[NUM_OF_BOOT_DEVICES]; /* Array of available boot devices structure */
    enum BOOT_DEVICE_TYPE current_boot_device;                /* Current boot device index */
} BOOT_DRIVER_SELECTION_T;

typedef void (*pu_irom_setup_boot_selection_f)(void);
typedef hab_image_entry_f (*download_image_f)(enum BOOT_DEVICE_TYPE boot_device);

typedef unsigned char (*download_initial_image_f)(void);
#define ROM_1_1_download_initial_image_addr (0x00001ead)
#define ROM_1_2_download_initial_image_addr (0x00001ead)   //download_initial_image
#define ROM_1_3_download_initial_image_addr (0x00001f49)

typedef hab_status_t (*my_pu_irom_hwcnfg_setup_f)(void **start, size_t *bytes, const void *boot_data);
#define ROM_1_1_hwcnfg_setup_addr (0x0000211d)
#define ROM_1_2_hwcnfg_setup_addr (0x0000211d)   //pu_irom_hwcnfg_setup
#define ROM_1_3_hwcnfg_setup_addr (0x000021b9)

typedef void (*pu_irom_boot_mmu_disable_f)(void);
#define ROM_1_1_boot_mmu_disable_addr (0x00006541)
#define ROM_1_2_boot_mmu_disable_addr (0x000065e5)     //pu_irom_boot_mmu_disable
#define ROM_1_3_boot_mmu_disable_addr (0x00006681)

#define PLATFORM_INIT   1 

#define ROM_1_1_shadow_src_addr  (0x009024b4)     //hapi_src_reg
#define ROM_1_2_shadow_src_addr  (0x009024b4)     //hapi_src_reg
#define ROM_1_3_shadow_src_addr  (0x009024b4)

#define ROM_1_1_setup_boot_selection_addr (0x00001f6f)
#define ROM_1_2_setup_boot_selection_addr (0x00001f6f)   //pu_irom_setup_boot_selection
#define ROM_1_3_setup_boot_selection_addr (0x0000200b)

#define ROM_1_1_download_image_addr (0x000021fb)
#define ROM_1_2_download_image_addr (0x000021fb)   //download_image
#define ROM_1_3_download_image_addr (0x00002297)  

#define ROM_VERSION_ADDR (0x00000048)
#define ROM_VERSION_1_2 (0x00000012)
#define ROM_VERSION_1_1 (0x00000011)

#define ROM_BOOT_DEVICE_MASK (0x000018F0)
//#if defined (BOARD_EVB)
//#define ROM_BOOT_DEVICE_SD (0x00000880) //SD3
//#else
#define ROM_BOOT_DEVICE_SD (0x00000840) // SD2
//#endif

extern void freq_populate(void);
extern uint64_t time_get_microseconds(void);

download_initial_image_f download_initial_image;
my_pu_irom_hwcnfg_setup_f irom_hwcnfg_setup;
pu_irom_boot_mmu_disable_f rom_boot_mmu_disable;

uint32_t epit_instance = HW_EPIT2;
uint32_t starttime_init, endtime_init; 
uint32_t starttime_auth, endtime_auth;
uint32_t starttime_hwcnfg, endtime_hwcnfg;
uint32_t time_counter;
uint32_t counter_init1, counter_init;
uint32_t counter_auth1, counter_auth;
uint32_t counter_hwcnfg1, counter_hwcnfg;

hab_status_t my_pu_irom_hwcnfg_setup(void **start, size_t *bytes, const void *boot_data)
{
	hab_status_t status = HAB_FAILURE;
#if PLATFORM_INIT == 1
    starttime_hwcnfg = (uint32_t)time_get_microseconds();
#else      
    counter_hwcnfg1 = epit_get_counter_value(epit_instance);
//    epit_counter_enable(epit_instance, 0xFFFFFFFF, 0);   //polling mode
#endif 	

    status = irom_hwcnfg_setup(start, bytes, boot_data);
    
#if PLATFORM_INIT == 1
    endtime_hwcnfg = (uint32_t)time_get_microseconds();
#else      
    counter_hwcnfg = epit_get_counter_value(epit_instance);
//    epit_counter_disable(epit_instance); //polling mode
    counter_hwcnfg = (uint32_t)(counter_hwcnfg - counter_hwcnfg1);  //us
#endif    

    return status;
//    return irom_hwcnfg_setup(start, bytes, boot_data);
}

/*==================================================================================================
FUNCTION: download_image

DESCRIPTION:
   This function downloads initial image and calls HAB4 authentication function.

ARGUMENTS PASSED:
   boot_device - Boot device selection (PRIMARY or RECOVERY).
RETURN VALUE:
   hab_image_entry_f  - valid entry function on success
                      - NULL on failure
PRE-CONDITIONS:
    hab_rvt.evtry() has already been called

POST-CONDITIONS:
    None


IMPORTANT NOTES:
    None

==================================================================================================*/
hab_image_entry_f my_download_image(enum BOOT_DEVICE_TYPE boot_device)
{
    hab_ivt_t *data_ptr = NULL;
    hab_image_entry_f hab_image_entry = NULL;
    unsigned char ext_iface_status = FALSE;
    
    hab_rvt_t * hab_api_table = (hab_rvt_t *)0x00000094;
    BOOT_DRIVER_SELECTION_T * boot_driver_sel = (BOOT_DRIVER_SELECTION_T *)0x009008B4;
    
    BOOT_DRIVER_DATA_T *driver_data = &(boot_driver_sel->boot_driver_data[boot_device]);

    /* Update boot_driver_selection structure to current boot device */
    boot_driver_sel->current_boot_device = boot_device;
    
#if PLATFORM_INIT == 1
    starttime_init = (uint32_t)time_get_microseconds();
#else 
    counter_init1 = epit_get_counter_value(epit_instance);     
//    epit_counter_enable(epit_instance, 0xFFFFFFFF, 0);   //polling mode
#endif 	      

    /* Download initial image */
    ext_iface_status = download_initial_image();

#if PLATFORM_INIT == 1
    endtime_init = (uint32_t)time_get_microseconds();
#else      
    counter_init = epit_get_counter_value(epit_instance);
//    epit_counter_disable(epit_instance); //polling mode
    counter_init = (uint32_t)(counter_init - counter_init1);  //us
//    counter_init = (0xFFFFFFFF - time_counter) / 1000;  //ms
#endif     

    if (ext_iface_status == TRUE) {
        /* Update data_ptr on success */
        data_ptr = (hab_ivt_t *)((uint32_t)driver_data->initial_image_address + driver_data->ivt_offset);
    }
    
    if(data_ptr != NULL)
    {

#if PLATFORM_INIT == 1
        starttime_auth = (uint32_t)time_get_microseconds();
#else
        counter_auth1 = epit_get_counter_value(epit_instance);      
//        epit_counter_enable(epit_instance, 0xFFFFFFFF, 0);   //polling mode
#endif           	
        /* Call HAB authentication function */
        hab_image_entry = hab_api_table->authenticate_image(HAB_CID_ROM,
                                                     driver_data->ivt_offset,
                                                     (void**)(&driver_data->initial_image_address),
                                                     (size_t *)(&driver_data->initial_image_size),
                                                     &my_pu_irom_hwcnfg_setup);
#if PLATFORM_INIT == 1
        endtime_auth = (uint32_t)time_get_microseconds();
#else      
        counter_auth = epit_get_counter_value(epit_instance);
        counter_auth = (uint32_t)(counter_auth - counter_auth1);  //us
//        epit_counter_disable(epit_instance); //polling mode
//        counter_auth = (0xFFFFFFFF - time_counter) / 1000;  //ms
#endif    
        /*
         * Search for HAB_FAILURE event in log buffer.
         */
//        check_pass_fail();

        /* Add authentication status log entry */
/*        pu_irom_log_and_data_add(ROM_LOG_AUTHENTICATION_STATUS,
                                 (UINT32)((pu_irom_error_status.reserved<<24) |
                                          (pu_irom_error_status.context <<16) |
                                          (pu_irom_error_status.reason<<8) |
                                          (pu_irom_error_status.status)));
*/        
        /* Disable MMU if enabled */
//        if(ROM_OCOTP_MMU_DISABLE_FUSE_VALUE() != 1)
//        {
            rom_boot_mmu_disable();
//        }
        
        /* If authentication passes the function will return non-NUL
         * pointer to entry function
         */
        if (hab_image_entry != NULL)
        {
            /*
             * Adjust data pointer to the headers new location
             */
            data_ptr = (hab_ivt_t *)((uint32_t)driver_data->initial_image_address + driver_data->ivt_offset);

            /* Check if the image is plugin or boot image */
//            boot_data = (BOOT_DATA_T*)data_ptr->boot_data;
//            if (PLUGIN_IMAGE(boot_data)) {
//                hab_image_entry = pu_irom_run_plugin((plugin_download_f)hab_image_entry);
//            }
        }
        else
        {
            /* entry is NULL, try secondary image if available */
//            GoSecondaryBoot();
        }
    }
                
    return hab_image_entry;
}


/*!
 * hab bench marking.
 *
 * @return  0
 */
int32_t rom_hab_benchmarking(void)
{
    hab_rvt_t * hab_api_table = (hab_rvt_t *)0x00000094;

    uint32_t * rom_version = (uint32_t *) ROM_VERSION_ADDR;
#if PLATFORM_INIT == 1
    uint32_t starttime, endtime; 
#else
    uint32_t time_counter;
//    uint32_t epit_instance = HW_EPIT2, epit_freq = 0;
    uint32_t epit_freq = 0;
#endif        
         
    volatile HAPI_CLOCK_SRC_SBMR_SHADOW_REG_T * hapi_src_reg; 
    pu_irom_setup_boot_selection_f pu_irom_setup_boot_selection;
    download_image_f download_image;
    hab_image_entry_f hab_image_entry = NULL;
    hab_status_t hab_status = HAB_SUCCESS;

    if (*rom_version == ROM_VERSION_1_1)
    {
        hapi_src_reg = (volatile HAPI_CLOCK_SRC_SBMR_SHADOW_REG_T * )ROM_1_1_shadow_src_addr;
        pu_irom_setup_boot_selection = (pu_irom_setup_boot_selection_f) ROM_1_1_setup_boot_selection_addr;
        download_image = (download_image_f)ROM_1_1_download_image_addr;

        download_initial_image = (download_initial_image_f)ROM_1_1_download_initial_image_addr;
        irom_hwcnfg_setup = (my_pu_irom_hwcnfg_setup_f)ROM_1_1_hwcnfg_setup_addr;
        rom_boot_mmu_disable = (pu_irom_boot_mmu_disable_f)ROM_1_1_boot_mmu_disable_addr;
    }
    else if ( *rom_version == ROM_VERSION_1_2)
//    else if(( *rom_version == ROM_VERSION_1_2) ||(*rom_version == ROM_VERSION_1_1))
    {
        hapi_src_reg = (volatile HAPI_CLOCK_SRC_SBMR_SHADOW_REG_T * )ROM_1_2_shadow_src_addr; 
        pu_irom_setup_boot_selection = (pu_irom_setup_boot_selection_f) ROM_1_2_setup_boot_selection_addr;
        download_image = (download_image_f)ROM_1_2_download_image_addr;

        download_initial_image = (download_initial_image_f)ROM_1_2_download_initial_image_addr;        
        irom_hwcnfg_setup = (my_pu_irom_hwcnfg_setup_f)ROM_1_2_hwcnfg_setup_addr;
        rom_boot_mmu_disable = (pu_irom_boot_mmu_disable_f)ROM_1_2_boot_mmu_disable_addr;
    }
    else
    {
        hapi_src_reg = (volatile HAPI_CLOCK_SRC_SBMR_SHADOW_REG_T * )ROM_1_3_shadow_src_addr; 
        pu_irom_setup_boot_selection = (pu_irom_setup_boot_selection_f) ROM_1_3_setup_boot_selection_addr;
        download_image = (download_image_f)ROM_1_3_download_image_addr;

        download_initial_image = (download_initial_image_f)ROM_1_3_download_initial_image_addr;        
        irom_hwcnfg_setup = (my_pu_irom_hwcnfg_setup_f)ROM_1_3_hwcnfg_setup_addr;
        rom_boot_mmu_disable = (pu_irom_boot_mmu_disable_f)ROM_1_3_boot_mmu_disable_addr;
    }

    /* Change boot mode to sd2 for Arik */
    hapi_src_reg->src_sbmr1 &= ~(ROM_BOOT_DEVICE_MASK);
    hapi_src_reg->src_sbmr1 |= ROM_BOOT_DEVICE_SD;
    
#if PLATFORM_INIT == 1
    starttime = (uint32_t)time_get_microseconds();
#else              
    epit_freq = get_main_clock(IPG_CLK);
    epit_init(epit_instance, CLKSRC_IPG_CLK, epit_freq / 1000000,
              SET_AND_FORGET, 10000, WAIT_MODE_EN | STOP_MODE_EN);
    epit_setup_interrupt(epit_instance, NULL, FALSE);
    epit_counter_enable(epit_instance, 0xFFFFFFFF, 0);   //polling mode
#endif
    /* Call hab entry here before going into serial downloader or boot device mode */
    hab_status = hab_api_table->entry();
    pu_irom_setup_boot_selection();
    hab_image_entry = my_download_image(PRIMARY_DEVICE);

    if (hab_image_entry != NULL)
    {
        hab_status = hab_api_table->exit();
#if PLATFORM_INIT == 1
        endtime =(uint32_t) time_get_microseconds();

        endtime_init = (uint32_t) (endtime_init - starttime_init);
        endtime_auth = (uint32_t) (endtime_auth - starttime_auth);        
        endtime_hwcnfg = (uint32_t) (endtime_hwcnfg - starttime_hwcnfg);
        endtime = (uint32_t) (endtime - starttime);
        printf("Init = %d us\nAuthenticate = %d us\nhwcnfg = %d us\n\nTotal = %d us\n", endtime_init, endtime_auth, endtime_hwcnfg, endtime);

//        printf("Time - download_initial_image = %d us\n", endtime_init);
//        printf("Time - authenticate_image = %d us\n", endtime_auth);
//        printf("Time - pu_irom_hwcnfg_setup = %d us\n", endtime_hwcnfg);
//        printf("Total Image loading time = %d us\n", endtime);
#else      
        time_counter = epit_get_counter_value(epit_instance);
        epit_counter_disable(epit_instance); //polling mode
        time_counter = (uint32_t)(0xFFFFFFFF - time_counter);  //us

        printf("Initial = %d us\nAuth = %d us\nhwcnfg = %d us\nTotal time = %d us\n", counter_init, counter_auth, counter_hwcnfg, time_counter);
#endif    

        /* Jump to the image */
        hab_image_entry(); 
    }    
            
    return 0;
}


void main(void)
{
#if PLATFORM_INIT == 1
    platform_init();
#else    	
    //Enalbe interrupt
    gic_init();

    //Initialize clock source, dividers,...
    ccm_init();

    //Configure the EPIT timer used for system delay function 
    system_time_init();

    //populate the freq number of referenced hw_module
    freq_populate(); 

     uart_init(g_debug_uart_port, 115200, PARITY_NONE, STOPBITS_ONE, EIGHTBITS, FLOWCTRL_OFF);
     //flush uart RX fifo
     uint8_t c;
     do {
         c = uart_getchar(g_debug_uart_port);
     } while (c!=NONE_CHAR);
#endif
    
    // Run the blob generator
    rom_hab_benchmarking();
}
