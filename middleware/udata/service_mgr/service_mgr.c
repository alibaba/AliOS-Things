/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 * uData service manager part
 */

#include "service_mgr.h"
#include <stdio.h>
#include <string.h>
#include <aos/aos.h>
#include "service_process.h"
#include "abs_data_model.h"

#include "uData_queue.h"


static uData_service_t *g_service_db[UDATA_MAX_CNT];
static uint32_t         g_service_cnt = 0;
static udata_pkg_t      g_udata_buf[UDATA_MAX_CNT];
static udata_pkg_t      g_sensor_buf[SENSOR_MAX_NUM];

static int uData_find_free_index(void)
{
    int index = 0;
    for (index = 0; index < UDATA_MAX_CNT; index++) {
        if (g_service_db[index] == NULL){
            return index;
        }
    }

    return -1;
}


int uData_find_service(udata_type_e type)
{
    int index = 0;
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

int uData_service_get_payload(uint32_t abs_index, void **paddr, uint32_t *plen)
{
    work_mode_e      mode = abs_sensor_mode_get(abs_index);

    if ((NULL == paddr) || (NULL == plen)) {
        return -1;
    }

    if (mode == DEV_FIFO) {
        *paddr = abs_sensor_buff_addr_get(abs_index);
        *plen  = abs_sensor_buff_len_get(abs_index);
    } else {
        *paddr  = &(g_sensor_buf[abs_index].payload[0]);
        *plen   = DATA_SIZE;
    }

    if ((NULL == *paddr) || (0 == *plen)) {
        return -1;
    }

    return 0;
}


static int uData_dev_enable(uint32_t abs_index)
{
    int ret = 0;
    ret     = abs_data_dev_enable(abs_index);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

int uData_get_report_pkg(void *buf, uint32_t index)
{
    if (buf == NULL) {
        return -1;
    }
    if (index >= UDATA_MAX_CNT) {
        return -1;
    }
    memcpy(buf, &g_udata_buf[index], sizeof(udata_pkg_t));
    return 0;
}


static int uData_install_report_pkg(uint32_t index, void *pdata, size_t len)
{
    if (pdata == NULL) {
        return -1;
    }
    if (index >= UDATA_MAX_CNT) {
        return -1;
    }
    /* intall the report data package here */
    memset(&g_udata_buf[index], 0, sizeof(udata_pkg_t));
    g_udata_buf[index].valid = true;
    g_udata_buf[index].type  = g_service_db[index]->type;
    if (sizeof(g_udata_buf[index].payload) < len) {
        return -1;
    }
    memcpy(g_udata_buf[index].payload, pdata, len);

    // aos_post_event(EV_UDATA, CODE_UDATA_REPORT_PUBLISH, index);

    return 0;
}

int uData_service_subscribe(udata_type_e type)
{
    int ret   = 0;
    int index = 0;

    index = uData_find_service(type);
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

int uData_service_unsubscribe(udata_type_e type)
{
    int i     = 0;
    int ret   = 0;
    int index = 0;

    index = uData_find_service(type);
    if ((index < 0) || (index >= UDATA_MAX_CNT)){
        return -1;
    }

    if (g_service_db[index]->type == type) {
        if (g_service_db[index]->running != false) {
            for(i = 0; i < ABS_DATA_BITMAP_SUM; i++){
                if(!ABS_DATA_EXSIST(g_service_db[index]->abs_bitmap, i)){
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


int uData_service_register(uData_service_t *service)
{

    int ret = 0;
    int index;
    if (service == NULL) {
        return -1;
    }

    index = uData_find_free_index();
    if ((index < 0) || (index >= UDATA_MAX_CNT)){
        return -1;
    }

    g_service_db[index] = (uData_service_t *)aos_malloc(sizeof(uData_service_t));
    if (g_service_db[index] == NULL) {
        return -1;
    }

    memset(g_service_db[index], 0, sizeof(uData_service_t));

    /* also can copy the data struct by memcpy here */
    g_service_db[index]->type               = service->type;
    g_service_db[index]->task_flag          = service->task_flag;
    g_service_db[index]->config.inerval     = service->config.inerval;
    g_service_db[index]->config.range       = service->config.range;
    g_service_db[index]->service_process_cb = service->service_process_cb;
    g_service_db[index]->service_ioctl_cb   = service->service_ioctl_cb;
    memcpy(g_service_db[index]->abs_bitmap,service->abs_bitmap,sizeof(service->abs_bitmap));
    memcpy(g_service_db[index]->interval,service->interval,sizeof(service->interval));

    ret = abs_data_open(g_service_db[index]);
    if (unlikely(ret)) {
        LOG("%s %s fail line:%d \n", uDATA_STR, __func__, __LINE__);
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

int uData_service_unregister(udata_type_e type)
{
    int i     = 0;
    int index = 0;
    int ret   = 0;
    /* find the matched service, then del it from the list here */
    for (index = 0; index < UDATA_MAX_CNT; index++) {
        if (g_service_db[index] == NULL){
            continue;
        }
        
        if (g_service_db[index]->type != type) {
            continue;
        }
        for(i = 0; i < ABS_DATA_BITMAP_SUM; i++){
            if(!ABS_DATA_EXSIST(g_service_db[index]->abs_bitmap, i)){
                continue;
            }
            ret = abs_data_close(i);
            if (unlikely(ret)) {
                return -1;
            }

        }

        memset(g_service_db[index], 0, sizeof(uData_service_t));
        aos_free(g_service_db[index]);
        g_service_db[index] = NULL;
        
        if (g_service_cnt > 0) {
            g_service_cnt--;
        }
        return 0;
    }

    return -1;
}

int uData_data_publish(int index)
{
    if (g_service_db[index]->subscribe == true) {
        sensor_msg_pkg_t data_msg;
        memset(&data_msg, 0, sizeof(data_msg));
        data_msg.cmd   = UDATA_MSG_REPORT_PUBLISH;
        data_msg.value = g_service_db[index]->type;
        data_msg.index = index;
        int ret            = uData_post_msg(data_msg);
        if (unlikely(ret)) {
            return -1;
        }
    }
    return 0;
}

static int uData_service_process(uint32_t abs_index, void *pdata, uint32_t len)
{
    int    ret      = 0;
    int    index    = 0;
    size_t size = 0;
    uint64_t time_stamp;
    if (pdata == NULL) {
        return -1;
    }

    time_stamp = *(uint64_t*)pdata;
    /* find the matched service, then run the registered callback here */
    for (index = 0; index < UDATA_MAX_CNT; index++) {
        if (g_service_db[index] == NULL){
            continue;
        }
        //LOG("%s  == %d,  %d %d  %d\n",__func__,__LINE__,g_service_db[index]->abs_bitmap,abs_index,g_service_db[index]->service_process_cb);

        
        //LOG("%s  == %d,  %d %d %d \n",__func__,__LINE__,(uint32_t)time_stamp, (uint32_t)g_service_db[index]->time[abs_index],(uint32_t)(g_service_db[index]->interval[abs_index]));
        if (ABS_DATA_EXSIST(g_service_db[index]->abs_bitmap ,abs_index) &&
            (g_service_db[index]->service_process_cb != NULL) &&
            (time_stamp - g_service_db[index]->time[abs_index] >= (uint64_t)(g_service_db[index]->interval[abs_index]))) {
            size = g_service_db[index]->service_process_cb(abs_index, pdata, len);
            if (size != 0) {
                ret = uData_install_report_pkg(index, pdata, size);
                if (unlikely(ret)) {
                    return -1;
                }
                uData_data_publish(index);
            }
            g_service_db[index]->time[abs_index] = time_stamp;
        }
    }
    return 0;
}

int uData_service_ioctl(udata_type_e type, void *parm)
{
    int i;
    int index;
    int ret;
    dev_sensor_full_info_t *sensor_config;
    if (parm == NULL) {
        return -1;
    }
    sensor_config = parm;

    index = uData_find_service(type);
    if ((index < 0) || (index >= UDATA_MAX_CNT)){
        return -1;
    }
    
    /* find the matched service, then run the registered ioctl callback here */
    if ((g_service_db[index]->service_ioctl_cb != NULL)) {
        for(i = 0; i < ABS_DATA_BITMAP_SUM; i++){
            if(!ABS_DATA_EXSIST(g_service_db[index]->abs_bitmap, i)){
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


void uData_dispatcher_handle(sensor_msg_pkg_t *msg)
{
    int ret = 0;
    void *addr = NULL;
    uint32_t len = 0;

    /* all the cmd of sensorhub will be sent to be handled here;
       the dispatcher will asign the new sub task to the fitted model */
    if (msg == NULL) {
        return;
    }

    ret = uData_service_get_payload(msg->value, &addr, &len);
    if (unlikely(ret)) {
        return;
    }

    switch (msg->cmd) {
        case UDATA_MSG_DEV_READ: {
            ret = abs_data_read(msg->value, addr, len);
            if (ret <= 0) {
                return;
            }
        } break;

        case UDATA_MSG_SERVICE_PROCESS: {
            
            ret = uData_service_process(msg->value, addr, len);
            if(unlikely(ret)){
                return;
            }
        } break;

        case UDATA_MSG_DEV_IOCTL: {

        } break;

        case UDATA_MSG_DEV_OPEN: {
        } break;

        case UDATA_MSG_DEV_ENABLE: {
            ret = uData_dev_enable(msg->value);
            if(unlikely(ret)){
                return;
            }
        } break;

        case UDATA_MSG_DEV_DISABLE: {
        } break;

        case UDATA_MSG_SERVICE_SUBSRIBE: {
            ret = uData_service_subscribe(msg->value);
            if(unlikely(ret)){
                return;
            }
        } break;

        case UDATA_MSG_SERVICE_UNSUBSRIBE: {
            ret = uData_service_unsubscribe(msg->value);
            if(unlikely(ret)){
                return;
            }
        } break;

        case UDATA_MSG_DEV_CLOSE: {
            ret = abs_data_close(msg->value);
            if(unlikely(ret)){
                return;
            }
        } break;

        default:
            break;
    }
}


int uData_service_init(void)
{
    int ret;

    ret = service_process_init();
    if (0 != ret) {
        return ret;
    }
    return 0;
}

int uData_service_mgr_init(void)
{
    int ret       = 0;
    g_service_cnt = 0;


    ret = uData_register_msg_handler(uData_dispatcher_handle);
    if (ret == -1) {
        LOG("error occur reg uData_dispatcher_process \n");
        return ret;
    }

    LOG("%s %s successfully \n", uDATA_STR, __func__);
    return 0;
}
