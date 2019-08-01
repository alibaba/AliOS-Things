/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifdef QXWZ_ENABLED

#include "qxwz_types.h"
#include "qxwz_sdk.h"
#include <aos/kernel.h>

#define REPORT_GGA_DATA_SIZE 128
#define GPS_RAW_DATA_SIZE    1024
#define QXWZ_PRINT_BUFFER    1024
#define SOCKET_TIMEOUT       1000
#define SOCKET_BUFFER        2048
#define GGA_DATA_PERIOD      1000
#define GGA_DATA_PORT        PORT_UART_RS485
#define GGA_DATA_BAUD        115200

typedef qxwz_usr_config_t ulocation_qxwz_usr_config_t;

typedef struct {
    uint8_t status; /* 状态值 */
    int longitude;  /* 经度 */
    int latitude;   /* 纬度 */
    uint8_t nors;   /* 南北纬 */
    uint8_t wore;   /* 东西经 */
    int altitude;   /* 高度 */
} ulocation_gga_info;

#endif

