/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 * service data to cloud
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/log.h"
#include "aos/network.h"
#include <netmgr.h>
#include <aos/kernel.h>

#include <netmgr.h>
#include <aos/cli.h>
#include <aos/cloud.h>
#include "aos/uData.h"
#include "service_mgr.h"
#include "service_data_to_cloud.h"

#ifdef UDATA_YLOOP
#include <aos/yloop.h>
#else
#include "uData_queue.h"
#endif


#define DTC_PARA_NUM_1 (1)
#define DTC_PARA_NUM_4 (4)


#define DTC_PUBLISH_CYCLE_DEFAULT (2000)

#define DTC_DATA_CONVERT(a,b) (((float)(a))/((float)(b)))


typedef struct _service_pub_type_t
{
    sensor_tag_e tag;
    udata_type_e type;
    uint64_t     time_stamp;
    uint32_t     dtc_cycle;
    bool         pub_flag;
    int          name_num;
    void *       name_addr;

} service_pub_info_t;


service_pub_info_t g_service_info[UDATA_MAX_CNT] = {
    { TAG_DEV_ACC, UDATA_SERVICE_ACC, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_4, NULL },
    { TAG_DEV_MAG, UDATA_SERVICE_MAG, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_4, NULL },
    { TAG_DEV_GYRO, UDATA_SERVICE_GYRO, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_4, NULL },

    { TAG_DEV_ALS, UDATA_SERVICE_ALS, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_PS, UDATA_SERVICE_PS, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_BARO, UDATA_SERVICE_BARO, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_TEMP, UDATA_SERVICE_TEMP, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_UV, UDATA_SERVICE_UV, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_HUMI, UDATA_SERVICE_HUMI, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_NOISE, UDATA_SERVICE_NOISE, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_PM25, UDATA_SERVICE_PM25, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_CO2, UDATA_SERVICE_CO2, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_HCHO, UDATA_SERVICE_HCHO, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_TVOC, UDATA_SERVICE_TVOC, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_PH, UDATA_SERVICE_PH, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_VWC, UDATA_SERVICE_VWC, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_EC, UDATA_SERVICE_EC, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_SALINITY, UDATA_SERVICE_SALINITY, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_TDS, UDATA_SERVICE_TDS, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_WINDSPD, UDATA_SERVICE_WINDSPD, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_WINDDIR, UDATA_SERVICE_WINDDIR, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_RAIN, UDATA_SERVICE_RAIN, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },     
    { TAG_DEV_HALL, UDATA_SERVICE_HALL, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_HR, UDATA_SERVICE_HR, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_RGB, UDATA_SERVICE_RGB, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_4, NULL },
    { TAG_DEV_GS, UDATA_SERVICE_GS, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },
    { TAG_DEV_IR, UDATA_SERVICE_IR, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_1, NULL },

    { TAG_DEV_SENSOR_NUM_MAX, UDATA_SERVICE_PEDOMETER, 0,
      DTC_PUBLISH_CYCLE_DEFAULT, false, DTC_PARA_NUM_1, NULL },
    { TAG_DEV_SENSOR_NUM_MAX, UDATA_SERVICE_PDR, 0, DTC_PUBLISH_CYCLE_DEFAULT,
      false, DTC_PARA_NUM_1, NULL },
    { TAG_DEV_SENSOR_NUM_MAX, UDATA_SERVICE_VDR, 0, DTC_PUBLISH_CYCLE_DEFAULT,
      false, DTC_PARA_NUM_1, NULL },

    { TAG_DEV_GPS, UDATA_SERVICE_GPS, 0, DTC_PUBLISH_CYCLE_DEFAULT, false,
      DTC_PARA_NUM_4, NULL },
};

static bool  g_dtc_conn_flag = false;
static void *g_dtc_thing_id  = NULL;

SET_VALUE_FUNC     g_dtc_set_value_func     = NULL;
POST_PROPERTY_FUNC g_dtc_post_property_func = NULL;


extern long long aos_now_ms(void);
extern int       abs_data_timer_start(void);
extern int       abs_data_poll_start(void);

static int service_dtc_data_set(const char *identifier, const void *value,
                                const char *value_str)
{
    int ret;

    if (NULL == g_dtc_set_value_func) {
        return -1;
    }
    ret = g_dtc_set_value_func(g_dtc_thing_id, identifier, value, value_str);

    return ret;
}


static int service_dtc_data_post(const char *identifier)
{
    int ret;

    if (NULL == g_dtc_post_property_func) {
        return -1;
    }
    ret = g_dtc_post_property_func(g_dtc_thing_id, identifier);

    return ret;
}

static int service_dtc_acc_publish(udata_type_e type, void *data)
{
    int           ret = 0;
    int           len = 0;
    accel_data_t *acc;
    char          smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        return -1;
    }

    if (NULL == data) {
        return -1;
    }

    if (type != g_service_info[type].type) {
        return -1;
    }

    acc = (accel_data_t *)data;
    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%.3f",
                   DTC_DATA_CONVERT(acc->data[0],1000));
    ret = service_dtc_data_set(
      (g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN), NULL,
      (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        UDATA_ERROR(type, acc->data[0], ret);
        return -1;
    }

    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%.3f",
                   DTC_DATA_CONVERT(acc->data[1],1000));
    ret = service_dtc_data_set(
      (g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN * 2), NULL,
      (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        UDATA_ERROR(type, acc->data[1], ret);
        return -1;
    }


    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%.3f",
                   DTC_DATA_CONVERT(acc->data[2],1000));
    ret = service_dtc_data_set(
      (g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN * 3), NULL,
      (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        UDATA_ERROR(type, acc->data[2], ret);
        return -1;
    }

    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}


static int service_dtc_mag_publish(udata_type_e type, void *data)
{
    int         ret;
    int         len = 0;
    mag_data_t *mag;
    char        smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        return -1;
    }

    if (NULL == data) {
        return -1;
    }

    if (type != g_service_info[type].type) {
        return -1;
    }

    mag = (mag_data_t *)data;
    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%.3f",
                   DTC_DATA_CONVERT(mag->data[0],1000));
    ret = service_dtc_data_set(
      (g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN), NULL,
      (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        UDATA_ERROR(type, mag->data[0], ret);
        return -1;
    }


    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%.3f",
                   DTC_DATA_CONVERT(mag->data[1],1000));
    ret = service_dtc_data_set(
      (g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN * 2), NULL,
      (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        UDATA_ERROR(type, mag->data[1], ret);
        return -1;
    }


    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%.3f",
                   DTC_DATA_CONVERT(mag->data[2],1000));
    ret = service_dtc_data_set(
      (g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN * 3), NULL,
      (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        UDATA_ERROR(type, mag->data[2], ret);
        return -1;
    }

    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}


static int service_dtc_gyro_publish(udata_type_e type, void *data)
{
    int          ret;
    int          len = 0;
    gyro_data_t *gyro;
    char         smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        return -1;
    }

    if (NULL == data) {
        return -1;
    }

    if (type != g_service_info[type].type) {
        return -1;
    }

    gyro = (gyro_data_t *)data;
    len  = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%.6f",
                   DTC_DATA_CONVERT(gyro->data[0],1000000));
    ret  = service_dtc_data_set(
      (g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN), NULL,
      (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        UDATA_ERROR(type, gyro->data[0], ret);
        return -1;
    }

    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%.6f",
                   DTC_DATA_CONVERT(gyro->data[1],1000000));
    ret = service_dtc_data_set(
      (g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN * 2), NULL,
      (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        UDATA_ERROR(type, gyro->data[1], ret);
        return -1;
    }


    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%.6f",
                   DTC_DATA_CONVERT(gyro->data[2],1000000));
    ret = service_dtc_data_set(
      (g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN * 3), NULL,
      (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        UDATA_ERROR(type, gyro->data[2], ret);
        return -1;
    }
    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}


static int service_dtc_als_publish(udata_type_e type, void *data)
{
    int         ret;
    int         len = 0;
    als_data_t *als;
    char        smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        return -1;
    }

    if (NULL == data) {
        return -1;
    }

    if (type != g_service_info[type].type) {
        return -1;
    }

    als = (als_data_t *)data;
    len =
      snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d", als->lux);
    ret = service_dtc_data_set((g_service_info[type].name_addr), NULL,
                               (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        return -1;
    }
    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}


static int service_dtc_ps_publish(udata_type_e type, void *data)
{
    int               ret;
    int               len = 0;
    proximity_data_t *ps;
    char              smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        return -1;
    }

    if (NULL == data) {
        return -1;
    }

    if (type != g_service_info[type].type) {
        return -1;
    }

    ps  = (proximity_data_t *)data;
    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d",
                   ps->present);
    ret = service_dtc_data_set((g_service_info[type].name_addr), NULL,
                               (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        return -1;
    }
    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}

static int service_dtc_uv_publish(udata_type_e type, void *data)
{
    int        ret;
    int        len = 0;
    uv_data_t *uv;
    char       smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (NULL == data) {
        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (type != g_service_info[type].type) {

        UDATA_ERROR(type, data, g_service_info[type].type);
        return -1;
    }

    uv = (uv_data_t *)data;
    len =
      snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d", uv->uvi);
    ret = service_dtc_data_set((g_service_info[type].name_addr), NULL,
                               (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        UDATA_ERROR(type, data, ret);
        return -1;
    }
    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}

static int service_dtc_msg_format(udata_type_e type, integer_data_t *pdata,
                                  char *buffer, int length)
{
    if (type == UDATA_SERVICE_NOISE || type == UDATA_SERVICE_HUMI) {
        snprintf(buffer, length, "%.1f", DTC_DATA_CONVERT(pdata->data,10));
    } else if (type == UDATA_SERVICE_HCHO || type == UDATA_SERVICE_TVOC) {
        snprintf(buffer, length, "%.3f", DTC_DATA_CONVERT(pdata->data,1000));
    } else if (type == UDATA_SERVICE_TEMP) {
        int temp = (int)pdata->data;
        snprintf(buffer, length, "%.1f", DTC_DATA_CONVERT(temp,10));
        
    } else if (type == UDATA_SERVICE_BARO) {
        snprintf(buffer, length, "%.2f", DTC_DATA_CONVERT(pdata->data,100));
    } else {
        snprintf(buffer, length, "%u", pdata->data);
    }
    return 0;
}

static int service_dtc_integer_data_publish(udata_type_e type, void *pdata)
{
    int  ret;
    char smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        UDATA_ERROR(type, pdata, 0);
        return -1;
    }

    if (!pdata) {
        UDATA_ERROR(type, pdata, 0);
        return -1;
    }

    if (g_service_info[type].type != type) {

        UDATA_ERROR(type, pdata, g_service_info[type].type);
        return -1;
    }

    service_dtc_msg_format(type, (integer_data_t *)pdata, smcc_msg_pub,
                           sizeof(smcc_msg_pub));

    ret = service_dtc_data_set((g_service_info[type].name_addr), NULL,
                               (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        UDATA_ERROR(type, pdata, ret);
        return -1;
    }
    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}

static int service_dtc_hall_publish(udata_type_e type, void *data)
{
    int          ret;
    int          len = 0;
    hall_data_t *hall;
    char         smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (NULL == data) {
        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (type != g_service_info[type].type) {

        UDATA_ERROR(type, data, g_service_info[type].type);
        return -1;
    }

    hall = (hall_data_t *)data;
    len  = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d",
                   hall->hall_level);
    ret  = service_dtc_data_set((g_service_info[type].name_addr), NULL,
                               (void *)&smcc_msg_pub[0]);
    if (0 != ret) {

        UDATA_ERROR(type, data, ret);
        return -1;
    }
    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}


static int service_dtc_heart_publish(udata_type_e type, void *data)
{
    int                ret;
    int                len = 0;
    heart_rate_data_t *heart;
    char               smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (NULL == data) {
        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (type != g_service_info[type].type) {

        UDATA_ERROR(type, data, g_service_info[type].type);
        return -1;
    }

    heart = (heart_rate_data_t *)data;
    len   = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d",
                   heart->hear_rate);
    ret   = service_dtc_data_set((g_service_info[type].name_addr), NULL,
                               (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        UDATA_ERROR(type, data, ret);
        return -1;
    }

    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}

static int service_dtc_gps_publish(udata_type_e type, void *data)
{
    int         ret;
    int         len = 0;
    gps_data_t *gps;
    char        smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (NULL == data) {
        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (type != g_service_info[type].type) {

        UDATA_ERROR(type, data, g_service_info[type].type);
        return -1;
    }

    gps = (gps_data_t *)data;
    len =
      snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%f", gps->lat);
    ret = service_dtc_data_set(
      (g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN), NULL,
      (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        UDATA_ERROR(type, data, ret);
        return -1;
    }

    len =
      snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%f", gps->lon);
    ret = service_dtc_data_set(
      (g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN * 2), NULL,
      (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        UDATA_ERROR(type, data, ret);
        return -1;
    }

    len =
      snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%f", gps->elv);
    ret = service_dtc_data_set(
      (g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN * 3), NULL,
      (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        UDATA_ERROR(type, data, ret);
        return -1;
    }
    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}


static int service_dtc_rgb_publish(udata_type_e type, void *data)
{
    int           ret = 0;
    int           len = 0;
    rgb_data_t    *rgb;
    char          smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        return -1;
    }

    if (NULL == data) {
        return -1;
    }

    if (type != g_service_info[type].type) {
        return -1;
    }

    rgb = (accel_data_t *)data;
    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d",
                   rgb->data[0]);
    ret = service_dtc_data_set(
      (g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN), NULL,
      (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        UDATA_ERROR(type, rgb->data[0], ret);
        return -1;
    }

    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d",
                   rgb->data[1]);
    ret = service_dtc_data_set(
      (g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN * 2), NULL,
      (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        UDATA_ERROR(type, rgb->data[1], ret);
        return -1;
    }


    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d",
                   rgb->data[2]);
    ret = service_dtc_data_set(
      (g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN * 3), NULL,
      (void *)&smcc_msg_pub[0]);
    if (0 != ret) {
        UDATA_ERROR(type, rgb->data[2], ret);
        return -1;
    }

    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}


static int service_dtc_publish(udata_type_e type, void *pdata)
{
    int ret     = 0;
    int msg_len = 0;

    switch (type) {
        case UDATA_SERVICE_ACC: {
            ret = service_dtc_acc_publish(type, pdata);
            if (ret < 0) {
                return ret;
            }
            break;
        }

        case UDATA_SERVICE_MAG: {
            ret = service_dtc_mag_publish(type, pdata);
            if (ret < 0) {
                return ret;
            }
            break;
        }

        case UDATA_SERVICE_GYRO: {
            ret = service_dtc_gyro_publish(type, pdata);
            if (ret < 0) {
                return ret;
            }
            break;
        }

        case UDATA_SERVICE_ALS: {
            ret = service_dtc_als_publish(type, pdata);
            if (ret < 0) {
                return ret;
            }
            break;
        }
        case UDATA_SERVICE_PS: {
            ret = service_dtc_ps_publish(type, pdata);
            if (ret < 0) {
                return ret;
            }
            break;
        }

        case UDATA_SERVICE_UV: {
            ret = service_dtc_uv_publish(type, pdata);
            if (ret < 0) {
                return ret;
            }
            break;
        }

        case UDATA_SERVICE_HUMI:
        case UDATA_SERVICE_TEMP:
        case UDATA_SERVICE_BARO:
        case UDATA_SERVICE_NOISE:
        case UDATA_SERVICE_PM25:
        case UDATA_SERVICE_CO2:
        case UDATA_SERVICE_HCHO:
        case UDATA_SERVICE_TVOC:
        case UDATA_SERVICE_PH:
        case UDATA_SERVICE_VWC:
        case UDATA_SERVICE_EC:
        case UDATA_SERVICE_SALINITY:
        case UDATA_SERVICE_TDS:
        case UDATA_SERVICE_WINDSPD:
        case UDATA_SERVICE_WINDDIR:
        case UDATA_SERVICE_RAIN:  
        case UDATA_SERVICE_IR:
        case UDATA_SERVICE_GS:
            ret = service_dtc_integer_data_publish(type, pdata);
            if (ret < 0) {
                return ret;
            }
            break;

        case UDATA_SERVICE_HALL: {
            ret = service_dtc_hall_publish(type, pdata);
            if (ret < 0) {
                return ret;
            }
            break;
        }
        case UDATA_SERVICE_HR: {
            ret = service_dtc_heart_publish(type, pdata);
            if (ret < 0) {
                return ret;
            }
            break;
        }
        case UDATA_SERVICE_GPS: {
            ret = service_dtc_gps_publish(type, pdata);
            if (ret < 0) {
                return ret;
            }
            break;
        }
        case UDATA_SERVICE_RGB: {
            ret = service_dtc_rgb_publish(type, pdata);
            if (ret < 0) {
                return ret;
            }
            break;
        }
        
        default:
            break;
    }

    return 0;
}


#ifdef UDATA_YLOOP
void service_dtc_handle(input_event_t *event, void *priv_data)
{
    udata_pkg_t buf;
    int         ret   = 0;
    int         mtype = uData_find_type(event->value);
    if ((event == NULL) || (event->type != EV_UDATA)) {
        return;
    }
    if ((false == service_dtc_is_connect()) ||
        (false == service_dtc_is_publish(mtype))) {
        return;
    }
    switch (event->code) {
        case CODE_UDATA_REPORT_PUBLISH: {
            ret = uData_report_publish(event, &buf);
            if (ret != 0) {
                return;
            }

            ret = service_dtc_publish(buf.type, (void *)&buf.payload[0]);
            if (ret != 0) {
                return;
            }

            break;
        }
        default:
            break;
    }
}
#else

int uData_msg_report_publish(sensor_msg_pkg_t *msg)
{
    int         ret = 0;
    udata_pkg_t buf;
    if (false == service_dtc_is_connect()) {
        return -1;
    }
    if (false == service_dtc_is_publish(msg->value)) {
        return -1;
    }
    ret = uData_report_publish(msg, &buf);
    if (ret != 0) {
        return -1;
    }
    ret = service_dtc_publish(buf.type, (void *)&buf.payload[0]);
    if (ret != 0) {
        return -1;
    }
    return 0;
}


void service_dtc_handle(sensor_msg_pkg_t *msg)
{
    udata_pkg_t buf;
    int         ret = 0;
    if (msg == NULL) {
        return;
    }

    switch (msg->cmd) {
        case UDATA_MSG_REPORT_PUBLISH: {
            ret = uData_msg_report_publish(msg);
            if (ret != 0) {
                return;
            }
            break;
        }
        default:
            break;
    }
}
#endif

void service_dtc_connect_set(bool flag)
{
    g_dtc_conn_flag = flag;
}


bool service_dtc_is_connect()
{
    return g_dtc_conn_flag;
}

int service_dtc_publish_set(udata_type_e type, bool flag)
{
    int ret;

    if (type >= UDATA_MAX_CNT) {
        return -1;
    }

    if (flag == g_service_info[type].pub_flag) {
        return flag;
    }

    ret = (int)g_service_info[type].pub_flag;

    g_service_info[type].pub_flag   = flag;
    g_service_info[type].time_stamp = aos_now_ms();
    return ret;
}

bool service_dtc_is_publish(udata_type_e type)
{
    uint64_t time = aos_now_ms();

    if (type >= UDATA_MAX_CNT) {
        return false;
    }

    uint64_t tmp = g_service_info[type].time_stamp / 10000;
    if (0 == g_service_info[type].dtc_cycle) {
        return false;
    }

    if ((time - g_service_info[type].time_stamp) >=
        (uint64_t)g_service_info[type].dtc_cycle) {
        g_service_info[type].time_stamp = time;
        return g_service_info[type].pub_flag;
    } else {
        return false;
    }
}

int service_dtc_publish_cycle_set(udata_type_e type, uint32_t cycle)
{

    if (type >= UDATA_MAX_CNT) {
        return -1;
    }
    LOG("service_dtc_publish_cycle_set %d  %d\n", type, cycle);
    g_service_info[type].dtc_cycle = cycle;

    return 0;
}


int service_dtc_unregister(void)
{
    service_dtc_connect_set(false);

    if (NULL != g_dtc_set_value_func) {
        g_dtc_set_value_func = NULL;
    }

    if (NULL != g_dtc_post_property_func) {
        g_dtc_post_property_func = NULL;
    }

    if (NULL != g_dtc_thing_id) {
        g_dtc_thing_id = NULL;
    }

    return 0;
}


int service_dtc_name_addr_alloc(udata_type_e type)
{

    if (type >= UDATA_MAX_CNT) {
        return -1;
    }

    if (0 == g_service_info[type].name_num) {
        return -1;
    }


    g_service_info[type].name_addr =
      aos_malloc(g_service_info[type].name_num * SERVICE_PUB_NAME_LEN);
    if (NULL == g_service_info[type].name_addr) {
        return -1;
    }

    return 0;
}


int service_dtc_name_copy(void *dest, void *src, uint32_t maxlen)
{
    uint32_t len = strlen(src);

    if (len >= maxlen) {
        return -1;
    }

    memcpy(dest, src, len);

    *((char *)(dest + len)) = '\0';

    return 0;
}


int service_dtc_name_set(udata_type_e type, char *src[], int num)
{
    int   i   = 0;
    int   ret = 0;
    void *addr;
    int   flag;

    if (type >= UDATA_MAX_CNT) {
        return -1;
    }

    if (NULL == src) {
        return -1;
    }

    if (0 == num) {
        return -1;
    }

    if (g_service_info[type].type != type) {
        return -1;
    }

    if (NULL == g_service_info[type].name_addr) {
        ret = service_dtc_name_addr_alloc(type);
        if (0 != ret) {
            return -1;
        }
    }

    (void)service_dtc_publish_set(type, false);

    addr = g_service_info[type].name_addr;
    ret  = snprintf(addr, SERVICE_PUB_NAME_LEN, "%s", src[0]);
    if (ret < 0) {
        return -1;
    }

    if (num > DTC_PARA_NUM_1) {
        for (i = 1; i < num; i++) {
            addr = (char *)(g_service_info[type].name_addr +
                            i * SERVICE_PUB_NAME_LEN);

            ret = snprintf(addr, SERVICE_PUB_NAME_LEN, "%s.%s", src[0], src[i]);
            if (ret < 0) {
                return -1;
            }
        }
    }

    (void)service_dtc_publish_set(type, true);

    return 0;
}


int uData_dtc_name_set(udata_type_e type, char *src[], int num)
{
    return service_dtc_name_set(type, src, num);
}


int service_dtc_default_name_init(udata_type_e type)
{
    char len = 0;
    int  name_num;
    int  ret = 0;
    switch (type) {
        case UDATA_SERVICE_ACC: {
            name_num                  = DTC_PARA_NUM_4;
            char *acc[DTC_PARA_NUM_4] = { "Accelerometer", "x", "y", "z" };

            ret = service_dtc_name_set(type, acc, name_num);
            break;
        }

        case UDATA_SERVICE_MAG: {
            name_num                  = DTC_PARA_NUM_4;
            char *mag[DTC_PARA_NUM_4] = { "Magnetometer", "x_gs", "y_gs",
                                          "z_gs" };
            ret = service_dtc_name_set(type, mag, name_num);
            break;
        }

        case UDATA_SERVICE_GYRO: {
            name_num                   = DTC_PARA_NUM_4;
            char *gyro[DTC_PARA_NUM_4] = { "Gyroscope", "x_dps", "y_dps",
                                           "z_dps" };
            ret = service_dtc_name_set(type, gyro, name_num);
            break;
        }

        case UDATA_SERVICE_ALS: {
            name_num                  = DTC_PARA_NUM_1;
            char *als[DTC_PARA_NUM_1] = { "LightLux" };
            ret = service_dtc_name_set(type, als, name_num);
            break;
        }
        case UDATA_SERVICE_PS: {
            name_num                 = DTC_PARA_NUM_1;
            char *ps[DTC_PARA_NUM_1] = { "Proximity" };
            ret                      = service_dtc_name_set(type, ps, name_num);
            break;
        }

        case UDATA_SERVICE_BARO: {
            name_num                   = DTC_PARA_NUM_1;
            char *baro[DTC_PARA_NUM_1] = { "Barometer" };
            ret = service_dtc_name_set(type, baro, name_num);
            break;
        }

        case UDATA_SERVICE_TEMP: {
            name_num                   = DTC_PARA_NUM_1;
            char *temp[DTC_PARA_NUM_1] = { "CurrentTemperature" };
            ret = service_dtc_name_set(type, temp, name_num);
            break;
        }

        case UDATA_SERVICE_NOISE: {
            name_num                   = DTC_PARA_NUM_1;
            char *noise[DTC_PARA_NUM_1] = { "NoiseLoudness" };
            ret = service_dtc_name_set(type, noise, name_num);
            break;
        }

        case UDATA_SERVICE_PM25: {
            name_num                   = DTC_PARA_NUM_1;
            char *pm25[DTC_PARA_NUM_1] = { "PM25Value" };
            ret = service_dtc_name_set(type, pm25, name_num);
            break;
        }

        case UDATA_SERVICE_CO2: {
            name_num                   = DTC_PARA_NUM_1;
            char *co2[DTC_PARA_NUM_1] = { "CO2Value" };
            ret = service_dtc_name_set(type, co2, name_num);
            break;
        }

        case UDATA_SERVICE_HCHO: {
            name_num                   = DTC_PARA_NUM_1;
            char *hcho[DTC_PARA_NUM_1] = { "HCHOLevel" };
            ret = service_dtc_name_set(type, hcho, name_num);
            break;
        }

        case UDATA_SERVICE_TVOC: {
            name_num                   = DTC_PARA_NUM_1;
            char *tvoc[DTC_PARA_NUM_1] = { "TVOCLevel" };
            ret = service_dtc_name_set(type, tvoc, name_num);
            break;
        }
        case UDATA_SERVICE_PH: {
            name_num                   = DTC_PARA_NUM_1;
            char *ph[DTC_PARA_NUM_1] = { "PHLevel" };
            ret = service_dtc_name_set(type, ph, name_num);
            break;
        }
        case UDATA_SERVICE_VWC: {
            name_num                   = DTC_PARA_NUM_1;
            char *vwc[DTC_PARA_NUM_1] = { "VWCLevel" };
            ret = service_dtc_name_set(type, vwc, name_num);
            break;
        }
        case UDATA_SERVICE_EC: {
            name_num                   = DTC_PARA_NUM_1;
            char *ec[DTC_PARA_NUM_1] = { "ECLevel" };
            ret = service_dtc_name_set(type, ec, name_num);
            break;
        }
        case UDATA_SERVICE_SALINITY: {
            name_num                   = DTC_PARA_NUM_1;
            char *salinity[DTC_PARA_NUM_1] = { "SALINITYLevel" };
            ret = service_dtc_name_set(type, salinity, name_num);
            break;
        }
        case UDATA_SERVICE_TDS: {
            name_num                   = DTC_PARA_NUM_1;
            char *tds[DTC_PARA_NUM_1] = { "TDSLevel" };
            ret = service_dtc_name_set(type, tds, name_num);
            break;
        }
        case UDATA_SERVICE_WINDSPD: {
            name_num                   = DTC_PARA_NUM_1;
            char *windspd[DTC_PARA_NUM_1] = { "WINDSPD" };
            ret = service_dtc_name_set(type, windspd, name_num);
            break;
        }
        case UDATA_SERVICE_WINDDIR: {
            name_num                   = DTC_PARA_NUM_1;
            char *winddir[DTC_PARA_NUM_1] = { "WINDDIR" };
            ret = service_dtc_name_set(type, winddir, name_num);
            break;
        }
        case UDATA_SERVICE_RAIN: {
            name_num                   = DTC_PARA_NUM_1;
            char *rain[DTC_PARA_NUM_1] = { "RAIN" };
            ret = service_dtc_name_set(type, rain, name_num);
            break;
        }
        case UDATA_SERVICE_UV: {
            name_num                 = DTC_PARA_NUM_1;
            char *uv[DTC_PARA_NUM_1] = { "Ultraviolet" };
            ret                      = service_dtc_name_set(type, uv, name_num);
            break;
        }

        case UDATA_SERVICE_HUMI: {
            name_num                   = DTC_PARA_NUM_1;
            char *humi[DTC_PARA_NUM_1] = { "CurrentHumidity" };
            ret = service_dtc_name_set(type, humi, name_num);
            break;
        }

        case UDATA_SERVICE_HALL: {
            name_num                   = DTC_PARA_NUM_1;
            char *hall[DTC_PARA_NUM_1] = { "Hall_level" };
            ret = service_dtc_name_set(type, hall, name_num);
            break;
        }

        case UDATA_SERVICE_HR: {
            name_num                    = DTC_PARA_NUM_1;
            char *heart[DTC_PARA_NUM_1] = { "Heart_rate" };
            ret = service_dtc_name_set(type, heart, name_num);
            break;
        }

        case UDATA_SERVICE_GPS: {
            name_num                  = DTC_PARA_NUM_4;
            char *gps[DTC_PARA_NUM_4] = { "Gps", "Latitude", "Longitude",
                                          "Elevation" };
            ret = service_dtc_name_set(type, gps, name_num);
            break;
        }
        case UDATA_SERVICE_IR: {
            name_num                    = DTC_PARA_NUM_1;
            char *ir[DTC_PARA_NUM_1] = { "IR" };
            ret = service_dtc_name_set(type, ir, name_num);
            break;
        }
        case UDATA_SERVICE_GS: {
            name_num                    = DTC_PARA_NUM_1;
            char *gs[DTC_PARA_NUM_1] = { "Gesture" };
            ret = service_dtc_name_set(type, gs, name_num);
            break;
        }
        case UDATA_SERVICE_RGB: {
            name_num                  = DTC_PARA_NUM_4;
            char *rgb[DTC_PARA_NUM_4] = { "RGB", "Red", "Green",
                                          "Blue" };
            ret = service_dtc_name_set(type, rgb, name_num);
            break;
        }

        default: {
            ret = -1;
            break;
        }
    }

    return ret;
}


int service_dtc_name_init(void)
{
    int i;
    int ret;

    for (i = 0; i < UDATA_MAX_CNT; i++) {

        if (i != g_service_info[i].type) {
            continue;
        }

        if (0 == g_service_info[i].name_num) {
            continue;
        }

        ret = uData_find_service(g_service_info[i].type);
        if (ret < 0) {
            continue;
        }

        LOG("%s %s %d get proc for udata_service %d \n", uDATA_STR, __func__,
            __LINE__, g_service_info[i].type);

        ret = service_dtc_default_name_init(i);
        if (0 != ret) {

            LOG("service_dtc_name_init ret = 0x%08x , service type = %d\n", ret,
                i);
        }
    }

    return 0;
}

int service_dtc_register(void *thing_id, SET_VALUE_FUNC set_value,
                         POST_PROPERTY_FUNC post_property)
{
    int ret;
    if ((NULL == set_value) || (NULL == post_property)) {
        return -1;
    }
    if (NULL == thing_id) {
        return -1;
    }

    if (NULL != g_dtc_thing_id) {
        return -1;
    }

    if ((NULL != g_dtc_set_value_func) || (NULL != g_dtc_post_property_func)) {
        return -1;
    }

    ret = service_dtc_name_init();
    if (ret != 0) {
        LOG("error occur service_dtc_init \n");
        return ret;
    }


#ifdef UDATA_YLOOP
    ret = aos_register_event_filter(EV_UDATA, service_dtc_handle, NULL);
    if (ret != 0) {
        LOG("error occur reg service_dtc_handle \n");
        return ret;
    }
#else
    ret = uData_register_msg_handler(service_dtc_handle);
    LOG("uData_queue_registerslot service_dtc_handle ret=%d\n", ret);

    if (ret == -1) {
        LOG("error occur reg service_dtc_handle \n");
        return ret;
    }
#endif

    g_dtc_thing_id           = thing_id;
    g_dtc_set_value_func     = set_value;
    g_dtc_post_property_func = post_property;

    service_dtc_connect_set(true);
    LOG("service_dtc_start sucessfully\n");
    return 0;
}
