/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifdef QXWZ_ENABLED
#include "qxwz_types.h"
#include <aos/kernel.h>

#define REPORT_GGA_DATA_SIZE 128
#define GPS_RAW_DATA_SIZE    1024
#define QXWZ_PRINT_BUFFER    1024
#define SOCKET_TIMEOUT       1000
#define SOCKET_BUFFER        2048
#define GGA_DATA_PERIOD      1000
#define GGA_DATA_PORT        PORT_UART_RS485
#define GGA_DATA_BAUD        115200

typedef enum  {
    NONE,
    SYNC,
    SYNC1,
    SYNC2,
    SYNC3,
    SYNC4,
    GET_DATA,
    END_CR,
    END_NL
} gga_data_filter_index;

typedef struct {
    char status;       /* 状态值 */
    double longitude;  /* 经度 */
    double latitude;   /* 纬度 */
    char nors;         /* 南北纬 */
    char wore;         /* 东西经 */
    double altitude;   /* 高度 */
} uloc_gga_info;

#endif