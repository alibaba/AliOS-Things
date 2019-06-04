#include "include.h"
#include "arm_arch.h"

#include "dd_pub.h"

#include "sdio_pub.h"
#include "sys_ctrl_pub.h"
#include "uart_pub.h"
#include "gpio_pub.h"
#include "icu_pub.h"
#include "wdt_pub.h"
#include "usb_pub.h"
#include "pwm_pub.h"
#include "flash_pub.h"
#include "spi_pub.h"
#include "fft_pub.h"
#include "i2s_pub.h"
#include "saradc_pub.h"
#include "irda_pub.h"
#include "mac_phy_bypass_pub.h"
#include "bk_timer_pub.h"

#if CFG_USE_CAMERA_INTF
#include "jpeg_encoder_pub.h"
#include "i2c_pub.h"
#endif

#if CFG_USE_AUDIO
#include "audio_pub.h"
#endif

#if CFG_USE_SPIDMA
#include "spidma_pub.h"
#endif

#if CFG_GENERAL_DMA
#include "general_dma_pub.h"
#endif

#if CFG_USE_SDCARD_HOST
#include "sdcard_pub.h"
#endif

#if CFG_USE_STA_PS
#include "power_save_pub.h"
#endif

#if (CFG_SOC_NAME == SOC_BK7221U)
#include "security_pub.h"
#endif

static DD_INIT_S dd_init_tbl[] =
{
    /* name*/              /* init function*/          /* exit function*/
    {SCTRL_DEV_NAME,        sctrl_init,                 sctrl_exit},
    {ICU_DEV_NAME,          icu_init,                   icu_exit},
    {WDT_DEV_NAME,          wdt_init,                   wdt_exit},
    {GPIO_DEV_NAME,         gpio_init,                  gpio_exit},
    
#ifndef KEIL_SIMULATOR    
    {UART2_DEV_NAME,        uart2_init,                 uart2_exit},
#endif    

#if CFG_USE_UART1
    {UART1_DEV_NAME,        uart1_init,                 uart1_exit},
#endif

    {FLASH_DEV_NAME,        flash_init,                 flash_exit},
    
#if CFG_GENERAL_DMA
    {GDMA_DEV_NAME,         gdma_init,                  gdma_exit},
#endif

#if CFG_USE_HSLAVE_SPI
    {SPIDMA_DEV_NAME,       spidma_init,                spidma_uninit},
#endif

#if CFG_USE_CAMERA_INTF
    {EJPEG_DEV_NAME,        ejpeg_init,                 ejpeg_exit},
    {I2C1_DEV_NAME,         i2c1_init,                  i2c1_exit},        
    {I2C2_DEV_NAME,         i2c2_init,                  i2c2_exit},            
#endif

#if CFG_USE_AUDIO
    {AUD_DAC_DEV_NAME,      audio_init,                 audio_exit},        
#endif

#if CFG_SDIO || CFG_SDIO_TRANS
    {SDIO_DEV_NAME,         sdio_init,                  sdio_exit},
#endif

#if CFG_USB
    {USB_DEV_NAME,          usb_init,                   usb_exit},
#endif

#if (CFG_SOC_NAME == SOC_BK7221U)
    {USB_PLUG_DEV_NAME,     usb_plug_inout_init,        usb_plug_inout_exit},
#endif

    {PWM_DEV_NAME,          pwm_init,                   pwm_exit},
#if (CFG_SOC_NAME != SOC_BK7231)
    {TIMER_DEV_NAME,        bk_timer_init,              bk_timer_exit},
#endif
    {SPI_DEV_NAME,          spi_init,                   spi_exit},

    {FFT_DEV_NAME,          fft_init,                   fft_exit},
    {I2S_DEV_NAME,          i2s_init,                   i2s_exit},
    {SARADC_DEV_NAME,       saradc_init,                saradc_exit},
    {IRDA_DEV_NAME,         irda_init,                  irda_exit},

#if CFG_MAC_PHY_BAPASS
    {MPB_DEV_NAME,          mpb_init,                   mpb_exit},
#endif

#if CFG_USE_SDCARD_HOST
    {SDCARD_DEV_NAME,       sdcard_init,                sdcard_exit},
#endif

#if CFG_USE_STA_PS
    {"power_save",       sctrl_sta_ps_init,                NULLPTR},
#endif

#if (CFG_SOC_NAME == SOC_BK7221U)
	{SEC_DEV_NAME,			bk_secrity_init,					bk_secrity_exit},
#endif
    {NULL,                  NULLPTR,                    NULLPTR}
};

void g_dd_init(void)
{
    UINT32 i;
    UINT32 tbl_count;
    DD_INIT_S *dd_element;

    tbl_count = sizeof(dd_init_tbl) / sizeof(DD_INIT_S);
    for(i = 0; i < tbl_count; i ++)
    {
        dd_element = &dd_init_tbl[i];
        if(dd_element->dev_name && dd_element->init)
        {
            (dd_element->init)();
        }
        else
        {
            return;
        }
    }
}

void g_dd_exit(void)
{
    UINT32 i;
    UINT32 tbl_count;
    DD_INIT_S *dd_element;

    tbl_count = sizeof(dd_init_tbl) / sizeof(DD_INIT_S);
    for(i = 0; i < tbl_count; i ++)
    {
        dd_element = &dd_init_tbl[i];
        if(dd_element->dev_name && dd_element->exit)
        {
            (dd_element->exit)();
        }
        else
        {
            return;
        }
    }
}

// EOF
