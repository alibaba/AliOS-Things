/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifdef QXWZ_ENABLED

#include "aos/hal/uart.h"
#include "ulog/ulog.h"
#include "qxwz_types.h"
#include "ulocation_qxwz_common.h"

static uart_dev_t uart_dev;
uint8_t gps_raw_data[GPS_RAW_DATA_SIZE] = {0};

static int32_t ulocation_gga_filter(char *gps_raw_data, uint8_t *gga_raw_data)
{
    int32_t ret = -1;
    uint8_t *p1 = NULL;
    uint8_t *p2 = NULL;

    if (gps_raw_data == NULL || gga_raw_data == NULL) {
        LOGE("uLocation-qxwz", "GPS data is null!");
        return ret;
    }
    p2 = gga_raw_data;
    p1=(uint8_t*)strstr((const char *)gps_raw_data,"$GNGGA");
    while (*p1 != '\n') {
        *p2++ = *p1++;
    }
    *p2 = '\n';

    return 0;
}

static uint8_t ulocation_comma_pos(uint8_t *buf, uint8_t cx)
{
    uint8_t *p = buf;

    while(cx) {
        if ((*buf == '*') || (*buf < ' ') || (*buf > 'z')) {
            return 0xFF;
        }
        if (*buf == ',') {
            cx--;
        }
        buf++;
    }

    return buf-p;
}

static int ulocation_num_pow(uint8_t m, uint8_t n)
{
    int result = 1;

    while (n--) {
        result *= m;
    }

    return result;
}

static int ulocation_str2num(uint8_t *buf, uint8_t *dx)
{
    uint8_t *p = buf;
    uint8_t ilen = 0, flen = 0, mask = 0, i;
    int ires = 0, fres = 0;
    int res;

    while (1) {
        if (*p == '-') {
            mask|=0x02;
            p++;
        }
        if ((*p == ',') || (*p=='*')) {
            break;
        }
        if (*p == '.') {
            mask |= 0x01;
            p++;
        } else if ((*p > '9') || (*p<'0')) {
            ilen=0;
            flen=0;
            break;
        }
        if (mask&0x01) {
            flen++;
        } else {
            ilen++;
        }
        p++;
    }

    if (mask&0x02) {
        buf++;
    }

    for (i = 0; i < ilen; i++) {
        ires += ulocation_num_pow(10, ilen-1-i)*(buf[i] - '0');
    }
    if (flen > 5) {
        flen = 5;
    }
    *dx = flen;
    for (i = 0; i < flen; i++) {
        fres += ulocation_num_pow(10, flen-1-i)*(buf[ilen+1+i] - '0');
    }

    res = ires * ulocation_num_pow(10, flen) + fres;
    if (mask&0x02) {
        res = -res;
    }

    return res;
}

int32_t ulocation_ggainfo_parse(uint8_t *gga_data, ulocation_gga_info *location)
{
    int32_t ret = -1;
    uint32_t temp;
    uint8_t *buf,dx;
    uint8_t posx;
    float rs;

    if (gga_data == NULL || location == NULL) {
        return ret;
    }
    buf = gga_data;

    posx = ulocation_comma_pos(buf, 2); /* latitude */
    if (posx != 0xFF) {
        temp = ulocation_str2num((buf + posx), &dx);
        location->latitude = temp/ulocation_num_pow(10, dx+2);
        rs = temp % ulocation_num_pow(10, dx+2);
        location->latitude = location->latitude * ulocation_num_pow(10, 5) + (rs * ulocation_num_pow(10, 5-dx))/60;
    }

    posx = ulocation_comma_pos(buf,3); /* N or S*/
    if (posx != 0xFF) {
        location->nors = *(buf + posx);
    }

    posx = ulocation_comma_pos(buf, 4); /* longitude */
    if (posx != 0xFF) {
        temp = ulocation_str2num((buf + posx), &dx);
        location->longitude = temp/ulocation_num_pow(10, dx+2);
        rs = temp % ulocation_num_pow(10, dx+2);
        location->longitude = location->longitude * ulocation_num_pow(10, 5) + (rs * ulocation_num_pow(10, 5-dx))/60;
    }

    posx = ulocation_comma_pos(buf,5); /* W or E*/
    if (posx != 0xFF) {
        location->wore = *(buf + posx);
    }

    posx = ulocation_comma_pos(buf,6); /* status */
    if (posx != 0xFF) {
        location->status=ulocation_str2num(buf+posx, &dx);
    }

    posx = ulocation_comma_pos(buf,9); /* altitude */
    if (posx != 0xFF) {
        location->altitude = ulocation_str2num(buf+posx, &dx);
    }

    return 0;
}

void show_ggainfo(ulocation_gga_info *location)
{
    if (location->status > 0) {
    LOGI("uLocation-qxwz", "status : %d", location->status);
    LOGI("uLocation-qxwz", "longitude : %f %c", (float)location->longitude / 100000, location->wore);
    LOGI("uLocation-qxwz", "latitude : %f %c", (float)location->latitude / 100000, location->nors);
    LOGI("uLocation-qxwz", "altitude : %f", (float)location->altitude / 1000);
    }
}

int32_t ulocation_qxwz_init(uint8_t port, uint32_t baud)
{
    int32_t ret = -1;
    uint32_t *recv_size;

    uart_dev.port = port;
    uart_dev.config.baud_rate = baud;
    uart_dev.config.data_width = DATA_WIDTH_8BIT;
    uart_dev.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_dev.config.mode = MODE_TX_RX;
    uart_dev.config.parity = NO_PARITY;
    uart_dev.config.stop_bits = STOP_BITS_1;

    ret = hal_uart_init(&uart_dev);
    if (ret != 0) {
        LOGE("uLocation-qxwz", "qxwz uart init failed!");
        return ret;
    }

    ret = hal_uart_recv_II(&uart_dev, gps_raw_data, GPS_RAW_DATA_SIZE, recv_size, GGA_DATA_PERIOD);
    if (ret != 0) {
        LOGE("uLocation-qxwz", "can not received gps data!");
        return ret;
    }

    return ret;
}

int32_t qxwz_rtcm_send(qxwz_void_t *rtcm, uint32_t len)
{
    int32_t ret = -1;

    if (rtcm == NULL || len < 0) {
        LOGE("uLocation-qxwz", "qxwz rtcm is null");
        return ret;
    }
    ret = hal_uart_send(&uart_dev, (uint8_t*)rtcm, len, AOS_WAIT_FOREVER);
    if (ret != 0) {
        LOGE("uLocation-qxwz", "qxwz rtcm data send failed!");
        return ret;
    }
    return ret;
}

/* GPS data receive interface implement */
int32_t ulocation_qxwz_getgga(uint8_t *gga_raw_data)
{
    int32_t ret = -1;
    uint32_t *recv_size;

    if (gga_raw_data == NULL) {
        LOGE("uLocation-qxwz", "qxwz gga data is null!");
        return ret;
    }

    ret = hal_uart_recv_II(&uart_dev, gps_raw_data, GPS_RAW_DATA_SIZE, recv_size, AOS_WAIT_FOREVER);
    if (ret != 0) {
        LOGE("uLocation-qxwz", "qxwz gps data recv failed!");
        return ret;
    }

    ret = ulocation_gga_filter(&gps_raw_data, gga_raw_data);
    if (ret != 0) {
        return ret;
    }

    return ret;
}

#endif    /* QXWZ_ENABLED */

