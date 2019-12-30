/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 * uData para parse
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include "abs_data_model.h"
#include "service_mgr.h"
#include "udata_queue.h"
#include "udata_parse.h"
#include "service_data_to_cloud.h"

uint32_t  g_uDataServiceNum = 0;
uint32_t  g_uDataDtcNum = 0;

#ifdef UDATA_CJSON_SUPPORTED
#include "cJSON.h"
#include "config/udata_config.data"

#define  UDATA_SERVICE_TASK_EXCLUSIVE        "exclusive"
#define  UDATA_SERVICE_TASK_SHARED           "shared"

#define  UDATA_DTC_FLAG_ON                  "on"
#define  UDATA_DTC_FLAG_OFF                 "off"

#define  UDATA_CONFIG_DESC_NAME             "udata_config_desc"
#define  UDATA_SERVICE_CONFIG_NAME          "service_config"
#define  UDATA_DTC_CONFIG_NAME              "data_to_cloud"


#define  UDATA_SERVICE_TYPE_NAME            "type"
#define  UDATA_SERVICE_TASKFLAG_NAME        "task"
#define  UDATA_SERVICE_DEVICES_NAME         "devices"
#define  UDATA_SERVICE_TAG_NAME             "tag"
#define  UDATA_SERVICE_INDEX_NAME           "instance"
#define  UDATA_SERVICE_INTERVAL_NAME        "interval"


#define  UDATA_DTC_FLAG_NAME                "dtc_enable"
#define  UDATA_DTC_OUTDATA_NAME             "sub_para"
#define  UDATA_DTC_PROPERTY_NAME            "name"
#define  UDATA_DTC_DATATYPE_NAME            "data_type"
#define  UDATA_DTC_COEFF_NAME               "coefficient"
#define  UDATA_DTC_INTERVAL_NAME            "dtc_interval"

#define  UDATA_SERVICE_CONFIG_DEFAULT_NUM   (1)
#define  UDATA_SERVICE_CONFIG_DEFAULT_IDX   (0)

#define  UDATA_DTC_CONFIG_DEFAULT_NUM       (1)
#define  UDATA_DTC_CONFIG_DEFAULT_IDX       (0)


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
void udata_cjson_init(void)
{
    cJSON_Hooks cjson_hooks;
    cjson_hooks.malloc_fn = aos_malloc;
    cjson_hooks.free_fn = aos_free;
    cJSON_InitHooks(&cjson_hooks);
}
int g_config_num_flag = 0;
int udata_service_num_get(uint32_t* pNum)
{
    int ret;
    cJSON *root = NULL;
    if(pNum == NULL){
        return -1;
    }
    
    if(g_config_num_flag == 1){
        *pNum = (uint32_t)g_uDataServiceNum;
        return 0;
    }
    
    root = cJSON_Parse(service_config_str);
    if (root == NULL || !cJSON_IsObject(root)) {
        return -1;
    }
    g_uDataItem = cJSON_GetObjectItem(root,UDATA_CONFIG_DESC_NAME);
    if (g_uDataItem == NULL) {
        return -1;
    }

    ret = cJSON_GetArraySize(g_uDataItem);
    if(ret <= 0){
        return -1;
    }
    *pNum = (uint32_t)ret;
    g_uDataServiceNum = (uint32_t)ret;
    
    g_config_num_flag = 1;
    return 0;
}

int udata_service_config_parse(uint32_t index, udata_service_t* svc)
{
    int ret;
    uint32_t i,j;
    int num = 0;
    cJSON* temp = NULL;
    cJSON *array = NULL;
    cJSON *item = NULL;
    cJSON *root = NULL;
    uint32_t idx = 0;
    sensor_tag_e tag;

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

    /*get service_config*/
    item=cJSON_GetObjectItem(root,UDATA_SERVICE_CONFIG_NAME);
    if (item == NULL) {
        return -1;
    }
    num = cJSON_GetArraySize(item);
    if (num != UDATA_SERVICE_CONFIG_DEFAULT_NUM) {
        return -1;
    }
    array = cJSON_GetArrayItem(item,UDATA_DTC_CONFIG_DEFAULT_IDX);
    temp=cJSON_GetObjectItem(array,UDATA_SERVICE_TYPE_NAME);
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


    temp=cJSON_GetObjectItem(array,UDATA_SERVICE_TASKFLAG_NAME);
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

    item=cJSON_GetObjectItem(array,UDATA_SERVICE_DEVICES_NAME);
    if (item == NULL) {
        return -1;
    }
    num = cJSON_GetArraySize(item);
    if (num <= 0) {
        return -1;
    }

    for(i = 0; i < num; i++){
        array = cJSON_GetArrayItem(item,i);
        temp=cJSON_GetObjectItem(array,UDATA_SERVICE_TAG_NAME);
        if (temp == NULL) {
            return -1;
        }

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

        temp=cJSON_GetObjectItem(array,UDATA_SERVICE_INDEX_NAME);
        if (temp == NULL) {
            return -1;
        }
        ret = abs_data_get_abs_index(tag, temp->valueint, &idx);
        if (unlikely(ret)){
            return -1;
        }
        UDATA_BITMAP_SET(svc->abs_bitmap,idx);

        temp=cJSON_GetObjectItem(array,UDATA_SERVICE_INTERVAL_NAME);
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


int udata_dtc_num_get(uint32_t* pNum)
{
    int ret;
    cJSON *root = NULL;
    if(pNum == NULL){
        return -1;
    }
    
    if(g_config_num_flag == 1){
        *pNum = (uint32_t)g_uDataServiceNum;
        return 0;
    }
    
    root = cJSON_Parse(service_config_str);
    if (root == NULL || !cJSON_IsObject(root)) {
        return -1;
    }
    g_uDataItem = cJSON_GetObjectItem(root,UDATA_CONFIG_DESC_NAME);
    if (g_uDataItem == NULL) {
        return -1;
    }

    ret = cJSON_GetArraySize(g_uDataItem);
    if(ret <= 0){
        return -1;
    }
    *pNum = (uint32_t)ret;
    g_uDataServiceNum = (uint32_t)ret;
    g_config_num_flag = 1;
    return 0;
}

int udata_dtc_config_parse(uint32_t index, service_pub_info_t* dtc)
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

    if(dtc == NULL){
        return -1;
    }

    if(index >= g_uDataServiceNum){
        return -1;
    }
    root = cJSON_GetArrayItem(g_uDataItem,index);
    if (root == NULL) {
        return -1;
    }

    /*get service config*/

    item=cJSON_GetObjectItem(root,UDATA_SERVICE_CONFIG_NAME);
    if (item == NULL) {
        return -1;
    }
    num = cJSON_GetArraySize(item);
    if (num != UDATA_SERVICE_CONFIG_DEFAULT_NUM) {
        return -1;
    }
    array = cJSON_GetArrayItem(item,UDATA_DTC_CONFIG_DEFAULT_IDX);

    /* get service type */
    temp=cJSON_GetObjectItem(array,UDATA_SERVICE_TYPE_NAME);
    if (temp == NULL) {
        return -1;
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
        return -1;
    }

    /*get data_to_cloud config*/
    item=cJSON_GetObjectItem(root,UDATA_DTC_CONFIG_NAME);
    if (item == NULL) {
        return -1;
    }
    num = cJSON_GetArraySize(item);
    if (num != UDATA_DTC_CONFIG_DEFAULT_NUM) {
        return -1;
    }
    array = cJSON_GetArrayItem(item,UDATA_DTC_CONFIG_DEFAULT_IDX);

    /* get dtc flag */
    temp=cJSON_GetObjectItem(array,UDATA_DTC_FLAG_NAME);
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

    /* get dtc data type */
    temp=cJSON_GetObjectItem(array,UDATA_DTC_DATATYPE_NAME);
    if (temp == NULL) {
        return -1;
    }
    
    dtc->data_type = UDATA_TYPE_MAX;
    for(j = 0; j < UDATA_TYPE_MAX; j++){ 
        if((strlen(temp->valuestring) == strlen(g_udata_data_type[j].type_name)) && 
            (0 == strncmp(temp->valuestring,g_udata_data_type[j].type_name,strlen(temp->valuestring))))
        {
            dtc->data_type = g_udata_data_type[j].type;
            break;
        }
    }

    if (dtc->data_type == UDATA_TYPE_MAX){
        return -1;
    }
    
    /* get dtc data coefficient */
    temp=cJSON_GetObjectItem(array,UDATA_DTC_COEFF_NAME);
    if (temp == NULL) {
        return -1;
    }

    if(temp->valueint <= 0){
        return -1;
    }
    dtc->coeff = (uint32_t)temp->valueint;

    temp=cJSON_GetObjectItem(array,UDATA_DTC_INTERVAL_NAME);
    if (temp == NULL) {
        return -1;
    }

    if(temp->valueint < 0){
        return -1;
    }
    dtc->dtc_cycle = temp->valueint;

    /* get dtc output data name */
    item=cJSON_GetObjectItem(array,UDATA_DTC_OUTDATA_NAME);
    if (item == NULL) {
        num = 1;
        //return -1;
    }
    else{
        num = cJSON_GetArraySize(item);
        if(num <= 0){
            return -1;
        }
    }
    
    num = num==1? 1:(num+1);
    dtc->name_num = num;
    dtc->name_addr = aos_malloc(num*SERVICE_PUB_NAME_LEN);
    if(NULL == dtc->name_addr){
        return -1;
    }
    
    temp=cJSON_GetObjectItem(array,UDATA_DTC_PROPERTY_NAME);
    if (temp == NULL) {
        goto error;
    }
    
    len = strlen(temp->valuestring);
    if(len >= SERVICE_PUB_NAME_LEN){
        goto error;
    }

    memcpy(dtc->name_addr,temp->valuestring,len);
    ((char*)dtc->name_addr)[len] = '\0';
    
    num = num-1;

    for(i = 0; i < num; i++){
        array = cJSON_GetArrayItem(item,i);
        temp=cJSON_GetObjectItem(array,UDATA_DTC_PROPERTY_NAME);
        if (temp == NULL) {
            goto error;
        }
        
        len = strlen(temp->valuestring);
        if(len >= SERVICE_PUB_NAME_LEN){
            goto error;
        }
        str = (char*)((uint32_t)dtc->name_addr + (i+1)*SERVICE_PUB_NAME_LEN);
        
        memcpy((void*)str,temp->valuestring,len);
        str[len] = '\0';
    }

    return 0;

error:
    if(NULL!=dtc->name_addr){
        aos_free(dtc->name_addr);
        dtc->name_addr = NULL;
    }

    return -1;

}

#else
#include "config/udata_config.c"
int udata_service_num_get(uint32_t* pNum)
{
    uint32_t size = 0;
    if(pNum == NULL){
        return -1;
    }
    size = sizeof(g_service_para) / sizeof(g_service_para[0]);
    *pNum = size;
    g_uDataServiceNum = size;
    return 0;
}

int udata_service_config_parse(uint32_t index, udata_service_t* svc)
{
    int ret;
    uint32_t i;
    sensor_tag_e tag;
    int                idx;
    uint32_t           abs_idx;
    
    if(svc == NULL){
        return -1;
    }

    if(index >= g_uDataServiceNum){
        return -1;
    }

    if(g_service_para[index].type >= UDATA_MAX_CNT){
        return -1;
    }

    svc->type = g_service_para[index].type;
    svc->task_flag =  g_service_para[index].task_flag;

    if(NULL == g_service_para[index].p_tag_para){
        return -1;
    }

    for(i = 0; i < g_service_para[index].dev_num; i++){
        tag = g_service_para[index].p_tag_para[i].tag;
        if(tag >= TAG_DEV_SENSOR_NUM_MAX){
            return -1;
        }
        idx = g_service_para[index].p_tag_para[i].instance;
        
        ret = abs_data_get_abs_index(tag, (uint8_t)idx, &abs_idx);
        if (unlikely(ret)){
            return -1;
        }

        UDATA_BITMAP_SET(svc->abs_bitmap,abs_idx);
        
        if(g_service_para[index].p_tag_para[i].interval <= 0){
            return -1;
        }
        
        svc->interval[abs_idx] = g_service_para[index].p_tag_para[i].interval;
    }

    return 0;
}

int udata_dtc_num_get(uint32_t* pNum)
{
    uint32_t size = 0;

    if(pNum == NULL){
        return -1;
    }
    size = sizeof(g_dtc_para) / sizeof(g_dtc_para[0]);
    *pNum = size;
    g_uDataDtcNum = size;
    return 0;
}

int udata_dtc_config_parse(uint32_t index, service_pub_info_t* dtc)
{
    int len;
    char* str;
    uint32_t i;
    
    if(dtc == NULL){
        return -1;
    }

    if(index >= g_uDataDtcNum){
        return -1;
    }

    if(g_dtc_para[index].type >= UDATA_MAX_CNT){
        return -1;
    }
    if(g_dtc_para[index].data_type >= UDATA_TYPE_MAX){
        return -1;
    }

    if(g_dtc_para[index].name_num <= 0){
        return -1;
    }

    if(g_dtc_para[index].name == NULL){
        return -1;
    }

    len = strlen(g_dtc_para[index].name);
    if((len >= SERVICE_PUB_NAME_LEN) || (len == 0)){
        return -1;
    }
    
    dtc->type = g_dtc_para[index].type;
    dtc->dtcFlag =  g_dtc_para[index].dtc_flag;
    dtc->dtc_cycle =  g_dtc_para[index].dtc_cycle;

    dtc->name_num =  (g_dtc_para[index].name_num == 1)? 1 : (g_dtc_para[index].name_num + 1);
    dtc->name_addr = aos_malloc(dtc->name_num*SERVICE_PUB_NAME_LEN);
    if(NULL == dtc->name_addr){
        return -1;
    }

    str = (char*)dtc->name_addr;
    memcpy((void*)str, g_dtc_para[index].name,len);
    str[len] = '\0';
    for(i = 1; i < dtc->name_num; i++){
        if (g_dtc_para[index].para_name == NULL){
            goto error;
        }
        len = strlen(g_dtc_para[index].para_name[i-1]);
        if ((len >= SERVICE_PUB_NAME_LEN) || (len == 0)){
            goto error;
        }
        str = (char*)((uint32_t)dtc->name_addr + (i * SERVICE_PUB_NAME_LEN));
        memcpy((void*)str, g_dtc_para[index].para_name[i-1],len);
        str[len] = '\0';
    }
    dtc->data_type = g_dtc_para[index].data_type;
    dtc->coeff     = g_dtc_para[index].coeff;

    return 0;
error:
    if(dtc->name_addr != NULL){
        aos_free(dtc->name_addr);
        dtc->name_addr = NULL;
    }
    return -1;
    
}

#endif

