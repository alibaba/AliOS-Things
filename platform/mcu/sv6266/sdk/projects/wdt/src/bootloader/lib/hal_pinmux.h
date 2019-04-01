#ifndef __HAL_PINMUX_H__
#define __HAL_PINMUX_H__

typedef enum
{
    FUNCTION_NONE   = -1,
    SEL_I2STRX_II   = 0,
    SEL_I2STRX_I    = 1,
    SEL_SPI_SLV     = 2,
    SEL_SPI_MST     = 3,
    SEL_I2C_SLV     = 4,
    SEL_I2C_MST_II  = 5,
    SEL_I2C_MST_I   = 6,
    SEL_UART0_II    = 7,
    SEL_UART0_I     = 8,
    SEL_BTCX        = 9,
    SEL_FLASH       = 10,
    SEL_RF          = 11,
    SEL_PWM_0       = 12,
    SEL_PWM_1       = 13,
    SEL_PWM_2       = 14,
    SEL_PWM_3       = 15,
    SEL_PWM_4       = 16,
    SEL_DEBUG_I     = 17,
    SEL_DEBUG_II    = 18,
    SEL_MEM_BIST    = 19,
    SEL_USB_BIST    = 20,
    SEL_USB_TEST    = 21,
    SEL_USB_IDDQ    = 22,
    SEL_ADC_0       = 23,
    SEL_ADC_1       = 24,
    SEL_ADC_2       = 25,
    SEL_ADC_3       = 26,
    SEL_UART1       = 27,
    I2S_RAW_DATA    = 30,
    SPI_RAW_DATA    = 31
} pinmux_fun_t;

int8_t hal_pinmux_manual_function_select_enable(pinmux_fun_t fun_sel);
int8_t hal_pinmux_manual_function_select_disable(pinmux_fun_t fun_sel);
void hal_pinmux_set_raw(uint32_t io_sel, uint32_t fun_sel);

#endif  // __HAL_PINMUX_H__
