/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef ULOCATION_H
#define ULOCATION_H

typedef enum {
    ULOC_SCENARIO_OUTDOOR = 0,
    ULOC_SCENARIO_INDOOR  = 1,
    ULOC_SCENARIO_COUNT
} uloc_scenario_e;

typedef struct {
    float longitude;
    float latitude;
    float altitude;
} outdoor_location_t;

typedef struct {
    float x;
    float y;
    float z;
} indoor_location_t;

typedef union {
    indoor_location_t  indoor;
    outdoor_location_t outdoor;
    struct {
        float x;
        float y;
        float z;
    } coordinate;
} location_t;

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************
*  initial location service
*
*  @return:
*******************************************************************/
int uloc_init(uloc_scenario_e scen, int update_inv);

/*******************************************************************
*  terminate locating service
*
*  @return:
*******************************************************************/
int uloc_deinit();

/* OUTDOOR */
/*******************************************************************
*  get latitude
*
*  @return:
*******************************************************************/
int uloc_get_latitude(float * latitude);

/*******************************************************************
*  get altitude
*
*  @return:
*******************************************************************/
int uloc_get_altitude(float * altitude);

/*******************************************************************
*  get longitude
*
*  @return:
*******************************************************************/
int uloc_get_longitude(float *longitude);

/* INDOOR */
/*******************************************************************
*  get x
*
*  @return:
*******************************************************************/
int uloc_get_x(float * x);

/*******************************************************************
*  get y
*
*  @return:
*******************************************************************/
int uloc_get_y(float * y);

/*******************************************************************
*  get z
*
*  @return:
*******************************************************************/
int uloc_get_z(float * z);

/*******************************************************************
*  get location
*
*  @return:
*******************************************************************/
int uloc_get_location(location_t * rlt);

/*******************************************************************
*  fetch new location value, this method should be invoked by user
*  when there is no individual self task.
*
*  @return:
*******************************************************************/
int uloc_update_locationinfo(location_t ** gps,
                             char ** mmac, char ** macs,   /* wifi       */
                             char ** cdma, char ** bts,  char ** nearbts);

/*******************************************************************
*  uloc_update_gpsinfo
*
*  @return:
*******************************************************************/
int uloc_update_gpsinfo(location_t * lo);

#ifdef __cplusplus
}
#endif /* extern c */

#endif /* _U_LOCATION_H_ */

