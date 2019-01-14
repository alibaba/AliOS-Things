/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 * uData service manager part
 */

#include "service_mgr.h"
#include <stdio.h>
#include <string.h>
#include "aos/kernel.h"
#include "service_process.h"
#include "abs_data_model.h"

#include "udata_queue.h"


static udata_service_t *g_service_db[UDATA_MAX_CNT];
static uint32_t         g_service_cnt = 0;
static udata_pkg_t      g_udata_buf[UDATA_MAX_CNT];
static udata_pkg_t      g_sensor_buf[SENSOR_MAX_NUM];

static int udata_find_free_index(void)
{
    int index = 0;
    for (index = 0; index < UDATA_MAX_CNT; index++) {
        if (g_service_db[index] == NULL){
            return index;
        }
    }

    return -1;
}


int udata_find_service(udata_type_e type)
{
    int index = 0;

    if (type >= UDATA_MAX_CNT) {
        return -1;
    }
    
    for (index = 0; index < UDATA_MAX_CNT; index++) {
        if (g_service_db[index] == NULL){
            continue;
        }
        if (g_service_db[index]->type == type) {
            return index;
        }
    }

    return -1;
}

int udata_service_get_payload(uint32_t abs_index, void **pdata, uint32_t *plen)
{
    work_mode_e      mode;

    if ((NULL == pdata) || (NULL == plen)) {
        return -1;
    }
    
    mode = abs_sensor_mode_get(abs_index);
    if (DEV_MODE_INVALID == mode){
        return -1;
    }

    if (mode == DEV_FIFO) {
        *pdata = abs_sensor_buff_addr_get(abs_index);
        *plen  = abs_sensor_buff_len_get(abs_index);
    } else {
        *pdata  = &(g_sensor_buf[abs_index].payload[0]);
        *plen   = DATA_SIZE;
    }

    if ((NULL == *pdata) || (0 == *plen)) {
        return -1;
    }

    return 0;
}


static int udata_dev_enable(uint32_t abs_index)
{
    int ret = 0;
    ret     = abs_data_dev_enable(abs_index);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}
int udata_data_publish(int idx)
{
    int ret; 
    int index;
    sensor_msg_pkg_t data_msg;
    
    index = udata_find_service(idx);
    if ((index < 0) || (index >= UDATA_MAX_CNT)){
        return -1;
    }

    if (g_service_db[index] == NULL){
        return -1;
    }
#ifdef DATA_TO_CLOUD
    if (true == service_dtc_is_publish(g_service_db[index]->type))
    {
        memset(&data_msg, 0, sizeof(data_msg));
        data_msg.cmd   = UDATA_MSG_DATA_TO_CLOUD;
        data_msg.value = g_service_db[index]->type;
        ret            = udata_post_msg(data_msg);
        if (unlikely(ret)) {
            return -1;
        }
    }
#endif
    if (g_service_db[index]->subscribe == true) 
    {
        memset(&data_msg, 0, sizeof(data_msg));
        data_msg.cmd   = UDATA_MSG_REPORT_PUBLISH;
        data_msg.value = g_service_db[index]->type;
        ret            = udata_post_msg(data_msg);
        if (unlikely(ret)) {
            return -1;
        }
    }
    return 0;
}

int udata_get_report_pkg(udata_type_e type,void *buf)
{
    if (buf == NULL) {
        return -1;
    }
    if (type >= UDATA_MAX_CNT) {
        return -1;
    }
    memcpy(buf, &g_udata_buf[type], sizeof(udata_pkg_t));
    return 0;
}


int udata_install_report_pkg(udata_type_e type, void *data, size_t len)
{
    if (data == NULL) {
        return -1;
    }
    if (type >= UDATA_MAX_CNT) {
        return -1;
    }
    /* intall the report data package here */
    memset(&g_udata_buf[type], 0, sizeof(udata_pkg_t));
    g_udata_buf[type].valid = true;
    g_udata_buf[type].type  = type;
    if (sizeof(g_udata_buf[type].payload) < len) {
        return -1;
    }
    memcpy(g_udata_buf[type].payload, data, len);

    return 0;
}

int udata_service_subscribe(udata_type_e type)
{
    int ret   = 0;
    int index = 0;

    index = udata_find_service(type);
    if ((index < 0) || (index >= UDATA_MAX_CNT)){
        return -1;
    }

    if (g_service_db[index]->type == type) {
        if (g_service_db[index]->running != true) {
            ret = abs_data_open(g_service_db[index]);
            if (unlikely(ret)) {
                return -1;
            }
        }
        g_service_db[index]->subscribe = true;
        g_service_db[index]->running   = true;
    }
    return 0;
}

int udata_service_unsubscribe(udata_type_e type)
{
    int i     = 0;
    int ret   = 0;
    int index = 0;

    index = udata_find_service(type);
    if ((index < 0) || (index >= UDATA_MAX_CNT)){
        return -1;
    }

    if (g_service_db[index]->type == type) {
        if (g_service_db[index]->running != false) {
            for(i = 0; i < ABS_DATA_BITMAP_SUM; i++){
                if(!UDATA_BITMAP_COMPARE(g_service_db[index]->abs_bitmap, i)){
                    continue;
                }
                ret = abs_data_close(i);
                if (unlikely(ret)) {
                    return -1;
                }
            }
        }
        g_service_db[index]->subscribe = false;
    }
    return 0;
}


int udata_service_register(udata_service_t *service)
{

    int ret = 0;
    int index;
    if (service == NULL) {
        return -1;
    }

    index = udata_find_free_index();
    if ((index < 0) || (index >= UDATA_MAX_CNT)){
        return -1;
    }

    g_service_db[index] = (udata_service_t *)aos_malloc(sizeof(udata_service_t));
    if (g_service_db[index] == NULL) {
        return -1;
    }

    memset(g_service_db[index], 0, sizeof(udata_service_t));

    /* also can copy the data struct by memcpy here */
    g_service_db[index]->type               = service->type;
    g_service_db[index]->task_flag          = service->task_flag;
    g_service_db[index]->config.inerval     = service->config.inerval;
    g_service_db[index]->config.range       = service->config.range;
    g_service_db[index]->config.id          = service->config.id;
    g_service_db[index]->service_process_cb = service->service_process_cb;
    g_service_db[index]->service_ioctl_cb   = service->service_ioctl_cb;
    memcpy(g_service_db[index]->abs_bitmap,service->abs_bitmap,sizeof(service->abs_bitmap));
    memcpy(g_service_db[index]->interval,service->interval,sizeof(service->interval));

    ret = abs_data_open(g_service_db[index]);
    if (unlikely(ret)) {
        LOG("%s %s fail line:%d type %d \n", uDATA_STR, __func__, __LINE__,service->type);
        goto error;
    }

    /* set the defaul status and no subsrcribed from aliyun side */
    g_service_db[index]->running   = true;
    g_service_db[index]->subscribe = false;
    g_service_cnt++;

    return 0;

error:
    if(g_service_db[index] != NULL){
        aos_free(g_service_db[index]);
        g_service_db[index] = NULL;
    }
    return -1;
}

int udata_service_unregister(udata_type_e type)
{
    int i     = 0;
    int index = 0;
    int ret   = 0;
    
    if (type >= UDATA_MAX_CNT) {
        return -1;
    }
    /* find the matched service, then del it from the list here */
    for (index = 0; index < UDATA_MAX_CNT; index++) {
        if (g_service_db[index] == NULL){
            continue;
        }
        
        if (g_service_db[index]->type != type) {
            continue;
        }
        for(i = 0; i < ABS_DATA_BITMAP_SUM; i++){
            if(!UDATA_BITMAP_COMPARE(g_service_db[index]->abs_bitmap, i)){
                continue;
            }
            ret = abs_data_close(i);
            if (unlikely(ret)) {
                return -1;
            }

        }

        memset(g_service_db[index], 0, sizeof(udata_service_t));
        aos_free(g_service_db[index]);
        g_service_db[index] = NULL;
        
        if (g_service_cnt > 0) {
            g_service_cnt--;
        }
        return 0;
    }

    return -1;
}


static int udata_service_process(uint32_t abs_index, void *data, uint32_t len)
{
    int    index    = 0;
    size_t size = 0;
    uint64_t time_stamp;
    if (data == NULL) {
        return -1;
    }

    time_stamp = *(uint64_t*)data;
    /* find the matched service, then run the registered callback here */
    for (index = 0; index < UDATA_MAX_CNT; index++) {
        if (g_service_db[index] == NULL){
            continue;
        }
        
        if (UDATA_BITMAP_COMPARE(g_service_db[index]->abs_bitmap ,abs_index) &&
            (!g_service_db[index]->task_flag) &&
            (g_service_db[index]->service_process_cb != NULL) &&
            (time_stamp - g_service_db[index]->time[abs_index] >= (uint64_t)(g_service_db[index]->interval[abs_index]))) {
            size = g_service_db[index]->service_process_cb(g_service_db[index]->type, abs_index, data, len);
            if (size != 0) {
                udata_data_publish(g_service_db[index]->type);
            }
            g_service_db[index]->time[abs_index] = time_stamp;
        }
    }
    return 0;
}

int udata_service_ioctl(udata_type_e type, void *parm)
{
    int i;
    int index;
    int ret;
    dev_sensor_full_info_t *sensor_config;
    if (parm == NULL) {
        return -1;
    }
    sensor_config = parm;

    index = udata_find_service(type);
    if ((index < 0) || (index >= UDATA_MAX_CNT)){
        return -1;
    }
    
    /* find the matched service, then run the registered ioctl callback here */
    if ((g_service_db[index]->service_ioctl_cb != NULL)) {
        for(i = 0; i < ABS_DATA_BITMAP_SUM; i++){
            if(!UDATA_BITMAP_COMPARE(g_service_db[index]->abs_bitmap, i)){
                continue;
            }

            g_service_db[index]->service_ioctl_cb(type, i);
            /* if get the dev info, then run here */
            ret = abs_data_ioctl(i, sensor_config);
            if (unlikely(ret)) {
                return -1;
            }

        }
    }

    LOG("%s %s successfully \n", uDATA_STR, __func__);
    return 0;
}

int udata_service_cb_register(udata_type_e type, SERVICE_PROCESS_CB proc,SERVICE_IOCTL_CB ioctl)
{
    int i;
    if(type >= UDATA_MAX_CNT){
        return -1;
    }

    if(proc == NULL){
        return -1;
    }
    if(ioctl == NULL){
        return -1;
    }

    for(i = 0; i < UDATA_MAX_CNT; i++){
        if(g_service_db[i] == NULL){
            continue;
        }

        if(g_service_db[i]->type == type){
            break;
        }
    }

    if(i == UDATA_MAX_CNT){
        return -1;
    }
    
    g_service_db[i]->service_process_cb = proc;
    g_service_db[i]->service_ioctl_cb   = ioctl;

    return 0;
}



int udata_service_cb_unregister(udata_type_e type)
{
    int i;
    if(type >= UDATA_MAX_CNT){
        return -1;
    }

    for(i = 0; i < UDATA_MAX_CNT; i++){
        if(g_service_db[i] == NULL){
            continue;
        }

        if(g_service_db[i]->type == type){
            break;
        }
    }

    if(i == UDATA_MAX_CNT){
        return -1;
    }
    
    g_service_db[i]->service_process_cb = NULL;
    g_service_db[i]->service_ioctl_cb   = NULL;

    return 0;
}


void udata_dispatcher_handle(sensor_msg_pkg_t *msg)
{
    int ret = 0;
    void *data = NULL;
    uint32_t len = 0;

    /* all the cmd of sensorhub will be sent to be handled here;
       the dispatcher will asign the new sub task to the fitted model */
    if (msg == NULL) {
        return;
    }

    switch (msg->cmd) {
        case UDATA_MSG_DEV_READ: {
            ret = udata_service_get_payload(msg->index, &data, &len);
            if (unlikely(ret)) {
                return;
            }
            ret = abs_data_read(msg->index, data, len);
            if (ret <= 0) {
                return;
            }
        } break;

        case UDATA_MSG_SERVICE_PROCESS: {
            ret = udata_service_get_payload(msg->index, &data, &len);
            if (unlikely(ret)) {
                return;
            }
            ret = udata_service_process(msg->index, data, len);
            if(unlikely(ret)){
                return;
            }
        } break;

        case UDATA_MSG_DEV_IOCTL: {

        } break;

        case UDATA_MSG_DEV_OPEN: {
        } break;

        case UDATA_MSG_DEV_ENABLE: {
            ret = udata_dev_enable(msg->index);
            if(unlikely(ret)){
                return;
            }
        } break;

        case UDATA_MSG_DEV_DISABLE: {
        } break;

        case UDATA_MSG_SERVICE_SUBSRIBE: {
            ret = udata_service_subscribe(msg->value);
            if(unlikely(ret)){
                return;
            }
        } break;

        case UDATA_MSG_SERVICE_UNSUBSRIBE: {
            ret = udata_service_unsubscribe(msg->value);
            if(unlikely(ret)){
                return;
            }
        } break;

        case UDATA_MSG_DEV_CLOSE: {
            ret = abs_data_close(msg->index);
            if(unlikely(ret)){
                return;
            }
        } break;

        default:
            break;
    }
}


int udata_service_init(void)
{
    int ret;

    ret = service_process_init();
    if (unlikely(ret)) {
        return ret;
    }
    return 0;
}

int udata_service_mgr_init(void)
{
    int ret       = 0;
    g_service_cnt = 0;


    ret = udata_register_msg_handler(udata_dispatcher_handle);
    if (ret == -1) {
        LOG("error occur reg udata_dispatcher_process \n");
        return ret;
    }

    LOG("%s %s successfully \n", uDATA_STR, __func__);
    return 0;
}
