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

#ifdef QXWZ_ENABLED

typedef struct ulocation_qxwz_usr_config {
    char *appkey;
    char *appsecret;
    char *device_ID;
    char *device_Type;
} ulocation_qxwz_usr_config_t;

typedef struct {
    uint8_t status;
    int longitude;
    int latitude;
    uint8_t nors;
    uint8_t wore;
    int altitude;
} ulocation_gga_info_t;

#endif

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
 * ulocation_update_gpsinfo
 */
int ulocation_update_gpsinfo(location_t *lo);

#ifdef QXWZ_ENABLED
/**
 * ulocation_qxwz_service
 */
int ulocation_qianxun_service(ulocation_qxwz_usr_config_t *usr_config, ulocation_gga_info_t *location);
#endif

#ifdef __cplusplus
}
#endif

#endif /* ULOCATION_H */

