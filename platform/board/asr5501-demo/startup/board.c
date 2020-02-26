#include <stdio.h>
#include "aos/init.h"
#include "lega_cm4.h"
#include "lega_common.h"
#include "systick_delay.h"
#ifdef ALIOS_SUPPORT
#include <k_config.h>
#include "hw.h"
#include "board.h"
#include "aos/hal/uart.h"
#include "network/hal/wifi.h"
#else
#include "FreeRTOSConfig.h"
#endif
#include "lega_uart.h"
#include "lega_wdg.h"
#include "lega_flash.h"
#include "lega_common.h"
#include "lega_wlan_api.h"
#include "lega_cm4.h"

#define SYS_APP_VERSION_SEG __attribute__((section("app_version_sec")))
SYS_APP_VERSION_SEG const char app_version[] = SYSINFO_APP_VERSION;

extern hal_wifi_module_t sim_aos_wifi_lega;
extern void NVIC_init();
extern int soc_pre_init(void);

#ifdef ALIOS_SUPPORT
extern void ota_roll_back_pro(void);
#endif

#define ALWAYS_ON_REGFILE           0x40000A00
#define REG_AHB_BUS_CTRL            *((volatile uint32_t *)(ALWAYS_ON_REGFILE + 0x90))

#ifdef SYSTEM_RECOVERY
lega_wdg_dev_t lega_wdg;
#endif

/***********************************************************
* init IRQ, set priority and enable IRQ
*
**********************************************************/
void NVIC_init()
{
    //set irq priority, default set configLIBRARY_NORMAL_INTERRUPT_PRIORITY
    NVIC_SetPriority(UART0_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(UART1_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(UART2_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(CEVA_RW_IP_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(D_APLL_UNLOCK_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(D_SX_UNLOCK_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(SLEEP_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(WDG_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(FLASH_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(GPIO_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(TIMER_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(CRYPTOCELL310_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(DMA_CTRL_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(SPI0_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(SPI1_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(SPI2_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(I2C0_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(I2C1_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(SDIO_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(PLF_WAKEUP_IRQn,configLIBRARY_NORMAL_INTERRUPT_PRIORITY);
}

void delay_nop(unsigned int dly)
{
    volatile unsigned int i;
    for(i=dly; i>0; i--)
    {
    }
}

void ahb_sync_brid_open(void)
{
    unsigned int is_using_sync_down = (REG_AHB_BUS_CTRL & (0x1<<1));
    if(!is_using_sync_down)
    {
        REG_AHB_BUS_CTRL |= (0x1<<1); //0x40000A90 bit1 sw_use_hsync

        __enable_irq();
        NVIC_EnableIRQ(24);
        __asm volatile("DSB");
        __asm volatile("WFI");
        __asm volatile("ISB");

        delay_nop(50);
    }
}


/**
  * @general board init entry board_basic_init
  * @retval None
  */
void board_basic_init(void)
{
    ahb_sync_brid_open();
    lega_flash_alg_cache_flush();

#ifdef LEGA_A0V1
    // Clear RFA Only Mode
    REG_PMU_CTRL &= ~ENABLE_RFA_DEBUG;
    REG_WR(SYS_REG_BASE_FLASH_CLK, 0x01); //26MHz flash,default 13MHz
#endif

    NVIC_init();

#ifdef DCDC_PFMMODE_CLOSE
    lega_drv_close_dcdc_pfm();
#endif

    lega_drv_rco_cal();
}

/**
  * @general board tick init entry board_tick_init
  * @retval None
  */
void board_tick_init(void)
{
    SysTick_Config(SYSTEM_CLOCK/RHINO_CONFIG_TICKS_PER_SECOND);
}

uart_dev_t uart_0;
void hal_uart1_callback_handler(char ch);

/**
* @init the default uart init example.
*/
void board_stduart_init(void)
{
    //init uart
    //uart_0.port=LEGA_UART1_INDEX;
    uart_0.port = PORT_UART_STD;  /*logic port*/
    uart_0.config.baud_rate=UART_BAUDRATE_115200;
    uart_0.config.data_width = DATA_8BIT;
    uart_0.config.flow_control = FLOW_CTRL_DISABLED;
    uart_0.config.parity = PARITY_NO;
    uart_0.config.stop_bits = STOP_1BIT;
    //uart_0.priv = (void *)(hal_uart1_callback_handler);
    hal_uart_init(&uart_0);
}

/**
  * @brief WIFI Initialization Function
  * @param None
  * @retval None
  */
void board_wifi_init(void)
{
    printf("start------wifi_hal\r\n");
    hal_wifi_register_module(&sim_aos_wifi_lega);
    hal_wifi_init();
}

/**
  * Enable DMA controller clock
  */
void board_dma_init(void)
{

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void board_gpio_init(void)
{

}

void board_network_init(void)
{
    tcpip_init( NULL, NULL );
    hw_start_hal();
}

/**
  * @brief flash control Initialization Function
  * @param None
  * @retval None
  */
void board_flash_init(void) {
#ifdef _SPI_FLASH_ENABLE_
    lega_flash_init();
#endif
}

/**
  * @brief watchdog Initialization Function
  * @param None
  * @retval None
  */
void board_wdg_init(void) {
#ifdef SYSTEM_RECOVERY
    lega_wdg.port = 0;
    lega_wdg.config.timeout = WDG_TIMEOUT_MS;
    lega_wdg_init(&lega_wdg);
#endif
}

/**
  * @brief ota Initialization Function
  * @param None
  * @retval None
  */
void board_ota_init(void) {
#ifdef ALIOS_SUPPORT
    ota_roll_back_pro();
#endif
}

void board_kinit_init(kinit_t* init_args)
{
    return;
}