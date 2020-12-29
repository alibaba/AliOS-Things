/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _BT_IBECONS_H_
#define _BT_IBECONS_H_


typedef struct _beacon_info {
    uint8_t id[2];
    uint8_t type[2];
    uint8_t uuid[16];
    uint8_t major[2];
    uint8_t minor[2];
    uint8_t measure_power;
} beacon_info;


int ibeacon_start(uint8_t _id[2], uint8_t _uuid[16], uint8_t _major[2], uint8_t _minor[2], uint8_t
                 _measure_power, char *_sd);



int ibeacon_stop(void);




#endif
