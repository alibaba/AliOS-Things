/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_BOARD_MARKER_H
#define BE_BOARD_MARKER_H
#ifdef __cplusplus
extern "C" {
#endif

/* app.json configuration */
#define APP_CONFIG_PAGES    "pages"
#define APP_CONFIG_IO       "io"
#define APP_CONFIG_AUDIO    "audio"
#define APP_CONFIG_NET      "net"
#define APP_CONFIG_DEBUG    "debugLevel"
#define APP_CONFIG_REPL     "repl"
#define APP_CONFIG_VERSION  "version"

#define MARKER_ID           "type"
#define MARKER_PORT         "port"

/* GPIO */
#define MARKER_GPIO         "GPIO"
#define GPIO_DIR            "dir"
#define GPIO_PULL           "pull"
#define GPIO_INTMODE        "intMode"

#define GPIO_DIR_OUTPUT     "output"
#define GPIO_DIR_INPUT      "input"
#define GPIO_DIR_IRQ        "irq"
#define GPIO_DIR_ANALOG     "analog"

#define GPIO_PULL_DOWN      "pulldown"
#define GPIO_PULL_UP        "pullup"
#define GPIO_PULL_OPEN      "opendrain"

#define GPIO_INT_RISING     "rising"
#define GPIO_INT_FALLING    "falling"
#define GPIO_INT_BOTH       "both"

/* UART */
#define MARKER_UART         "UART"
#define UART_DATA_WIDTH     "dataWidth"
#define UART_BAUD_RATE      "baudRate"
#define UART_STOP_BITS      "stopBits"
#define UART_FLOW_CONTROL   "flowControl"
#define UART_PARITY_CONFIG  "parity"
#define UART_MODE           "mode"

#define UART_FC_DISABLE     "disable"
#define UART_FC_CTS         "cts"
#define UART_FC_RTS         "rts"
#define UART_FC_RTSCTS      "rtscts"

#define UART_PARITY_NONE    "none"
#define UART_PARITY_ODD     "odd"
#define UART_PARITY_EVEN    "even"

/* I2C */
#define MARKER_I2C          "I2C"
#define I2C_ADDR_WIDTH      "addrWidth"
#define I2C_FREQ            "freq"
#define I2C_MODE            "mode"
#define I2C_ADDR_DEV        "devAddr"

#define I2C_MASTER          "master"
#define I2C_SLAVE           "slave"

/* SPI */
#define MARKER_SPI          "SPI"
#define SPI_MODE            "mode"
#define SPI_FREQ            "freq"

#define SPI_MODE_0          "mode0"
#define SPI_MODE_1          "mode1"
#define SPI_MODE_2          "mode2"
#define SPI_MODE_3          "mode3"

/* ADC */
#define MARKER_ADC          "ADC"
#define ADC_SAMPLING        "sampling"

/* DAC */
#define MARKER_DAC          "DAC"

/* CAN */
#define MARKER_CAN          "CAN"
#define CAN_BAUD_RATE       "baudRate"
#define CAN_IDE             "ide"
#define CAN_AUTO_BUS_OFF    "auto_bus_off"
#define CAN_RETRY_TRANSMIT  "retry_transmit"

#define IDE_NORMAL_CAN      "normal"
#define IDE_EXTEND_CAN      "extend"

#define CAN_DISABLE         "disable"
#define CAN_ENABLE          "enable"

/* PWM */
#define MARKER_PWM          "PWM"

/* TIMER */
#define MARKER_TIMER        "TIMER"

/* AUDIO */
#define MARKER_AUDIO        "AUDIO"
#define OUT_DEVICE          "out_device"
#define EXTERNAL_PA         "external_pa"
#define EXTERNAL_PA_PIN     "external_pa_pin"
#define EXTERNAL_PA_DELAY   "external_pa_delay_ms"
#define EXTERNAL_PA_ACTIVE  "external_pa_active_level"

#ifdef __cplusplus
}
#endif

#endif /* BE_BOARD_MARKER_H */
