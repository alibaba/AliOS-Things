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
		.pin_tx		= PB_5,
		.pin_rx		= PB_4,
		.pin_rts	= NC,
		.pin_cts	= NC
	},
};
const int i32BoardMaxUartNum = sizeof( board_uart ) / sizeof( board_uart[0] );

/* I2C */
struct i2c_s board_i2c[] = {
	{ /* I2C PORT 0 */
		.i2c		= I2C_1,
		.pin_scl	= I2C_SCL,
		.pin_sda	= I2C_SDA
	},
	{ /* I2C PORT 1 */
		.i2c		= I2C_0,
		.pin_scl	= PA_5,
		.pin_sda	= PA_4
	},
};
const int i32BoardMaxI2CNum = sizeof( board_i2c ) / sizeof( board_i2c[0] );

/* Analog-In */
struct analogin_s board_analogin [] = 
{
	/* Analog-In port */
	{ .adc = ADC_0_0,	.pin = A0	},
	{ .adc = ADC_0_1,	.pin = A1	},
	{ .adc = ADC_0_2,	.pin = A2	},
	{ .adc = ADC_0_3,	.pin = A3	},
	{ .adc = ADC_0_4,	.pin = A4	},
	{ .adc = ADC_0_5,	.pin = A5	},
};
const int i32BoardMaxADCNum  = sizeof( board_analogin ) / sizeof( board_analogin[0] );

/* Analog-Out */
struct analogout_s board_analogout [] = 
{
	/* Analog-In port */
	{ .dac = DAC_0,	.pin = PC_6	},
	{ .dac = DAC_1,	.pin = PC_7	},
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
	{ .pin = SW1 },	
	{ .pin = SW2 },	
	{ .pin = LED_RED },	
	{ .pin = LED_YELLOW },
	{ .pin = LED_GREEN },
};
const int i32BoardMaxGPIONum  = sizeof( board_gpio ) / sizeof( board_gpio[0] );

/* PWM port-M & channel-N */
struct pwmout_s board_pwm [] = 
{
	{	.pwm = PWM_0_0,  .pin = PWM0_CH0	},
	{	.pwm = PWM_0_1,  .pin = PWM0_CH1	},
	{	.pwm = PWM_0_2,  .pin = PWM0_CH2	},
	{	.pwm = PWM_0_3,  .pin = PWM0_CH3	},

	{	.pwm = PWM_1_0,  .pin = PWM1_CH0	},
	{	.pwm = PWM_1_1,  .pin = PWM1_CH1	},
	{	.pwm = PWM_1_2,  .pin = PWM1_CH2	},
	{	.pwm = PWM_1_3,  .pin = PWM1_CH3	},
};
const int i32BoardMaxPWMNum  = sizeof( board_pwm ) / sizeof( board_pwm[0] );

/* SPI */
struct spi_s board_spi [] = 
{
	{	/* SPI0 */
		.spi 		= SPI_0,
		.pin_mosi 	= PC_3,
		.pin_miso	= PC_2,
		.pin_sclk	= PC_1,
		.pin_ssel	= PC_0,
	}
};
const int i32BoardMaxSPINum  = sizeof( board_spi ) / sizeof( board_spi[0] );

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
#define DEF_APPLICATION_START_ADDR	FMC_APROM_BASE
#define DEF_APPLICATION_SIZE		(0x10000)					//64k bytes
#define DEF_PARAMETER_1_START_ADDR	(DEF_APPLICATION_START_ADDR+DEF_APPLICATION_SIZE)
#define DEF_PARAMETER_1_SIZE		(FMC_FLASH_PAGE_SIZE*8)			//4k bytes
#define DEF_PARAMETER_2_START_ADDR	(DEF_PARAMETER_1_START_ADDR+DEF_PARAMETER_1_SIZE)
#define DEF_PARAMETER_2_SIZE		(FMC_FLASH_PAGE_SIZE*16)		//8k bytes
#define DEF_PARAMETER_3_START_ADDR	(DEF_PARAMETER_2_START_ADDR+DEF_PARAMETER_2_SIZE)
#define DEF_PARAMETER_3_SIZE		(FMC_FLASH_PAGE_SIZE*16)		//8k bytes
#define DEF_PARAMETER_4_START_ADDR	(DEF_PARAMETER_3_START_ADDR+DEF_PARAMETER_3_SIZE)
#define DEF_PARAMETER_4_SIZE		(FMC_FLASH_PAGE_SIZE*16)		//8k bytes
#define DEF_CUSTOM_1_START_ADDR		(DEF_PARAMETER_4_START_ADDR+DEF_PARAMETER_4_SIZE)
#define DEF_CUSTOM_1_SIZE			(FMC_FLASH_PAGE_SIZE*16) 	//8k bytes
#define DEF_CUSTOM_2_START_ADDR		(DEF_CUSTOM_1_START_ADDR+DEF_CUSTOM_1_SIZE)
#define DEF_CUSTOM_2_SIZE			(FMC_FLASH_PAGE_SIZE*16) 	//8k bytes

const hal_logic_partition_t hal_partitions[HAL_PARTITION_MAX] = {
	
  [HAL_PARTITION_APPLICATION] = {
		.partition_owner		= HAL_FLASH_EMBEDDED,
		.partition_description	= "Application",
		.partition_start_addr	= DEF_APPLICATION_START_ADDR,
		.partition_length		= DEF_APPLICATION_SIZE,
		.partition_options		= PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
	},

	[HAL_PARTITION_PARAMETER_1] = {
		.partition_owner    	= HAL_FLASH_EMBEDDED,
		.partition_description      = "PARAMETER1",
		.partition_start_addr       = DEF_PARAMETER_1_START_ADDR,
		.partition_length           = DEF_PARAMETER_1_SIZE,
		.partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
	},

  [HAL_PARTITION_PARAMETER_2] = {
		.partition_owner            = HAL_FLASH_EMBEDDED,
    .partition_description      = "PARAMETER2",
    .partition_start_addr       = DEF_PARAMETER_2_START_ADDR,
    .partition_length           = DEF_PARAMETER_2_SIZE,
    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN
	},
	
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
};

void board_init(void)
{
 //   board_cli_init();
}
