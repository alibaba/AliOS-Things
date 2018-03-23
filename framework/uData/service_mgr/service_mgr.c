/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
* uData service manager part
*/

#include "service_mgr.h"
#include <stdio.h> 
#include <aos/aos.h>
#include <aos/yloop.h>
#include "service_algo.h"

static uData_service_t* g_service_db[UDATA_MAX_CNT];
static uint32_t g_service_cnt = 0;
static udata_pkg_t g_pkg_buf[UDATA_MAX_CNT];

static uData_service_t* uData_get_service_obj(sensor_tag_e tag)
{
    int index = 0;
    for(index = 0; index < g_service_cnt; index++){
        if(g_service_db[index]->tag ==  tag){
            return g_service_db[index];
        }
    }
    return NULL;
}

static int uData_find_service(udata_type_e type)
{
    int index = 0;
    for(index = 0; index < g_service_cnt; index++){
        if(g_service_db[index]->type ==  type){
            return index;
        }
    }
    
    return -1;
}

static int uData_dev_enable(sensor_tag_e tag)
{
    int ret  = 0;
    ret = abs_data_dev_enable(tag);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}
int uData_get_report_pkg(void* buf,int index)
{
    if(buf == NULL){
        return -1;
    }
    if(index >= UDATA_MAX_CNT){
        return -1;
    }
    memcpy(buf, &g_pkg_buf[index], sizeof(udata_pkg_t));
    return 0;
}
static int uData_install_report_pkg(int index, void* pdata, size_t len)
{
    if(pdata == NULL){
        return -1;
    }
    if(index >= UDATA_MAX_CNT){
        return -1;
    }
    /* intall the report data package here */
    memset(&g_pkg_buf[index], 0, sizeof(udata_pkg_t));
    g_pkg_buf[index].valid = true;
    g_pkg_buf[index].type = g_service_db[index]->type;
    if(sizeof(g_pkg_buf[index].payload) < len){
        return -1;
    }
    memcpy(g_pkg_buf[index].payload, pdata, len);
    
    aos_post_event(EV_UDATA, CODE_UDATA_REPORT_PUBLISH, index);

    return 0;
}

static int uData_get_dev_list(void* pdata)
{
    dev_sensor_full_info_t* sensor;
    sensor = aos_malloc(sizeof(dev_sensor_full_info_t));
    if(sensor == NULL){
        return -1;
    }
    sensor->config.id = SENSOR_IOCTL_GET_SENSOR_LIST;
    int ret = abs_data_ioctl(NULL, sensor);
    if(unlikely(ret)){
        aos_free(sensor);    
        return -1;
    }
    aos_free(sensor);    
    return 0;
}


int uData_service_subscribe(udata_type_e type)
{
    int ret  = 0;
    int index = 0;

    index = uData_find_service(type);
    if(index < 0){
        return -1;
    }
    
    if(g_service_db[index]->type ==  type){
        if(g_service_db[index]->running != true){
            ret = abs_data_open(g_service_db[index]);
            if(unlikely(ret)){
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
    int ret  = 0;
    int index = 0;
    
    index = uData_find_service(type);
    if(index < 0)
        return -1;

    if(g_service_db[index]->type ==  type){
        if(g_service_db[index]->running != false){
        ret = abs_data_close(g_service_db[index]->tag);
            if(unlikely(ret)){
                return -1;
            }
        }
        g_service_db[index]->subscribe = false;
    }
    return 0;
}


int uData_service_register(uData_service_t* service)
{

    int ret = 0;
    if(service == NULL){
        return -1;
    }
    g_service_db[g_service_cnt] = (uData_service_t*)aos_malloc(sizeof(uData_service_t));
    if(g_service_db[g_service_cnt] == NULL){
        return -1;
    }
    
    memset(g_service_db[g_service_cnt], 0, sizeof(uData_service_t));
    
    /* also can copy the data struct by memcpy here */
    g_service_db[g_service_cnt]->type =  service->type;
    g_service_db[g_service_cnt]->tag =  service->tag;
    g_service_db[g_service_cnt]->config.odr =  service->config.odr;
    g_service_db[g_service_cnt]->config.range =  service->config.range;
    g_service_db[g_service_cnt]->service_process_cb =  service->service_process_cb;
    g_service_db[g_service_cnt]->service_ioctl_cb =  service->service_ioctl_cb;
    
    ret = abs_data_open(g_service_db[g_service_cnt]);
    if(unlikely(ret)){
        goto error;
    }

    /* set the defaul status and no subsrcribed from aliyun side */
    g_service_db[g_service_cnt]->running   =  true;
    g_service_db[g_service_cnt]->subscribe =  false;
    g_service_cnt++;
    
    return 0;

error:
    aos_free(g_service_db[g_service_cnt]);
    return -1;
}

int uData_service_unregister(udata_type_e type)
{

    int index = 0;
    int ret = 0;
    /* find the matched service, then del it from the list here */
    for(index = 0; index < g_service_cnt; index++){
        if(g_service_db[index]->type == type){
            ret = abs_data_close(g_service_db[index]->tag);
            if(unlikely(ret)){
                return -1;
            }
            memset(g_service_db[index], 0, sizeof(uData_service_t));
            if(g_service_cnt > 0){
                g_service_cnt--;
            }
            return 0;
        }
    }

    return -1;
}

static int uData_service_process(sensor_tag_e tag, void* pdata)
{

    int ret = 0;
    int index = 0;
    size_t len = 0;
    if(pdata == NULL){
        return -1;
    }
    
    /* find the matched service, then run the registered callback here */
    for(index = 0; index < g_service_cnt; index++){    
        if((g_service_db[index]->tag == tag) && (g_service_db[index]->service_process_cb != NULL)){
            len = g_service_db[index]->service_process_cb(tag, pdata);
            if((len != 0) && (g_service_db[index]->subscribe == true)){
                ret = uData_install_report_pkg(index, pdata, len);
                if(unlikely(ret)){
                    return -1;
                }
            }
        }
    }
    return 0;
}

int uData_service_ioctl(udata_type_e type, void* parm)
{
    int index = 0;
    int ret = 0;
    sensor_tag_e tag;
    dev_sensor_full_info_t * sensor_config = parm;
    if(parm == NULL)
        return -1;
    
    /* find the matched service, then run the registered ioctl callback here */
    for(index = 0; index < g_service_cnt; index++){    
        if((g_service_db[index]->type == type) && (g_service_db[index]->service_ioctl_cb != NULL)){
            g_service_db[index]->service_ioctl_cb(type, tag);
            /* if get the dev info, then run here */
            ret = abs_data_ioctl(tag, sensor_config);
            if(unlikely(ret)){
                return -1;
            }
        }
    }

    LOG("%s %s successfully \n", uDATA_STR, __func__);
    return 0;
}


static void uData_service_dispatcher(input_event_t *event, void *priv_data)
{
    int ret = 0;
    
    /* all the cmd of sensorhub will be sent to be handled here;
       the dispatcher will asign the new sub task to the fitted model */
    if ((event == NULL)||(event->type != EV_UDATA)) {
        return;
    }
    
    uData_service_t* service = uData_get_service_obj(event->value);
    if(service == NULL){
        return -1;
    }
    
    switch(event->code){
        case CODE_UDATA_DEV_READ:{
            memset(service->payload, 0, DATA_SIZE);
            ret = abs_data_read(event->value, service->payload, DATA_SIZE);
            if(ret <= 0){
                LOG("%s %s fail line:%d \n", uDATA_STR, __func__,__LINE__);
                return;
            }
        }break;
        
        case CODE_UDATA_SERVICE_PROCESS:{
            uData_service_process(event->value, service->payload);
        }break;        
        
        case CODE_UDATA_DEV_IOCTL:{
            abs_data_ioctl(event->value, service->payload);
        }break;
        
        case CODE_UDATA_DEV_OPEN:{
            abs_data_open(priv_data);
        }break;  
        
        case CODE_UDATA_DEV_ENABLE:{
            uData_dev_enable(event->value);
        }break;    
        
        case CODE_UDATA_DEV_DISABLE:{
        }break;  
        
        case CODE_UDATA_SERVICE_SUBSRIBE:{
            uData_service_subscribe(event->value);
        }break;  
        
        case CODE_UDATA_SERVICE_UNSUBSRIBE:{
            uData_service_unsubscribe(event->value);
        }break;          
        
        case CODE_UDATA_DEV_CLOSE:{
             abs_data_close(event->value);
        }break;

        default:break;
    }
}

int uData_service_init(void)
{
#ifdef AOS_UDATA_SERVICE_BARO  
    udata_baro_service_init();
#endif /* UDATA_SERVICE_BARO */

#ifdef AOS_UDATA_SERVICE_GPS
    udata_gps_service_init();
#endif

        
    return 0;
}

int uData_service_mgr_init(void)
{
    int ret = 0;
    g_service_cnt = 0;
    
    ret = aos_register_event_filter(EV_UDATA, uData_service_dispatcher, NULL);
    if(unlikely(ret)){
        return -1;
    }

    LOG("%s %s successfully \n", uDATA_STR, __func__);
    return 0;
}

