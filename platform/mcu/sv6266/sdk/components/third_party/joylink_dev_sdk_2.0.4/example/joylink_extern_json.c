/* --------------------------------------------------
 * @brief: 
 *
 * @version: 1.0
 *
 * @date: 10/08/2015 09:28:27 AM
 *
 * @author: 
 * --------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

#include "joylink_json.h"
#include "joylink_dev.h"
#include "joylink_log.h"

/**
 * brief: 
 *
 * @Param: pCtrl
 * @Param: pMsg
 *
 * @Returns: 
 */
int 
joylink_dev_parse_ctrl(LightCtrl_t *pCtrl, const char *pMsg)
{
    int ret = -1;
    if(NULL == pMsg || NULL == pCtrl){
        return ret;
    }
    log_debug("json_org:%s", pMsg);
    cJSON * pSub;
    cJSON * pJson = cJSON_Parse(pMsg);

    if(NULL == pJson){
      log_error("--->:ERROR: pMsg is NULL\n");
      goto RET;
    }

    char tmp_str[64];
    cJSON *pStreams = cJSON_GetObjectItem(pJson, "streams");
    if(NULL != pStreams){
        int iSize = cJSON_GetArraySize(pStreams);
        int iCnt;
        for( iCnt = 0; iCnt < iSize; iCnt++){
            pSub = cJSON_GetArrayItem(pStreams, iCnt);
            if(NULL == pSub){
                continue;
            }

            cJSON *pSId = cJSON_GetObjectItem(pSub, "stream_id");
            if(NULL != pSId){
                cJSON *pV = cJSON_GetObjectItem(pSub, "current_value");
                if(NULL == pV){
                    continue;
                }

                if(!strcmp(pSId->valuestring, "power")){
                   pCtrl->cmd = LIGHT_CMD_POWER;
				   strcpy(tmp_str, pV->valuestring);
                   pCtrl->para_value = atoi(tmp_str);
                }

                log_debug("power:%d", pCtrl->para_value);
                char *dout = cJSON_Print(pSub);
                if(NULL != dout){
                    log_debug("org streams:%s", dout);
                    free(dout);
                }
            }
        }  // for
    }                                                                                                         
    cJSON_Delete(pJson);
RET:
    return ret;
}

/**
 * brief: 
 * NOTE: If return is not NULL, 
 * must free it, after use.
 *
 * @Param: retMsg
 * @Param: retCode
 * @Param: wci
 * @Param: devlist
 *
 * @Returns: char * 
 */
char * 
joylink_dev_package_info(const int retCode, const LightCtrl_t *pCtrl)
{
    if(NULL == pCtrl){
        return NULL;
    }
    cJSON *root, *arrary;
    char *out  = NULL; 

    root = cJSON_CreateObject();
    if(NULL == root){
        goto RET;
    }
    arrary = cJSON_CreateArray();
    if(NULL == arrary){
        cJSON_Delete(root);
        goto RET;
    }
    cJSON_AddNumberToObject(root, "code", retCode);
    cJSON_AddItemToObject(root, "streams", arrary);
    
    char i2str[32];
    bzero(i2str, sizeof(i2str));
    cJSON *element = cJSON_CreateObject();
    cJSON_AddItemToArray(arrary, element);
    cJSON_AddStringToObject(element, "stream_id", "power");
    sprintf(i2str, "%d", pCtrl->para_value);
    cJSON_AddStringToObject(element, "current_value", i2str);
    /*cJSON_AddNumberToObject(element, "current_value", pCtrl->para_value); */

    out=cJSON_Print(root);  
    cJSON_Delete(root); 
RET:
    return out;
}

char * 
joylink_dev_modelcode_info(const int retCode, const LightCtrl_t *pCtrl)
{
    if(NULL == pCtrl){
        return NULL;
    }
    cJSON *root, *arrary;
    char *out  = NULL; 

    root = cJSON_CreateObject();
    if(NULL == root){
        goto RET;
    }
    arrary = cJSON_CreateArray();
    if(NULL == arrary){
        cJSON_Delete(root);
        goto RET;
    }
    cJSON_AddItemToObject(root, "model_codes", arrary);
    
    char i2str[32];
    bzero(i2str, sizeof(i2str));
    cJSON *element = cJSON_CreateObject();
    cJSON_AddItemToArray(arrary, element);
    cJSON_AddStringToObject(element, "feedid", "247828880060773075");
    //sprintf(i2str, "%d", pCtrl->para_value);
    cJSON_AddStringToObject(element, "model_code", "12345678123456781234567812345678");

    out=cJSON_Print(root);  
    cJSON_Delete(root); 
RET:
    return out;
}

/**
 * brief: 
 *
 * @Param: jlp
 * @Param: pMsg
 *
 * @Returns: 
 */
int 
joylink_parse_jlp(JLPInfo_t *jlp, char * pMsg)
{
    int ret = -1;
    if(NULL == pMsg || NULL == jlp){
        return ret;
    }
    cJSON *pVal;
    cJSON * pJson = cJSON_Parse(pMsg);

    if(NULL == pJson){
      log_error("--->:ERROR: pMsg is NULL\n");
      goto RET;
    }

    pVal = cJSON_GetObjectItem(pJson, "uuid");
    if(NULL != pVal){
        strcpy(jlp->uuid, pVal->valuestring);
    }

    pVal = cJSON_GetObjectItem(pJson, "feedid");
    if(NULL != pVal){
        strcpy(jlp->feedid, pVal->valuestring);
    }

    pVal = cJSON_GetObjectItem(pJson, "accesskey");
    if(NULL != pVal){
        strcpy(jlp->accesskey, pVal->valuestring);
    }

    pVal = cJSON_GetObjectItem(pJson, "localkey");
    if(NULL != pVal){
        strcpy(jlp->localkey, pVal->valuestring);
    }

    pVal = cJSON_GetObjectItem(pJson, "version");
    if(NULL != pVal){
        jlp->version = pVal->valueint;
    }

    cJSON_Delete(pJson);
    ret = 0;
RET:
    return ret;
}
