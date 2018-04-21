/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <cJSON.h>

#include "iot_import.h"
#include "iot_export.h"
#include "iot_export_mqtt.h"
#include "mqtt_instance.h"
#include "aos/log.h"

#define ACTIVE_TOPIC_LEN        (96)
#define ACTIVE_KEY_LEN          (32)
#define ACTIVE_INFO_LEN         (100)

#define MSG_REPORT_LEN  (384)
#define MSG_INFORM_LEN  (128)

#define TOPIC_UPDATE   "update"
#define ACT_LOGD(format, ...) LOG( format,##__VA_ARGS__)
#define ACT_LOGI(format, ...) LOG( format,##__VA_ARGS__)
#define ACT_LOGW(format, ...) LOG( format,##__VA_ARGS__)
#define ACT_LOGE(format, ...) LOG( format,##__VA_ARGS__)
// #define ACT_LOGD(format, ...) LOGD(LOG_TAG, format,##__VA_ARGS__)
// #define ACT_LOGI(format, ...) LOGI(LOG_TAG, format,##__VA_ARGS__)
// #define ACT_LOGW(format, ...) LOGW(LOG_TAG, format,##__VA_ARGS__)
// #define ACT_LOGE(format, ...) LOGE(LOG_TAG, format,##__VA_ARGS__)
/*
TOPIC: /sys/${productKey}/${deviceName}/thing/deviceinfo/update  
payload:
{
    "id" : 123,    					
	"version":"1.0",				
    "params" : [{
        "attrKey" : "SYS_ALIOS_ACTIVATION",
        "attrValue" : "some encrypted data"
    }],
    "method" : "thing.deviceinfo.update"  
}

response:
{
    "id" : 123,
	"code":200,
    "data" : {}
}
*/
//extern UINT32 aos_get_version_info(UINT8 version_num[4], UINT8 random_num[4], UINT8 mac_address[4], UINT8 chip_code[4], UINT8 *output_buffer, UINT32 output_buffer_size);

// typedef struct active_info {
//     char version[4];
//     char mac_address[4];
//     char chip_code[4];
//     char output[ACTIVE_INFO_LEN]
// } active_info_t;

typedef struct active_resp_info {
    int id;
    int code;
} active_resp_info_t;

// active_info_t dev_active_info={{0},{0},{0},{0}};


//Generate topic name according to @product_key, @device_name @topic
//and then copy to @buf.
//0, successful; -1, failed
static int active_gen_topic_name(char *buf, size_t buf_len,const char *product_key,const char *device_name,const char *topic)
{
    int ret;
    ret = snprintf(buf, buf_len, "/sys/%s/%s/thing/deviceinfo/%s", product_key, device_name,topic);

    if (ret < 0) {
        ACT_LOGE("snprintf failed");
        return -1;
    }

    return 0;
}

static int active_publish(const char *topic_type, const char *msg)
{
    int ret;
    char topic_name[ACTIVE_TOPIC_LEN] = {0};
    
    char product_key[ACTIVE_KEY_LEN]={0};
    char device_name[ACTIVE_KEY_LEN]={0};
    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);

    ret = active_gen_topic_name(topic_name, ACTIVE_TOPIC_LEN,product_key,device_name,topic_type);
    if (ret < 0) {
        ACT_LOGE("generate topic name of info failed");
        return -1;
    }
    printf("--- public topic=%s ,payload=%s\n", topic_name, msg);
    ret =  mqtt_publish(topic_name,1,msg,strlen(msg)+1);
    if (ret < 0) {
        ACT_LOGE("publish failed");
        return -1;
    }
    return 0;
}


static int active_gen_update_msg(char *buf, size_t buf_len,int id, const char* active_info)
{
    int ret;
    ret = snprintf(buf,
                   buf_len,
                   "{\"id\":%d,\"version\":\"1.0\",\"params\":[{\"attrKey\":\"SYS_ALIOS_ACTIVATION\",\"attrValue\":\"%s\"}],\"method\":\"thing.deviceinfo.update\"}",
                   id,active_info);

    if (ret < 0) {
        ACT_LOGE("snprintf failed");
        return -1;
    }

    return 0;
}

static int active_gen_delete_msg(char *buf, size_t buf_len,int id, const char* active_info)
{
    int ret;
    ret = snprintf(buf,
                   buf_len,
                   "{\"id\":%d,\"version\":\"1.0\",\"params\":[{\"attrKey\":\"SYS_ALIOS_ACTIVATION\"}],\"method\":\"thing.deviceinfo.delete\"}",
                   id,active_info);

    if (ret < 0) {
        ACT_LOGE("snprintf failed");
        return -1;
    }

    return 0;
}


static int active_parse_response(const char *response, int buf_len, active_resp_info_t *response_parmas)
{
    cJSON *root = cJSON_Parse(response);
    if (!root) {
        ACT_LOGE("Error before: [%s]\n", cJSON_GetErrorPtr());
        goto parse_failed;
    } else {
        // char *info = cJSON_Print(root);
        // ACT_LOGD("root is %s", info);
        // free(info);

        cJSON *message =  cJSON_GetObjectItem(root, "id");
        
        if (NULL == message) {
            ACT_LOGE("invalid json key-value:id ");
            goto parse_failed;
        }

        if(cJSON_IsNumber(message)){
            response_parmas->id=message->valueint;
        }

        message =  cJSON_GetObjectItem(root, "code");
        if (NULL == message) {
            ACT_LOGE("invalid json key-value:code ");
            goto parse_failed;
        }
        if(cJSON_IsNumber(message)){
            response_parmas->code=message->valueint;
        }
    }

    ACT_LOGD("parse_json success");
    goto parse_success;

parse_failed:
    if (root) {
        cJSON_Delete(root);
    }
    return -1;

parse_success:
    if (root) {
        cJSON_Delete(root);
    }
    return 0;
}

static int get_hex_version(char *str,char hex[4])
{
    //AOS-R-1.3.0
    char *p=NULL;
    char *q=NULL;
    int arr[4];
    int i=0;
    char str_ver[32]={0};
    if(str == NULL){
        return -1;
    }
    if(hex ==NULL){
        return -1;
    }
    strncpy(str_ver,str,sizeof(str_ver)-1);
    //ACT_LOGD("cur str=%s",str);
    p=strtok(str_ver,"-");
    for(i=0;i<2;i++)
    {
            if( p == NULL)
            {
                ACT_LOGE("get_hex_version err ");
                return -1;
            }
            p=strtok(NULL,"-");

    }
    ACT_LOGD("cur str=%s",p);

    q=strtok(p,".");
    for(i=0;i<3;i++)
   {
        if( q == NULL)
        {
            ACT_LOGE("get_hex_version err i=%d",i);
            return -1;
        }
        else
        {
           // ACT_LOGD("cur str=%s",q);
            hex[i]=atoi(q);
        }
        q=strtok(NULL,".");

    }
   return 0;
}

static void produce_random(unsigned char *random, unsigned int len)
{
    int i = 0;
    long long  seed = aos_now();
    srand((unsigned int)seed);
    for (i = 0; i < len; i ++) {
        random[i] = rand() & 0xFF;
    }
}

int version_report()
{
    int ret=0;
    uint8_t mac[8];
    uint8_t version[4];
    uint8_t random_num[4];
    uint8_t chip_code[4]={0};
    char output[ACTIVE_INFO_LEN]={0};
    //AOS-R-1.3.0
    ret = get_hex_version((char*)aos_get_kernel_version(),version);
    if(ret==-1){
        ACT_LOGE("get os version failed");
    }
    ACT_LOGD( "os ver=%d.%d.%d.%d\n",version[0],version[1],version[2],version[3]);

    hal_wifi_get_mac_addr(NULL, mac);
    ACT_LOGD( "mac=%02x.%02x.%02x.%02x.%02x.%02x\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

    produce_random(random_num, 4);
    
    ACT_LOGD( "random =%02x%02x%02x%02x\n",random_num[0],random_num[1],random_num[2],random_num[3]);

    char msg[MSG_REPORT_LEN]={0};

    ret=aos_get_version_info(version, random_num, mac, chip_code, output, ACTIVE_INFO_LEN);
    if(ret){
        ACT_LOGE("aos_get_version_info failed");
       return -1;
    }
	
	ACT_LOGD( "-----------------\n");
	ACT_LOGD( "after aos_get_version_info output:\n" );
	ACT_LOGD( "%s\n", output );
	ACT_LOGD( "-----------------\n");
	
    ret=active_gen_update_msg(msg,MSG_REPORT_LEN,1, output);
    if(ret){
        ACT_LOGE("active_gen_update_msg failed");
        return -1;
    }
    ret=active_publish(TOPIC_UPDATE, msg);
    if(ret){
        ACT_LOGE("active_publish failed");
        return -1;
    }
    return ret;
}
