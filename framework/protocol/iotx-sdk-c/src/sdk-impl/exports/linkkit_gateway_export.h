#ifndef LINKKIT_EXPORT_H
#define LINKKIT_EXPORT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/***************************Gateway Interface***************************/

enum {
    LINKKIT_EVENT_CLOUD_DISCONNECTED = 0,   /* cloud disconnected */
    LINKKIT_EVENT_CLOUD_CONNECTED    = 1,   /* cloud connected    */
    LINKKIT_EVENT_SUBDEV_DELETED     = 2,   /* subdev deleted     */
    LINKKIT_EVENT_SUBDEV_PERMITED    = 3,   /* subdev permit join */
    LINKLIT_EVENT_SUBDEV_SETUP       = 4,   /* subdev install     */
};

/*
 *         option                 | default | minimum | maximum
 *--------------------------------|---------|---------|---------
 * LINKKIT_OPT_MAX_MSG_SIZE       |  20480  |  512    | 51200
 * LINKKIT_OPT_MAX_MSG_QUEUE_SIZE |  16     |  2      | 32
 * LINKKIT_OPT_THREAD_POOL_SIZE   |  4      |  1      | 16
 * LINKKIT_OPT_THREAD_STACK_SIZE  |  8192   |  1024   | 8388608
 * LINKKIT_OPT_LOG_LEVEL          |  3      |  0      | 5
 */

enum {
    LINKKIT_OPT_MAX_MSG_SIZE       = 1,
    LINKKIT_OPT_MAX_MSG_QUEUE_SIZE = 2,
    LINKKIT_OPT_THREAD_POOL_SIZE   = 3,
    LINKKIT_OPT_THREAD_STACK_SIZE  = 4,
    LINKKIT_OPT_LOG_LEVEL          = 5, /* 0 - Emergency, 1 - Critical, 2 - Error, 3 - Warnning, 4 - Info, 5 - Debug */
};

typedef struct linkkit_params_s linkkit_params_t;

/**
 * @brief get default initialize parameters
 *
 * @return linkkit default parameters.
 */
linkkit_params_t *linkkit_gateway_get_default_params(void);

/**
 * @brief set option in paremeters
 *
 * @param params, linkkit initialize parameters, return from linkkit_get_default_params().
 * @param option, see LINKKIT_OPT_XXX for more detail.
 * @param value, value of option.
 * @param value_len, value length.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_set_option(linkkit_params_t *params, int option, void *value, int value_len);

typedef struct {
    int event_type; /* see LINKKIT_EVENT_XXX for more details */

    union {
        struct {
            char *productKey;
            char *deviceName;
        } subdev_deleted;

        struct {
            char *productKey;
            int   timeoutSec;
        } subdev_permited;

        struct {
            char *subdevList;   /* json format:[{"productKey":"","deviceName":""},...] */
        } subdev_install;
    } event_data;
} linkkit_event_t;

/**
 * @brief set event callback
 *
 * @param params, linkkit initialize parameters, return from linkkit_get_default_params().
 * @param event_cb, event callback.
 * @param ctx, user private data.
 *
 * @return 0 when success, < 0 when fail.
 */
int linkkit_gateway_set_event_callback(linkkit_params_t *params, int (*event_cb)(linkkit_event_t *ev, void *ctx), void *ctx);

/**
 * @brief linkkit initialization.
 *
 * @param initParams, linkkit initialize parameters, see linkkit_params_t for more detail.
 *
 * @return 0 when success, < 0 when fail.
 */
int linkkit_gateway_init(linkkit_params_t *initParams);

/**
 * @brief linkkit deinitialization.
 *
 * @return 0 when success, < 0 when fail.
 */
int linkkit_gateway_exit(void);

typedef struct {

	int (*register_complete) (void *ctx);
    /**
     * @brief get property callback.
     *
     * @param in, properties to be get, in JSON array format, terminated by NULL.
     * @param out, output buffer fill by user, in json format, terminated by NULL.
     * @param out_len, out buffer length.
     * @param ctx, user private data passed by linkkit_gateway_start() or linkkit_subdev_create()
     *
     * @return 0 when success, -1 when fail.
     */
    int (*get_property)(char *in, char *out, int out_len, void *ctx);

    /**
     * @brief set property callback.
     *
     * @param in, properties to be set, in JSON object format, terminated by NULL.
     * @param ctx, user private data passed by linkkit_gateway_start() or linkkit_subdev_create()
     *
     * @return 0 when success, -1 when fail.
     */
    int (*set_property)(char *in, void *ctx);

    /**
     * @brief call service callback.
     *
     * @param identifier, service identifier, available services define in TSL file.
     * @param in, service input data, in JSON object format, terminated by NULL.
     * @param out, service output, this buffer will be filled by user, in json format, terminated by NULL.
     * @param out_len, out buffer length.
     * @param ctx, user private data passed by linkkit_gateway_start() or linkkit_subdev_create().
     *
     * @return 0 when success, -1 when fail.
     */
    int (*call_service)(char *identifier, char *in, char *out, int out_len, void *ctx);

    /**
     * @brief raw data from cloud.
     *
     * @param in, input data from cloud.
     * @param in_len, input data length.
     * @param out, output data to cloud, allocated by linkkit fill by user, no need to be free.
     * @param out_len, out buffer length.
     * @param ctx, user private data passed by linkkit_gateway_start() or linkkit_subdev_create().
     *
     * @return output data size. < 0 when fail.
     */
    ssize_t (*down_rawdata)(const void *in, int in_len, void *out, int out_len, void *ctx);

    /**
     * @brief return data from cloud when calling linkkit_post_rawdata().
     *
     * @param data, return raw data from cloud.
     * @param len, data length.
     * @param ctx, user private data passed by linkkit_gateway_start() or linkkit_subdev_create().
     *
     * @return 0 when success, -1 when fail.
     */
    int (*post_rawdata_reply)(const void *data, int len, void *ctx);
} linkkit_cbs_t;

/**
 * @brief start linkkit gateway routines and install callback funstions.
 *
 * @param cbs, callback function struct to be installed.
 * @param ctx, user context pointer.
 *
 * @return device id, 0 > when success, < 0 when fail.
 */
int linkkit_gateway_start(linkkit_cbs_t *cbs, void *ctx);

/**
 * @brief stop linkkit gateway.

 * @param devid, device id return from linkkit_gateway_start().
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_stop(int devid);

/**
 * @brief register subdev to gateway.

 * @param productKey, subdev's product key.
 * @param deviceName, subdev's device name.
 * @param deviceSecret, subdev's device secret.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_subdev_register(char *productKey, char *deviceName, char *deviceSecret);

/**
 * @brief deregister subdev from gateway.

 * @param productKey, subdev's product key.
 * @param deviceName, subdev's device name.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_subdev_unregister(char *productKey, char *deviceName);

/**
 * @brief create subdev and install callback funstions.
 *
 * @param productKey, subdev's product key.
 * @param deviceName, subdev's device name.
 * @param cbs, callback function struct to be installed.
 * @param ctx, user context pointer.
 *
 * @return device id, 0 > when success, < 0 when fail.
 */
int linkkit_gateway_subdev_create(char *productKey, char *deviceName, linkkit_cbs_t *cbs, void *ctx);

/**
 * @brief destroy subdev by device id.

 * @param devid, device id return from linkkit_subdev_create().
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_subdev_destroy(int devid);

/**
 * @brief make subdev accessible from cloud.

 * @param devid, device id return from linkkit_subdev_create().
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_subdev_login(int devid);

/**
 * @brief make subdev inaccessible on cloud.

 * @param devid, device id return from linkkit_subdev_create().
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_subdev_logout(int devid);

enum {
    LINKKIT_STATE_ENABLED  = 0, /* device is enabled by cloud  */
    LINKKIT_STATE_DISABLED,     /* device is disabled by cloud */
    LINKKIT_STATE_REMOVED,      /* device is deleted by cloud  */
};

typedef struct {
    char *productKey;   /* device's product key */
    char *deviceName;   /* device's device name */

    int   devtype;      /* Device Type: 0 - gateway, 1 - subdev */
    int   login;        /* Login State: 0 - logout,  1 - login  */
    int   state;        /* Device State: see LINKKIT_STATE_xxx  */
    int   online;       /* 0 - offline, 1 - online */
} linkkit_devinfo_t;

/**
 * @brief get device infomation specific by devid.
 *
 * @param devinfo, device information, see linkkit_devinfo_t for more detail.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_get_devinfo(int devid, linkkit_devinfo_t *devinfo);

/**
 * @brief post event to cloud.
 *
 * @param devid, device id return from linkkit_gateway_start() or linkkit_subdev_create().
 * @param identifier, event identifier, see tsl file for more detail.
 * @param event, event data, in JSON format.
 * @param timeout_ms, transmission timeout, in milliseconds. when timeout_ms is 0, wait no response.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_trigger_event_json_sync(int devid, char *identifier, char *event, int timeout_ms);

/**
 * @brief post event to cloud asynchronously.
 *
 * @param devid, device id return from linkkit_gateway_start() or linkkit_subdev_create().
 * @param identifier, event identifier, see tsl file for more detail.
 * @param event, event data, in JSON format.
 * @param timeout_ms, transmission timeout, in milliseconds. when timeout_ms is 0, wait no response.
 * @param func, callback function when success(retval > 0), timeout(retval = 0) or failed(retval < 0).
 * @param ctx, user data passed to 'func'.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_trigger_event_json(int devid, char *identifier, char *event, int timeout_ms, void (*func)(int retval, void *ctx), void *ctx);


/**
 * @brief post property to cloud.
 *
 * @param devid, device id return from linkkit_gateway_start() or linkkit_subdev_create().
 * @param property, property data, in JSON format.
 * @param timeout_ms, transmission timeout, in milliseconds. when timeout_ms is 0, wait no response.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_post_property_json_sync(int devid, char *json, int timeout_ms);

/**
 * @brief post property to cloud asynchronously.
 *
 * @param devid, device id return from linkkit_gateway_start() or linkkit_subdev_create().
 * @param property, property data, in JSON format.
 * @param timeout_ms, transmission timeout, in milliseconds. when timeout_ms is 0, wait no response.
 * @param func, callback function when success(retval > 0), timeout(retval = 0) or failed(retval < 0).
 * @param ctx, user data passed to 'func'.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_post_property_json(int devid, char *property, int timeout_ms, void (*func)(int retval, void *ctx), void *ctx);

/**
 * @brief post raw data to cloud.
 *
 * @param devid, device id return from linkkit_gateway_start() or linkkit_subdev_create().
 * @param data, raw data buffer pointer.
 * @param len, raw data length.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_post_rawdata(int devid, void *data, int len);

typedef enum {
    LINKKIT_OTA_EVENT_NEW_VERSION_DETECTED = 1,
} linkkit_ota_event_t;

/**
 * @brief init fota service and install callback funstion.
 *
 * @param cb, callback function to be installed.
 * @param ctx, user data passed to callback function.
 *
 * @return int, 0 when success, -1 when fail.
 */
int linkkit_gateway_ota_init(void (*cb)(int event, const char *version, void *ctx), void *ctx);

/**
 * @brief perform system upgrade when "new version detected" event reported.
 *
 * @param recvbuf_length, receive buffer length that used to receive upgrade package.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_ota_update(int recvbuf_length);

typedef struct {
    char *attrKey;    /* the key of extend info. */
    char *attrValue;  /* the value of extend info. */
} linkkit_extinfo_t;

/**
 * @brief post group of extend info to cloud
 *
 * @param devid, device id return from linkkit_gateway_start() or linkkit_subdev_create().
 * @param extinfos, group of extend info to be post.
 * @param nb_extinfos, number of extend infos in extinfos.
 * @param timeout_ms, transmission timeout, in milliseconds. when timeout_ms is 0, wait no response.
 *
 * @return 0 when success, < 0 when fail.
 */
int linkkit_gateway_post_extinfos(int devid, linkkit_extinfo_t *extinfos, int nb_extinfos, int timeout_ms);

/**
 * @brief delete extend info specific by key
 *
 * @param devid, device id return from linkkit_gateway_start() or linkkit_subdev_create().
 * @param extinfos, group of extend info to be deleted, attrValue in linkkit_extinfo_t will be ignore.
 * @param nb_extinfos, number of extend infos in extinfos.
 * @param timeout_ms, transmission timeout, in milliseconds. when timeout_ms is 0, wait no response.
 *
 * @return 0 when success, < 0 when fail.
 */
int linkkit_gateway_delete_extinfos(int devid, linkkit_extinfo_t *extinfos, int nb_extinfos, int timeout_ms);

/**
 * @brief get number devices currently in gateway
 *
 * @return number devinfos.
 */
int linkkit_gateway_get_num_devices(void);

/**
 * @brief get all devices currently in gateway
 *
 * @param devinfos, device information array, see linkkit_devinfo_t for more detail.
 * @param nb_devinfos, number of extend infos in extinfos.
 *
 * @return number devinfos filled.
 */
int linkkit_gateway_get_devinfos(linkkit_devinfo_t *devinfos, int nb_devinfos);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINKKIT_EXPORT_H */
