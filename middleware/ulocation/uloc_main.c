/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <ulocation/ulocation.h>
#include <aos/kernel.h>

#ifndef bool
#define bool unsigned char
#endif

#ifndef true
#define true  1
#endif

#ifndef false
#define false 0
#endif


#define DIFF_SPOT_THREASHOLD 0.1
#define MIN_INTERVAL 10000

static uloc_scenario_e _loc_scen = ULOC_SCENARIO_OUTDOOR;
static int _retrive_gps_interval = MIN_INTERVAL;  

#define INVALID_LOCATION { \
                            .coordinate.x = -99999, \
                            .coordinate.y = -99999, \
                         }   

static const location_t inv_loc  = INVALID_LOCATION;

static aos_mutex_t _self_position_lock;
static location_t _self_position = INVALID_LOCATION;

static bool is_same_spot(const location_t * loc1, const location_t * loc2)
{
    if (loc1 && loc2)
    {
        if ( ((loc1->coordinate.x - loc2->coordinate.x) * (loc1->coordinate.x - loc2->coordinate.x)
             +(loc1->coordinate.y - loc2->coordinate.y) * (loc1->coordinate.y - loc2->coordinate.y)) < DIFF_SPOT_THREASHOLD)
        {
            return true;
        }
    }
    return false;
}

#if ULOCATION_CONFIG_SELFTASK

static void uloc_tsk(void * arg)
{
    location_t lo;
    
    while (1)
    {
        aos_msleep(_retrive_gps_interval > MIN_INTERVAL ? _retrive_gps_interval : MIN_INTERVAL);
        uloc_update_gpsinfo(&lo);
    }
}
#endif

/*******************************************************************
*  initial location service
*
*  @return: 
*******************************************************************/
int uloc_init(uloc_scenario_e scen, int update_inv)
{
    _loc_scen = scen;

    if (update_inv > MIN_INTERVAL)
    {
       _retrive_gps_interval = update_inv;
    }
    
    aos_mutex_new(&_self_position_lock);

#if ULOCATION_CONFIG_SELFTASK
    aos_task_t  task;

    if (aos_task_new_ext(&task, "uloc_tsk", uloc_tsk, 
                         NULL, 1024, AOS_DEFAULT_APP_PRI) != 0)
    {
        return -1;
    }
#endif

    return 0;
}

/*******************************************************************
*  terminate locating service
*
*  @return: 
*******************************************************************/
int ulocation_deinit()
{
    aos_mutex_free(&_self_position_lock);
#if ULOCATION_CONFIG_SELFTASK    
    aos_task_delete("uloc_tsk");
#endif
    return 0;
}

/* call when location info is avaliable after set by cloud */
void set_location(const location_t * l)
{
    aos_mutex_lock(&_self_position_lock, -1);
    
    _self_position = *l;

    aos_mutex_unlock(&_self_position_lock);
}

int uloc_get_location(location_t * rlt)
{
    aos_mutex_lock(&_self_position_lock, -1);
    
    if (is_same_spot(&_self_position, &inv_loc))
    {
        aos_mutex_unlock(&_self_position_lock);
        return -1;
    }
    
    *rlt = _self_position;
    
    aos_mutex_unlock(&_self_position_lock);
    return 0;
}

/*******************************************************************
*  get x
*
*  @return: 
*******************************************************************/
int uloc_get_x(float * x)
{
    aos_mutex_lock(&_self_position_lock, -1);
    
    if (is_same_spot(&_self_position, &inv_loc))
    {
        aos_mutex_unlock(&_self_position_lock);
        return -1;
    }

    *x = _self_position.indoor.x;

    aos_mutex_unlock(&_self_position_lock);
    
    return 0;
}

/*******************************************************************
*  get y
*
*  @return: 
*******************************************************************/
int uloc_get_y(float * y)
{
    aos_mutex_lock(&_self_position_lock, -1);
    
    if (is_same_spot(&_self_position, &inv_loc))
    {
        aos_mutex_unlock(&_self_position_lock);
        return -1;
    }

    *y = _self_position.indoor.y;
    
    aos_mutex_unlock(&_self_position_lock);
    return 0;
}

/*******************************************************************
*  get z
*
*  @return: 
*******************************************************************/
int uloc_get_z(float * z)
{
    aos_mutex_lock(&_self_position_lock, -1);
    
    if (is_same_spot(&_self_position, &inv_loc))
    {
        aos_mutex_unlock(&_self_position_lock);
        return -1;
    }

    *z = _self_position.indoor.z;

    aos_mutex_unlock(&_self_position_lock);
    
    return 0;
}

/*******************************************************************
*  get latitude
*
*  @return: 
*******************************************************************/
int uloc_get_altitude(float * altitude)
{
    aos_mutex_lock(&_self_position_lock, -1);
    
    if (is_same_spot(&_self_position, &inv_loc))
    {
        aos_mutex_unlock(&_self_position_lock);
        return -1;
    }

    *altitude = _self_position.outdoor.latitude;
    aos_mutex_unlock(&_self_position_lock);
    return 0;
}


/*******************************************************************
*  get latitude
*
*  @return: 
*******************************************************************/
int uloc_get_latitude(float * latitude)
{
    aos_mutex_lock(&_self_position_lock, -1);
    
    if (is_same_spot(&_self_position, &inv_loc))
    {
        aos_mutex_unlock(&_self_position_lock);
        return -1;
    }

    *latitude = _self_position.outdoor.latitude;
    aos_mutex_unlock(&_self_position_lock);
    return 0;
}

/*******************************************************************
*  get longitude
*
*  @return: 
*******************************************************************/
int uloc_get_longitude(float * longitude)
{
    aos_mutex_lock(&_self_position_lock, -1);
    
    if (is_same_spot(&_self_position, &inv_loc))
    {
        aos_mutex_unlock(&_self_position_lock);
        return -1;
    }

    *longitude = _self_position.outdoor.longitude;
    aos_mutex_unlock(&_self_position_lock);
    return 0;
}

int uloc_update_gpsinfo(location_t * lo)
{
    int rlt = -1;
    rlt = uloc_hal_get_gps(&lo);
    
    if (is_same_spot(&inv_loc, lo))
    {
        return -1;
    }
    
    set_location(lo);

    return rlt;
}

/*********************************************************************
 *   Gaode positioning service
 *********************************************************************/


