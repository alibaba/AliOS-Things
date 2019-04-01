#ifndef __HAL_GPIO_H__
#define __HAL_GPIO_H__

typedef enum
{
    PIN_MODE_GPIO     = 0,       /**< Gpio mode */
    PIN_MODE_ZSTATE   = 1
} pin_mode_t;


typedef enum
{
    PIN_NONE            = -1,
    PWM_0               = 0,
    PWM_1               = 1,
    PWM_2               = 2,
    PWM_3               = 3,
    PWM_4               = 4,
    UART0_TXD_I         = 21,
    UART0_RXD_I         = 22,
    UART0_RXD_II        = 3,
    UART0_TXD_II        = 4,
    UART1_RTS           = 5,
    UART1_RXD           = 6,
    UART1_TXD           = 7,
    UART1_CTS           = 9,
    I2C_M_SDA_I         = 10,
    I2C_M_SCL_I         = 12,
    I2C_M_SDA_II        = 21,
    I2C_M_SCL_II        = 22,
    I2C_S_SCL           = 21,
    I2C_S_SDA           = 22,
    SPI_M_CLK           = 8,
    SPI_M_MISO          = 10,
    SPI_M_MOSI          = 12,
    SPI_M_CS            = 13,
    SPI_S_MOSI          = 10,
    SPI_S_CLK           = 11,
    SPI_S_MISO          = 12,
    SPI_S_CS            = 13,    
    SPI1_S_MISO         = 16,
    SPI1_S_CSN          = 18,
    SPI1_S_MOSI         = 19,
    SPI1_S_CLK          = 20,
    I2S_BCLK_I          = 5,
    I2S_DI_I            = 6,
    I2S_DO_I            = 7,
    I2S_LRCLK_I         = 9,
    I2S_MCLK_I          = 20,
    I2S_BCLK_II         = 8,
    I2S_DI_II           = 10,
    I2S_DO_II           = 12,
    I2S_LRCLK_II        = 13,
    I2S_MCLK_II         = 20,
    SPI_flash_IO0_DI    = 14,
    SPI_flash_CLK       = 15,
    SPI_flash_IO3_HD    = 16,
    SPI_flash_CSN       = 17,
    SPI_flash_IO1_DO    = 18,
    SPI_flash_IO2_WP    = 19,
    ADC_0               = 22,
    ADC_1               = 21,
    ADC_2               = 20,
    ADC_3               = 0,
    WIFI_TX_SW          = 0,
    WIFI_RX_SW          = 1,
    BT_SW               = 2,
    BT_PRIORITY         = 3,
    WLAN_ACTIVE         = 4,
    BT_ACTIVE           = 22,
    HS3W_DATA           = 8,
    HS3W_CSN            = 9,
    HS3W_CLK            = 10,
    EXT_LNAGC_0         = 11,
    RX_RSSI_INDICATOR   = 12,
    EXT_LNAGC_1         = 13,
    EXT_PGAGC_0         = 14,
    CLK_REF_40M         = 15,
    EXT_PGAGC_3         = 16,
    EXT_PGAGC_1         = 18,
    EXT_PGAGC_2         = 19,
    GPIO_00             = 0,
    GPIO_01             = 1,
    GPIO_02             = 2,
    GPIO_03             = 3,
    GPIO_04             = 4,
    GPIO_05             = 5,
    GPIO_06             = 6,
    SIO_07              = 7,
    GPIO_08             = 8,
    GPIO_09             = 9,
    GPIO_10             = 10,
    GPIO_11             = 11,
    GPIO_12             = 12,
    GPIO_13             = 13,
    SIO_14              = 14,
    SIO_15              = 15,
    GPIO_16             = 16,
    GPIO_17             = 17,
    GPIO_18             = 18,
    GPIO_19             = 19,
    SIO_20              = 20,
    GPIO_21             = 21,
    GPIO_22             = 22
} gpio_pin_t;


typedef enum
{
    GPIO_DIR_IN         = 0,        /**< GPIO direction INPUT */
    GPIO_DIR_OUT        = 1,        /**< GPIO direction OUTPUT */
    GPIO_DIR_IN_OUT     = 2,        /**< GPIO direction INPUT and OUTPUT */
    GPIO_DIR_IN_OUT_OFF = 3         /**< GPIO direction INPUT and OUTPUT OFF*/
} gpio_dir_t;

typedef enum
{
    GPIO_PULL_DOWN      = 0,        /**< GPIO Pull Down */
    GPIO_PULL_UP        = 1,         /**< GPIO Pull Up */
    GPIO_PULL_NONE      = 2
} gpio_pull_t;

typedef enum
{
    GPIO_DRIVING_WEAK       = 0,         /**< GPIO weak driving strenght */
    GPIO_DRIVING_STRONG     = 1          /**< GPIO strong driving streght */
} gpio_driving_strengh_t;


typedef enum
{
    GPIO_LOGIC_LOW    = 0,         /**< GPIO logic state LOW */
    GPIO_LOGIC_HIGH   = 1          /**< GPIO logic state HIGH */
} gpio_logic_t;


typedef enum
{
    GPIO_INTC_HIGH_LEVEL    = 2,
    GPIO_INTC_RISING_EDGE   = 3,
    GPIO_INTC_LOW_LEVEL     = 4,
    GPIO_INTC_FALLING_EDGE  = 5    
} gpio_intc_trigger_mode_t;

typedef enum
{
    GPIO_00_TO_07   = 0,
    GPIO_08_TO_15   = 1,
    GPIO_16_TO_22   = 2    
} gpio_intc_group_t;


int8_t hal_gpio_set_mode(gpio_pin_t pin, pin_mode_t mode);
int8_t hal_gpio_set_dir(gpio_pin_t pin, gpio_dir_t dir);
gpio_dir_t hal_gpio_get_dir(gpio_pin_t pin);
int8_t hal_gpio_set_pull(gpio_pin_t pin, gpio_pull_t pull);
gpio_pull_t hal_gpio_get_pull(gpio_pin_t pin);
int8_t hal_gpio_set_logic(gpio_pin_t pin, gpio_logic_t logic);
int8_t hal_gpio_set_driving_strengh(gpio_pin_t pin, gpio_driving_strengh_t driving_streght);
int8_t hal_gpio_set_interrupt(gpio_pin_t pin);
gpio_logic_t hal_gpio_get_logic(gpio_pin_t pin);

int8_t hal_gpio_intc_trigger_mode(gpio_pin_t pin, gpio_intc_trigger_mode_t mode);
int8_t hal_gpio_intc_clear_counter(gpio_pin_t pin);

#endif  // __HAL_GPIO_H__

