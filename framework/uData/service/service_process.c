/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
* uData service process example
*/

#include "service_mgr.h"

typedef int (*udata_service_fuc)(void);
typedef struct udata_service_info {
    sensor_tag_e        sensor_tag;
    udata_type_e        service_type;
    udata_service_fuc   service_func;

} udata_service_info;


#ifdef AOS_UDATA_SERVICE_ACC
static int udata_acc_service_ioctl_cb(udata_type_e type, sensor_tag_e tag)
{
    (void)type;
    (void)tag;

    return 0;
}

static size_t udata_acc_service_process_cb(sensor_tag_e tag, void *pdata)
{
    accel_data_t *acc =  (accel_data_t *)pdata;
    size_t len = sizeof(accel_data_t);

    LOG("%s udata_acc_service_cb = (%d), (%d,%d,%d), (%d)\n", uDATA_STR, tag, acc->data[0], acc->data[1], acc->data[2],
        acc->timestamp);
    return len;
}


static int udata_acc_service_register(void)
{
    int ret = 0;
    uData_service_t *acc;
    acc = (uData_service_t *)malloc(sizeof(uData_service_t));
    if (acc == NULL) {
        return -1;
    }
    acc->type = UDATA_SERVICE_ACC;
    acc->tag = TAG_DEV_ACC;
    acc->config.id = SENSOR_IOCTL_ODR_SET;
    acc->config.odr = 10; /* 10Hz */
    acc->config.range = 0; /* no need here, set by the default value in the driver layer */
    acc->service_process_cb = udata_acc_service_process_cb;
    acc->service_ioctl_cb = udata_acc_service_ioctl_cb;
    ret = uData_service_register(acc);
    if (unlikely(ret)) {
        free(acc);
        LOG("%s %s %s %d\n",  uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    free(acc);
    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}

#endif
int udata_acc_service_init(void)
{
    int ret = 0;

#ifdef AOS_UDATA_SERVICE_ACC
    ret = udata_acc_service_register();
#endif

    return ret;
}


#ifdef AOS_UDATA_SERVICE_GYRO
static int udata_gyro_service_ioctl_cb(udata_type_e type, sensor_tag_e tag)
{
    (void)type;
    (void)tag;

    return 0;
}

static size_t udata_gyro_service_process_cb(sensor_tag_e tag, void *pdata)
{
    gyro_data_t *gyro =  (gyro_data_t *)pdata;
    size_t len = sizeof(gyro_data_t);

    LOG("%s udata_gyro_service_cb = (%d), (%d,%d,%d), (%d)\n", uDATA_STR, tag, gyro->data[0], gyro->data[1], gyro->data[2],
        gyro->timestamp);
    return len;
}

static int udata_gyro_service_register(void)
{
    int ret = 0;
    uData_service_t *gyro;
    gyro = (uData_service_t *)malloc(sizeof(uData_service_t));
    if (gyro == NULL) {
        return -1;
    }
    gyro->type = UDATA_SERVICE_GYRO;
    gyro->tag = TAG_DEV_GYRO;
    gyro->config.id = SENSOR_IOCTL_ODR_SET;
    gyro->config.odr = 10; /* 10Hz */
    gyro->config.range = 0; /* no need here, set by the default value in the driver layer */
    gyro->service_process_cb = udata_gyro_service_process_cb;
    gyro->service_ioctl_cb = udata_gyro_service_ioctl_cb;
    ret = uData_service_register(gyro);
    if (unlikely(ret)) {
        free(gyro);
        LOG("%s %s %s %d\n",  uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    free(gyro);
    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}

#endif
int udata_gyro_service_init(void)
{
    int ret = 0;

#ifdef AOS_UDATA_SERVICE_GYRO
    ret = udata_gyro_service_register();
#endif

    return ret;
}

#ifdef AOS_UDATA_SERVICE_MAG
static int udata_mag_service_ioctl_cb(udata_type_e type, sensor_tag_e tag)
{
    (void)type;
    (void)tag;

    return 0;
}

static size_t udata_mag_service_process_cb(sensor_tag_e tag, void *pdata)
{
    mag_data_t *mag =  (mag_data_t *)pdata;
    size_t len = sizeof(mag_data_t);

    LOG("%s udata_mag_service_cb = (%d), (%d,%d,%d), (%d)\n", uDATA_STR, tag, mag->data[0], mag->data[1], mag->data[2],
        mag->timestamp);
    return len;
}

static int udata_mag_service_register(void)
{
    int ret = 0;
    uData_service_t *mag;
    mag = (uData_service_t *)malloc(sizeof(uData_service_t));
    if (mag == NULL) {
        return -1;
    }
    mag->type = UDATA_SERVICE_MAG;
    mag->tag = TAG_DEV_MAG;
    mag->config.id = SENSOR_IOCTL_ODR_SET;
    mag->config.odr = 10; /* 10Hz */
    mag->config.range = 0; /* no need here, set by the default value in the driver layer */
    mag->service_process_cb = udata_mag_service_process_cb;
    mag->service_ioctl_cb = udata_mag_service_ioctl_cb;
    ret = uData_service_register(mag);
    if (unlikely(ret)) {
        free(mag);
        LOG("%s %s %s %d\n",  uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    free(mag);
    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}

#endif
int udata_mag_service_init(void)
{
    int ret = 0;

#ifdef AOS_UDATA_SERVICE_MAG
    ret = udata_mag_service_register();
#endif

    return ret;
}


#ifdef AOS_UDATA_SERVICE_BARO

static int udata_baro_service_ioctl_cb(udata_type_e type, sensor_tag_e tag)
{

    (void)type;
    (void)tag;
    return 0;
}

static size_t udata_baro_service_process_cb(sensor_tag_e tag, void *pdata)
{
    barometer_data_t *baro =  (barometer_data_t *)pdata;
    size_t len = sizeof(barometer_data_t);

    LOG("%s udata_baro_service_cb = (%d), (%d), (%d)\n", uDATA_STR, tag, baro->p, baro->timestamp);
    return len;
}

static int udata_baro_service_register(void)
{
    int ret = 0;
    uData_service_t *baro;
    baro = (uData_service_t *)malloc(sizeof(uData_service_t));
    if (baro == NULL) {
        return -1;
    }
    baro->type = UDATA_SERVICE_BARO;
    baro->tag = TAG_DEV_BARO;
    baro->config.id = SENSOR_IOCTL_ODR_SET;
    baro->config.odr = 1; /* 1Hz */
    baro->config.range = 0; /* no need here, set by the default value in the driver layer */
    baro->service_process_cb = udata_baro_service_process_cb;
    baro->service_ioctl_cb = udata_baro_service_ioctl_cb;
    ret = uData_service_register(baro);
    if (unlikely(ret)) {
        free(baro);
        LOG("%s %s %s %d\n",  uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    free(baro);
    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}

#endif

int udata_baro_service_init(void)
{
    int ret = 0;

#ifdef AOS_UDATA_SERVICE_BARO
    ret = udata_baro_service_register();
#endif
    return 0;
}

#ifdef AOS_UDATA_SERVICE_TEMP

static int udata_temp_service_ioctl_cb(udata_type_e type, sensor_tag_e tag)
{

    (void)type;
    (void)tag;
    return 0;
}

static size_t udata_temp_service_process_cb(sensor_tag_e tag, void *pdata)
{
    temperature_data_t *temp =  (temperature_data_t *)pdata;
    size_t len = sizeof(temperature_data_t);

    LOG("%s udata_temp_service_cb = (%d), (%d), (%d)\n", uDATA_STR, tag, temp->t, temp->timestamp);
    return len;
}

static int udata_temp_service_register(void)
{
    int ret = 0;
    uData_service_t *temp;

    temp = (uData_service_t *)malloc(sizeof(uData_service_t));
    if (temp == NULL) {
        return -1;
    }

    temp->type = UDATA_SERVICE_TEMP;
    temp->tag = TAG_DEV_TEMP;
    temp->config.id = SENSOR_IOCTL_ODR_SET;
    temp->config.odr = 1; /* 1Hz */
    temp->config.range = 0; /* no need here, set by the default value in the driver layer */
    temp->service_process_cb = udata_temp_service_process_cb;
    temp->service_ioctl_cb = udata_temp_service_ioctl_cb;
    ret = uData_service_register(temp);
    if (unlikely(ret)) {
        free(temp);
        LOG("%s %s %s %d  fail\n",  uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    free(temp);
    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}

#endif
int udata_temp_service_init(void)
{
    int ret = 0;

#ifdef AOS_UDATA_SERVICE_TEMP
    ret = udata_temp_service_register();
#endif

    return ret;
}



#ifdef AOS_UDATA_SERVICE_UV

static int udata_uv_service_ioctl_cb(udata_type_e type, sensor_tag_e tag)
{

    (void)type;
    (void)tag;
    return 0;
}

static size_t udata_uv_service_process_cb(sensor_tag_e tag, void *pdata)
{
    uv_data_t *uv =  (uv_data_t *)pdata;
    size_t len = sizeof(uv_data_t);

    LOG("%s udata_uv_service_cb = (%d), (%d), (%d)\n", uDATA_STR, tag, uv->uvi, uv->timestamp);
    return len;
}

static int udata_uv_service_register(void)
{
    int ret = 0;
    uData_service_t *uv;

    uv = (uData_service_t *)malloc(sizeof(uData_service_t));
    if (uv == NULL) {
        return -1;
    }


    uv->type = UDATA_SERVICE_UV;
    uv->tag = TAG_DEV_UV;
    uv->config.id = SENSOR_IOCTL_ODR_SET;
    uv->config.odr = 1; /* 1Hz */
    uv->config.range = 0; /* no need here, set by the default value in the driver layer */
    uv->service_process_cb = udata_uv_service_process_cb;
    uv->service_ioctl_cb = udata_uv_service_ioctl_cb;
    ret = uData_service_register(uv);
    if (unlikely(ret)) {
        free(uv);
        LOG("%s %s %s %d  fail\n",  uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    free(uv);
    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}

#endif
int udata_uv_service_init(void)
{
    int ret = 0;

#ifdef AOS_UDATA_SERVICE_UV
    ret = udata_uv_service_register();
#endif

    return ret;
}


#ifdef AOS_UDATA_SERVICE_HUMI

static int udata_humi_service_ioctl_cb(udata_type_e type, sensor_tag_e tag)
{
    (void)type;
    (void)tag;

    return 0;
}

static size_t udata_humi_service_process_cb(sensor_tag_e tag, void *pdata)
{
    humidity_data_t *humi =  (humidity_data_t *)pdata;
    size_t len = sizeof(humidity_data_t);

    LOG("%s udata_humi_service_cb = (%d), (%d), (%d)\n", uDATA_STR, tag, humi->h, humi->timestamp);
    return len;
}

static int udata_humi_service_register(void)
{
    int ret = 0;
    uData_service_t *humi;
    humi = (uData_service_t *)malloc(sizeof(uData_service_t));
    if (humi == NULL) {
        return -1;
    }
    humi->type = UDATA_SERVICE_HUMI;
    humi->tag = TAG_DEV_HUMI;
    humi->config.id = SENSOR_IOCTL_ODR_SET;
    humi->config.odr = 1; /* 1Hz */
    humi->config.range = 0; /* no need here, set by the default value in the driver layer */
    humi->service_process_cb = udata_humi_service_process_cb;
    humi->service_ioctl_cb = udata_humi_service_ioctl_cb;
    ret = uData_service_register(humi);
    if (unlikely(ret)) {
        free(humi);
        LOG("%s %s %s %d type\n",  uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    free(humi);
    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}

#endif
int udata_humi_service_init(void)
{
    int ret = 0;

#ifdef AOS_UDATA_SERVICE_HUMI
    ret = udata_humi_service_register();
#endif

    return ret;
}



#ifdef AOS_UDATA_SERVICE_HALL

static int udata_hall_service_ioctl_cb(udata_type_e type, sensor_tag_e tag)
{

    (void)type;
    (void)tag;
    return 0;
}

static size_t udata_hall_service_process_cb(sensor_tag_e tag, void *pdata)
{
    hall_data_t *hall =  (hall_data_t *)pdata;
    size_t len = sizeof(hall_data_t);

    LOG("%s udata_hall_service_cb = (%d), (%d), (%d)\n", uDATA_STR, tag, hall->hall_level, hall->timestamp);
    return len;
}

static int udata_hall_service_result(void *pdata)
{
    return 0;
}

static int udata_hall_service_register(void)
{
    int ret = 0;
    uData_service_t *hall;

    hall = (uData_service_t *)malloc(sizeof(uData_service_t));
    if (hall == NULL) {
        return -1;
    }

    hall->type = UDATA_SERVICE_HALL;
    hall->tag = TAG_DEV_HALL;
    hall->config.id = SENSOR_IOCTL_ODR_SET;
    hall->config.odr = 1; /* 1Hz */
    hall->config.range = 0; /* no need here, set by the default value in the driver layer */
    hall->service_process_cb = udata_hall_service_process_cb;
    hall->service_ioctl_cb = udata_hall_service_ioctl_cb;
    ret = uData_service_register(hall);
    if (unlikely(ret)) {
        free(hall);
        LOG("%s %s %s %d  fail\n",  uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    free(hall);
    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}

#endif
int udata_hall_service_init(void)
{
    int ret = 0;

#ifdef AOS_UDATA_SERVICE_HALL
    ret = udata_hall_service_register();
#endif

    return ret;
}

#ifdef AOS_UDATA_SERVICE_HEART

static int udata_heart_service_ioctl_cb(udata_type_e type, sensor_tag_e tag)
{

    (void)type;
    (void)tag;
    return 0;
}

static size_t udata_heart_service_process_cb(sensor_tag_e tag, void *pdata)
{
    heart_rate_data_t *heart =  (heart_rate_data_t *)pdata;
    size_t len = sizeof(heart_rate_data_t);

    LOG("%s udata_heart_service_cb = (%d), (%d), (%d)\n", uDATA_STR, tag, heart->hear_rate, heart->timestamp);
    return len;
}

static int udata_heart_service_register(void)
{
    int ret = 0;
    uData_service_t *heart;

    heart = (uData_service_t *)malloc(sizeof(uData_service_t));
    if (heart == NULL) {
        return -1;
    }

    heart->type = UDATA_SERVICE_HR;
    heart->tag = TAG_DEV_HR;
    heart->config.id = SENSOR_IOCTL_ODR_SET;
    heart->config.odr = 1; /* 1Hz */
    heart->config.range = 0; /* no need here, set by the default value in the driver layer */
    heart->service_process_cb = udata_heart_service_process_cb;
    heart->service_ioctl_cb = udata_heart_service_ioctl_cb;
    ret = uData_service_register(heart);
    if (unlikely(ret)) {
        free(heart);
        LOG("%s %s %s %d  fail\n",  uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    free(heart);
    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}

#endif
int udata_heart_service_init(void)
{
    int ret = 0;

#ifdef AOS_UDATA_SERVICE_HEART
    ret = udata_heart_service_register();
#endif

    return ret;
}


#ifdef AOS_UDATA_SERVICE_ALS


static int udata_als_service_ioctl_cb(udata_type_e type, sensor_tag_e tag)
{
    (void)type;
    (void)tag;

    return 0;
}

static size_t udata_als_service_process_cb(sensor_tag_e tag, void *pdata)
{
    als_data_t *als =  (als_data_t *)pdata;
    size_t len = sizeof(als_data_t);

    LOG("%s udata_als_service_cb = (%d), (%d), (%d) \n", uDATA_STR, tag, als->lux, (uint32_t)(als->timestamp));

    return len;
}

static int udata_als_service_register(void)
{
    int ret = 0;
    uData_service_t *als;
    als = (uData_service_t *)malloc(sizeof(uData_service_t));
    if (als == NULL) {
        return -1;
    }

    als->type = UDATA_SERVICE_ALS;
    als->tag = TAG_DEV_ALS;
    als->config.id = SENSOR_IOCTL_ODR_SET;
    als->config.odr = 1; /* 1Hz */
    als->config.range = 0; /* no need here, set by the default value in the driver layer */
    als->service_process_cb = udata_als_service_process_cb;
    als->service_ioctl_cb = udata_als_service_ioctl_cb;
    ret = uData_service_register(als);
    if (unlikely(ret)) {
        free(als);
        LOG("%s %s %s %d\n",  uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    free(als);
    LOG("%s %s successfully !\n", uDATA_STR, __func__);
    return 0;
}

#endif
int udata_als_service_init(void)
{
    int ret = 0;

#ifdef AOS_UDATA_SERVICE_ALS
    ret = udata_als_service_register();
#endif

    return ret;
}


#ifdef AOS_UDATA_SERVICE_PS
static int udata_ps_service_ioctl_cb(udata_type_e type, sensor_tag_e tag)
{
    (void)type;
    (void)tag;

    return 0;
}

static size_t udata_ps_service_process_cb(sensor_tag_e tag, void *pdata)
{
    proximity_data_t *ps =  (proximity_data_t *)pdata;
    size_t len = sizeof(proximity_data_t);

    LOG("%s udata_ps_service_cb = (%d), (%d), (%d)\n", uDATA_STR, tag, ps->present, ps->timestamp);
    return len;
}

static int udata_ps_service_register(void)
{
    int ret = 0;
    uData_service_t *ps;
    ps = (uData_service_t *)malloc(sizeof(uData_service_t));
    if (ps == NULL) {
        return -1;
    }
    ps->type = UDATA_SERVICE_PS;
    ps->tag = TAG_DEV_PS;
    ps->config.id = SENSOR_IOCTL_ODR_SET;
    ps->config.odr = 1; /* 1Hz */
    ps->config.range = 0; /* no need here, set by the default value in the driver layer */
    ps->service_process_cb = udata_ps_service_process_cb;
    ps->service_ioctl_cb = udata_ps_service_ioctl_cb;
    ret = uData_service_register(ps);
    if (unlikely(ret)) {
        free(ps);
        LOG("%s %s %s %d\n",  uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    free(ps);
    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}

#endif
int udata_ps_service_init(void)
{
    int ret = 0;

#ifdef AOS_UDATA_SERVICE_PS
    ret = udata_ps_service_register();
#endif

    return ret;
}

#ifdef AOS_UDATA_SERVICE_GPS

/*read 4 times beforce report once */
#define  SERVICE_GPS_REORT_FREQ_REDUCE     4
static  int g_gpsreadcount = 0;

static int udata_gps_service_ioctl_cb(udata_type_e type, sensor_tag_e tag)
{
    (void)type;
    (void)tag;

    return 0;
}

static size_t udata_gps_service_process_cb(sensor_tag_e tag, void *pdata)
{
    gps_data_t *gps =  (gps_data_t *)pdata;
    size_t len = sizeof(gps_data_t);

    LOG("%s udata_gps_service_process_cb = (%d), (%f), (%f)\n", uDATA_STR, tag, gps->lon, gps->lat);

    if (0 == ((g_gpsreadcount++) % SERVICE_GPS_REORT_FREQ_REDUCE)) {
        return len;
    } else {
        return 0;
    }
}

static int udata_gps_service_result(void *pdata)
{
    return 0;
}

static int udata_gps_service_register(void)
{
    int ret = 0;
    uData_service_t *gps;
    gps = (uData_service_t *)malloc(sizeof(uData_service_t));
    if (gps == NULL) {
        return -1;
    }
    gps->type = UDATA_SERVICE_GPS;
    gps->tag = TAG_DEV_GPS;
    gps->config.id = SENSOR_IOCTL_ODR_SET;
    gps->config.odr = 1; /* 1Hz */
    gps->config.range = 0; /* no need here, set by the default value in the driver layer */
    gps->service_process_cb = udata_gps_service_process_cb;
    gps->service_ioctl_cb = udata_gps_service_ioctl_cb;
    ret = uData_service_register(gps);
    if (unlikely(ret)) {
        free(gps);
        LOG("%s %s %s %d\n",  uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    free(gps);
    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}

#endif

int udata_gps_service_init(void)
{
    int ret = 0;

#ifdef AOS_UDATA_SERVICE_GPS
    ret = udata_gps_service_register();
#endif
    return 0;
}


udata_service_info g_udata_service[UDATA_MAX_CNT] = {
    {TAG_DEV_ACC,               UDATA_SERVICE_ACC ,        udata_acc_service_init},
    {TAG_DEV_MAG,               UDATA_SERVICE_MAG,         udata_mag_service_init},
    {TAG_DEV_GYRO,              UDATA_SERVICE_GYRO,        udata_gyro_service_init},
    {TAG_DEV_ALS,               UDATA_SERVICE_ALS,         udata_als_service_init},
    {TAG_DEV_PS,                UDATA_SERVICE_PS,          udata_ps_service_init},
    {TAG_DEV_BARO,              UDATA_SERVICE_BARO,        udata_baro_service_init},
    {TAG_DEV_TEMP,              UDATA_SERVICE_TEMP,        udata_temp_service_init},
    {TAG_DEV_UV,                UDATA_SERVICE_UV,          udata_uv_service_init},
    {TAG_DEV_HUMI,              UDATA_SERVICE_HUMI,        udata_humi_service_init},
    {TAG_DEV_HALL,              UDATA_SERVICE_HALL,        udata_hall_service_init},
    {TAG_DEV_HR,                UDATA_SERVICE_HR,          udata_heart_service_init},
    {TAG_DEV_SENSOR_NUM_MAX,    UDATA_SERVICE_PEDOMETER,   NULL},
    {TAG_DEV_SENSOR_NUM_MAX,    UDATA_SERVICE_PDR,         NULL},
    {TAG_DEV_SENSOR_NUM_MAX,    UDATA_SERVICE_VDR,         NULL},
    {TAG_DEV_GPS,               UDATA_SERVICE_GPS,         udata_gps_service_init}
};

int service_process_init(void)
{
    int ret;
    int i, j;

    for (i = 0; i < UDATA_MAX_CNT; i++) {

        ret = abs_data_check_dev_tree(g_udata_service[i].sensor_tag);
        if (true != ret) {

            continue;
        }
        
        LOG("%s %s %d get drv for udata_service %d \n", uDATA_STR, __func__, __LINE__, g_udata_service[i].sensor_tag);

        if (NULL !=  g_udata_service[i].service_func) {

            ret = g_udata_service[i].service_func();
            if (0 != ret) {
                LOG("%s %s %d fail \n", uDATA_STR, __func__, __LINE__);
            }
        } else {
            LOG("%s %s %d no func for tag %d \n", uDATA_STR, __func__, __LINE__, g_udata_service[i].sensor_tag);
        }

    }

    return 0;
}



