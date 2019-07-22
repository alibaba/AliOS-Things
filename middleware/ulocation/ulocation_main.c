/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "ulocation/ulocation.h"

#ifndef bool
#define bool unsigned char
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#define DIFF_SPOT_THREASHOLD 0.1

#define MIN_INTERVAL 10000

static ulocation_scenario_e loc_scen = ULOC_SCENARIO_OUTDOOR;

static int retrive_gps_interval = MIN_INTERVAL;

#define INVALID_LOCATION { \
                            .coordinate.x = -99999, \
                            .coordinate.y = -99999, \
                         }

static const location_t inv_loc = INVALID_LOCATION;

static aos_mutex_t self_position_lock;

static location_t self_position = INVALID_LOCATION;

static bool is_same_spot(const location_t *loc1, const location_t *loc2)
{
    if (loc1 != NULL && loc2 != NULL) {
        if (((loc1->coordinate.x - loc2->coordinate.x) * (loc1->coordinate.x - loc2->coordinate.x)
            +(loc1->coordinate.y - loc2->coordinate.y) * (loc1->coordinate.y - loc2->coordinate.y))
            < DIFF_SPOT_THREASHOLD) {
            return true;
        }
    }

    return false;
}

#if ULOCATION_CONFIG_SELFTASK
static void ulocation_tsk(void *arg)
{
    location_t lo;

    while (1) {
        aos_msleep(retrive_gps_interval > MIN_INTERVAL ? retrive_gps_interval : MIN_INTERVAL);
        ulocation_update_gpsinfo(&lo);
    }
}
#endif

int ulocation_init(ulocation_scenario_e scen, int update_inv)
{
    loc_scen = scen;

    if (update_inv > MIN_INTERVAL) {
       retrive_gps_interval = update_inv;
    }

    aos_mutex_new(&self_position_lock);

#if ULOCATION_CONFIG_SELFTASK
    aos_task_t  task;

    if (aos_task_new_ext(&task, "ulocation_tsk", ulocation_tsk,
                         NULL, 1024 * 2, AOS_DEFAULT_APP_PRI) != 0) {
        return -1;
    }
#endif

    return 0;
}

int ulocation_deinit(void)
{
    aos_mutex_free(&self_position_lock);

#if ULOCATION_CONFIG_SELFTASK
    aos_task_delete("ulocation_tsk");
#endif

    return 0;
}

/* call when location info is avaliable after set by cloud */
void set_location(const location_t *l)
{
    aos_mutex_lock(&self_position_lock, -1);

    self_position = *l;

    aos_mutex_unlock(&self_position_lock);
}

int ulocation_get_location(location_t *rlt)
{
    aos_mutex_lock(&self_position_lock, -1);

    if (is_same_spot(&self_position, &inv_loc)) {
        aos_mutex_unlock(&self_position_lock);
        return -1;
    }

    *rlt = self_position;

    aos_mutex_unlock(&self_position_lock);

    return 0;
}

int ulocation_get_x(float *x)
{
    aos_mutex_lock(&self_position_lock, -1);

    if (is_same_spot(&self_position, &inv_loc)) {
        aos_mutex_unlock(&self_position_lock);
        return -1;
    }

    *x = self_position.indoor.x;

    aos_mutex_unlock(&self_position_lock);

    return 0;
}

int ulocation_get_y(float *y)
{
    aos_mutex_lock(&self_position_lock, -1);

    if (is_same_spot(&self_position, &inv_loc)) {
        aos_mutex_unlock(&self_position_lock);
        return -1;
    }

    *y = self_position.indoor.y;

    aos_mutex_unlock(&self_position_lock);

    return 0;
}

int ulocation_get_z(float *z)
{
    aos_mutex_lock(&self_position_lock, -1);

    if (is_same_spot(&self_position, &inv_loc)) {
        aos_mutex_unlock(&self_position_lock);
        return -1;
    }

    *z = self_position.indoor.z;

    aos_mutex_unlock(&self_position_lock);

    return 0;
}

int ulocation_get_altitude(float *altitude)
{
    aos_mutex_lock(&self_position_lock, -1);

    if (is_same_spot(&self_position, &inv_loc)) {
        aos_mutex_unlock(&self_position_lock);
        return -1;
    }

    *altitude = self_position.outdoor.latitude;

    aos_mutex_unlock(&self_position_lock);

    return 0;
}

int ulocation_get_latitude(float *latitude)
{
    aos_mutex_lock(&self_position_lock, -1);

    if (is_same_spot(&self_position, &inv_loc)) {
        aos_mutex_unlock(&self_position_lock);
        return -1;
    }

    *latitude = self_position.outdoor.latitude;

    aos_mutex_unlock(&self_position_lock);

    return 0;
}

int ulocation_get_longitude(float *longitude)
{
    aos_mutex_lock(&self_position_lock, -1);

    if (is_same_spot(&self_position, &inv_loc)) {
        aos_mutex_unlock(&self_position_lock);
        return -1;
    }

    *longitude = self_position.outdoor.longitude;

    aos_mutex_unlock(&self_position_lock);

    return 0;
}

int ulocation_update_gpsinfo(location_t *lo)
{
    int rlt = ulocation_hal_get_gps(&lo);

    if (lo == NULL || is_same_spot(&inv_loc, lo)) {
        return -1;
    }

    set_location(lo);

    return rlt;
}

