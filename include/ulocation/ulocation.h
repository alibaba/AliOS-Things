/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef ULOCATION_H
#define ULOCATION_H

typedef enum {
    ULOCATION_SCENARIO_OUTDOOR = 0,
    ULOCATION_SCENARIO_INDOOR  = 1,
    ULOCATION_SCENARIO_COUNT
} ulocation_scenario_e;

typedef enum {
    ULOCATION_COORDINATESYS_WGS84 = 1,
    ULOCATION_COORDINATESYS_GCJ02 = 2,
    ULOCATION_COORDINATESYS_COUNT
} location_coordinatesys_e;

typedef struct _location_time {
    int year;
    int mon;
    int day;
    int hour;
    int min;
    int sec;
    int hsec;
} location_time_t;

typedef struct _outdoor_location {
    float longitude;
    float latitude;
    float altitude;
    location_time_t time;
    location_coordinatesys_e coordinatesys;
} outdoor_location_t;

typedef struct _indoor_location {
    float x;
    float y;
    float z;
    location_time_t time;
    location_coordinatesys_e coordinatesys;
} indoor_location_t;

typedef union {
    indoor_location_t  indoor;
    outdoor_location_t outdoor;
} location_t;

#ifdef QXWZ_ENABLED

typedef struct ulocation_qxwz_usr_config {
    char *appkey;
    char *appsecret;
    char *device_ID;
    char *device_Type;
} ulocation_qxwz_usr_config_t;

typedef struct {
    unsigned int status;
    int longitude;
    int latitude;
    unsigned int nors;
    unsigned int wore;
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
 *  get the indoor location coordinate axis-x.
 */
int ulocation_get_x(float *x);

/**
 *  get the indoor location coordinate axis-y.
 */
int ulocation_get_y(float *y);

/**
 *  get the indoor location coordinate axis-z.
 */
int ulocation_get_z(float *z);

/**
 *  get location information from gps.
 */
int ulocation_update_gpsinfo(location_t *gps);

/**
 *  get location information from wifi.
 */
int ulocation_update_wifiinfo(char *mmac, char *macs);

/**
 *  get location information from gprs.
 */
int ulocation_update_gprsinfo(char *cdma, char *bts, char *nearbts);

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
