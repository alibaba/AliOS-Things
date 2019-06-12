/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifdef QXWZ_ENABLED
#include "aos/hal/uart.h"
#include "ulog/ulog.h"
#include "qxwz_types.h"
#include "qxwz_common.h"

static uart_dev_t uart_dev;
static uint8_t report_ggadata[REPORT_GGA_DATA_SIZE] = {0};

static int32_t qxwz_gga_filter(uint8_t gps_rawdata[], int len)
{
    int32_t ret = -1;
    static int i,j;
    static uint8_t data;
    static gga_data_filter_index cur_state = NONE;

    if (len < GPS_RAW_DATA_SIZE){
        LOGE("uLocation-qxwz", "gps_rawdata is error!\n");
        return ret;
    }

    for (i = 0; i < len; i++) {
        data = gps_rawdata[i];
        switch (cur_state) {
            case NONE:
                if('$' == data){
                    cur_state = SYNC;
                    report_ggadata[j] = data;
                    j++;
                }
                break;
            case SYNC:
                if('G' == data){
                    cur_state = SYNC1;
                    report_ggadata[j] = data;
                    j++;
                }
                break;
            case SYNC1:
                if('N' == data){
                    cur_state = SYNC2;
                    report_ggadata[j] = data;
                    j++;
                }
                break;
            case SYNC2:
                if('G' == data){
                    cur_state = SYNC3;
                    report_ggadata[j] = data;
                    j++;
                }
                break;
            case SYNC3:
                if('G' == data){
                    cur_state = SYNC4;
                    report_ggadata[j] = data;
                    j++;
                }
                break;
            case SYNC4:
                if('A' == data){
                    cur_state = GET_DATA;
                    report_ggadata[j] = data;
                    j++;
                }
                break;
            case GET_DATA:
                if('\r' != data){
                    report_ggadata[j] = data;
                    j++;
                    break;
                }
                else{
                    cur_state = END_CR;
                }
            case END_CR:
                if('\r' == data){
                    cur_state = END_NL;
                    report_ggadata[j] = data;
                    j++;
                }
                break;
            case END_NL:
                if('\n' == data){
                    cur_state = NONE;
                    report_ggadata[j] = data;
                    j=0;
                    ret = 0;
                }
                break;
        }
    }
    return ret;
}

int32_t qxwz_gga_init(uint8_t port, uint32_t baud)
{
    int32_t ret = -1;

    uart_dev.port = port;
    uart_dev.config.baud_rate = baud;
    uart_dev.config.data_width = DATA_WIDTH_8BIT;
    uart_dev.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_dev.config.mode = MODE_TX_RX;
    uart_dev.config.parity = NO_PARITY;
    uart_dev.config.stop_bits = STOP_BITS_1;

    ret = hal_uart_init(&uart_dev);
    if (ret == -1){
        LOGE("uLocation-qxwz", "qxwz uart init failed!\n");
    }

    return ret;
}

int32_t qxwz_rtcm_send(qxwz_void_t *rtcm, qxwz_u32_t len)
{
    int32_t ret = -1;

    if (rtcm == NULL || len < 0){
        LOGE("uLocation-qxwz", "qxwz rtcm is null\n");
        return ret;
    }
    ret = hal_uart_send(&uart_dev, (uint8_t*)rtcm, len, WAIT_FOREVER);
    if (ret != 0){
        LOGE("uLocation-qxwz", "qxwz rtcm data send failed!\n");
        return ret;
    }
    return ret;
}

/* GPS data receive interface implement */
int32_t qxwz_gga_get(uint8_t data[], uint32_t data_len)
{
    int32_t ret = -1;
    uint32_t *recv_size;
    uint8_t gps_raw_data[GPS_RAW_DATA_SIZE] = {0};

    if (data_len < 0){
        LOGE("uLocation-qxwz", "qxwz gps data recv is null!\n");
        return ret;
    }

    ret = hal_uart_recv_II(&uart_dev, &gps_raw_data, GPS_RAW_DATA_SIZE, recv_size, WAIT_FOREVER);
    if (ret != 0){
        LOGE("uLocation-qxwz", "qxwz gps data recv failed!\n");
        return ret;
    }

    ret = qxwz_gga_filter(&gps_raw_data, GPS_RAW_DATA_SIZE);
    if (ret != 0){
        return ret;
    }
    for (int i = 0; i < data_len; i++){
        data[i] = report_ggadata[i];
    }

    return ret;
}

#endif    /* QXWZ_ENABLED */

