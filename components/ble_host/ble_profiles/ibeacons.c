/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include <aos/osal_debug.h>
#include <aos/ble.h>
#include <yoc/ibeacons.h>

uint8_t BEACON_TYPE[2] =   {0X02, 0X15}   ; //define by apple for all Proximity Beacons,Cant not change,

union u {
    beacon_info  init_becons;
    uint8_t data[25];
} uu;

static uint8_t *ibeacon_Init(uint8_t _id[2], uint8_t _uuid[16], uint8_t _major[2], uint8_t _minor[2], uint8_t _measure_power)
{
    int i;

    uu.init_becons.id[0] = _id[0];
    uu.init_becons.id[1] = _id[1];
    uu.init_becons.type[0] = BEACON_TYPE[0];
    uu.init_becons.type[1] = BEACON_TYPE[1];

    for (i = 0; i < 16; i++) {
        uu.init_becons.uuid[i] = _uuid[i];
    }

    uu.init_becons.major[0] = _major[0];
    uu.init_becons.major[1] = _major[1];

    uu.init_becons.minor[0] = _minor[0];
    uu.init_becons.minor[1] = _minor[1];
    uu.init_becons.measure_power = _measure_power;

    return uu.data;//(uint8_t *)usdata;
}


int ibeacon_start(uint8_t _id[2], uint8_t _uuid[16], uint8_t _major[2], uint8_t _minor[2], uint8_t
                 _measure_power, char *_sd)
{
    int ret;
    ad_data_t sd[1] = {0};
    ad_data_t ad[2] = {0};

    uint8_t flag = AD_FLAG_GENERAL | AD_FLAG_NO_BREDR;
    ad[0].type = AD_DATA_TYPE_FLAGS;
    ad[0].data = (uint8_t *)&flag;
    ad[0].len = 1;

    ad[1].type = AD_DATA_TYPE_MANUFACTURER_DATA;
    ad[1].data = (uint8_t *)ibeacon_Init(_id, _uuid, _major, _minor, _measure_power);
    ad[1].len = sizeof(beacon_info);

    //SD DATA
    sd[0].type = AD_DATA_TYPE_NAME_COMPLETE;
    sd[0].data = (uint8_t *)_sd;
    sd[0].len = strlen(_sd);

    adv_param_t param = {
        ADV_NONCONN_IND,
        ad,
        sd,
        BLE_ARRAY_NUM(ad),
        BLE_ARRAY_NUM(sd),
        ADV_FAST_INT_MIN_2,
        ADV_FAST_INT_MIN_2,
    };
    ret = ble_stack_adv_start(&param);

    return ret;
}

int ibeacon_stop(void)
{
    int ret;
    ret = ble_stack_adv_stop();
    return ret;
}

