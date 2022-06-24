#define MICROPY_HW_BOARD_NAME       "01Studio Columbus"
#define MICROPY_HW_MCU_NAME         "STM32F407ZGT6"
#define	MICROPY_HW_FLASH_FS_LABEL   "COLUMBUS"
#define MICROPY_SW_VENDOR_NAME      "HaaSPython"

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)
#define MICROPY_HW_HAS_SWITCH       	(1)
#define MICROPY_HW_HAS_FLASH        	(1)
#define MICROPY_HW_ENABLE_RNG       	(1)
#define MICROPY_HW_ENABLE_RTC       	(1)
#define MICROPY_HW_ENABLE_DAC       	(1)
#define MICROPY_HW_ENABLE_ADC			(1)
#define MICROPY_HW_ENABLE_USB       	(1)
#define	MICROPY_HW_ENABLE_SERVO			(1) 

#define MICROPY_ENABLE_SDCARD_NIRQ		(1)
//===================================================================================
//01studio
//------------------------------------------------------------------------------------
#define MICROPY_HW_BOARD_COLUMBUS 		(1) //哥伦布
#define MICROPY_PY_PICLIB				(1) //图片解码
#define MICROPY_ENABLE_NEOPIXEL			(0)

//gui
#define MICROPY_ENABLE_GUI				(1) 	//GUI支持
#define MICROPY_GUI_BUTTON				(1) 	//按钮控件
#define GUI_BTN_NUM_MAX					(30) 	//最大支持按钮数量
#define GUI_BTN_STR_LEN					(20)

//touch
#define	MICROPY_ENABLE_TOUCH			(1)   //enable mode
#define	MICROPY_HW_GT1151				(1)   //使用触摸芯片型号

//------------------------------------------------------------------------------------
//TFTLCD
#define MICROPY_ENABLE_TFTLCD			(1)
#define	MICROPY_HW_LCD43M				(1)		//液晶屏模块

//------------------------------------------------------------------------------------
//audio
#define MICROPY_ENABLE_AUDIO            (1)
#define MICROPY_HW_WM8978               (1)
#define MICROPY_ENABLE_MP3              (0)
//------------------------------------------------------------------------------------
//video
#define MICROPY_ENABLE_VIDEO			(1)
//------------------------------------------------------------------------------------
//sensor
#define MICROPY_ENABLE_SENSOR			(1)
#define MICROPY_HW_OV2640				(0)
//===================================================================================

#if MICROPY_HW_OV2640
#define MICROPY_HW_DCMI_RESE			(pin_G15)
#define MICROPY_HW_DCMI_PWDN			(pin_G9)
#define MICROPY_HW_DCMI_HSYNC			(pin_A4)
#define MICROPY_HW_DCMI_PIXCK			(pin_A6)
#define MICROPY_HW_DCMI_VSYNC			(pin_B7)
#define MICROPY_HW_DCMI_D0       		(pin_C6)
#define MICROPY_HW_DCMI_D1       		(pin_C7)
#define MICROPY_HW_DCMI_D2       		(pin_C8)
#define MICROPY_HW_DCMI_D3       		(pin_C9)
#define MICROPY_HW_DCMI_D4       		(pin_C11)
#define MICROPY_HW_DCMI_D5       		(pin_B6)
#define MICROPY_HW_DCMI_D6       		(pin_E5)
#define MICROPY_HW_DCMI_D7       		(pin_E6)
#endif
//===================================================================================
#define MICROPY_PY_THREAD_GIL			(1)
#define MICROPY_PY_THREAD				(1)

#define MICROPY_HW_CLK_USE_HSE

// HSE is 12MHz
#define MICROPY_HW_CLK_PLLM (12)
#define MICROPY_HW_CLK_PLLN (336)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (7)
#define MICROPY_12HW_CLK_LAST_FREQ 		(1)

// The pyboard has a 32kHz crystal for the RTC
#define MICROPY_HW_RTC_USE_LSE      	(1)
#define MICROPY_HW_RTC_USE_US       	(0)
#define MICROPY_HW_RTC_USE_CALOUT   	(1)

#define MICROPY_HW_UART_REPL        PYB_UART_3
#define MICROPY_HW_UART_REPL_BAUD   115200

#define MICROPY_HW_UART1_NAME   "UART1"    // on RX / TX
#define MICROPY_HW_UART1_TX     (pin_B6)
#define MICROPY_HW_UART1_RX     (pin_B7)

#define MICROPY_HW_UART2_NAME   "UART2"    // on RX / TX
#define MICROPY_HW_UART2_TX     (pin_A2)
#define MICROPY_HW_UART2_RX     (pin_A3)

#define MICROPY_HW_UART3_NAME   "UART3"    // on RX / TX
#define MICROPY_HW_UART3_TX     (pin_B10)
#define MICROPY_HW_UART3_RX     (pin_B11)

#define MICROPY_HW_UART4_NAME   "UART4"    // on RX / TX
#define MICROPY_HW_UART4_TX     (pin_A0)
#define MICROPY_HW_UART4_RX     (pin_A1)

#define MICROPY_HW_UART6_NAME   "UART6"
#define MICROPY_HW_UART6_TX     (pin_C6)
#define MICROPY_HW_UART6_RX     (pin_C7)

// use external SPI flash for storage
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE	(1)
#define MICROPY_HW_SPIFLASH_SIZE_BITS (128 * 1024 * 1024)
#define MICROPY_HW_SPIFLASH_CS      	(pin_B14)
#define MICROPY_HW_SPIFLASH_SCK     	(pin_B3)
#define MICROPY_HW_SPIFLASH_MOSI    	(pin_B5)
#define MICROPY_HW_SPIFLASH_MISO    	(pin_B4)

#define MICROPY_HW_SPI2_NAME "SPI2"
#define MICROPY_HW_SPI2_NSS  (pin_B12)
#define MICROPY_HW_SPI2_SCK  (pin_B13)
#define MICROPY_HW_SPI2_MISO (pin_C2)
#define MICROPY_HW_SPI2_MOSI (pin_B15)

// block device config for SPI flash
extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;
#define MICROPY_HW_BDEV_IOCTL(op, arg) ( \
    (op) == BDEV_IOCTL_NUM_BLOCKS ? (MICROPY_HW_SPIFLASH_SIZE_BITS / 8 / FLASH_BLOCK_SIZE) : \
    (op) == BDEV_IOCTL_INIT ? spi_bdev_ioctl(&spi_bdev, (op), (uint32_t)&spiflash_config) : \
    spi_bdev_ioctl(&spi_bdev, (op), (arg)) \
)
#define MICROPY_HW_BDEV_READBLOCKS(dest, bl, n) spi_bdev_readblocks(&spi_bdev, (dest), (bl), (n))
#define MICROPY_HW_BDEV_WRITEBLOCKS(src, bl, n) spi_bdev_writeblocks(&spi_bdev, (src), (bl), (n))
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_4

#define MICROPY_HW_I2C1_NAME "I2C1"
#define MICROPY_HW_I2C1_SCL (pin_B8)
#define MICROPY_HW_I2C1_SDA (pin_B9)

#define MICROPY_HW_I2C2_NAME "I2C2"
#define MICROPY_HW_I2C2_SCL (pin_B10)
#define MICROPY_HW_I2C2_SDA (pin_B11)

// CAN busses
#define MICROPY_HW_CAN1_NAME "CAN1"
#define MICROPY_HW_CAN1_TX (pin_B9)		//I2C1占用
#define MICROPY_HW_CAN1_RX (pin_B8) 	//I2C1占用

#define MICROPY_HW_CAN2_NAME "CAN2"
#define MICROPY_HW_CAN2_TX (pin_B13)	//I2S1复用
#define MICROPY_HW_CAN2_RX (pin_B12)	//I2S1复用

// Ethernet via RMII
#define MICROPY_HW_ETH_MDC          (pin_C1)
#define MICROPY_HW_ETH_MDIO         (pin_A2)
#define MICROPY_HW_ETH_RMII_REF_CLK (pin_A1)
#define MICROPY_HW_ETH_RMII_CRS_DV  (pin_A7)
#define MICROPY_HW_ETH_RMII_RXD0    (pin_C4)
#define MICROPY_HW_ETH_RMII_RXD1    (pin_C5)
#define MICROPY_HW_ETH_RMII_RXER    (pin_D3)
#define MICROPY_HW_ETH_RMII_TX_EN   (pin_G11)
#define MICROPY_HW_ETH_RMII_TXD0    (pin_G13)
#define MICROPY_HW_ETH_RMII_TXD1    (pin_G14)

#define MICROPY_PY_NETWORK_LAN8720  (0)

// SD card detect switch
#define MICROPY_HW_ENABLE_SDCARD			(1)
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_C0)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_NOPULL)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)

// LEDs
#define MICROPY_HW_LED1             	(pin_E2) // red
#define MICROPY_HW_LED2             	(pin_F8) // green
#define MICROPY_HW_LED3             	(pin_F9) // orange
#define MICROPY_HW_LED4             	(pin_F10) // blue
#define MICROPY_HW_LED_OFF(pin)      	(mp_hal_pin_low(pin))
#define MICROPY_HW_LED_ON(pin)     		(mp_hal_pin_high(pin))
// USB config
#define MICROPY_HW_USB_FS              (1)
#define MICROPY_HW_USB_VBp	US_DETECT_PIN (pin_A9)
#define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)
// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_E4)
#define MICROPY_HW_USRSW_PULL       (GPIO_PULLUP)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)
//sram
#define MICROPY_HW_SRAM_SIZE  (1 * 1024*1024)  // 1 M
#define MICROPY_HW_SRAM_STARTUP_TEST       (1)
#define MICROPY_HEAP_START  ((sram_valid) ? sram_start() : &_heap_start)
#define MICROPY_HEAP_END    ((sram_valid) ? sram_end() : &_heap_end)

#if MICROPY_HW_SRAM_SIZE
#define MICROPY_HW_FSMC_NBL0     (pin_E0)
#define MICROPY_HW_FSMC_NBL1     (pin_E1)
#define MICROPY_HW_FSMC_NOE      (pin_D4)
#define MICROPY_HW_FSMC_NWE      (pin_D5)
#define MICROPY_HW_FSMC_NE3    	 (pin_G10)
		
#define MICROPY_HW_FSMC_A0       (pin_F0)
#define MICROPY_HW_FSMC_A1       (pin_F1)
#define MICROPY_HW_FSMC_A2       (pin_F2)
#define MICROPY_HW_FSMC_A3       (pin_F3)
#define MICROPY_HW_FSMC_A4       (pin_F4)
#define MICROPY_HW_FSMC_A5       (pin_F5)
#define MICROPY_HW_FSMC_A6       (pin_F12)
#define MICROPY_HW_FSMC_A7       (pin_F13)
#define MICROPY_HW_FSMC_A8       (pin_F14)
#define MICROPY_HW_FSMC_A9       (pin_F15)
#define MICROPY_HW_FSMC_A10      (pin_G0)
#define MICROPY_HW_FSMC_A11      (pin_G1)
#define MICROPY_HW_FSMC_A12      (pin_G2)
#define MICROPY_HW_FSMC_A13      (pin_G3)
#define MICROPY_HW_FSMC_A14      (pin_G4)
#define MICROPY_HW_FSMC_A15      (pin_G5)
#define MICROPY_HW_FSMC_A16      (pin_D11)
#define MICROPY_HW_FSMC_A17      (pin_D12)
#define MICROPY_HW_FSMC_A18      (pin_D13)

#define MICROPY_HW_FSMC_D0       (pin_D14)
#define MICROPY_HW_FSMC_D1       (pin_D15)
#define MICROPY_HW_FSMC_D2       (pin_D0)
#define MICROPY_HW_FSMC_D3       (pin_D1)
#define MICROPY_HW_FSMC_D4       (pin_E7)
#define MICROPY_HW_FSMC_D5       (pin_E8)
#define MICROPY_HW_FSMC_D6       (pin_E9)
#define MICROPY_HW_FSMC_D7       (pin_E10)
#define MICROPY_HW_FSMC_D8       (pin_E11)
#define MICROPY_HW_FSMC_D9       (pin_E12)
#define MICROPY_HW_FSMC_D10      (pin_E13)
#define MICROPY_HW_FSMC_D11      (pin_E14)
#define MICROPY_HW_FSMC_D12      (pin_E15)
#define MICROPY_HW_FSMC_D13      (pin_D8)
#define MICROPY_HW_FSMC_D14      (pin_D9)
#define MICROPY_HW_FSMC_D15      (pin_D10)
#endif

#if MICROPY_HW_LCD43M
//lcd
#define MICROPY_HW_LCD43M_BL    (pin_B15)
#define MICROPY_HW_LCD_NE4    	(pin_G12)
#define MICROPY_HW_LCD_NOE      (pin_D4)
#define MICROPY_HW_LCD_NWE      (pin_D5)
#define MICROPY_HW_MCULCD_CS		MICROPY_HW_LCD_NE4

#define MICROPY_HW_LCD_A0       (pin_F0)

#define MICROPY_HW_LCD_D0       (pin_D14)
#define MICROPY_HW_LCD_D1       (pin_D15)
#define MICROPY_HW_LCD_D2       (pin_D0)
#define MICROPY_HW_LCD_D3       (pin_D1)
#define MICROPY_HW_LCD_D4       (pin_E7)
#define MICROPY_HW_LCD_D5       (pin_E8)
#define MICROPY_HW_LCD_D6       (pin_E9)
#define MICROPY_HW_LCD_D7       (pin_E10)
#define MICROPY_HW_LCD_D8       (pin_E11)
#define MICROPY_HW_LCD_D9       (pin_E12)
#define MICROPY_HW_LCD_D10      (pin_E13)
#define MICROPY_HW_LCD_D11      (pin_E14)
#define MICROPY_HW_LCD_D12      (pin_E15)
#define MICROPY_HW_LCD_D13      (pin_D8)
#define MICROPY_HW_LCD_D14      (pin_D9)
#define MICROPY_HW_LCD_D15      (pin_D10)
#endif

