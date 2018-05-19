/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
*/

#ifndef GPS_HAL_H
#define GPS_HAL_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
    
#include <aos/aos.h>
#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_register.h>
#include <hal/base.h>
#include "common.h"
#include "hal/sensor.h"
    
#ifdef AOS_ATCMD
#include <atparser.h>
#endif

#define GPS_EV_UDATA            (0x40)
#define GPS_DEV_READ            (1)
#define GPS_DEV_PROC            (2)
#define GPS_DEV_SEND            (3)

#define GPS_RCV_DATA_LEN        (256)

typedef struct _gps_obj_t {
    char*                    path;
    sensor_tag_e             tag;
    dev_io_port_e            io_port;
    work_mode_e              mode;
    dev_power_mode_e         power;
    gpio_dev_t               gpio;
    dev_sensor_full_info_t   info;
    uint8_t                  ref;
    int (*open)(void);
    int (*close)(void);
    int (*read)(void *, size_t);
    int (*write)(const void *buf, size_t len);
    int (*ioctl)(int cmd, unsigned long arg);
    void(*irq_handle)(void);
}gps_obj_t;

extern int gps_simcom_sim868_init(void);
extern int gps_simcom_sim868_proc(const char* str, gps_data_t* pgpsdata);
extern int gps_gp_proc(const char* str, gps_data_t* pgpsdata);
extern work_mode_e gps_mode_get(void);
extern int gps_create_obj(gps_obj_t* gpsobj);


#endif /* GPS_HAL_H */


