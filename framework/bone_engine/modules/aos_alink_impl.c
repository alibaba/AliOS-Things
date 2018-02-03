/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>

#include <aos/aos.h>

#include "aos_alink_impl.h"

#ifdef USE_ALINK

#include "aos/network.h"
#include "alink_export.h"
#include "json_parser.h"
#include "kvmgr.h"
#include <netmgr.h>
#include <accs.h>




#define AOS_ALINK_LOG  LOGW

static aos_log_level_t  log_level = AOS_LL_WARN;

static int is_alink_started = 0;

extern int cloud_is_connected(void);
extern const char *config_get_main_uuid(void);


/*
    把Alink收到的云端数据转发到yloop

    aos_task_name: DEFAULT-WORKQUEUE ----> aos_task_name=app

*/
static dlist_t gAlinkMsgList;
typedef struct _alink_msg {
    dlist_t list_head;
    int type;
    char* msg;
} alink_msg_t;
static aos_mutex_t gMutex;


static int alink_get_status()
{
    if( is_alink_started ) {
        if( cloud_is_connected() ) {
            return 2;
        } else {
            // 正在连接
            return 1;
        }
    }
    return 0;
}

static void aos_alink_loop(void *arg)
{
    alink_msg_t *pos;
    dlist_t *tmp = NULL;

    aos_mutex_lock(&gMutex, AOS_WAIT_FOREVER);

//    if( dlist_empty(&gAlinkMsgList) ){
//        AOS_ALINK_LOG("aos_alink_impl","dlist_empty aos_task_name=%s  \r\n", aos_task_name());
//    }

    dlist_for_each_entry_safe(&gAlinkMsgList, tmp, pos, alink_msg_t,
                              list_head) {
        if (pos->type == 0) {
            AOS_ALINK_LOG("aos_alink_impl","aos_task_name=%s  \r\n", aos_task_name());
            AOS_ALINK_LOG("aos_alink_impl","---> ALINK_GET_DEVICE_STATUS :  %s",pos->msg);
            alink_module_onGetAttrs(pos->msg);

        } else {
            AOS_ALINK_LOG("aos_alink_impl","aos_task_name=%s  \r\n", aos_task_name());
            AOS_ALINK_LOG("aos_alink_impl","---> ALINK_SET_DEVICE_STATUS :  %s",pos->msg);
            alink_module_onSetAttrs(pos->msg);
        }

        dlist_del(&pos->list_head);
        aos_free(pos->msg);
        aos_free(pos);
    }

    aos_schedule_call(aos_alink_loop, NULL);
    aos_mutex_unlock(&gMutex);

}


static int alink_cloud_report(const char *method, const char *json_buffer)
{
    return alink_report_async(method, json_buffer, NULL, NULL);
}

static void alink_cloud_connected(void)
{
    aos_post_event(EV_YUNIO, CODE_YUNIO_ON_CONNECTED, 0);
    AOS_ALINK_LOG("aos_alink_impl","alink cloud connected!");

    aos_cloud_register_backend(&alink_cloud_report);
    aos_cloud_trigger(CLOUD_CONNECTED, NULL);
}

static void alink_cloud_disconnected(void)
{
    aos_post_event(EV_YUNIO, CODE_YUNIO_ON_DISCONNECTED, 0);
    aos_cloud_trigger(CLOUD_DISCONNECTED, NULL);
}

static void alink_cloud_get_device_status(char *json_buffer)
{
    aos_cloud_trigger(GET_DEVICE_STATUS, json_buffer);
}

static void alink_cloud_set_device_status(char *json_buffer)
{
    aos_cloud_trigger(SET_DEVICE_STATUS, json_buffer);
}

// method "postDeviceData"
static int alink_post_data(const char* method, const char* jsonBuf)
{
    int ret;

    AOS_ALINK_LOG("aos_alink_impl","method=%s  jsonBuf = %s \n", method, jsonBuf);

    // 等同调用 alink_cloud_report
    ret = aos_cloud_report(method, jsonBuf, NULL, NULL);

    return ret;
}

/*
static void do_report(void)
{
        snprintf(post_data_buffer, post_data_buffer_size, PostDataFormat,
                device_state[ATTR_ERRORCODE_INDEX],
                device_state[ATTR_HUE_INDEX],
                device_state[ATTR_LUMINANCE_INDEX],
                device_state[ATTR_SWITCH_INDEX],
                device_state[ATTR_WORKMODE_INDEX]);
        AOS_ALINK_LOG("aos_alink_impl","start report async");
        aos_cloud_report(Method_PostData, post_data_buffer, NULL, NULL);
}
*/

static void cloud_get_device_status(int cb_type, const char *json_buffer)
{
    /*
    aos_task_name: DEFAULT-WORKQUEUE
    需要发到 aos_task_name=app
    */

    AOS_ALINK_LOG("aos_alink_impl","---> get device status :  %s",json_buffer);

    alink_msg_t *pMsg;

    if( json_buffer ) {
        pMsg = (alink_msg_t*)aos_malloc(sizeof(pMsg));
        pMsg->type = 0;
        pMsg->msg = aos_malloc(strlen(json_buffer)+1);
        strcpy(pMsg->msg, json_buffer);
        aos_mutex_lock(&gMutex, AOS_WAIT_FOREVER);
        dlist_add(&pMsg->list_head, &gAlinkMsgList);
        aos_mutex_unlock(&gMutex);
        aos_schedule_call(aos_alink_loop, NULL);
    }


}

static void cloud_set_device_status(int cb_type, const char *json_buffer)
{
    AOS_ALINK_LOG("aos_alink_impl","---> set device status : cb_type=%d , status= %s",cb_type, json_buffer);

    alink_msg_t *pMsg;

    if( json_buffer ) {
        pMsg = (alink_msg_t*)aos_malloc(sizeof(pMsg));
        pMsg->type = 1;
        pMsg->msg = aos_malloc(strlen(json_buffer)+1);
        strcpy(pMsg->msg, json_buffer);
        aos_mutex_lock(&gMutex, AOS_WAIT_FOREVER);
        dlist_add(&pMsg->list_head, &gAlinkMsgList);
        aos_mutex_unlock(&gMutex);
        aos_schedule_call(aos_alink_loop, NULL);
    }
}

static void cloud_connected(int cb_type, const char *json_buffer)
{
    // [cloud_connected#272] : taskname: DEFAULT-WORKQUEUE
    AOS_ALINK_LOG("aos_alink_impl","aos_task_name: %s", aos_task_name());
    //do_report();
}

static void cloud_disconnected(int cb_type, const char *json_buffer)
{
    AOS_ALINK_LOG("aos_alink_impl","alink cloud disconnected!");
}



static void alink_cloud_init(void)
{
    alink_register_callback(ALINK_CLOUD_CONNECTED, &alink_cloud_connected);
    alink_register_callback(ALINK_CLOUD_DISCONNECTED, &alink_cloud_disconnected);

    //alink_register_callback(ALINK_GET_DEVICE_RAWDATA, &alink_cloud_get_device_raw_data);
    //alink_register_callback(ALINK_SET_DEVICE_RAWDATA, &alink_cloud_set_device_raw_data);

    // 直连设备
    alink_register_callback(ALINK_GET_DEVICE_STATUS, &alink_cloud_get_device_status);
    alink_register_callback(ALINK_SET_DEVICE_STATUS, &alink_cloud_set_device_status);

    aos_cloud_register_callback(CLOUD_CONNECTED, &cloud_connected);
    aos_cloud_register_callback(CLOUD_DISCONNECTED, &cloud_disconnected);

    aos_cloud_register_callback(GET_DEVICE_STATUS, &cloud_get_device_status);
    aos_cloud_register_callback(SET_DEVICE_STATUS, &cloud_set_device_status);
}


int alink_cmd_cb(ALINK_CMD_e cmd, int arg0, void* arg1, void* arg2)
{
    int ret = 0;
    switch(cmd) {
        case ALINK_GETLOGLEVEL:
            ret = log_level;
            break;
        case ALINK_SETLOGLEVEL:
            //alink_set_loglevel(arg0); 未实现
            log_level = arg0;
            aos_set_log_level(log_level);
            break;
        case ALINK_GETUUID:
            if(config_get_main_uuid() && arg1)
                strcpy(arg1, config_get_main_uuid());
            break;
        case ALINK_GETSTATUS:
            ret = alink_get_status();
            break;
        case ALINK_ISRUNNING:
            if( alink_get_status() )
                ret = 1;
            break;
        case ALINK_START:
            if(is_alink_started == 0) {
                is_alink_started = 1;
                ret = alink_start();
            }
            break;
        case ALINK_STOP:
            if(is_alink_started) {
                is_alink_started = 0;
                ret = alink_end();
            }
            break;
        case ALINK_POSTDATA:
            ret = alink_post_data(arg1, arg2);
            break;
        default:
            break;
    }

    return ret;
}

static void alink_service_event(input_event_t *event, void *priv_data)
{

    AOS_ALINK_LOG("aos_alink_impl","aos_task_name=%s  \r\n", aos_task_name());

    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    if(is_alink_started == 0) {
        is_alink_started = 1;
        alink_start();
        alink_module_onStatusChange(alink_get_status(), config_get_main_uuid());
    }
}


static void alink_connect_event(input_event_t *event, void *priv_data)
{

    AOS_ALINK_LOG("aos_alink_impl","aos_task_name=%s \r\n", aos_task_name());

    if (event->type != EV_SYS) {
        return;
    }

    if (event->code == CODE_SYS_ON_ALINK_ONLINE ) {
        aos_post_event(EV_SYS, CODE_SYS_ON_START_FOTA, 0);

        //do_report();

        // zhengxiang.zzx
        // 设备上线了

        alink_module_onStatusChange(alink_get_status(), config_get_main_uuid());

        //unsigned int time;
        //int ret = alink_get_time(&time);
        //AOS_ALINK_LOG("aos_alink_impl","get alink utc time: %d", time);

        return;
    }
}


void aos_alink_init()
{
    // 使用 健康-体重秤
    //extern void alink_use_scales();
    //alink_use_scales();

    AOS_ALINK_LOG("aos_alink_impl","aos_task_name=%s \r\n", aos_task_name());

    if (aos_mutex_new(&gMutex) != 0) {
        LOGE("aos_alink_impl", "aos_mutex_new failed !");
    }

    dlist_init(&gAlinkMsgList);

    //aos_set_log_level(log_level);

    // alink初始化
    alink_cloud_init();

    // alink jse扩展注册
    alink_module_register_cmd_callback(alink_cmd_cb);

//  网络联通之后自动启动alink连接
//    aos_register_event_filter(EV_WIFI, alink_service_event, NULL);
    aos_register_event_filter(EV_SYS, alink_connect_event, NULL);

    alink_module_onStatusChange(alink_get_status(), config_get_main_uuid());

}

void aos_alink_exit()
{
    if(is_alink_started) {
        is_alink_started = 0;
        alink_end();
    }
}

int aos_alink_get_time(unsigned int *utc_time)
{
#define TIME_STR_LEN    (32)
    char buf[TIME_STR_LEN] = { 0 }, *attr_str;
    int size = TIME_STR_LEN, attr_len = 0;
    int ret;

    ret = alink_query("getAlinkTime", "{}", buf, &size);
    if (!ret) {
        attr_str = json_get_value_by_name(buf, size, "time", &attr_len, NULL);
        if (attr_str && utc_time) {
            sscanf(attr_str, "%u", utc_time);
        }
    }

    return ret;
}

#endif
