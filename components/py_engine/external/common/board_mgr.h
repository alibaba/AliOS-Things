/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_BOARD_MGR_H
#define BE_BOARD_MGR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* app.json configuration */
#define APP_CONFIG_PAGES    "pages"
#define APP_CONFIG_IO       "io"
#define APP_CONFIG_NET      "net"
#define APP_CONFIG_DEBUG    "debugLevel"
#define APP_CONFIG_REPL     "repl"

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

#define SPI_MODE_MASTER     "master"
#define SPI_MODE_SLAVE      "slave"

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
typedef enum addon_module {

    MODULE_GPIO = 0x1324,
    MODULE_UART,
    MODULE_I2C,
    MODULE_PWM,
    MODULE_ADC,
    MODULE_DAC,
    MODULE_SPI,
    MODULE_TIMER,
    MODULE_RTC,
    MODULE_BT,
    MODULE_IR,
    MODULE_I2S,
    MODULE_CAN,
    MODULE_SDIO,
    MODULE_USB,
    MODULE_I2C_GPIO,
    MODULE_NUMS,
} addon_module_m;

typedef struct item_handle {
    void *handle;
} item_handle_t;

typedef struct {
    int irq_mode;
    int js_cb_ref;
    void *reserved;
} gpio_params_t;

/**
 * initialize mgr system
 *
 * @param[in]  json_path  the path of boar-mgr file
 * @return  the operation status, 0 is OK, others is error
 */
//int32_t board_mgr_init(const char* json_path);
int32_t py_board_mgr_init();

/**
 * load driver config
 *
 * @param[in]  driver  the path of boar-driver file
 * @return  the operation status, 0 is OK, others is error
 */
int8_t py_board_load_drivers(const char* driver);

/**
 * attach a  driver resource
 *
 * @param[in]  module   the module type of a driver
 * @param[in]  name_id  the name of a driver
 * @param[out] name_id  the resource of a driver
 * @return  the operation status, 0 is OK, others is error
 */
int8_t py_board_attach_item(addon_module_m module, const char* name_id,
                         item_handle_t* out);

/**
 * release a  driver resource
 *
 * @param[in]  module   the module type of a driver
 * @param[in]  handle  the resource of a driver
 * @return  the operation status, 0 is OK, others is error
 */
int8_t py_board_disattach_item(addon_module_m module, item_handle_t* handle);

/**
 * the attach status of the driver and resource
 *
 * @param[in]  module   the module type of a driver
 * @param[in]  handle  the resource of a driver
 * @return  the attach status, 1 is attach, others is dis-attach
 */
int8_t py_board_check_attach_status(addon_module_m module, item_handle_t* handle);

/**
 * get the resource of a driver by name
 *
 * @param[in]  module   the module type of a driver
 * @param[in]  name_id  the name of a driver
 * @return  driver resource,  null if not exist,otherwise it's right
 */
void* py_board_get_node_by_name(addon_module_m module, const char* name_id);

/**
 * get the resource of a driver by the handle of a driver resource
 *
 * @param[in]  module   the module type of a driver
 * @param[in]  handle  the resource of a driver
 * @return  driver resource,  null if not exist,otherwise it's right
 */
void* py_board_get_node_by_handle(addon_module_m module, item_handle_t* handle);

#ifdef __cplusplus
}
#endif

#endif /* BE_BOARD_MGR_H */
