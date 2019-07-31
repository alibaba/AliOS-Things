/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 * service data to cloud
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/kernel.h"
#include "service_data_to_cloud.h"
#include "service_mgr.h"
#include "udata_parse.h"
#include "udata_queue.h"
#include "udata/udata.h"
#include "ulog/ulog.h"

#define DTC_PUBLISH_STR_LEN         (128)
#define DTC_PUBLISH_DELAY           (50)

#define DTC_DATA_FLOAT_CONVERT(a,b) (((float)(a))/((float)(b)))
#define DTC_DATA_INT_CONVERT(a,b)   (((int)(a))/((int)(b)))
#define DTC_DATA_UINT_CONVERT(a,b)  (((uint32_t)(a))/((uint32_t)(b)))

#define PROP_POST_FORMAT_1_FLOAT     "{\"%s\":%%s%%d.%%d%%d}"
#define PROP_POST_FORMAT_1_INT       "{\"%s\":%%d}"
#define PROP_POST_FORMAT_1_UINT      "{\"%s\":%%u}"

#define PROP_POST_FORMAT_3_FLOAT     "{\"%s\":{\"%s\":%%s%%d.%%d%%d,\"%s\":%%s%%d.%%d%%d,\"%s\":%%s%%d.%%d%%d}}"
#define PROP_POST_FORMAT_3_INT       "{\"%s\":{\"%s\":%%d,\"%s\":%%d,\"%s\":%%d}}"
#define PROP_POST_FORMAT_3_UINT      "{\"%s\":{\"%s\":%%u,\"%s\":%%u,\"%s\":%%u}}"

#define PROP_POST_FORMAT_8_UINT      "{\"%s\":{\"%s\":%%u,\"%s\":%%u,\"%s\":%%u,\"%s\":%%u,\"%s\":%%u,\"%s\":%%u,\"%s\":%%u}}"

#define  FLOAT_PRINT_DATA_GET(IN,OUT,SIGN) \
do{ \
    (SIGN) = ((IN) >= 0)?"":"-";  \
    (IN) = ((IN) >= 0) ?(IN):((IN)*-1); \
    (OUT)[0] = (int)(IN); \
    (OUT)[1] = (int)(((IN)-(float)(OUT)[0])*10)%10; \
    (OUT)[2] = (int)(((IN)-(float)(OUT)[0])*100)%10; \
}while(0);

#define  FLOAT_PRINT_DATA(DATA,SIGN)  SIGN,DATA[0],DATA[1],DATA[2]

service_pub_info_t *g_service_info[UDATA_MAX_CNT] = {NULL};
static int          g_reg_index = -1;
static bool         g_dtc_conn_flag = false;
static uint32_t     g_dtc_num = 0;

static int udata_dtc_publish(udata_type_e type, void *pdata, uint32_t len)
{
    int   ret = 0;
    int   coeff;
    int   data[3][3] = { 0 };
    char *sign[3] = { 0 };

    accel_data_t      *acc;
    gyro_data_t       *gyro;
    mag_data_t        *mag;
    gps_data_t        *gps;
    integer_data_t    *data_f;
    integer_data_t    *data_i;
    als_data_t        *als;
    proximity_data_t  *ps;
    uv_data_t         *uv;
    hall_data_t       *hall;
    heart_rate_data_t *hr;
    rtc_data_t        *rtc;
    rgb_data_t        *rgb;

    float value[3] = { 0 };
    char  param[DTC_PUBLISH_STR_LEN] = { 0 };

    if (type >= UDATA_MAX_CNT) {
        return -1;
    }
    if (pdata == NULL) {
        return -1;
    }
    if (g_service_info[type] == NULL) {
        return -1;
    }

    if (g_service_info[type]->str == NULL) {
        return -1;
    }

    coeff = (int)g_service_info[type]->coeff;
    if(coeff == 0) {
        return -1;
    }

    ret = -1;
    switch(type) {
        case UDATA_SERVICE_ACC: {
            acc = (accel_data_t*) pdata;

            value[0] = DTC_DATA_FLOAT_CONVERT(acc->data[0], coeff);
            value[1] = DTC_DATA_FLOAT_CONVERT(acc->data[1], coeff);
            value[2] = DTC_DATA_FLOAT_CONVERT(acc->data[2], coeff);

            FLOAT_PRINT_DATA_GET(value[0], data[0], sign[0]);
            FLOAT_PRINT_DATA_GET(value[1], data[1], sign[1]);
            FLOAT_PRINT_DATA_GET(value[2], data[2], sign[2]);
            ret = sprintf(param, g_service_info[type]->str, FLOAT_PRINT_DATA(data[0], sign[0]), FLOAT_PRINT_DATA(data[1], sign[1]), FLOAT_PRINT_DATA(data[2], sign[2]));
            break;
        }
        case UDATA_SERVICE_GYRO: {
            gyro = (gyro_data_t*) pdata;

            value[0] = DTC_DATA_FLOAT_CONVERT(gyro->data[0], coeff);
            value[1] = DTC_DATA_FLOAT_CONVERT(gyro->data[1], coeff);
            value[2] = DTC_DATA_FLOAT_CONVERT(gyro->data[2], coeff);

            FLOAT_PRINT_DATA_GET(value[0],data[0],sign[0]);
            FLOAT_PRINT_DATA_GET(value[1],data[1],sign[1]);
            FLOAT_PRINT_DATA_GET(value[2],data[2],sign[2]);
            ret = sprintf(param, g_service_info[type]->str, FLOAT_PRINT_DATA(data[0], sign[0]), FLOAT_PRINT_DATA(data[1], sign[1]), FLOAT_PRINT_DATA(data[2], sign[2]));
            break;
        }
        case UDATA_SERVICE_MAG: {
            mag = (mag_data_t*) pdata;

            value[0] = DTC_DATA_FLOAT_CONVERT(mag->data[0],coeff);
            value[1] = DTC_DATA_FLOAT_CONVERT(mag->data[1],coeff);
            value[2] = DTC_DATA_FLOAT_CONVERT(mag->data[2],coeff);

            FLOAT_PRINT_DATA_GET(value[0],data[0],sign[0]);
            FLOAT_PRINT_DATA_GET(value[1],data[1],sign[1]);
            FLOAT_PRINT_DATA_GET(value[2],data[2],sign[2]);
            ret = sprintf(param, g_service_info[type]->str, FLOAT_PRINT_DATA(data[0], sign[0]), FLOAT_PRINT_DATA(data[1], sign[1]), FLOAT_PRINT_DATA(data[2], sign[2]));
            break;

        }
        case UDATA_SERVICE_GPS: {
            gps = (gps_data_t*) pdata;

            value[0] = DTC_DATA_FLOAT_CONVERT(gps->lat, coeff);
            value[1] = DTC_DATA_FLOAT_CONVERT(gps->lon, coeff);
            value[2] = DTC_DATA_FLOAT_CONVERT(gps->elv, coeff);

            FLOAT_PRINT_DATA_GET(value[0], data[0], sign[0]);
            FLOAT_PRINT_DATA_GET(value[1], data[1], sign[1]);
            FLOAT_PRINT_DATA_GET(value[2], data[2], sign[2]);
            ret = sprintf(param, g_service_info[type]->str, FLOAT_PRINT_DATA(data[0], sign[0]), FLOAT_PRINT_DATA(data[1], sign[1]), FLOAT_PRINT_DATA(data[2], sign[2]));
            break;
        }

        case UDATA_SERVICE_TVOC:
        case UDATA_SERVICE_HCHO:
        case UDATA_SERVICE_NOISE:
        case UDATA_SERVICE_HUMI:
        case UDATA_SERVICE_TEMP: {
            data_f = (integer_data_t*) pdata;
            value[0] = DTC_DATA_FLOAT_CONVERT(data_f->data, coeff);
            FLOAT_PRINT_DATA_GET(value[0], data[0], sign[0]);
            ret = sprintf(param, g_service_info[type]->str, FLOAT_PRINT_DATA(data[0], sign[0]));
            break;
        }

        case UDATA_SERVICE_BARO:
        case UDATA_SERVICE_PM25:
        case UDATA_SERVICE_CO2:
        case UDATA_SERVICE_PH:
        case UDATA_SERVICE_VWC:
        case UDATA_SERVICE_EC:
        case UDATA_SERVICE_SALINITY:
        case UDATA_SERVICE_TDS:
        case UDATA_SERVICE_WINDSPD:
        case UDATA_SERVICE_WINDDIR:
        case UDATA_SERVICE_RAIN:
        case UDATA_SERVICE_IR:
        case UDATA_SERVICE_GS: {
            data_i = (integer_data_t*) pdata;
            ret = sprintf(param, g_service_info[type]->str, DTC_DATA_INT_CONVERT(data_i->data, coeff));
            break;
        }

        case UDATA_SERVICE_ALS: {
            als = (als_data_t*) pdata;
            ret = sprintf(param, g_service_info[type]->str, DTC_DATA_UINT_CONVERT(als->lux, coeff));
            break;
        }

        case UDATA_SERVICE_PS: {
            ps = (proximity_data_t*) pdata;
            ret = sprintf(param, g_service_info[type]->str, DTC_DATA_UINT_CONVERT(ps->present, coeff));
            break;
        }

        case UDATA_SERVICE_UV: {
            uv = (uv_data_t*) pdata;
            ret = sprintf(param, g_service_info[type]->str, DTC_DATA_UINT_CONVERT(uv->uvi, coeff));
            break;
        }

        case UDATA_SERVICE_HALL: {
            hall = (hall_data_t*) pdata;
            ret = sprintf(param, g_service_info[type]->str, DTC_DATA_UINT_CONVERT(hall->hall_level, coeff));
            break;
        }

        case UDATA_SERVICE_HR: {
            hr = (heart_rate_data_t*) pdata;
            ret = sprintf(param, g_service_info[type]->str, DTC_DATA_UINT_CONVERT(hr->hear_rate, coeff));
            break;
        }

        case UDATA_SERVICE_RTC: {
            rtc = (rtc_data_t*) pdata;
            ret = sprintf(param, g_service_info[type]->str, (unsigned int)(rtc->year), (unsigned int)(rtc->month), (unsigned int)(rtc->date), (unsigned int)(rtc->day), (unsigned int)(rtc->hours), (unsigned int)(rtc->minutes), (unsigned int)(rtc->seconds));
            break;
        }

        case UDATA_SERVICE_RGB: {
            rgb = (rgb_data_t*) pdata;
            ret = sprintf(param, g_service_info[type]->str, DTC_DATA_UINT_CONVERT(rgb->data[0], coeff), DTC_DATA_UINT_CONVERT(rgb->data[1], coeff), DTC_DATA_UINT_CONVERT(rgb->data[2], coeff));
            break;
        }
        default : break;
    }

    if(ret < 0) {
        return -1;
    }

    ret = udata_cloud_report(&param[0], strlen(param));

    return ret;

}


int udata_msg_report_publish(udata_type_e type)
{
    int         ret = 0;
    udata_pkg_t buf;

    if (type >= UDATA_MAX_CNT) {
        return -1;
    }
    if (false == service_dtc_is_connect()) {
        return -1;
    }
    if (false == service_dtc_is_timeout(type)) {
        return -1;
    }
    ret = udata_report_publish(type, &buf);
    if (ret != 0) {
        return -1;
    }
    ret = udata_dtc_publish(type, (void *)&buf.payload[0], 0);
    if (ret != 0) {
        return -1;
    }
    return 0;
}


void service_dtc_handle(sensor_msg_pkg_t *msg)
{
    int ret = 0;

    if (msg == NULL) {
        return;
    }

    switch (msg->cmd) {
        case UDATA_MSG_DATA_TO_CLOUD: {
            ret = udata_msg_report_publish(msg->value);
            if (ret != 0) {
                return;
            }
            break;
        }
        default:
            break;
    }
}
int service_dtc_connect_set(bool flag)
{
    int ret;

    if ((g_reg_index == -1) && (flag == true)) {
        ret = udata_register_msg_handler(service_dtc_handle);
        if (ret == -1) {
            LOG("error occur reg service_dtc_handle \n");
            return -1;
        }
        g_reg_index = ret;
    }

    if ((g_reg_index != -1) && (flag == false)) {
        ret = udata_unregister_msg_handler(g_reg_index);
        if (unlikely(ret)){
            LOG("error occur unreg service_dtc_handle \n");
            return -1;
        }
        g_reg_index = -1;
    }

    g_dtc_conn_flag = flag;

    return 0;
}


bool service_dtc_is_connect()
{
    return g_dtc_conn_flag;
}

int service_dtc_publish_set(udata_type_e type, bool flag)
{

    if (type >= UDATA_MAX_CNT) {
        return -1;
    }

    if (NULL == g_service_info[type]) {
        return -1;
    }

    if (flag == g_service_info[type]->dtcFlag) {
        return 0;
    }

    g_service_info[type]->dtcFlag   = flag;
    g_service_info[type]->time_stamp = aos_now_ms();

    return 0;
}

bool service_dtc_is_publish(udata_type_e type)
{

    if (type >= UDATA_MAX_CNT) {
        return false;
    }

    if (NULL == g_service_info[type]) {
        return false;
    }

    if (false == g_service_info[type]->dtcFlag) {
        return false;
    }

    if (0 == g_service_info[type]->dtc_cycle) {
        return false;
    }

    return true;
}

bool service_dtc_is_timeout(udata_type_e type)
{
    uint64_t time = aos_now_ms();

    if (type >= UDATA_MAX_CNT) {
        return false;
    }

    if (NULL == g_service_info[type]) {
        return false;
    }

    if (false == g_service_info[type]->dtcFlag) {
        return false;
    }

    if (0 == g_service_info[type]->dtc_cycle) {
        return false;
    }

    if ((time - g_service_info[type]->time_stamp + DTC_PUBLISH_DELAY) >=
        (uint64_t)g_service_info[type]->dtc_cycle) {
        g_service_info[type]->time_stamp = time;
        return g_service_info[type]->dtcFlag;
    } else {
        return false;
    }
}

int service_dtc_publish_cycle_set(udata_type_e type, uint32_t cycle)
{

    if (type >= UDATA_MAX_CNT) {
        return -1;
    }

    if (NULL == g_service_info[type]) {
        return -1;
    }
    g_service_info[type]->dtc_cycle = cycle;

    return 0;
}


int service_dtc_register(service_pub_info_t *dtc)
{
    int          ret;
    void        *str;
    char        *name;
    udata_type_e type;
    uint32_t     len = 0;

    if (dtc == NULL) {
        return -1;
    }

    if((dtc->name_num <= 0) || (dtc->name_addr == NULL)) {
        return -1;
    }
    type = dtc->type;

    if (type >= UDATA_MAX_CNT) {
        return -1;
    }

    if(g_service_info[type] != NULL) {
        return -1;
    }

    if((dtc->name_num != 1) && (dtc->name_num != 4) && (dtc->name_num != 8)) {
        return -1;
    }

    g_service_info[type] = aos_malloc(sizeof(service_pub_info_t));
    if(g_service_info[type] == NULL) {
        return -1;
    }

    memset(g_service_info[type],0,sizeof(service_pub_info_t));
    g_service_info[type]->dtcFlag = dtc->dtcFlag;
    g_service_info[type]->coeff = dtc->coeff;
    g_service_info[type]->decimalPlace = dtc->decimalPlace;
    g_service_info[type]->dtc_cycle = dtc->dtc_cycle;

    g_service_info[type]->name_num = dtc->name_num;
    g_service_info[type]->data_type = dtc->data_type;

    g_service_info[type]->str_len = dtc->name_num * (SERVICE_PUB_NAME_LEN+SERVICE_PUB_VALUE_LEN);
    g_service_info[type]->str = aos_malloc(g_service_info[type]->str_len);
    if(g_service_info[type]->str == NULL) {
        goto error1;
    }

    ret  = -1;
    str  = g_service_info[type]->str;
    len  = g_service_info[type]->str_len;
    name = dtc->name_addr;

    if(g_service_info[type]->name_num == DTC_PARA_NUM_1) {
        if(g_service_info[type]->data_type == UDATA_INT32) {
            ret = snprintf(str, len, PROP_POST_FORMAT_1_INT, name);
        }
        else if(g_service_info[type]->data_type == UDATA_UINT32) {
            ret = snprintf(str, len, PROP_POST_FORMAT_1_UINT, name);
        }
        else if(g_service_info[type]->data_type == UDATA_FLOAT) {
            ret = snprintf(str, len, PROP_POST_FORMAT_1_FLOAT, name);
        }

    }
    else if(g_service_info[type]->name_num == DTC_PARA_NUM_4) {
        if(g_service_info[type]->data_type == UDATA_INT32) {
            ret = snprintf(str, len, PROP_POST_FORMAT_3_INT, name, (char*)((uint32_t)name + SERVICE_PUB_NAME_LEN), (char*)((uint32_t)name + SERVICE_PUB_NAME_LEN * 2), (char*)((uint32_t)name + SERVICE_PUB_NAME_LEN * 3));
        }
        else if(g_service_info[type]->data_type == UDATA_UINT32) {
            ret = snprintf(str, len, PROP_POST_FORMAT_3_UINT, name, (char*)((uint32_t)name + SERVICE_PUB_NAME_LEN), (char*)((uint32_t)name + SERVICE_PUB_NAME_LEN * 2), (char*)((uint32_t)name + SERVICE_PUB_NAME_LEN * 3));
        }
        else if(g_service_info[type]->data_type == UDATA_FLOAT) {
            ret = snprintf(str, len, PROP_POST_FORMAT_3_FLOAT, name, (char*)((uint32_t)name + SERVICE_PUB_NAME_LEN), (char*)((uint32_t)name + SERVICE_PUB_NAME_LEN * 2), (char*)((uint32_t)name + SERVICE_PUB_NAME_LEN*3));
        }

    }
    else if(g_service_info[type]->name_num == DTC_PARA_NUM_8) {
        if(g_service_info[type]->data_type == UDATA_INT32) {
            ret = snprintf(str, len, PROP_POST_FORMAT_8_UINT, name, (char*)((uint32_t)name + SERVICE_PUB_NAME_LEN), (char*)((uint32_t)name + SERVICE_PUB_NAME_LEN * 2), (char*)((uint32_t)name + SERVICE_PUB_NAME_LEN * 3),
                 (char*)((uint32_t)name + SERVICE_PUB_NAME_LEN * 4), (char*)((uint32_t)name + SERVICE_PUB_NAME_LEN * 5), (char*)((uint32_t)name + SERVICE_PUB_NAME_LEN * 6), (char*)((uint32_t)name + SERVICE_PUB_NAME_LEN * 7));
        }
    }
    else {
        ret = -1;
    }

    if(ret < 0) {
        goto error;
    }

    g_dtc_num++;

    return 0;
error:
    if(g_service_info[type]->str != NULL) {
        aos_free(g_service_info[type]->str);
        g_service_info[type]->str = NULL;
    }

error1:
    if(g_service_info[type] != NULL) {
        aos_free(g_service_info[type]);
        g_service_info[type] = NULL;
    }
    return -1;
}

int service_dtc_init(void)
{
    int      ret;
    uint32_t i;
    uint32_t num;

    service_pub_info_t dtc;

    memset(&g_service_info[0], 0, sizeof(g_service_info));

    ret = udata_dtc_num_get(&num);
    if (unlikely(ret)) {
        return -1;
    }

     if(num == 0) {
         return -1;
     }

     for (i = 0; i < num; i++) {
        memset(&dtc, 0, sizeof(service_pub_info_t));
        ret = udata_dtc_config_parse(i, &dtc);
        if(unlikely(ret)) {
            continue;
        }
        ret = service_dtc_register(&dtc);
        if(dtc.name_addr != NULL) {
            aos_free(dtc.name_addr);
            dtc.name_addr = NULL;
        }
        if(unlikely(ret)) {
            LOG("service_dtc_init type %d skip\n", dtc.type);
        }
    }

    return 0;

}

