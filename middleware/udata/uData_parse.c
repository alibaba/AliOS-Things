/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 * udata para parse
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sensor.h>
#include "abs_data_model.h"
#include "service_mgr.h"
#include "uData_queue.h"
#include "uData_parse.h"
#include "service_data_to_cloud.h"

uint32_t  g_uDataServiceNum = 0;

//#define UDATA_CJSON_SUPPORTED

//#ifdef UDATA_CJSON_SUPPORTED
#include "cJSON.h"
#include "config/udata_config.data"

#define  UDATA_SERVICE_TASK_EXCLUSIVE        "exclusive"
#define  UDATA_SERVICE_TASK_SHARED           "shared"

#define  UDATA_DTC_FLAG_ON                  "on"
#define  UDATA_DTC_FLAG_OFF                 "off"

typedef struct {
    udata_type_e type;
    char*        type_name;
}udata_service_name_st;


typedef struct {
    sensor_tag_e tag;
    char*        tag_name;
}udata_tag_name_st;

typedef struct {
    udata_data_type    type;
    char*             type_name;
}udata_data_type_name_st;


bool      g_uDataTaskExcl = false;
#define   DATA_ASSEMBLE(a)  {a,#a}

udata_tag_name_st  g_udata_tag_name[TAG_DEV_SENSOR_NUM_MAX] = {
DATA_ASSEMBLE(TAG_DEV_ACC),
DATA_ASSEMBLE(TAG_DEV_MAG),
DATA_ASSEMBLE(TAG_DEV_GYRO),
DATA_ASSEMBLE(TAG_DEV_ALS),
DATA_ASSEMBLE(TAG_DEV_PS),
DATA_ASSEMBLE(TAG_DEV_BARO),
DATA_ASSEMBLE(TAG_DEV_TEMP),
DATA_ASSEMBLE(TAG_DEV_UV),
DATA_ASSEMBLE(TAG_DEV_HUMI),
DATA_ASSEMBLE(TAG_DEV_NOISE),
DATA_ASSEMBLE(TAG_DEV_PM25),
DATA_ASSEMBLE(TAG_DEV_PM1P0),
DATA_ASSEMBLE(TAG_DEV_PM10),
DATA_ASSEMBLE(TAG_DEV_CO2),
DATA_ASSEMBLE(TAG_DEV_HCHO),
DATA_ASSEMBLE(TAG_DEV_TVOC),
DATA_ASSEMBLE(TAG_DEV_PH),
DATA_ASSEMBLE(TAG_DEV_VWC),
DATA_ASSEMBLE(TAG_DEV_EC),
DATA_ASSEMBLE(TAG_DEV_SALINITY),
DATA_ASSEMBLE(TAG_DEV_TDS),
DATA_ASSEMBLE(TAG_DEV_WINDSPD),
DATA_ASSEMBLE(TAG_DEV_WINDDIR),
DATA_ASSEMBLE(TAG_DEV_RAIN),
DATA_ASSEMBLE(TAG_DEV_HALL),
DATA_ASSEMBLE(TAG_DEV_HR),
DATA_ASSEMBLE(TAG_DEV_RGB),
DATA_ASSEMBLE(TAG_DEV_GS),
DATA_ASSEMBLE(TAG_DEV_IR),
DATA_ASSEMBLE(TAG_DEV_GPS),
DATA_ASSEMBLE(TAG_DEV_RTC)
};

udata_service_name_st  g_udata_service_name[UDATA_MAX_CNT] = {
DATA_ASSEMBLE(UDATA_SERVICE_ACC),
DATA_ASSEMBLE(UDATA_SERVICE_MAG),
DATA_ASSEMBLE(UDATA_SERVICE_GYRO),
DATA_ASSEMBLE(UDATA_SERVICE_ALS),
DATA_ASSEMBLE(UDATA_SERVICE_PS),
DATA_ASSEMBLE(UDATA_SERVICE_BARO),
DATA_ASSEMBLE(UDATA_SERVICE_TEMP),
DATA_ASSEMBLE(UDATA_SERVICE_UV),
DATA_ASSEMBLE(UDATA_SERVICE_HUMI),
DATA_ASSEMBLE(UDATA_SERVICE_NOISE),
DATA_ASSEMBLE(UDATA_SERVICE_PM25),
DATA_ASSEMBLE(UDATA_SERVICE_PM1P0),
DATA_ASSEMBLE(UDATA_SERVICE_PM10),
DATA_ASSEMBLE(UDATA_SERVICE_CO2),
DATA_ASSEMBLE(UDATA_SERVICE_HCHO),
DATA_ASSEMBLE(UDATA_SERVICE_TVOC),
DATA_ASSEMBLE(UDATA_SERVICE_PH),
DATA_ASSEMBLE(UDATA_SERVICE_VWC),
DATA_ASSEMBLE(UDATA_SERVICE_EC),
DATA_ASSEMBLE(UDATA_SERVICE_SALINITY),
DATA_ASSEMBLE(UDATA_SERVICE_TDS),
DATA_ASSEMBLE(UDATA_SERVICE_WINDSPD),
DATA_ASSEMBLE(UDATA_SERVICE_WINDDIR),
DATA_ASSEMBLE(UDATA_SERVICE_RAIN),
DATA_ASSEMBLE(UDATA_SERVICE_HALL),
DATA_ASSEMBLE(UDATA_SERVICE_HR),
DATA_ASSEMBLE(UDATA_SERVICE_RGB),
DATA_ASSEMBLE(UDATA_SERVICE_GS),
DATA_ASSEMBLE(UDATA_SERVICE_IR),
DATA_ASSEMBLE(UDATA_SERVICE_PEDOMETER),
DATA_ASSEMBLE(UDATA_SERVICE_PDR),
DATA_ASSEMBLE(UDATA_SERVICE_VDR),
DATA_ASSEMBLE(UDATA_SERVICE_GPS),
DATA_ASSEMBLE(UDATA_SERVICE_RTC)
};

udata_data_type_name_st  g_udata_data_type[UDATA_TYPE_MAX] = {
DATA_ASSEMBLE(UDATA_INT32),
DATA_ASSEMBLE(UDATA_UINT32),
DATA_ASSEMBLE(UDATA_FLOAT),
};

cJSON* g_uDataItem = NULL;
//const char* service_config_str = "{\"uData_cfg_desc\":[{\"name\":\"pedometer\",\"type\":\"UDATA_SERVICE_PEDOMETER\",\"task\":\"shared\",\"devices\":[{\"tag\":\"TAG_DEV_ACC\",\"index\":0,\"interval\":2000},{\"tag\":\"TAG_DEV_GYRO\",\"index\":0,\"interval\":2000}],\"outData\":[{\"name\":\"x\"},{\"name\":\"y\"},{\"name\":\"z\"}],\"dataType\":\"UDATA_FLOAT\",\"coeff\":1,\"dtcFlag\":\"on\",\"dtcInterval\":2000},{\"name\":\"temp\",\"type\":\"UDATA_SERVICE_TEMP\",\"task\":\"shared\",\"devices\":[{\"tag\":\"TAG_DEV_TEMP\",\"index\":0,\"interval\":2000}],\"outData\":[{\"name\":\"null\"}],\"dataType\":\"UDATA_FLOAT\",\"coeff\":10,\"dtcFlag\":\"on\",\"dtcInterval\":2000}]}";

int uData_config_num_get(uint32_t* pNum)
{
    int ret;
    cJSON *root = NULL;
    
    root = cJSON_Parse(service_config_str);
    if (root == NULL || !cJSON_IsObject(root)) {
        return -1;
    }
    g_uDataItem = cJSON_GetObjectItem(root,"uData_cfg_desc");
    if (g_uDataItem == NULL) {
        return -1;
    }

    ret = cJSON_GetArraySize(g_uDataItem);
    if(ret <= 0){
        return -1;
    }
    *pNum = (uint32_t)ret;
    g_uDataServiceNum = ret;
    return 0;
}


int uData_service_config_parse(uint32_t index, uData_service_t* svc)
{
    int ret;
    uint32_t i,j;
    int num = 0;
    cJSON* temp = NULL;
    cJSON *array = NULL;
    cJSON *item = NULL;
    cJSON *root = NULL;
    uint32_t idx = 0;

    if(svc == NULL){
        return -1;
    }

    if(index >= g_uDataServiceNum){
        return -1;
    }

    root = cJSON_GetArrayItem(g_uDataItem,index);
    if (root == NULL) {
        return -1;
    }

    temp=cJSON_GetObjectItem(root,"type");
    if (temp == NULL) {
        return -1;
    }

    for(i = 0; i < UDATA_MAX_CNT; i++){
        if((strlen(temp->valuestring) == strlen(g_udata_service_name[i].type_name)) && 
            (0 == strncmp(temp->valuestring,g_udata_service_name[i].type_name,strlen(temp->valuestring))))
        {
            svc->type = g_udata_service_name[i].type;
            
            break;
        }
    }

    if(i == UDATA_MAX_CNT){
        return -1;
    }


    temp=cJSON_GetObjectItem(root,"task");
    if (temp == NULL) {
        return -1;
    }
    if(0 == strncmp(temp->valuestring,UDATA_SERVICE_TASK_EXCLUSIVE,strlen(temp->valuestring)))
    {
        svc->task_flag = true;
    }
    else if(0 == strncmp(temp->valuestring,UDATA_SERVICE_TASK_SHARED,strlen(temp->valuestring)))
    {
        svc->task_flag = false;
    }
    else{
        return -1;
    }

    item=cJSON_GetObjectItem(root,"devices");
    if (item == NULL) {
        return -1;
    }
    num = cJSON_GetArraySize(item);

    for(i = 0; i < num; i++){
        array = cJSON_GetArrayItem(item,i);
        temp=cJSON_GetObjectItem(array,"tag");
        if (temp == NULL) {
            return -1;
        }

        ret = -1;
        sensor_tag_e tag;
        for(j = 0; j < TAG_DEV_SENSOR_NUM_MAX; j++){
            if((strlen(temp->valuestring) == strlen(g_udata_tag_name[j].tag_name)) && 
                (0 == strncmp(temp->valuestring,g_udata_tag_name[j].tag_name,strlen(temp->valuestring))))
            {
                tag = g_udata_tag_name[j].tag;
                break;
            }
        }

        if(j == TAG_DEV_SENSOR_NUM_MAX){
            return -1;
        }

        temp=cJSON_GetObjectItem(array,"index");
        if (temp == NULL) {
            return -1;
        }
        ret = abs_data_get_abs_index(tag, temp->valueint, &idx);
        if (unlikely(ret)){
            //LOG("service %d tag %d index %d\n",svc->type, tag,temp->valueint);
            return -1;
        }
        UDATA_SERVICE_BITMAP_SET(svc->abs_bitmap,idx);

        temp=cJSON_GetObjectItem(array,"interval");
        if (temp == NULL) {
            return -1;
        }

        if(temp->valueint < 0){
            return -1;
        }
        svc->interval[idx] = temp->valueint;

    }
    return 0;
}


int uData_cjson_dtc_flag_get(uint32_t index, bool* pFlag)
{
    cJSON* temp = NULL;
    cJSON* root = NULL;

    if(index >= g_uDataServiceNum){
        return -1;
    }

    root = cJSON_GetArrayItem(g_uDataItem,index);
    if (root == NULL) {
        return -1;
    }


    temp=cJSON_GetObjectItem(root,"dtcFlag");
    if (temp == NULL) {
        return -1;
    }
    if(0 == strncmp(temp->valuestring,UDATA_DTC_FLAG_ON,strlen(temp->valuestring)))
    {
        *pFlag = true;
    }
    else if(0 == strncmp(temp->valuestring,UDATA_DTC_FLAG_OFF,strlen(temp->valuestring)))
    {
        *pFlag = false;
    }
    else{
        return -1;
    }

    return 0;
}


int uData_dtc_config_parse(uint32_t index, service_pub_info_t* dtc)
{
    bool dtcFlag = false;
    uint32_t i,j;
    int num = 0;
    char* str;
    size_t len = 0;
    cJSON* temp = NULL;
    cJSON *array = NULL;
    cJSON *item = NULL;
    cJSON *root = NULL;

    if(index >= g_uDataServiceNum){
        return -1;
    }

    root = cJSON_GetArrayItem(g_uDataItem,index);
    if (root == NULL) {
        return -1;
    }


    temp=cJSON_GetObjectItem(root,"dtcFlag");
    if (temp == NULL) {
        return -1;
    }
    if(0 == strncmp(temp->valuestring,UDATA_DTC_FLAG_ON,strlen(temp->valuestring)))
    {
        dtcFlag = true;
    }
    else if(0 == strncmp(temp->valuestring,UDATA_DTC_FLAG_OFF,strlen(temp->valuestring)))
    {
        dtcFlag = false;
    }
    else{
        return -1;
    }
    dtc->dtcFlag = dtcFlag;

    item=cJSON_GetObjectItem(root,"outData");
    if (item == NULL) {
        return -1;
    }
    num = cJSON_GetArraySize(item);
    if(num <= 0){
        return -1;
    }
    
    num = num==1? 1:(num+1);
    dtc->name_num = num;
    dtc->name_addr = aos_malloc(num*SERVICE_PUB_NAME_LEN);
    if(NULL == dtc->name_addr){
        return -1;
    }
    
    temp=cJSON_GetObjectItem(root,"name");
    if (temp == NULL) {
        goto error;
    }
    
    len = strlen(temp->valuestring);
    if(len >= SERVICE_PUB_NAME_LEN){
        goto error;
    }

    memcpy(dtc->name_addr,temp->valuestring,len);
    ((char*)dtc->name_addr)[len] = '\0';
    
    //num = (num==1)?1:(num-1);
    num = num-1;

    for(i = 0; i < num; i++){
        array = cJSON_GetArrayItem(item,i);
        temp=cJSON_GetObjectItem(array,"name");
        if (temp == NULL) {
            goto error;
        }
        
        len = strlen(temp->valuestring);
        if(len >= SERVICE_PUB_NAME_LEN){
            goto error;
        }
        str = (char*)(dtc->name_addr + (i+1)*SERVICE_PUB_NAME_LEN);
        
        memcpy(str,temp->valuestring,len);
        str[len] = '\0';

    }

    temp=cJSON_GetObjectItem(root,"dataType");
    if (temp == NULL) {
        goto error;
    }
    

    for(j = 0; j < UDATA_TYPE_MAX; j++){ 
        if((strlen(temp->valuestring) == strlen(g_udata_data_type[j].type_name)) && 
            (0 == strncmp(temp->valuestring,g_udata_data_type[j].type_name,strlen(temp->valuestring))))
        {
            dtc->data_type = g_udata_data_type[j].type;
            break;
        }
    }
    
    temp=cJSON_GetObjectItem(root,"coeff");
    if (temp == NULL) {
        goto error;
    }

    if(temp->valueint <= 0){
        goto error;
    }
    
    dtc->coeff = (uint32_t)temp->valueint;

    temp=cJSON_GetObjectItem(root,"dtcInterval");
    if (temp == NULL) {
        goto error;
    }

    if(temp->valueint < 0){
        goto error;
    }
    dtc->dtc_cycle = temp->valueint;
    
    temp=cJSON_GetObjectItem(root,"type");
    if (temp == NULL) {
        goto error;
    }

    for(i = 0; i < UDATA_MAX_CNT; i++){
        if((strlen(temp->valuestring) == strlen(g_udata_service_name[i].type_name)) && 
            (0 == strncmp(temp->valuestring,g_udata_service_name[i].type_name,strlen(temp->valuestring))))
        {
            dtc->type = g_udata_service_name[i].type;
            break;
        }
    }

    if(i == UDATA_MAX_CNT){
        goto error;
    }

    return 0;

error:
    if(NULL!=dtc->name_addr){
        aos_free(dtc->name_addr);
        dtc->name_addr = NULL;
    }

    return -1;

}





