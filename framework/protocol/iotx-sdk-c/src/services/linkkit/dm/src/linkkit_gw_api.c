#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "iot_import.h"
#include "iot_export.h"
#include "lite-utils.h"
#include "lite-cjson.h"

#include "iotx_dm_common.h"
#include "queue.h"
#include "tmpmsg.h"
#include "packet.h"
#include "thread.h"
#include "watcher.h"
#include "mempool.h"
#include "lite-log.h"
#include "lite-utils.h"
#include "iot_export_dm.h"
#include "linkkit_gateway_export.h"

#define TOPIC_MAXLEN    (128)
enum {
    MSG_TYPE_CLOUD_REQUEST,
    MSG_TYPE_CLOUD_CONNECTED,
    MSG_TYPE_CLOUD_DISCONNECTED,
};

#define LINKKIT_GW_API_KEY_ID          "id"
#define LINKKIT_GW_API_KEY_CODE        "code"
#define LINKKIT_GW_API_KEY_DEVID       "devid"
#define LINKKIT_GW_API_KEY_SERVICEID   "serviceid"
#define LINKKIT_GW_API_KEY_PROPERTYID  "propertyid"
#define LINKKIT_GW_API_KEY_EVENTID     "eventid"
#define LINKKIT_GW_API_KEY_PAYLOAD     "payload"
#define LINKKIT_GW_API_KEY_PRODUCT_KEY "product_key"
#define LINKKIT_GW_API_KEY_TIMEOUT     "timeout"

typedef struct linkkit_dev_s {
    uint8_t online; /* called linkkit_subdev_login() and subscribed all topics */
    uint8_t login;  /* called linkkit_subdev_login() */

    int devid;
    linkkit_cbs_t cbs;
    void *ctx;
} linkkit_dev_t;

typedef struct topo_node_s {
    linkkit_dev_t *subdev;

    struct topo_node_s *next;
} topo_node_t;

struct linkkit_params_s {
    int maxMsgSize;         /* max message size          */
    int maxMsgQueueSize;    /* max message queue size    */

    int threadPoolSize;     /* number threads in pool    */
    int threadStackSize;    /* default thread stack size */

    int (*event_cb)(linkkit_event_t *ev, void *ctx);

    /* user private data */
    void *ctx;
};

typedef struct {
    lk_queue_t  *linkkit_msgq;

    AMemPool *msgbuf_pool;             /* message buffer pool */
    AMemPool *msgqblk_pool;

    int max_msg_size;
    int max_msgq_size;

    int threadPoolSize;
    int threadStackSize;
    int linkkit_abort;
    lk_thread_t **threadPool;

    void *mutex;

    int connected;

    int main_device_inited;

    lk_thread_t *subdev_monitor_tid;

    int (*event_cb)(linkkit_event_t *ev, void *ctx);
    void *ctx;
} linkkit_gbl_t;

typedef struct {
    void (*func)(int retval, void *ctx);
    void *ctx;
} post_event_t;

typedef struct {
    int timeout_ms;
    watcher_t *w;

    void (*cb)(int code, char *resp, int resp_len, void *ctx);
    void *ctx;
} wait_obj_t;

typedef struct {
    int type;   /* See MSG_TYPE_* */
    int id;

    char method[64];
    char topic[TOPIC_MAXLEN];

    char *params;
    int params_len;
} lk_msg_t;

static void* g_linkkit_dispatch = NULL;
static linkkit_gbl_t gbl;
static linkkit_dev_t *main_device = NULL;
static topo_node_t *first_node = NULL;
static void *gbl_lock = NULL;

static int _checkInitParams(linkkit_params_t *initParams);
static void _dump_params(linkkit_params_t *params);
static void *_response_waiter(void *arg);
static void _async_post_event(int code, char *out, int out_len, void *ctx);
void _linkkit_gw_event_callback(iotx_dm_event_types_t type, char *payload);
static int _validOps(linkkit_cbs_t *cbs);
static int _subdev_node_add(linkkit_dev_t *dev);
static void _subdev_node_del(linkkit_dev_t *dev);
static topo_node_t *_subdev_find_node_by_devid(int devid);
static inline int _is_gateway(linkkit_dev_t *dev);
static inline int _is_subdev(linkkit_dev_t *dev);
static linkkit_dev_t *_find_device_by_devid(int devid);
static int _online_all_subdevs(void);
static int _offline_all_subdevs(void);


static int _subdev_node_add(linkkit_dev_t *dev)
{
    topo_node_t *node = LITE_malloc(sizeof(topo_node_t));
    if (!node)
        return -1;
    memset(node, 0, sizeof(topo_node_t));

    HAL_MutexLock(gbl_lock);
    node->subdev = dev;
    node->next = first_node;
    first_node = node;
    HAL_MutexUnlock(gbl_lock);

    return 0;
}

static void _subdev_node_del(linkkit_dev_t *dev)
{
    if (!dev) {
        return;
    }
    HAL_MutexLock(gbl_lock);
    topo_node_t *dev_node = _subdev_find_node_by_devid(dev->devid);
    topo_node_t **dp, *d1;

    /* remove connection from list */
    dp = &first_node;
    while ((*dp) != NULL) {
        d1 = *dp;
        if (d1 == dev_node)
            *dp = dev_node->next;
        else
            dp = &d1->next;
    }
    HAL_MutexUnlock(gbl_lock);

    LITE_free(dev_node);
}

static topo_node_t *_subdev_find_node_by_devid(int devid)
{
    topo_node_t *node;
    HAL_MutexLock(gbl_lock);
    for (node = first_node; node; node = node->next) {
        linkkit_dev_t *dev = node->subdev;
        if (!dev)
            continue;

        if (dev->devid == devid) {
            HAL_MutexUnlock(gbl_lock);
            return node;
        }
    }
    HAL_MutexUnlock(gbl_lock);

    return NULL;
}


static inline int _is_gateway(linkkit_dev_t *dev)
{
    return dev == main_device;
}

static inline int _is_subdev(linkkit_dev_t *dev)
{
    return dev != main_device;
}

static linkkit_dev_t *_find_device_by_devid(int devid)
{
    if (devid == main_device->devid)
        return main_device;

    topo_node_t *n = _subdev_find_node_by_devid(devid);
    if (!n) {
        dm_log_info("invalid devid 0x%08x\n", devid);
        return NULL;
    }

    return n->subdev;
}


static int _validOps(linkkit_cbs_t *cbs)
{
    if (!cbs)
        return 0;

    if (cbs->get_property || cbs->set_property       ||
        cbs->down_rawdata || cbs->post_rawdata_reply)
        return 1;

    return 0;
}

static void _async_post_event(int code, char *out, int out_len, void *ctx)
{
    post_event_t *ev = ctx;
    if (!ev)
        return;

    ev->func(code, ev->ctx);

    LITE_free(ev);
}

static void *_response_waiter(void *arg)
{
    wait_obj_t *obj = arg;
    if (!obj)
        return NULL;

    int code = -1;
    char *data = NULL;
    int data_len = 0;

    if (watcher_wait(obj->w, &code, &data, &data_len, obj->timeout_ms) < 0) {
        if (obj->cb)
            obj->cb(0, NULL, 0, obj->ctx);
        goto fail;
    }

    if (code != 200) {
        code = -code;
        if (obj->cb)
            obj->cb(code, NULL, 0, obj->ctx);
        goto fail;
    }

    if (data && data_len)
        data[data_len]= '\0';

    if (obj->cb)
        obj->cb(1, data, data_len, obj->ctx);

fail:
    if (data)
        AMemPool_Put(gbl.msgbuf_pool, data);
    watcher_free(obj->w);
    LITE_free(obj);

    return NULL;
}

static int _checkInitParams(linkkit_params_t *initParams)
{
    if (!initParams)
        return -1;

    if (initParams->maxMsgSize < 256) {
        dm_log_err("maxMsgSize should not less than 256 bytes\n");
        return -1;
    }

    if (initParams->maxMsgQueueSize < 1) {
        dm_log_err("maxMsgQueueSize should not less than 1\n");
        return -1;
    }

    if (initParams->threadPoolSize < 1) {
        dm_log_err("threadPoolSize should not less than 1\n");
        return -1;
    }

    if (initParams->threadStackSize < 1024) {
        dm_log_err("threadStackSize should not less than 1024\n");
        return -1;
    }

    return 0;
}


static void _dump_params(linkkit_params_t *params)
{
    dm_log_info("++++++++++++\n");
    dm_log_info("     maxMsgSize: %d\n", params->maxMsgSize);
    dm_log_info("maxMsgQueueSize: %d\n", params->maxMsgQueueSize);
    dm_log_info(" threadPoolSize: %d\n", params->threadPoolSize);
    dm_log_info("threadStackSize: %d\n", params->threadStackSize);
    dm_log_info("------------\n");
}

static int _online_all_subdevs(void)
{
#if 0
    topo_node_t *n;

    HAL_MutexLock(gbl_lock);
    for (n = first_node; n; n = n->next) {
        linkkit_dev_t *dev = n->subdev;
        if (!dev)
            continue;

        if (dev->login && dev->online == 0) {
            int ret = linkkit_gateway_subdev_login(dev);    /* relogin all logined subdev */
            if (ret == 0)
                dev->online = 1;
            handle_return_code(dev, ret);
        }
    }
    HAL_MutexUnlock(gbl_lock);
#endif
    return 0;
}

static int _offline_all_subdevs(void)
{
#if 0
    topo_node_t *n = NULL;

    HAL_MutexLock(gbl_lock);
    for (n = first_node; n; n = n->next) {
        linkkit_dev_t *dev = n->subdev;
        if (!dev)
            continue;
        dev->online = 0;
    }
    HAL_MutexUnlock(gbl_lock);
#endif
    return 0;
}

static void* _linkkit_dispatch(void *params)
{
	while (1) {
		IOT_DM_Dispatch();
		HAL_SleepMs(200);
	}
	return NULL;
}

static void _linkkit_event_subdev_register_reply(char *payload)
{
    int res = 0;
    lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid;

    if (payload == NULL) {return;}

    /* Parse JSON */
    res = lite_cjson_parse(payload,strlen(payload),&lite);
    if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {return;}

    /* Parse Message ID */
    res = lite_cjson_object_item(&lite,LINKKIT_GW_API_KEY_ID,strlen(LINKKIT_GW_API_KEY_ID),&lite_item_id);
    if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_id)) {return;}
    dm_log_info("Current Msg ID: %d",lite_item_id.value_int);

    /* Parse Message Code */
    res = lite_cjson_object_item(&lite,LINKKIT_GW_API_KEY_CODE,strlen(LINKKIT_GW_API_KEY_CODE),&lite_item_code);
    if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_code)) {return;}
    dm_log_info("Current Code: %d",lite_item_code.value_int);

    /* Parse Devid */
    res = lite_cjson_object_item(&lite,LINKKIT_GW_API_KEY_DEVID,strlen(LINKKIT_GW_API_KEY_DEVID),&lite_item_devid);
    if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_devid)) {return;}
    dm_log_info("Current devid: %d",lite_item_devid.value_int);

    IOT_DM_Subdev_Topo_Add(lite_item_devid.value_int);
}

static void _linkkit_event_topo_add_reply(char *payload)
{
    int res = 0;
    lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid;

    if (payload == NULL) {return;}

    /* Parse JSON */
    res = lite_cjson_parse(payload,strlen(payload),&lite);
    if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {return;}

    /* Parse Message ID */
    res = lite_cjson_object_item(&lite,LINKKIT_GW_API_KEY_ID,strlen(LINKKIT_GW_API_KEY_ID),&lite_item_id);
    if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_id)) {return;}
    dm_log_info("Current Msg ID: %d",lite_item_id.value_int);

    /* Parse Message Code */
    res = lite_cjson_object_item(&lite,LINKKIT_GW_API_KEY_CODE,strlen(LINKKIT_GW_API_KEY_CODE),&lite_item_code);
    if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_code)) {return;}
    dm_log_info("Current Code: %d",lite_item_code.value_int);

    /* Parse Devid */
    res = lite_cjson_object_item(&lite,LINKKIT_GW_API_KEY_DEVID,strlen(LINKKIT_GW_API_KEY_DEVID),&lite_item_devid);
    if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_devid)) {return;}
    dm_log_info("Current devid: %d",lite_item_devid.value_int);

    IOT_DM_Subdev_Login(lite_item_devid.value_int);
}


void _linkkit_gw_event_callback(iotx_dm_event_types_t type, char *payload)
{
    dm_log_info("Receive Message Type: %d",type);
    if (payload) {dm_log_info("Receive Message: %s",payload);}

    switch(type) {
        case IOTX_DM_EVENT_CLOUD_CONNECTED: {
                gbl.connected = 1;
                main_device->online = 1;
                _online_all_subdevs();
                #if 0
                if (gbl.main_device_inited == 0) {
                    subscribe_all_topics(main_device);
                    topo_get_all_devices();
                    cmp_add_alcs_topics(main_device->productKey, main_device->deviceName);
                    gbl.main_device_inited = 1;
                }
                #endif
                if (gbl.event_cb) {
                    linkkit_event_t ev = {
                        .event_type = LINKKIT_EVENT_CLOUD_CONNECTED,
                    };
                    gbl.event_cb(&ev, gbl.ctx);
                }
            }
            break;
        case IOTX_DM_EVENT_CLOUD_DISCONNECT:
            {
                gbl.connected = 0;
                _offline_all_subdevs();
                main_device->online = 0;

                if (gbl.event_cb) {
                    linkkit_event_t ev = {
                        .event_type = LINKKIT_EVENT_CLOUD_DISCONNECTED,
                    };
                    gbl.event_cb(&ev, gbl.ctx);
                }
                break;
            }
        case IOTX_DM_EVENT_SUBDEV_REGISTER_REPLY:
            {
                _linkkit_event_subdev_register_reply(payload);
                break;
            }
        case IOTX_DM_EVENT_SUBDEV_UNREGISTER_REPLY:
            {

            }
            break;
        case IOTX_DM_EVENT_TOPO_ADD_REPLY:
            {
                _linkkit_event_topo_add_reply(payload);
            }
            break;
        case IOTX_DM_EVENT_PROPERTY_SET: {
            int res = 0, devid = 0;
			lite_cjson_t lite,lite_item_devid,lite_item_payload;
            char *params = NULL;

			/* Parse Payload */
			memset(&lite,0,sizeof(lite_cjson_t));
			res = lite_cjson_parse(payload,strlen(payload),&lite);
			if (res != SUCCESS_RETURN) {return;}

			/* Parse Devid */
			memset(&lite_item_devid,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_GW_API_KEY_DEVID,strlen(LINKKIT_GW_API_KEY_DEVID),&lite_item_devid);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Devid: %d",lite_item_devid.value_int);

			/* Parse Payload */
			memset(&lite_item_payload,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_GW_API_KEY_PAYLOAD,strlen(LINKKIT_GW_API_KEY_PAYLOAD),&lite_item_payload);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Payload: %.*",lite_item_payload.value_length,lite_item_payload.value);

			params = DM_malloc(lite_item_payload.value_length + 1);
			if (params == NULL) {
				dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
				return;
			}
			memset(params,0,lite_item_payload.value_length + 1);
			memcpy(params,lite_item_payload.value,lite_item_payload.value_length);

            linkkit_dev_t *dev = _find_device_by_devid(devid);
            dev->cbs.set_property(params, dev->ctx);

			DM_free(params);
            }
            break;

        case IOTX_DM_EVENT_GATEWAY_PERMIT: {
				int res = 0;
				char product_key[PRODUCT_KEY_MAXLEN] = {0};
				lite_cjson_t lite,lite_item_pk,lite_item_timeout;

				/* Parse Payload */
				memset(&lite,0,sizeof(lite_cjson_t));
				res = lite_cjson_parse(payload,strlen(payload),&lite);
				if (res != SUCCESS_RETURN) {return;}

				/* Parse Product Key */
				memset(&lite_item_pk,0,sizeof(lite_cjson_t));
				res = lite_cjson_object_item(&lite,LINKKIT_GW_API_KEY_PRODUCT_KEY,strlen(LINKKIT_GW_API_KEY_PRODUCT_KEY),&lite_item_pk);
				if (res != SUCCESS_RETURN || lite_item_pk.value_length >= PRODUCT_KEY_MAXLEN) {return;}
				dm_log_debug("Current Product Key: %.*s",lite_item_pk.value_length,lite_item_pk.value);

				/* Parse Timeout */
				memset(&lite_item_timeout,0,sizeof(lite_cjson_t));
				res = lite_cjson_object_item(&lite,LINKKIT_GW_API_KEY_TIMEOUT,strlen(LINKKIT_GW_API_KEY_TIMEOUT),&lite_item_timeout);
				if (res != SUCCESS_RETURN) {return;}
				dm_log_debug("Current Timeout: %d",lite_item_timeout.value_int);

				memcpy(product_key,lite_item_pk.value,lite_item_pk.value_length);

                if (gbl.event_cb) {
                    linkkit_event_t ev = {
                        .event_type = LINKKIT_EVENT_SUBDEV_PERMITED,
                        .event_data.subdev_permited.productKey = product_key,
                        .event_data.subdev_permited.timeoutSec = lite_item_timeout.value_int,
                    };
                    gbl.event_cb(&ev, gbl.ctx);
                }
            }
            break;
        case IOTX_DM_EVENT_THING_SERVICE_REQUEST:{
                int res = 0;
				lite_cjson_t lite, lite_item_id, lite_item_devid, lite_item_serviceid, lite_item_paylaod;
                char *identifier = NULL;
                char *payload = NULL;
                char *out_json = NULL;

				/* Parse Payload */
				memset(&lite,0,sizeof(lite_cjson_t));
				res = lite_cjson_parse(payload,strlen(payload),&lite);
				if (res != SUCCESS_RETURN) {return;}

				/* Parse Id */
				memset(&lite_item_id,0,sizeof(lite_cjson_t));
				res = lite_cjson_object_item(&lite,LINKKIT_GW_API_KEY_ID,strlen(LINKKIT_GW_API_KEY_ID),&lite_item_id);
				if (res != SUCCESS_RETURN) {return;}
				dm_log_debug("Current Id: %d",lite_item_id.value_int);

				/* Parse Devid */
				memset(&lite_item_devid,0,sizeof(lite_cjson_t));
				res = lite_cjson_object_item(&lite,LINKKIT_GW_API_KEY_DEVID,strlen(LINKKIT_GW_API_KEY_DEVID),&lite_item_devid);
				if (res != SUCCESS_RETURN) {return;}
				dm_log_debug("Current Devid: %d",lite_item_devid.value_int);

				/* Parse Serviceid */
				memset(&lite_item_serviceid,0,sizeof(lite_cjson_t));
				res = lite_cjson_object_item(&lite,LINKKIT_GW_API_KEY_SERVICEID,strlen(LINKKIT_GW_API_KEY_SERVICEID),&lite_item_serviceid);
				if (res != SUCCESS_RETURN) {return;}
				dm_log_debug("Current ServiceID: %.*s",lite_item_serviceid.value_length,lite_item_serviceid.value);

				/* Parse Payload */
				memset(&lite_item_paylaod,0,sizeof(lite_cjson_t));
				res = lite_cjson_object_item(&lite,LINKKIT_GW_API_KEY_PAYLOAD,strlen(LINKKIT_GW_API_KEY_PAYLOAD),&lite_item_paylaod);
				if (res != SUCCESS_RETURN) {return;}
				dm_log_debug("Current Payload: %.*s",lite_item_paylaod.value_length,lite_item_paylaod.value);

				identifier = DM_malloc(lite_item_serviceid.value_length + 1);
				if (identifier == NULL) {
					dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
					return;
				}
				memset(identifier,0,lite_item_serviceid.value_length + 1);
				memcpy(identifier,lite_item_serviceid.value,lite_item_serviceid.value_length);

				payload = DM_malloc(lite_item_paylaod.value_length + 1);
				if (payload == NULL) {
					DM_free(identifier);
					dm_log_warning(IOTX_DM_LOG_MEMORY_NOT_ENOUGH);
					return;
				}
				memset(payload,0,lite_item_paylaod.value_length + 1);
				memcpy(payload,lite_item_paylaod.value,lite_item_paylaod.value_length);

				out_json = AMemPool_Get(gbl.msgbuf_pool);
			    if (!out_json) {
					IOT_DM_Legacy_Send_Service_Response(lite_item_devid.value_int,lite_item_id.value_int,202,lite_item_serviceid.value,lite_item_serviceid.value_length,"{}",strlen("{}"));
			        DM_free(identifier);DM_free(payload);
			        return;
			    }
			    out_json[0] = '\0';
			    out_json[gbl.max_msg_size - 1] = '\0';

                linkkit_dev_t *dev = _find_device_by_devid(lite_item_devid.value_int);
                if (dev->cbs.call_service)
                    res = dev->cbs.call_service(identifier, payload, out_json, gbl.max_msg_size - 1, dev->ctx);

				if (res == SUCCESS_RETURN) {
					IOT_DM_Legacy_Send_Service_Response(lite_item_devid.value_int,lite_item_id.value_int,200,lite_item_serviceid.value,lite_item_serviceid.value_length,out_json,strlen(out_json));
				}else{
					IOT_DM_Legacy_Send_Service_Response(lite_item_devid.value_int,lite_item_id.value_int,202,lite_item_serviceid.value,lite_item_serviceid.value_length,"{}",strlen("{}"));
				}

				DM_free(identifier);DM_free(payload);
				AMemPool_Put(gbl.msgbuf_pool, out_json);
            }
            break;
		case IOTX_DM_EVENT_MODEL_DOWN_RAW: {
			int res = 0;
			lite_cjson_t lite, lite_item_devid, lite_item_rawdata;

			/* Parse Payload */
			memset(&lite,0,sizeof(lite_cjson_t));
			res = lite_cjson_parse(payload,strlen(payload),&lite);
			if (res != SUCCESS_RETURN) {return;}

			/* Parse Devid */
			memset(&lite_item_devid,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_GW_API_KEY_DEVID,strlen(LINKKIT_GW_API_KEY_DEVID),&lite_item_devid);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Devid: %d",lite_item_devid.value_int);

			/* Parse Raw Data */
			memset(&lite_item_rawdata,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_GW_API_KEY_PAYLOAD,strlen(LINKKIT_GW_API_KEY_PAYLOAD),&lite_item_rawdata);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Raw Data: %.*s",lite_item_rawdata.value_length,lite_item_rawdata.value);

			linkkit_dev_t *dev = _find_device_by_devid(lite_item_devid.value_int);
		    if (!dev) {return;}

		    void *respbuf = AMemPool_Get(gbl.msgbuf_pool);
 		    if (!respbuf) {return;}

		    ssize_t ret = -1;

		    if (dev->cbs.down_rawdata)
		        ret = dev->cbs.down_rawdata(lite_item_rawdata.value, lite_item_rawdata.value_length, respbuf, gbl.max_msg_size, dev->ctx);

		    if (ret > 0) {
				IOT_DM_Legacy_Send_Rawdata(lite_item_devid.value_int,(char *)respbuf,ret);
		    }

		    AMemPool_Put(gbl.msgbuf_pool, respbuf);
		}
		break;
		case IOTX_DM_EVENT_MODEL_UP_RAW_REPLY: {
			int res = 0;
			lite_cjson_t lite, lite_item_devid, lite_item_rawdata;

			/* Parse Payload */
			memset(&lite,0,sizeof(lite_cjson_t));
			res = lite_cjson_parse(payload,strlen(payload),&lite);
			if (res != SUCCESS_RETURN) {return;}

			/* Parse Devid */
			memset(&lite_item_devid,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_GW_API_KEY_DEVID,strlen(LINKKIT_GW_API_KEY_DEVID),&lite_item_devid);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Devid: %d",lite_item_devid.value_int);

			/* Parse Raw Data */
			memset(&lite_item_rawdata,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_GW_API_KEY_PAYLOAD,strlen(LINKKIT_GW_API_KEY_PAYLOAD),&lite_item_rawdata);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Raw Data: %.*s",lite_item_rawdata.value_length,lite_item_rawdata.value);

			linkkit_dev_t *dev = _find_device_by_devid(lite_item_devid.value_int);
		    if (!dev) {return;}

		    if (dev->cbs.post_rawdata_reply)
		        dev->cbs.post_rawdata_reply(lite_item_rawdata.value, lite_item_rawdata.value_length, dev->ctx);
		}
		break;
		case IOTX_DM_EVENT_REGISTER_COMPLETED: {
			int res = 0;
			lite_cjson_t lite, lite_item_devid;

			/* Parse Payload */
			memset(&lite,0,sizeof(lite_cjson_t));
			res = lite_cjson_parse(payload,strlen(payload),&lite);
			if (res != SUCCESS_RETURN) {return;}

			/* Parse Devid */
			memset(&lite_item_devid,0,sizeof(lite_cjson_t));
			res = lite_cjson_object_item(&lite,LINKKIT_GW_API_KEY_DEVID,strlen(LINKKIT_GW_API_KEY_DEVID),&lite_item_devid);
			if (res != SUCCESS_RETURN) {return;}
			dm_log_debug("Current Devid: %d",lite_item_devid.value_int);

			linkkit_dev_t *dev = _find_device_by_devid(lite_item_devid.value_int);
		    if (!dev) {return;}
			
			if (dev->cbs.register_complete)
				dev->cbs.register_complete(dev->ctx);
		}
		break;
        default:
            dm_log_info("Not Found Type For Now, Smile");
            break;
    }
}


static int _linkkit_recv_data_with_timeout(int id, int timeout_ms)
{
    int code = -1;
    char *data = NULL;
    int data_len = 0;

    watcher_t *w = watcher_new(id);
    if (!w) {
        return -1;
    }

    if (watcher_wait(w, &code, &data, &data_len, timeout_ms) < 0)
        goto fail;

    if (code != 200) {
        code = -code;
        goto fail;
    }

    if (data)
        AMemPool_Put(gbl.msgbuf_pool, data);
    watcher_free(w);
    return 0;

fail:
    if (data)
        AMemPool_Put(gbl.msgbuf_pool, data);
    watcher_free(w);
    return code;
}

linkkit_params_t *linkkit_gateway_get_default_params(void)
{
    static linkkit_params_t params = {
        .maxMsgSize      = 20 * 1024,
        .maxMsgQueueSize = 16,
        .threadPoolSize  = 4,
        .threadStackSize = 8 * 1024,
    };

    return &params;
}


int linkkit_gateway_set_option(linkkit_params_t *params, int option, void *value, int value_len)
{
    if (!params || !value || !value_len)
        return -1;

    switch (option) {
    case LINKKIT_OPT_MAX_MSG_SIZE:
        if (value_len != sizeof(int))
            return -1;
        params->maxMsgSize = *((int *)value);
        break;
    case LINKKIT_OPT_MAX_MSG_QUEUE_SIZE:
        if (value_len != sizeof(int))
            return -1;
        params->maxMsgQueueSize = *((int *)value);
        break;
    case LINKKIT_OPT_THREAD_POOL_SIZE:
        if (value_len != sizeof(int))
            return -1;
        params->threadPoolSize = *((int *)value);
        break;
    case LINKKIT_OPT_THREAD_STACK_SIZE:
        if (value_len != sizeof(int))
            return -1;
        params->threadStackSize = *((int *)value);
        break;
    default:
        dm_log_err("unknow option: %d\n", option);
        return -1;
    }

    return 0;
}
/**
 * @brief set event callback
 *
 * @param params, linkkit initialize parameters, return from linkkit_get_default_params().
 * @param event_cb, event callback.
 * @param ctx, user private data.
 *
 * @return 0 when success, < 0 when fail.
 */
int linkkit_gateway_set_event_callback(linkkit_params_t *params, int (*event_cb)(linkkit_event_t *ev, void *ctx), void *ctx)
{
    if (!params || !event_cb)
        return -1;

    params->event_cb = event_cb;
    params->ctx = ctx;

    return 0;
}
/**
 * @brief linkkit initialization.
 *
 * @param initParams, linkkit initialize parameters, see linkkit_params_t for more detail.
 *
 * @return 0 when success, < 0 when fail.
 */
int linkkit_gateway_init(linkkit_params_t *initParams)
{
    _dump_params(initParams);

    if (_checkInitParams(initParams) < 0) {
        dm_log_info("invalid initParams\n");
        return -1;
    }

    memset(&gbl, 0, sizeof(linkkit_gbl_t));

    gbl.event_cb = initParams->event_cb;
    gbl.ctx = initParams->ctx;

    gbl.max_msg_size = initParams->maxMsgSize;
    gbl.max_msgq_size = initParams->maxMsgQueueSize;

    gbl.mutex = HAL_MutexCreate();
    if (!gbl.mutex)
        goto fail;

    gbl.linkkit_msgq = lk_queue_new(gbl.max_msgq_size);
    if (!gbl.linkkit_msgq)
        goto fail;

    gbl.msgqblk_pool = AMemPool_New(sizeof(lk_msg_t), 4, NULL, NULL);
    if (!gbl.msgqblk_pool)
        goto fail;

    gbl.msgbuf_pool = AMemPool_New(gbl.max_msg_size, 2, NULL, NULL);
    if (!gbl.msgbuf_pool)
        goto fail;

    if (watcher_init() < 0)
        goto fail;

    gbl_lock = HAL_MutexCreate();
    if (!gbl_lock)
        goto fail;

    gbl.threadPoolSize = initParams->threadPoolSize;
    gbl.threadStackSize = initParams->threadStackSize;

    dm_log_info("!!! linkkit initialized !!!\n");

    return 0;

fail:
    linkkit_gateway_exit();
    return -1;
}


/**
 * @brief linkkit deinitialization.
 *
 * @return 0 when success, < 0 when fail.
 */
int linkkit_gateway_exit(void)
{
    gbl.linkkit_abort = 1;

    if (gbl.msgqblk_pool) {
        AMemPool_Free(gbl.msgqblk_pool);
        gbl.msgqblk_pool = NULL;
    }

    if (gbl.msgbuf_pool) {
        AMemPool_Free(gbl.msgbuf_pool);
        gbl.msgbuf_pool = NULL;
    }

    if (gbl.linkkit_msgq) {
        lk_queue_free(gbl.linkkit_msgq);
        gbl.linkkit_msgq = NULL;
    }

    if (gbl.mutex) {
        HAL_MutexDestroy(gbl.mutex);
        gbl.mutex = NULL;
    }

    if (gbl_lock) {
        HAL_MutexDestroy(gbl_lock);
        gbl_lock = NULL;
    }

    watcher_exit();

    memset(&gbl, 0, sizeof(linkkit_gbl_t));

    gbl.linkkit_abort = 0;

    dm_log_info("!!! linkkit exited !!!\n");

    return 0;
}


int linkkit_gateway_start(linkkit_cbs_t *cbs, void *ctx)
{
	int res = 0, stack_used = 0;
    iotx_dm_init_params_t dm_init_params;
    if (!_validOps(cbs)) {
        dm_log_info("invalid cbs: %p\n", cbs);
        return -1;
    }

    linkkit_dev_t *dev = LITE_malloc(sizeof(linkkit_dev_t));
    if (!dev) {
        dm_log_info("malloc linkkit_dev_t failed\n");
        return -1;
    }
    memset(dev, 0, sizeof(linkkit_dev_t));
    dev->devid = IOTX_DMGR_LOCAL_NODE_DEVID;
    dev->cbs = *cbs;
    dev->ctx = ctx;

    main_device = dev;
    memset(&dm_init_params,0,sizeof(iotx_dm_init_params_t));
    dm_init_params.secret_type = IOTX_DM_DEVICE_SECRET_DEVICE;
    dm_init_params.domain_type = IOTX_DM_CLOUD_DOMAIN_SHANGHAI;
    dm_init_params.event_callback = _linkkit_gw_event_callback;

    res = IOT_DM_Construct(&dm_init_params);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}

	res = IOT_DM_Set_TSL(IOTX_DMGR_LOCAL_NODE_DEVID,IOTX_DM_TSL_SOURCE_CLOUD,NULL,0);
	if (res != SUCCESS_RETURN) {return FAIL_RETURN;}
	
	res = HAL_ThreadCreate(&g_linkkit_dispatch,_linkkit_dispatch,NULL,NULL,&stack_used);
	if (res != SUCCESS_RETURN) {IOT_DM_Destroy();return FAIL_RETURN;}

	return SUCCESS_RETURN;
}

int linkkit_gateway_stop(int devid)
{
    LITE_free(main_device);
    return IOT_DM_Destroy();
}
/**
 * @brief register subdev to gateway.

 * @param productKey, subdev's product key.
 * @param deviceName, subdev's device name.
 * @param deviceSecret, subdev's device secret.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_subdev_register(char *productKey, char *deviceName, char *deviceSecret)
{
    int devid;
    int res = FAIL_RETURN;

    res = IOT_DM_Legacy_Get_Devid_By_Pkdn(productKey, deviceName, &devid);
    if (res != SUCCESS_RETURN) {
        return res;
    }
    res = IOT_DM_Subdev_Register(devid);
    return res;
}
/**
 * @brief deregister subdev from gateway.

 * @param productKey, subdev's product key.
 * @param deviceName, subdev's device name.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_subdev_unregister(char *productKey, char *deviceName)
{
    int devid;
    int res = FAIL_RETURN;


    res = IOT_DM_Legacy_Get_Devid_By_Pkdn(productKey, deviceName, &devid);
    if (res != SUCCESS_RETURN) {
        return res;
    }

    //Todo get device api from pk dn
    res = IOT_DM_Subdev_Topo_Del(devid);
    if (res != SUCCESS_RETURN) {
        return res;
    }
    res = IOT_DM_Subdev_Unregister(devid);

    linkkit_dev_t *dev = _find_device_by_devid(devid);
    if (dev != NULL && _is_subdev(dev)) {
        _subdev_node_del(dev);
        LITE_free(dev);
    }

    return res;
}
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
int linkkit_gateway_subdev_create(char *productKey, char *deviceName, linkkit_cbs_t *cbs, void *ctx)
{
    int devid;
    int res = FAIL_RETURN;

   //need to save cb & ctx
    res = IOT_DM_Subdev_Create(productKey, deviceName, &devid);
    if (res != SUCCESS_RETURN) {
        return res;
    }

    linkkit_dev_t *dev = LITE_malloc(sizeof(linkkit_dev_t));
    if (!dev) {
        dm_log_info("malloc failed: len = %ld\n", (long)sizeof(linkkit_dev_t));
        return ERROR_NO_MEM;
    }
    memset(dev, 0, sizeof(linkkit_dev_t));
    dev->devid = devid;
    dev->cbs = *cbs;
    dev->ctx = ctx;
    _subdev_node_add(dev);

    return devid;
}
/**
 * @brief destroy subdev by device id.

 * @param devid, device id return from linkkit_subdev_create().
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_subdev_destroy(int devid)
{
    linkkit_dev_t *dev = _find_device_by_devid(devid);
    if (dev != NULL && _is_subdev(dev)) {
        _subdev_node_del(dev);
        LITE_free(dev);
    }

    return IOT_DM_Subdev_Destroy(devid);
}
/**
 * @brief make subdev accessible from cloud.

 * @param devid, device id return from linkkit_subdev_create().
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_subdev_login(int devid)
{
    return IOT_DM_Subdev_Login(devid);
}

/**
 * @brief make subdev inaccessible on cloud.

 * @param devid, device id return from linkkit_subdev_create().
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_subdev_logout(int devid)
{
    return IOT_DM_Subdev_Logout(devid);
}
/**
 * @brief get device infomation specific by devid.
 *
 * @param devinfo, device information, see linkkit_devinfo_t for more detail.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_get_devinfo(int devid, linkkit_devinfo_t *devinfo)
{
    int res = FAIL_RETURN;
    if (devid < 0 || devinfo == NULL) {
        return -1;
    }
    memset(devinfo, 0, sizeof(linkkit_devinfo_t));
    res = IOT_DM_Legacy_Get_Pkdn_Ptr_By_Devid(devid, &(devinfo->productKey), &(devinfo->deviceName));
    if (res != SUCCESS_RETURN) {
        return res;
    }
    int type;
    iotx_dm_dev_status_t status;
    iotx_dm_dev_avail_t available;
    res = IOT_DM_Get_Device_Type(devid, &type);
    if (res != SUCCESS_RETURN) {
        return res;
    }
    if (type == IOTX_DM_DEVICE_GATEWAY) {
        devinfo->devtype = 0;
    } else if (type == IOTX_DM_DEVICE_SUBDEV) {
        devinfo->devtype = 1;
    } else {
        dm_log_info("wrong device type\n");
        return -1;
    }

    res = IOT_DM_Get_Device_Status(devid, &status);
    if (res != SUCCESS_RETURN) {
        return res;
    }
    if (status >= IOTX_DMGR_DEV_STATUS_LOGINED) {
        devinfo->login = 1;
    }
    if (status == IOTX_DMGR_DEV_STATUS_ONLINE) {
        devinfo->online = 1;
    }

    res = IOT_DM_Get_Device_Avail_Status(devid, &available);
    if (res != SUCCESS_RETURN) {
        return res;
    }
    devinfo->state = available;
    return res;
}
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
int linkkit_gateway_trigger_event_json_sync(int devid, char *identifier, char *event, int timeout_ms)
{
    int res = FAIL_RETURN;
    int id;

    if (!identifier || !strlen(identifier) || !event || !strlen(event) || timeout_ms < 0) {
        dm_log_info("invalid parameters\n");
        return -1;
    }

    if (timeout_ms == 0)
        return IOT_DM_Post_Event_Direct(devid, identifier, strlen(identifier), event, strlen(event));

    res = IOT_DM_Post_Event_Direct(devid, identifier, strlen(identifier), event, strlen(event));
    if (res < 0) {
        dm_log_err("DM", "%d", res);
        log_err_online("DM", "%d", res);
        return -1;
    } else {
        id = res;
    }

    res = _linkkit_recv_data_with_timeout(id, timeout_ms);
    return res;
}

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
int linkkit_gateway_trigger_event_json(int devid, char *identifier, char *event, int timeout_ms, void (*func)(int retval, void *ctx), void *ctx)
{
    int res = FAIL_RETURN;
    int id;

    if (!identifier || !strlen(identifier) || !event || !strlen(event) || timeout_ms < 0) {
        dm_log_info("invalid parameters\n");
        return -1;
    }
    if (timeout_ms == 0)
        return IOT_DM_Post_Event_Direct(devid, identifier, strlen(identifier), event, strlen(event));

    post_event_t *ev = LITE_malloc(sizeof(post_event_t));
    if (!ev) {
        return -1;
    }

    ev->func = func;
    ev->ctx = ctx;

    wait_obj_t *obj = LITE_malloc(sizeof(wait_obj_t));
    if (!obj)
        return -1;
    memset(obj, 0, sizeof(wait_obj_t));

    obj->cb = _async_post_event;
    obj->ctx = ev;
    obj->timeout_ms = timeout_ms;

    res = IOT_DM_Post_Event_Direct(devid, identifier, strlen(identifier), event, strlen(event));
    if (res < 0) {
        LITE_free(obj);
        return -1;
    }
    id = res;
    obj->w = watcher_new(id);
    if (!obj->w) {
        LITE_free(obj);
        return -1;
    }

    if (lk_thread_create_detached("linkkit_transfer_async", 2048, _response_waiter, obj) < 0) {
        watcher_free(obj->w);
        LITE_free(obj);
        return -1;
    }
    return res;
}


/**
 * @brief post property to cloud.
 *
 * @param devid, device id return from linkkit_gateway_start() or linkkit_subdev_create().
 * @param property, property data, in JSON format.
 * @param timeout_ms, transmission timeout, in milliseconds. when timeout_ms is 0, wait no response.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_post_property_json_sync(int devid, char *property, int timeout_ms)
{
    int res = FAIL_RETURN;
    int id;

    if (!property || !strlen(property) || timeout_ms < 0)
        return -1;

    if (timeout_ms == 0)
        return IOT_DM_Post_Property_Direct(devid, property, strlen(property));

    res = IOT_DM_Post_Property_Direct(devid, property, strlen(property));
    if (res < 0) {
        dm_log_err("DM", "%d", res);
        log_err_online("DM", "%d", res);
        return -1;
    } else {
        id = res;
    }

    res = _linkkit_recv_data_with_timeout(id, timeout_ms);
    return res;
}

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
int linkkit_gateway_post_property_json(int devid, char *property, int timeout_ms, void (*func)(int retval, void *ctx), void *ctx)
{
    int res = FAIL_RETURN;
    int id;

    if (!property || !strlen(property) || timeout_ms < 0)
        return -1;

    if (timeout_ms == 0)
        return IOT_DM_Post_Property_Direct(devid, property, strlen(property));

    post_event_t *ev = LITE_malloc(sizeof(post_event_t));
    if (!ev) {
        return -1;
    }

    ev->func = func;
    ev->ctx = ctx;

    wait_obj_t *obj = LITE_malloc(sizeof(wait_obj_t));
    if (!obj)
        return -1;
    memset(obj, 0, sizeof(wait_obj_t));

    obj->cb = _async_post_event;
    obj->ctx = ev;
    obj->timeout_ms = timeout_ms;

    res = IOT_DM_Post_Property_Direct(devid, property, strlen(property));
    if (res < 0) {
        LITE_free(obj);
        return -1;
    }
    id = res;
    obj->w = watcher_new(id);
    if (!obj->w) {
        LITE_free(obj);
        return -1;
    }

    if (lk_thread_create_detached("linkkit_transfer_async", 2048, _response_waiter, obj) < 0) {
        watcher_free(obj->w);
        LITE_free(obj);
        return -1;
    }
    return 0;
}

/**
 * @brief post raw data to cloud.
 *
 * @param devid, device id return from linkkit_gateway_start() or linkkit_subdev_create().
 * @param data, raw data buffer pointer.
 * @param len, raw data length.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_post_rawdata(int devid, void *data, int len)
{
    return IOT_DM_Post_Rawdata(devid, data, len);
}

/**
 * @brief init fota service and install callback funstion.
 *
 * @param cb, callback function to be installed.
 * @param ctx, user data passed to callback function.
 *
 * @return int, 0 when success, -1 when fail.
 */
int linkkit_gateway_ota_init(void (*cb)(int event, const char *version, void *ctx), void *ctx)
{
    //not support
    return -1;
}

/**
 * @brief perform system upgrade when "new version detected" event reported.
 *
 * @param recvbuf_length, receive buffer length that used to receive upgrade package.
 *
 * @return 0 when success, -1 when fail.
 */
int linkkit_gateway_ota_update(int recvbuf_length)
{
    //not support
    return -1;
}

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
int linkkit_gateway_post_extinfos(int devid, linkkit_extinfo_t *extinfos, int nb_extinfos, int timeout_ms)
{
    int res = FAIL_RETURN;
    int id;

    if (timeout_ms < 0) {
        return -1;
    }
    char *reqbuf = AMemPool_Get(gbl.msgbuf_pool);
    if (reqbuf == NULL) {
        return -1;
    }

    if (tmpmsg_post_extinfos(reqbuf, gbl.max_msg_size, extinfos, nb_extinfos) < 0) {
        AMemPool_Put(gbl.msgbuf_pool, reqbuf);
        return -1;
    }

    res = IOT_DM_DeviceInfo_Update(devid, reqbuf, strlen(reqbuf));
    if (res < 0) {
        AMemPool_Put(gbl.msgbuf_pool, reqbuf);
        return res;
    } else {
        //res return id
        id = res;
    }
    dm_log_info(IOTX_DM_LOG_OPERATOR_RES, 2, res);
    res = _linkkit_recv_data_with_timeout(id, timeout_ms);
    AMemPool_Put(gbl.msgbuf_pool, reqbuf);
    return res;
}

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
int linkkit_gateway_delete_extinfos(int devid, linkkit_extinfo_t *extinfos, int nb_extinfos, int timeout_ms)
{
    int res = FAIL_RETURN;
    int id;

    if (timeout_ms < 0) {
        return -1;
    }
    char *reqbuf = AMemPool_Get(gbl.msgbuf_pool);
    if (!reqbuf)
        return -1;

    if (tmpmsg_delete_extinfos(reqbuf, gbl.max_msg_size, extinfos, nb_extinfos) < 0) {
        AMemPool_Put(gbl.msgbuf_pool, reqbuf);
        return -1;
    }
    res = IOT_DM_DeviceInfo_Delete(devid, reqbuf, strlen(reqbuf));
    if (res < 0) {
        AMemPool_Put(gbl.msgbuf_pool, reqbuf);
        return res;
    } else {
        //res return id
        id = res;
    }
    dm_log_info(IOTX_DM_LOG_OPERATOR_RES, 2, res);
    res = _linkkit_recv_data_with_timeout(id, timeout_ms);
    AMemPool_Put(gbl.msgbuf_pool, reqbuf);
    return res;
}

/**
 * @brief get number devices currently in gateway
 *
 * @return number devinfos.
 */
int linkkit_gateway_get_num_devices(void)
{
    return IOT_DM_Subdev_Number();
}
/**
 * @brief get all devices currently in gateway
 *
 * @param devinfos, device information array, see linkkit_devinfo_t for more detail.
 * @param nb_devinfos, number of extend infos in extinfos.
 *
 * @return number devinfos filled.
 */
int linkkit_gateway_get_devinfos(linkkit_devinfo_t *devinfos, int nb_devinfos)
{
    return 0;
}
