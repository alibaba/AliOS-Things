/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 * uData service process example
 */

#include <stdlib.h>
#include "service_mgr.h"
#include "abs_data_model.h"

static sensor_tag_e udata_service_type_to_tag(udata_type_e type)
{
    switch (type) {
        case UDATA_SERVICE_ACC:
            return TAG_DEV_ACC;
        case UDATA_SERVICE_MAG:
            return TAG_DEV_MAG;
        case UDATA_SERVICE_GYRO:
            return TAG_DEV_GYRO;
        case UDATA_SERVICE_ALS:
            return TAG_DEV_ALS;
        case UDATA_SERVICE_PS:
            return TAG_DEV_PS;
        case UDATA_SERVICE_BARO:
            return TAG_DEV_BARO;
        case UDATA_SERVICE_TEMP:
            return TAG_DEV_TEMP;
        case UDATA_SERVICE_HUMI:
            return TAG_DEV_HUMI;
        case UDATA_SERVICE_UV:
            return TAG_DEV_UV;
        case UDATA_SERVICE_NOISE:
            return TAG_DEV_NOISE;
        case UDATA_SERVICE_PM25:
            return TAG_DEV_PM25;
        case UDATA_SERVICE_PM1P0:
            return TAG_DEV_PM1P0;
        case UDATA_SERVICE_PM10:
            return TAG_DEV_PM10;
        case UDATA_SERVICE_CO2:
            return TAG_DEV_CO2;
        case UDATA_SERVICE_HCHO:
            return TAG_DEV_HCHO;
        case UDATA_SERVICE_TVOC:
            return TAG_DEV_TVOC;
        case UDATA_SERVICE_PH:
            return TAG_DEV_PH;
        case UDATA_SERVICE_VWC:
            return TAG_DEV_VWC;
        case UDATA_SERVICE_EC:
            return TAG_DEV_EC;
        case UDATA_SERVICE_SALINITY:
            return TAG_DEV_SALINITY;
        case UDATA_SERVICE_TDS:
            return TAG_DEV_TDS;
        case UDATA_SERVICE_WINDSPD:
            return TAG_DEV_WINDSPD;
        case UDATA_SERVICE_WINDDIR:
            return TAG_DEV_WINDDIR;
        case UDATA_SERVICE_RAIN:
            return TAG_DEV_RAIN;
        case UDATA_SERVICE_HALL:
            return TAG_DEV_HALL;
        case UDATA_SERVICE_HR:
            return TAG_DEV_HR;
        case UDATA_SERVICE_RGB:
            return TAG_DEV_RGB;
        case UDATA_SERVICE_GS:
            return TAG_DEV_GS;
        case UDATA_SERVICE_IR:
            return TAG_DEV_IR;
        case UDATA_SERVICE_PEDOMETER:
        case UDATA_SERVICE_PDR:
        case UDATA_SERVICE_VDR:
            return TAG_DEV_SENSOR_NUM_MAX;
        case UDATA_SERVICE_GPS:
            return TAG_DEV_GPS;
        default:
            break;
    }
    return 0;
}


int udata_service_odr(sensor_tag_e tag)
{
    int odr = 1;

    switch (tag) {
#ifdef UDATA_MODBUS
        case TAG_DEV_TEMP:
            odr = 3000;
            break;
        case TAG_DEV_HUMI:
            odr = 5000;
            break;
        case TAG_DEV_NOISE:
            odr = 1000;
            break;
        case TAG_DEV_BARO:
            odr = 6000;
            break;
        case TAG_DEV_TVOC:
            odr = 8000;
            break;
        case TAG_DEV_PM25:
            odr = 10000;
            break;
        case TAG_DEV_PM1P0:
            odr = 10000;
            break;
        case TAG_DEV_PM10:
            odr = 10000;
            break;
        case TAG_DEV_HCHO:
            odr = 15000;
            break;
        case TAG_DEV_CO2:
            odr = 7000;
            break;
#endif
        case TAG_DEV_ACC:
        case TAG_DEV_GYRO:
        case TAG_DEV_MAG:
            odr = 10;
            break;
        default:
            odr = 1;
            break;
    }

    return odr;
}


static int udata_std_service_ioctl_cb(udata_type_e type, sensor_tag_e tag)
{
    (void)type;
    (void)tag;
    return 0;
}

static size_t udata_std_service_process_callback(sensor_tag_e tag, void *arg,
                                                 uint32_t len)
{

#ifdef UDATA_DISPLAY
    sensor_display_data_refresh(tag, arg);
#endif

    return DATA_SIZE;
}

static int udata_std_service_register(udata_type_e type)
{
    uData_service_t *svc;

    svc = (uData_service_t *)aos_malloc(sizeof(uData_service_t));
    if (!svc) {
        return -1;
    }

    svc->type       = type;
    svc->tag        = udata_service_type_to_tag(type);
    svc->config.id  = SENSOR_IOCTL_ODR_SET;
    svc->config.odr = udata_service_odr(svc->tag);
    svc->config.range =
      0; /* no need here, set by the default value in the driver layer */
    svc->service_process_cb = udata_std_service_process_callback;
    svc->service_ioctl_cb   = udata_std_service_ioctl_cb;

    if (uData_service_register(svc)) {
        free(svc);
        LOG("%s register service type %d failed !\n", uDATA_STR, type);
        return -2;
    }
    free(svc);
    LOG("%s register service type %d success\n", uDATA_STR, type);
    return 0;
}

static int udata_std_service_init(udata_type_e type)
{
    int ret = 0;
    switch (type) {
        case UDATA_SERVICE_ACC:
#ifdef AOS_UDATA_SERVICE_ACC
            ret = udata_std_service_register(type);
#endif
            break;

        case UDATA_SERVICE_GYRO:
#ifdef AOS_UDATA_SERVICE_GYRO
            ret = udata_std_service_register(type);
#endif
            break;

        case UDATA_SERVICE_MAG:
#ifdef AOS_UDATA_SERVICE_MAG
            ret = udata_std_service_register(type);
#endif
            break;

        case UDATA_SERVICE_UV:
#ifdef AOS_UDATA_SERVICE_UV
            ret = udata_std_service_register(type);
#endif
            break;

        case UDATA_SERVICE_TEMP:
#ifdef AOS_UDATA_SERVICE_TEMP
            ret = udata_std_service_register(type);
#endif
            break;

        case UDATA_SERVICE_HUMI:
#ifdef AOS_UDATA_SERVICE_HUMI
            ret = udata_std_service_register(type);
#endif
            break;

        case UDATA_SERVICE_BARO:
#ifdef AOS_UDATA_SERVICE_BARO
            ret = udata_std_service_register(type);
#endif
            break;

        case UDATA_SERVICE_NOISE:
#ifdef AOS_UDATA_SERVICE_NOISE
            ret = udata_std_service_register(type);
#endif
            break;

        case UDATA_SERVICE_PM25:
#ifdef AOS_UDATA_SERVICE_PM25
            ret = udata_std_service_register(type);
#endif
            break;

        case UDATA_SERVICE_CO2:
#ifdef AOS_UDATA_SERVICE_CO2
            ret = udata_std_service_register(type);
#endif
            break;

        case UDATA_SERVICE_HCHO:
#ifdef AOS_UDATA_SERVICE_HCHO
            ret = udata_std_service_register(type);
#endif
            break;

        case UDATA_SERVICE_TVOC:
#ifdef AOS_UDATA_SERVICE_TVOC
            ret = udata_std_service_register(type);
#endif
            break;
        case UDATA_SERVICE_PH:
#ifdef AOS_UDATA_SERVICE_PH
            ret = udata_std_service_register(type);
#endif
            break;
        case UDATA_SERVICE_VWC:
#ifdef AOS_UDATA_SERVICE_VWC
            ret = udata_std_service_register(type);
#endif
            break;
        case UDATA_SERVICE_EC:
#ifdef AOS_UDATA_SERVICE_EC
            ret = udata_std_service_register(type);
#endif
            break;
        case UDATA_SERVICE_SALINITY:
#ifdef AOS_UDATA_SERVICE_SALINITY
            ret = udata_std_service_register(type);
#endif
            break;
        case UDATA_SERVICE_TDS:
#ifdef AOS_UDATA_SERVICE_TDS
            ret = udata_std_service_register(type);
#endif
            break;
        case UDATA_SERVICE_WINDSPD:
#ifdef AOS_UDATA_SERVICE_WINDSPD
            ret = udata_std_service_register(type);
#endif
            break;
        case UDATA_SERVICE_WINDDIR:
#ifdef AOS_UDATA_SERVICE_WINDDIR
            ret = udata_std_service_register(type);
#endif
            break;
        case UDATA_SERVICE_RAIN:
#ifdef AOS_UDATA_SERVICE_RAIN
            ret = udata_std_service_register(type);
#endif
            break;


        case UDATA_SERVICE_HALL:
#ifdef AOS_UDATA_SERVICE_HALL
            ret = udata_std_service_register(type);
#endif
            break;

        case UDATA_SERVICE_HR:
#ifdef AOS_UDATA_SERVICE_HR
            ret = udata_std_service_register(type);
#endif
            break;

        case UDATA_SERVICE_ALS:
#ifdef AOS_UDATA_SERVICE_ALS
            ret = udata_std_service_register(type);
#endif
            break;

        case UDATA_SERVICE_PS:
#ifdef AOS_UDATA_SERVICE_PS
            ret = udata_std_service_register(type);
#endif
            break;

        case UDATA_SERVICE_GPS:
#ifdef AOS_UDATA_SERVICE_GPS
            ret = udata_std_service_register(type);
#endif
            break;

        case UDATA_SERVICE_RGB:
#ifdef AOS_UDATA_SERVICE_RGB
        ret = udata_std_service_register(type);
#endif
        break;

        case UDATA_SERVICE_GS:
#ifdef AOS_UDATA_SERVICE_GS
        ret = udata_std_service_register(type);
#endif
        break;

        case UDATA_SERVICE_IR:
#ifdef AOS_UDATA_SERVICE_IR
        ret = udata_std_service_register(type);
#endif
        break;


        default:
            break;
    }
    return ret;
}


int service_process_init(void)
{
    int i;
    for (i = 0; i < UDATA_MAX_CNT; i++) {
        if (!abs_data_check_dev_tree(udata_service_type_to_tag(i))) {

            continue;
        }
        udata_std_service_init(i);
    }
    return 0;
}
