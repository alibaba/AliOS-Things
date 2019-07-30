/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef ULOCATION_H
#define ULOCATION_H

typedef enum {
    ULOC_SCENARIO_OUTDOOR = 0,
    ULOC_SCENARIO_INDOOR  = 1,
    ULOC_SCENARIO_COUNT
} ulocation_scenario_e;

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

/**
 * Initial location service
 */
int ulocation_init(ulocation_scenario_e scen, int update_inv);

/**
 * Terminate locating service
 */
int ulocation_deinit(void);

/**
 * Get latitude
 */
int ulocation_get_latitude(float *latitude);

/**
 * Get altitude
 */
int ulocation_get_altitude(float *altitude);

/**
 * Get longitude
 */
int ulocation_get_longitude(float *longitude);

/**
 * Get x
 */
int ulocation_get_x(float *x);

/**
 * Get y
 */
int ulocation_get_y(float *y);

/**
 * Get z
 */
int ulocation_get_z(float *z);

/**
 * Get location
 */
int ulocation_get_location(location_t *rlt);

/**
 * Fetch new location value, this method should be invoked by user
 * when there is no individual self task.
 */
int ulocation_update_locationinfo(location_t **gps, char **mmac, char **macs, /* wifi */
                                  char **cdma, char **bts, char **nearbts);

/**
 * uloc_update_gpsinfo
 */
int ulocation_update_gpsinfo(location_t *lo);

#ifdef __cplusplus
}
#endif

#endif /* ULOCATION_H */

