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
#include "aos/yloop.h"
#include "aos/network.h"
#include <netmgr.h>
#include <aos/kernel.h>

#include <netmgr.h>
#include <aos/cli.h>
#include <aos/cloud.h>
#include "aos/uData.h"
#include "service_mgr.h"
#include "service_data_to_cloud.h"

#define  DTC_PARA_NUM_1             (1)
#define  DTC_PARA_NUM_4             (4)


#define  DTC_PUBLISH_CYCLE_DEFAULT  (2000)


typedef struct _service_pub_type_t {
    sensor_tag_e  tag;
    udata_type_e  type;
    uint64_t      time_stamp;
    uint32_t      dtc_cycle;
    bool          pub_flag;
    int           name_num;
    void         *name_addr;

} service_pub_info_t;


service_pub_info_t g_service_info[UDATA_MAX_CNT] = {
    { TAG_DEV_ACC,              UDATA_SERVICE_ACC,          0,  DTC_PUBLISH_CYCLE_DEFAULT,  false,    DTC_PARA_NUM_4, NULL},
    { TAG_DEV_MAG,              UDATA_SERVICE_MAG,          0,  DTC_PUBLISH_CYCLE_DEFAULT,  false,    DTC_PARA_NUM_4, NULL},
    { TAG_DEV_GYRO,             UDATA_SERVICE_GYRO,         0,  DTC_PUBLISH_CYCLE_DEFAULT,  false,    DTC_PARA_NUM_4, NULL},

    { TAG_DEV_ALS,              UDATA_SERVICE_ALS,          0,  DTC_PUBLISH_CYCLE_DEFAULT,  false,    DTC_PARA_NUM_1, NULL},
    { TAG_DEV_PS,               UDATA_SERVICE_PS,           0,  DTC_PUBLISH_CYCLE_DEFAULT,  false,    DTC_PARA_NUM_1, NULL},
    { TAG_DEV_BARO,             UDATA_SERVICE_BARO,         0,  DTC_PUBLISH_CYCLE_DEFAULT,  false,    DTC_PARA_NUM_1, NULL},
    { TAG_DEV_TEMP,             UDATA_SERVICE_TEMP,         0,  DTC_PUBLISH_CYCLE_DEFAULT,  false,    DTC_PARA_NUM_1, NULL},
    { TAG_DEV_UV,               UDATA_SERVICE_UV,           0,  DTC_PUBLISH_CYCLE_DEFAULT,  false,    DTC_PARA_NUM_1, NULL},
    { TAG_DEV_HUMI,             UDATA_SERVICE_HUMI,         0,  DTC_PUBLISH_CYCLE_DEFAULT,  false,    DTC_PARA_NUM_1, NULL},
    { TAG_DEV_HALL,             UDATA_SERVICE_HALL,         0,  DTC_PUBLISH_CYCLE_DEFAULT,  false,    DTC_PARA_NUM_1, NULL},
    { TAG_DEV_HR,               UDATA_SERVICE_HR,           0,  DTC_PUBLISH_CYCLE_DEFAULT,  false,    DTC_PARA_NUM_1, NULL},

    { TAG_DEV_SENSOR_NUM_MAX,   UDATA_SERVICE_PEDOMETER,    0,  DTC_PUBLISH_CYCLE_DEFAULT,  false,    DTC_PARA_NUM_1, NULL},
    { TAG_DEV_SENSOR_NUM_MAX,   UDATA_SERVICE_PDR,          0,  DTC_PUBLISH_CYCLE_DEFAULT,  false,    DTC_PARA_NUM_1, NULL},
    { TAG_DEV_SENSOR_NUM_MAX,   UDATA_SERVICE_VDR,          0,  DTC_PUBLISH_CYCLE_DEFAULT,  false,    DTC_PARA_NUM_1, NULL},

    { TAG_DEV_GPS,              UDATA_SERVICE_GPS,          0,  DTC_PUBLISH_CYCLE_DEFAULT,  false,    DTC_PARA_NUM_4, NULL},
};

static bool     g_dtc_conn_flag = false;
static void    *g_dtc_thing_id = NULL;

SET_VALUE_FUNC          g_dtc_set_value_func = NULL;
POST_PROPERTY_FUNC      g_dtc_post_property_func = NULL;


extern long long aos_now_ms(void);
extern int abs_data_timer_start(void);


static int service_dtc_data_set(const char *identifier, const void *value, const char *value_str)
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
    int ret = 0;
    int len = 0;
    accel_data_t *acc;
    char smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        return -1;
    }

    if (NULL ==  data) {
        return -1;
    }

    if (type != g_service_info[type].type) {
        return -1;
    }

    acc = (accel_data_t *)data;
    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d.%3d", acc->data[0] / 1000, acc->data[0] % 1000);
    ret = service_dtc_data_set((g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN), NULL, (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {
        UDATA_ERROR(type, acc->data[0], ret);
        return -1;
    }

    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d.%3d", acc->data[1] / 1000, acc->data[1] % 1000);
    ret = service_dtc_data_set((g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN  * 2), NULL, (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {
        UDATA_ERROR(type, acc->data[1], ret);
        return -1;
    }


    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d.%3d", acc->data[2] / 1000, acc->data[2] % 1000);
    ret = service_dtc_data_set((g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN * 3), NULL,
                                   (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {
        UDATA_ERROR(type, acc->data[2], ret);
        return -1;
    }

    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}


static int service_dtc_mag_publish(udata_type_e type, void *data)
{
    int ret;
    int len = 0;
    mag_data_t *mag;
    char smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        return -1;
    }

    if (NULL ==  data) {
        return -1;
    }

    if (type != g_service_info[type].type) {
        return -1;
    }

    mag = (mag_data_t *)data;
    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d.%3d", mag->data[0] / 1000, mag->data[0] % 1000);
    ret = service_dtc_data_set((g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN), NULL, (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {
        UDATA_ERROR(type, mag->data[0], ret);
        return -1;
    }


    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d.%3d", mag->data[1] / 1000, mag->data[1] % 1000);
    ret = service_dtc_data_set((g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN * 2), NULL, (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {
        UDATA_ERROR(type, mag->data[1], ret);
        return -1;
    }


    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d.%3d", mag->data[2] / 1000, mag->data[2] % 1000);
    ret = service_dtc_data_set((g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN * 3), NULL,
                                   (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {
        UDATA_ERROR(type, mag->data[2], ret);
        return -1;
    }
    
    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}



static int service_dtc_gyro_publish(udata_type_e type, void *data)
{
    int ret;
    int len = 0;
    gyro_data_t *gyro;
    char smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        return -1;
    }

    if (NULL ==  data) {
        return -1;
    }

    if (type != g_service_info[type].type) {
        return -1;
    }

    gyro = (gyro_data_t *)data;
    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d.%6d", gyro->data[0] / 1000000,
                   gyro->data[0] % 1000000);
    ret = service_dtc_data_set((g_service_info[type].name_addr  + SERVICE_PUB_NAME_LEN), NULL, (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {
        UDATA_ERROR(type, gyro->data[0], ret);
        return -1;
    }

    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d.%6d", gyro->data[1] / 1000000,
                   gyro->data[1] % 1000000);
    ret = service_dtc_data_set((g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN * 2), NULL, (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {
        UDATA_ERROR(type, gyro->data[1], ret);
        return -1;
    }


    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d.%6d", gyro->data[2] / 1000000,
                   gyro->data[2] % 1000000);
    ret = service_dtc_data_set((g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN * 3), NULL,
                                   (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {
        UDATA_ERROR(type, gyro->data[2], ret);
        return -1;
    }
    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}


static int service_dtc_als_publish(udata_type_e type, void *data)
{
    int ret;
    int len = 0;
    als_data_t *als;
    char smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        return -1;
    }

    if (NULL ==  data) {
        return -1;
    }

    if (type != g_service_info[type].type) {
        return -1;
    }

    als = (als_data_t *)data;
    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d", als->lux);
    ret = service_dtc_data_set((g_service_info[type].name_addr), NULL, (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {
        return -1;
    }
    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}


static int service_dtc_ps_publish(udata_type_e type, void *data)
{
    int ret;
    int len = 0;
    proximity_data_t *ps;
    char smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        return -1;
    }

    if (NULL ==  data) {
        return -1;
    }

    if (type != g_service_info[type].type) {
        return -1;
    }

    ps = (proximity_data_t *)data;
    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d", ps->present);
    ret = service_dtc_data_set((g_service_info[type].name_addr), NULL, (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {
        return -1;
    }
    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}

static int service_dtc_baro_publish(udata_type_e type, void *data)
{
    int ret;
    int len = 0;
    barometer_data_t *baro;
    char smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        return -1;
    }

    if (NULL ==  data) {
        return -1;
    }

    if (type != g_service_info[type].type) {

        UDATA_ERROR(type, data, g_service_info[type].type);
        return -1;
    }

    baro = (barometer_data_t *)data;
    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d", baro->p);
    ret = service_dtc_data_set((g_service_info[type].name_addr), NULL, (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {
        return -1;
    }
    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}

static int service_dtc_temp_publish(udata_type_e type, void *data)
{
    int ret;
    int len = 0;
    temperature_data_t *temp;
    char smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {

        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (NULL ==  data) {

        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (type != g_service_info[type].type) {
        return -1;
    }

    temp = (temperature_data_t *)data;
    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d.%d", temp->t / 10, temp->t % 10);
    ret = service_dtc_data_set((g_service_info[type].name_addr), NULL, (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {
        UDATA_ERROR(type, data, ret);
        return -1;
    }
    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}



static int service_dtc_uv_publish(udata_type_e type, void *data)
{
    int ret;
    int len = 0;
    uv_data_t *uv;
    char smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (NULL ==  data) {
        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (type != g_service_info[type].type) {

        UDATA_ERROR(type, data, g_service_info[type].type);
        return -1;
    }

    uv = (uv_data_t *)data;
    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d", uv->uvi);
    ret = service_dtc_data_set((g_service_info[type].name_addr), NULL, (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {
        UDATA_ERROR(type, data, ret);
        return -1;
    }
    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}


static int service_dtc_humi_publish(udata_type_e type, void *data)
{
    int ret;
    int len = 0;
    humidity_data_t *humi;
    char smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (NULL ==  data) {
        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (type != g_service_info[type].type) {

        UDATA_ERROR(type, data, g_service_info[type].type);
        return -1;
    }

    humi = (humidity_data_t *)data;
    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d", humi->h / 10);
    ret = service_dtc_data_set((g_service_info[type].name_addr), NULL, (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {
        UDATA_ERROR(type, data, ret);
        return -1;
    }
    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}


static int service_dtc_hall_publish(udata_type_e type, void *data)
{
    int ret;
    int len = 0;
    hall_data_t *hall;
    char smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (NULL ==  data) {
        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (type != g_service_info[type].type) {

        UDATA_ERROR(type, data, g_service_info[type].type);
        return -1;
    }

    hall = (hall_data_t *)data;
    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d", hall->hall_level);
    ret = service_dtc_data_set((g_service_info[type].name_addr), NULL, (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {

        UDATA_ERROR(type, data, ret);
        return -1;
    }
    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}


static int service_dtc_heart_publish(udata_type_e type, void *data)
{
    int ret;
    int len = 0;
    heart_rate_data_t *heart;
    char smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (NULL ==  data) {
        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (type != g_service_info[type].type) {

        UDATA_ERROR(type, data, g_service_info[type].type);
        return -1;
    }

    heart = (heart_rate_data_t *)data;
    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%d", heart->hear_rate);
    ret = service_dtc_data_set((g_service_info[type].name_addr), NULL, (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {
        UDATA_ERROR(type, data, ret);
        return -1;
    }

    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}

static int service_dtc_gps_publish(udata_type_e type, void *data)
{
    int ret;
    int len = 0;
    gps_data_t *gps;
    char smcc_msg_pub[SERVICE_PUB_VALUE_LEN];

    if (type > UDATA_MAX_CNT) {
        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (NULL ==  data) {
        UDATA_ERROR(type, data, 0);
        return -1;
    }

    if (type != g_service_info[type].type) {

        UDATA_ERROR(type, data, g_service_info[type].type);
        return -1;
    }

    gps = (gps_data_t *)data;
    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%f", gps->lat);
    ret = service_dtc_data_set((g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN), NULL, (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {
        UDATA_ERROR(type, data, ret);
        return -1;
    }

    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%f", gps->lon);
    ret = service_dtc_data_set((g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN * 2), NULL, (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {
        UDATA_ERROR(type, data, ret);
        return -1;
    }

    len = snprintf((void *)&smcc_msg_pub[0], sizeof(smcc_msg_pub), "%f", gps->elv);
    ret = service_dtc_data_set((g_service_info[type].name_addr + SERVICE_PUB_NAME_LEN * 3), NULL,
                                   (void *)&smcc_msg_pub[0]);
    if (0 !=  ret) {
        UDATA_ERROR(type, data, ret);
        return -1;
    }
    ret = service_dtc_data_post(g_service_info[type].name_addr);

    return ret;
}

static int service_dtc_publish(udata_type_e type, void *pdata)
{
    int ret = 0;
    int msg_len = 0;

    switch (type) {
        case UDATA_SERVICE_ACC: {
            ret = service_dtc_acc_publish(type, pdata);
            if (ret < 0) {
                return;
            }
            break;
        }

        case UDATA_SERVICE_MAG: {
            ret = service_dtc_mag_publish(type, pdata);
            if (ret < 0) {
                return;
            }
            break;
        }

        case UDATA_SERVICE_GYRO: {
            ret = service_dtc_gyro_publish(type, pdata);
            if (ret < 0) {
                return;
            }
            break;
        }

        case UDATA_SERVICE_ALS: {
            ret = service_dtc_als_publish(type, pdata);
            if (ret < 0) {
                return;
            }
            break;
        }
        case UDATA_SERVICE_PS: {
            ret = service_dtc_ps_publish(type, pdata);
            if (ret < 0) {
                return;
            }
            break;
        }
        case UDATA_SERVICE_BARO: {
            ret = service_dtc_baro_publish(type, pdata);
            if (ret < 0) {
                return;
            }
            break;
        }

        case UDATA_SERVICE_TEMP: {
            ret = service_dtc_temp_publish(type, pdata);
            if (ret < 0) {
                return;
            }
            break;
        }

        case UDATA_SERVICE_UV: {
            ret = service_dtc_uv_publish(type, pdata);
            if (ret < 0) {
                return;
            }
            break;
        }

        case UDATA_SERVICE_HUMI: {
            ret = service_dtc_humi_publish(type, pdata);
            if (ret < 0) {
                return;
            }
            break;
        }

        case UDATA_SERVICE_HALL: {
            ret = service_dtc_hall_publish(type, pdata);
            if (ret < 0) {
                return;
            }
            break;
        }
        case UDATA_SERVICE_HR: {
            ret = service_dtc_heart_publish(type, pdata);
            if (ret < 0) {
                return;
            }
            break;
        }
        case UDATA_SERVICE_GPS: {
            ret = service_dtc_gps_publish(type, pdata);
            if (ret < 0) {
                return;
            }
            break;
        }
        default:
            break;

    }

    return 0;
}



void service_dtc_handle(input_event_t *event, void *priv_data)
{
    udata_pkg_t buf;
    int ret = 0;
    if ((event == NULL) || (event->type != EV_UDATA)) {
        return;
    }

    switch (event->code) {
        case CODE_UDATA_DATA_PUBLISH: {
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
        case CODE_UDATA_DATA_SUBSCRIB: {
            break;
        }
        default:
            break;

    }

}


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

    g_service_info[type].pub_flag = flag;
    g_service_info[type].time_stamp = aos_now_ms();

    return ret;
}

bool service_dtc_is_publish(udata_type_e type)
{
    uint64_t time = aos_now_ms();

    if (type >= UDATA_MAX_CNT) {
        return false;
    }


    if (0 == g_service_info[type].dtc_cycle) {
        return false;
    }

    if ((time - g_service_info[type].time_stamp) >= (uint64_t)g_service_info[type].dtc_cycle) {
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


int service_dtc_register(void *thing_id, SET_VALUE_FUNC set_value,POST_PROPERTY_FUNC post_property)
{
    if ((NULL == set_value) || (NULL == post_property)){
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

    g_dtc_thing_id = thing_id;
    g_dtc_set_value_func = set_value;
    g_dtc_post_property_func = post_property;

    service_dtc_connect_set(true);

    return abs_data_timer_start();
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


    g_service_info[type].name_addr = malloc(g_service_info[type].name_num * SERVICE_PUB_NAME_LEN);
    if (NULL == g_service_info[type].name_addr) {
        return -1;
    }

    return 0;
}



int service_dtc_name_copy(void *dest, void *src, uint32_t maxlen)
{
    uint32_t len = strlen(src);

    if (len >=  maxlen) {
        return -1;
    }

    memcpy(dest, src, len);

    *((char *)(dest + len)) = '\0';

    return 0;
}


int service_dtc_name_set(udata_type_e type, char* src[], int num)
{
    int i = 0;
    int ret = 0;
    void *addr;
    int flag;

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
    ret = snprintf(addr, SERVICE_PUB_NAME_LEN, "%s", src[0]); 
    if (ret < 0) {
        return -1;
    }

    if(num > DTC_PARA_NUM_1){
        for (i = 1; i < num; i++) {
            addr = (char *)(g_service_info[type].name_addr + i * SERVICE_PUB_NAME_LEN);

            ret = snprintf(addr, SERVICE_PUB_NAME_LEN, "%s.%s", src[0], src[i]);
            if (ret < 0) {
                return -1;
            }
        }

    }
   
    (void)service_dtc_publish_set(type, true);

    return 0;

}


int uData_dtc_name_set(udata_type_e type, char* src[], int num)
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
            name_num = DTC_PARA_NUM_4;
            char* acc[DTC_PARA_NUM_4] = {"Accelerometer", "x", "y","z"};

            ret = service_dtc_name_set(type, acc, name_num);
            break;
        }

        case UDATA_SERVICE_MAG: {
            name_num = DTC_PARA_NUM_4;
            char* mag[DTC_PARA_NUM_4] = {"Magnetometer","x_gs", "y_gs", "z_gs"};
            ret = service_dtc_name_set(type, mag, name_num);
            break;
        }

        case UDATA_SERVICE_GYRO: {
            name_num = DTC_PARA_NUM_4;
            char* gyro[DTC_PARA_NUM_4] = {"Gyroscope","x_dps", "y_dps", "z_dps"};
            ret = service_dtc_name_set(type, gyro, name_num);
            break;
        }

        case UDATA_SERVICE_ALS: {
            name_num = DTC_PARA_NUM_1;
            char* als[DTC_PARA_NUM_1] = {"LightLux"};
            ret = service_dtc_name_set(type, als, name_num);
            break;
        }
        case UDATA_SERVICE_PS: {
            name_num = DTC_PARA_NUM_1;
            char* ps[DTC_PARA_NUM_1] =  {"Proximity"};
            ret = service_dtc_name_set(type, ps, name_num);
            break;
        }

        case UDATA_SERVICE_BARO: {
            name_num = DTC_PARA_NUM_1;
            char* baro[DTC_PARA_NUM_1]= {"Barometer"};
            ret = service_dtc_name_set(type, baro, name_num);
            break;
        }

        case UDATA_SERVICE_TEMP: {
            name_num = DTC_PARA_NUM_1;
            char* temp[DTC_PARA_NUM_1] = {"CurrentTemperature"};
            ret = service_dtc_name_set(type, temp, name_num);
            break;
        }

        case UDATA_SERVICE_UV: {
            name_num = DTC_PARA_NUM_1;
            char* uv[DTC_PARA_NUM_1] = {"Ultraviolet"};
            ret = service_dtc_name_set(type, uv, name_num);
            break;
        }

        case UDATA_SERVICE_HUMI: {
            name_num = DTC_PARA_NUM_1;
            char* humi[DTC_PARA_NUM_1] = {"CurrentHumidity"};
            ret = service_dtc_name_set(type, humi, name_num);
            break;
        }

        case UDATA_SERVICE_HALL: {
            name_num = DTC_PARA_NUM_1;
            char* hall[DTC_PARA_NUM_1] = {"Hall_level"};
            ret = service_dtc_name_set(type, hall, name_num);
            break;
        }

        case UDATA_SERVICE_HR: {
            name_num = DTC_PARA_NUM_1;
            char* heart[DTC_PARA_NUM_1] = {"Heart_rate"};
            ret = service_dtc_name_set(type, heart, name_num);
            break;
        }

        case UDATA_SERVICE_GPS: {
            name_num = DTC_PARA_NUM_4;
            char* gps[DTC_PARA_NUM_4] = {"Gps","Latitude","Longitude","Elevation"};
            ret = service_dtc_name_set(type,gps,name_num);
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
        if(ret < 0){
            continue;
        }
        
        LOG("%s %s %d get proc for udata_service %d \n", uDATA_STR, __func__,__LINE__,g_service_info[i].type);
        
        ret = service_dtc_default_name_init(i);
        if (0 != ret) {

            LOG("service_dtc_name_init ret = 0x%08x , service type = %d\n", ret, i);
        }
    }

    return 0;
}

int service_dtc_init()
{
    int ret;

    ret = service_dtc_name_init();
    if (ret != 0) {
        LOG("error occur service_dtc_init \n");
        return ret;
    }

    ret = aos_register_event_filter(EV_UDATA, service_dtc_handle, NULL);
    if (ret != 0) {
        LOG("error occur reg service_dtc_handle \n");
        return ret;
    }

    return 0;
}



