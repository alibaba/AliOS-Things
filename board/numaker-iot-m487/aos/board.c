#include <aos/kernel.h>
#include <aos/aos.h>
#include <hal/wifi.h>

#include "hal/soc/soc.h"
#include "board.h"

/* User-defined */
/* UART */
struct serial_s board_uart[] = {
#if 1
	{ /* UART PORT 0, STDIO UART, Tx/Rx over Nu-Link's VCOM */
		.uart 		= UART_0,
		.pin_tx		= STDIO_UART_TX,
		.pin_rx		= STDIO_UART_RX,
		.pin_rts	= NC,
		.pin_cts	= NC
	},
#endif	
	{ /* UART PORT 1, UART TX/RX in UDO IF */
		.uart 		= UART_1,
		.pin_tx		= PB_3,
		.pin_rx		= PB_2,
		.pin_rts	= NC,
		.pin_cts	= NC
	},
};
const int i32BoardMaxUartNum = sizeof( board_uart ) / sizeof( board_uart[0] );

/* I2C */
struct i2c_s board_i2c[] = {
	{ /* I2C PORT 0 */
		.i2c		= I2C_0,
		.pin_scl	= I2C_SCL,
		.pin_sda	= I2C_SDA
	},
	{ /* I2C PORT 1 */
		.i2c		= I2C_1,
		.pin_scl	= PB_1,
		.pin_sda	= PB_0
	},
};
const int i32BoardMaxI2CNum = sizeof( board_i2c ) / sizeof( board_i2c[0] );

/* Analog-In */
struct analogin_s board_analogin [] = 
{
	/* Analog-In port */
	{ .adc = ADC_0_6,	.pin = A0	},
	{ .adc = ADC_0_7,	.pin = A1	},
	{ .adc = ADC_0_8,	.pin = A2	},
	{ .adc = ADC_0_9,	.pin = A3	},
	{ .adc = ADC_0_0,	.pin = A4	},
	{ .adc = ADC_0_1,	.pin = A5	},
};
const int i32BoardMaxADCNum  = sizeof( board_analogin ) / sizeof( board_analogin[0] );

/* Analog-Out */
struct analogout_s board_analogout [] = 
{
	/* Analog-In port */
	{ .dac = DAC_0,	.pin = PB_12	},
	{ .dac = DAC_1,	.pin = PB_13	},
};
const int i32BoardMaxDACNum  = sizeof( board_analogout ) / sizeof( board_analogout[0] );

/* GPIO */
struct gpio_s board_gpio [] = 
{
	{ .pin = D0 },
	{ .pin = D1 },
	{ .pin = D2 },
	{ .pin = D3 },
	{ .pin = D4 },
	{ .pin = D5 },
	{ .pin = D6 },
	{ .pin = D7 },
	{ .pin = D8 },
	{ .pin = D9 },
	{ .pin = D10 },
	{ .pin = D11 },
	{ .pin = D12 },
	{ .pin = D13 },
	{ .pin = D14 },
	{ .pin = D15 },	
	{ .pin = SW2 },	
	{ .pin = SW3 },	
	{ .pin = LED_RED },	
	{ .pin = LED_YELLOW },
	{ .pin = LED_GREEN },
};
const int i32BoardMaxGPIONum  = sizeof( board_gpio ) / sizeof( board_gpio[0] );

/* PWM port-M & channel-N */
struct pwmout_s board_pwm [] = 
{
	{	.pwm = PWM_0_0,  .pin = EPWM0_CH0	},
	{	.pwm = PWM_0_1,  .pin = EPWM0_CH1	},
	{	.pwm = PWM_0_2,  .pin = EPWM0_CH2	},
	{	.pwm = PWM_0_3,  .pin = EPWM0_CH3	},
	{	.pwm = PWM_0_4,  .pin = EPWM0_CH4	},
	{	.pwm = PWM_0_5,  .pin = EPWM0_CH5	},

	{	.pwm = PWM_1_0,  .pin = EPWM1_CH0	},
	{	.pwm = PWM_1_1,  .pin = EPWM1_CH1	},
	{	.pwm = PWM_1_2,  .pin = EPWM1_CH2	},
	{	.pwm = PWM_1_3,  .pin = EPWM1_CH3	},

	{	.pwm = PWM_0_3,  .pin = D6	}, //The same with EPWM0_CH3.
	{	.pwm = PWM_0_2,  .pin = D10	}, //The same with EPWM0_CH2.
};
const int i32BoardMaxPWMNum  = sizeof( board_pwm ) / sizeof( board_pwm[0] );

/* SPI */
struct spi_s board_spi [] = 
{
	{	/* SPI0 */
		.spi 		= SPI_0,
		.pin_mosi 	= PA_0,
		.pin_miso	= PA_1,
		.pin_sclk	= PA_2,
		.pin_ssel	= PA_3,
	},
	{	/* SPI2 */
		.spi 		= SPI_2,
		.pin_mosi 	= PA_8,
		.pin_miso	= PA_9,
		.pin_sclk	= PA_10,
		.pin_ssel	= PA_11,
	},
};
const int i32BoardMaxSPINum  = sizeof( board_spi ) / sizeof( board_spi[0] );


/* QSPI */
struct qspi_s board_qspi [] = 
{
	{	
		.qspi 		= QSPI_0,
		.pin_d0 	= PA_0,		//QSPI0-MOSI0
		.pin_d1		= PA_1,		//QSPI0-MISO0
		.pin_d2		= PA_4,		//QSPI0-MOSI1
		.pin_d3		= PA_5,		//QSPI0-MISO1
		.pin_sclk	= PA_2,
		.pin_ssel	= PA_3,
	},
};
const int i32BoardMaxQSPINum  = sizeof( board_qspi ) / sizeof( board_qspi[0] );

/* SD */
struct sdh_s board_sdh [] = 
{
	{	
		.sdh	 		= SDH_0,
		.pin_dat0 = PE_2,
		.pin_dat1	= PE_3,
		.pin_dat2	= PB_4,
		.pin_dat3	= PB_5,
		.pin_cmd	= PE_7,
		.pin_clk	= PE_6,
		.pin_cdn	= PD_13,
	},
};
const int i32BoardMaxSDHNum  = sizeof( board_sdh ) / sizeof( board_sdh[0] );

// FIXME
gpio_dev_t board_gpio_table[] = 
{
    {0,  OUTPUT_PUSH_PULL, NULL},//0
    {1,  OUTPUT_PUSH_PULL, NULL},      
    {2,  OUTPUT_PUSH_PULL, NULL},     
    {3,  OUTPUT_PUSH_PULL, NULL},     
    {4,  OUTPUT_PUSH_PULL, NULL},       
    {5,  OUTPUT_PUSH_PULL, NULL},       
    {6,  OUTPUT_PUSH_PULL, NULL},        
    {7,  OUTPUT_PUSH_PULL, NULL},       
    {8,  OUTPUT_PUSH_PULL, NULL},        
    {9,  OUTPUT_PUSH_PULL, NULL},//9          
    {10, OUTPUT_PUSH_PULL, NULL},          
    {11, OUTPUT_PUSH_PULL, NULL},            
    {12, OUTPUT_PUSH_PULL, NULL},      
    {13, OUTPUT_PUSH_PULL, NULL},
    {14, OUTPUT_PUSH_PULL, NULL},
    {15, OUTPUT_PUSH_PULL, NULL},
    {16, IRQ_MODE, 				 NULL},//16
    {17, IRQ_MODE, 				 NULL},
    {18, OUTPUT_PUSH_PULL, NULL},
    {19, OUTPUT_PUSH_PULL, NULL},
    {20, OUTPUT_PUSH_PULL, NULL},
};

/* Logic partition on flash devices */
#if defined(USE_OTA_SPIM_FLASH)
	#define DEF_APPLICATION_START_ADDR	FMC_APROM_BASE
	#define DEF_APPLICATION_SIZE				(0x80000-FMC_FLASH_PAGE_SIZE*2-FMC_FLASH_PAGE_SIZE*2-FMC_FLASH_PAGE_SIZE*4)	// 480k bytes
	#define DEF_PARAMETER_1_START_ADDR	(DEF_APPLICATION_START_ADDR+DEF_APPLICATION_SIZE)
	#define DEF_PARAMETER_1_SIZE				(FMC_FLASH_PAGE_SIZE*2)		//8k bytes
	#define DEF_PARAMETER_2_START_ADDR	(DEF_PARAMETER_1_START_ADDR+DEF_PARAMETER_1_SIZE)
	#define DEF_PARAMETER_2_SIZE				(FMC_FLASH_PAGE_SIZE*2)		//8k bytes
#else
	#define DEF_APPLICATION_START_ADDR	FMC_APROM_BASE
	#define DEF_APPLICATION_SIZE				(0x3E000)			//248k bytes
	#define DEF_PARAMETER_1_START_ADDR	(DEF_APPLICATION_START_ADDR+DEF_APPLICATION_SIZE)
	#define DEF_PARAMETER_1_SIZE				(FMC_FLASH_PAGE_SIZE*2)				//8k bytes
	#define DEF_OTATEMP_START_ADDR			(DEF_PARAMETER_1_START_ADDR+DEF_PARAMETER_1_SIZE)
	#define DEF_OTATEMP_SIZE						(DEF_APPLICATION_SIZE)
	#define DEF_PARAMETER_2_START_ADDR	(DEF_OTATEMP_START_ADDR+DEF_OTATEMP_SIZE)
	#define DEF_PARAMETER_2_SIZE				(FMC_FLASH_PAGE_SIZE*2)		//8k bytes
#endif

#if 0
#define DEF_PARAMETER_3_START_ADDR	(DEF_PARAMETER_2_START_ADDR+DEF_PARAMETER_2_SIZE)
#define DEF_PARAMETER_3_SIZE				(FMC_FLASH_PAGE_SIZE*2)		//8k bytes
#define DEF_PARAMETER_4_START_ADDR	(DEF_PARAMETER_3_START_ADDR+DEF_PARAMETER_3_SIZE)
#define DEF_PARAMETER_4_SIZE				(FMC_FLASH_PAGE_SIZE*2)		//8k bytes
#define DEF_CUSTOM_1_START_ADDR			(DEF_PARAMETER_4_START_ADDR+DEF_PARAMETER_4_SIZE)
#define DEF_CUSTOM_1_SIZE						(FMC_FLASH_PAGE_SIZE*16) 	//64k bytes
#define DEF_CUSTOM_2_START_ADDR			(DEF_CUSTOM_1_START_ADDR+DEF_CUSTOM_1_SIZE)
#define DEF_CUSTOM_2_SIZE						(FMC_FLASH_PAGE_SIZE*16) 	//64k bytes
#endif
const hal_logic_partition_t hal_partitions[HAL_PARTITION_MAX] = {
	
  	[HAL_PARTITION_APPLICATION] = {
		.partition_owner          	= HAL_FLASH_EMBEDDED,
		.partition_description      = "Application",
		.partition_start_addr       = DEF_APPLICATION_START_ADDR,
    	.partition_length           = DEF_APPLICATION_SIZE,
    	.partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
	},
	[HAL_PARTITION_PARAMETER_1] = {
		.partition_owner            = HAL_FLASH_EMBEDDED,
		.partition_description      = "PARAMETER1",
		.partition_start_addr       = DEF_PARAMETER_1_START_ADDR,
		.partition_length           = DEF_PARAMETER_1_SIZE,
    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
	},
#if !defined(USE_OTA_SPIM_FLASH)	
	[HAL_PARTITION_OTA_TEMP] = {
		.partition_owner          	= HAL_FLASH_EMBEDDED,
		.partition_description      = "OTA",
		.partition_start_addr       = DEF_OTATEMP_START_ADDR,
    	.partition_length           = DEF_OTATEMP_SIZE,
    	.partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
	},
#endif
	[HAL_PARTITION_PARAMETER_2] = {
		.partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "PARAMETER2",
	    .partition_start_addr       = DEF_PARAMETER_2_START_ADDR,
	    .partition_length           = DEF_PARAMETER_2_SIZE,
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
	},
#if 0	
	[HAL_PARTITION_PARAMETER_3]	=	{
		.partition_owner            = HAL_FLASH_EMBEDDED,
    .partition_description      = "PARAMETER3",
    .partition_start_addr       = DEF_PARAMETER_3_START_ADDR,
    .partition_length           = DEF_PARAMETER_3_SIZE,
    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
	},

  [HAL_PARTITION_PARAMETER_4] = {
		.partition_owner            = HAL_FLASH_EMBEDDED,
    .partition_description      = "PARAMETER4",
    .partition_start_addr       = DEF_PARAMETER_4_START_ADDR,
    .partition_length           = DEF_PARAMETER_4_SIZE,
    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
	},
	
  [HAL_PARTITION_CUSTOM_1]		= {
		.partition_owner            = HAL_FLASH_EMBEDDED,
    .partition_description      = "CUSTOM1",
    .partition_start_addr       = DEF_CUSTOM_1_START_ADDR,
    .partition_length           = DEF_CUSTOM_1_SIZE,
    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
	
  [HAL_PARTITION_CUSTOM_2]		= {
		.partition_owner            = HAL_FLASH_EMBEDDED,
    .partition_description      = "CUSTOM2",
    .partition_start_addr       = DEF_CUSTOM_2_START_ADDR,
    .partition_length           = DEF_CUSTOM_2_SIZE,
    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
	}
#endif
};

#if defined(WITH_SAL) || defined(WITH_LWIP)
extern struct hal_ota_module_s numicro_ota_module;
#endif

void board_init(void)
{
    board_cli_init();

#if defined(DEV_SAL_MK3060)
	extern hal_wifi_module_t aos_wifi_module_mk3060;
	hal_wifi_register_module(&aos_wifi_module_mk3060);	
	hal_wifi_init();
#endif

#if defined(WITH_LWIP)    
    extern hal_wifi_module_t numicro_eth_m487;
    extern int lwip_tcpip_init(void);
    hal_wifi_register_module(&numicro_eth_m487);
    lwip_tcpip_init();
#endif

#if defined(WITH_SAL) || defined(WITH_LWIP)
    hal_ota_register_module(&numicro_ota_module);
#endif
}
