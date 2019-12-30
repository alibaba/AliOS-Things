/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_BOARD_MARKER_H
#define BE_BOARD_MARKER_H
#ifdef __cplusplus
extern "C" {
#endif

#define MARKER_ID "type"
#define MARKER_PORT "port"

/* UART */
#define MARKER_UART "UART"
#define UART_DATA_WIDTH "data_width"
#define UART_BAUD_RATE "baud_rate"
#define UART_STOP_BITS "stop_bits"
#define UART_FLOW_CONTROL "flow_control"
#define UART_PARITY_CONFIG "parity_config"
#define UART_MODE "mode"

/* GPIO */
#define MARKER_GPIO "GPIO"
#define GPIO_DIR "dir"
#define GPIO_PULL "pull"

#define GPIO_DIR_OUTPUT 0
#define GPIO_DIR_INPUT 1
#define GPIO_DIR_IRQ 2
#define GPIO_DIR_ANALOG 3

#define GPIO_PULL_DOWN 0
#define GPIO_PULL_UP 1
#define GPIO_PULL_OPEN 2

/* PWM */
#define MARKER_PWM "PWM"
#define PWM_FREQ "freq"
#define PWM_DUTY "duty"
#define PWM_CHAN "chan"

/* I2C */
#define MARKER_I2C "I2C"
#define I2C_ADDR_WIDTH "address_width"
#define I2C_FREQ "freq"
#define I2C_MODE "mode"
#define I2C_ADDR_DEV "dev_addr"

/* ADC */
#define MARKER_ADC "ADC"
#define ADC_SAMPLING "sampling"
#define ADC_CHAN "chan"

/* DAC */
#define MARKER_DAC "DAC"
#define DAC_VOLTAGE "voltage"

#ifdef __cplusplus
}
#endif

#endif /* BE_BOARD_MARKER_H */
