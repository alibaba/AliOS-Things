/* --------------------------------------------------
 * @brief: 
 *
 * @version: 1.0
 *
 * @date: 10/08/2015 09:28:27 AM
 *
 * --------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

#include "joylink.h"
#include "joylink_json.h"
#include "joylink_log.h"
#include "fsal.h"

#define JL_SUB_DEV_MAX  (10)

/**
 * brief: 
 *
 * @Param: de
 * @Param: num
 * @Param: out
 *
 * @Returns: 
 */
int 
joylink_packet_sub_add_rsp(const JLDevInfo_t *de, int num, char *out)
{
    if(NULL == de || NULL == out 
            || num < 0 || num > JL_SUB_DEV_MAX){
        return 0;
    }
    cJSON *root;
    int len = 0, i = 0;
    char *js_out;

    root = cJSON_CreateObject();
    if(NULL == root){
        goto RET;
    }

    cJSON_AddNumberToObject(root, "code", de[i].state);
    cJSON_AddStringToObject(root, "msg", de[i].msg);
    cJSON_AddStringToObject(root, "mac", de[i].jlp.mac);
    cJSON_AddStringToObject(root, "productuuid", de[i].jlp.uuid);

    js_out=cJSON_Print(root);  
    len = strlen(js_out);
    time_t c_t = xTaskGetTickCount();//time(NULL);

    memcpy(out, &c_t, 4);
    memcpy(out + 4, js_out, len);
    len += 4;

    cJSON_Delete(root); 
    free(js_out);
RET:
    return len;
}

/**
 * brief: 
 *
 * @Param: pMsg
 * @Param: out_num
 *
 * @Returns: 
 */
JLDevInfo_t *
joylink_parse_sub_add(const uint8_t* pMsg, int* out_num)
{
    JLDevInfo_t * ret = NULL;
    if(NULL == pMsg || NULL == out_num){
        printf("--->:ERROR: pMsg is NULL\n");
        return ret;
    }
    int count = 0;
    cJSON *pSub;
    cJSON * pJson = cJSON_Parse((char *)pMsg);

    if(NULL == pJson){
        printf("--->:cJSON_Parse is error:%s\n", pMsg);
      return ret;
    }

    JLDevInfo_t * devs = (JLDevInfo_t*)malloc(sizeof(JLDevInfo_t) * JL_SUB_DEV_MAX);
    if(NULL == devs){
        return ret;
    }

    bzero(devs, sizeof(JLDevInfo_t) * JL_SUB_DEV_MAX);
    cJSON *pData = cJSON_GetObjectItem(pJson, "data");
    if(NULL != pData){
        pSub = cJSON_GetObjectItem(pData, "mac");
        if(NULL != pSub){
            strcpy(devs[count].jlp.mac, pSub->valuestring);
        }
        pSub = cJSON_GetObjectItem(pData, "productuuid");
        if(NULL != pSub){
            strcpy(devs[count].jlp.uuid, pSub->valuestring);
        }
        pSub = cJSON_GetObjectItem(pData, "protocol");
        if(NULL != pSub){
            devs[count].jlp.protocol = pSub->valueint;
        }
        count++;
    }                                                                                                         
    ret = devs;
    *out_num = count;

    cJSON_Delete(pJson);

    return ret;
}

/**
 * brief: 
 *
 * @Param: pMsg
 * @Param: dev
 *
 * @Returns: 
 */
int
joylink_parse_sub_auth(const uint8_t* pMsg, JLDevInfo_t *dev)
{
    int ret = E_RET_ERROR;
    if(NULL == pMsg || NULL == dev){
        log_error("--->:ERROR: pMsg is NULL\n");
        return ret;
    }
    cJSON * pSub;
    cJSON * pJson = cJSON_Parse((char *)pMsg);

    if(NULL == pJson){
        log_error("--->:cJSON_Parse is error:%s\n", pMsg);
      return ret;
    }

    cJSON *jdev= cJSON_GetObjectItem(pJson, "data");
    if(NULL != jdev){
        pSub = cJSON_GetObjectItem(jdev, "feedid");
        if(NULL != pSub){
            strcpy(dev->jlp.feedid, pSub->valuestring);
        }
        pSub = cJSON_GetObjectItem(jdev, "accesskey");
        if(NULL != pSub){
            strcpy(dev->jlp.accesskey, pSub->valuestring);
        }
        pSub = cJSON_GetObjectItem(jdev, "localkey");
        if(NULL != pSub){
            strcpy(dev->jlp.localkey, pSub->valuestring);
        }
        pSub = cJSON_GetObjectItem(jdev, "mac");
        if(NULL != pSub){
            strcpy(dev->jlp.mac, pSub->valuestring);
        }
        pSub = cJSON_GetObjectItem(jdev, "productuuid");
        if(NULL != pSub){
            strcpy(dev->jlp.uuid, pSub->valuestring);
        }
    }

    cJSON_Delete(pJson);
    ret = E_RET_OK;

    return ret;
}

/**
 * brief: 
 *
 * @Param: de
 * @Param: out
 *
 * @Returns: 
 */
int 
joylink_packet_sub_auth_rsp(const JLDevInfo_t *de, char *out)
{
    if(NULL == de || NULL == out ){
        return 0;
    }
    cJSON *root;
    int len = 0;
    char *js_out = NULL;

    root = cJSON_CreateObject();
    if(NULL == root){
        goto RET;
    }

    cJSON_AddNumberToObject(root, "code", de->state);
    cJSON_AddStringToObject(root, "msg", de->msg);
    cJSON_AddStringToObject(root, "mac", de->jlp.mac);
    cJSON_AddStringToObject(root, "productuuid", de->jlp.uuid);

    js_out=cJSON_Print(root);  
    len = strlen(js_out);

    memcpy(out , js_out, len);

    cJSON_Delete(root); 
    if(js_out != NULL){
        free(js_out);
    }
RET:
    return len;
}

/**
 * brief: 
 *
 * @Param: sdev
 * @Param: count
 *
 * @Returns: 
 */
char * 
joylink_package_subdev(JLDevInfo_t *sdev, int count)
{
    if(NULL == sdev){
        return NULL;
    }
    cJSON *arrary; 
    char *out = NULL; 
    unsigned int i; 
   
    cJSON **js_devs = (cJSON **)malloc(sizeof(cJSON *) * count); 
    if(js_devs == NULL){
        goto RET;
    }
    if(NULL == (arrary = cJSON_CreateArray())){
        free(js_devs);
        goto RET;
    }

    for(i = 0; i < count; i ++){
        js_devs[i] =cJSON_CreateObject();
        if(NULL != js_devs[i]){
            cJSON_AddItemToArray(arrary, js_devs[i]);
            cJSON_AddNumberToObject(js_devs[i], "state", sdev[i].state);
            cJSON_AddNumberToObject(js_devs[i], "protocol", sdev[i].jlp.protocol);
            cJSON_AddStringToObject(js_devs[i], "mac", sdev[i].jlp.mac);
            cJSON_AddStringToObject(js_devs[i], "productuuid", sdev[i].jlp.uuid);
            cJSON_AddNumberToObject(js_devs[i], "lancon", sdev[i].jlp.lancon);
            cJSON_AddNumberToObject(js_devs[i], "trantype", sdev[i].jlp.cmd_tran_type);
            cJSON_AddStringToObject(js_devs[i], "feedid", sdev[i].jlp.feedid);
            cJSON_AddStringToObject(js_devs[i], "devkey", sdev[i].jlp.pubkeyS);
        }
    }

    out=cJSON_Print(arrary);  
    cJSON_Delete(arrary);
RET:
    return out;
}
