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
#include "joylink_log.h"

extern JLDevInfo_t *
joylink_dev_sub_devs_get(int *count, int scan_type);

/**
 * brief: 
 *
 * @Param: scan
 * @Param: pMsg
 *
 * @Returns: 
 */
int 
joylink_parse_scan(DevScan_t *scan, const char * pMsg)
{
    int ret = E_RET_ERROR;
    if(NULL == pMsg || NULL == scan){
        printf("--->:ERROR: pMsg is NULL\n");
        return ret;
    }

    cJSON * pJson = cJSON_Parse(pMsg);
    if(NULL == pJson){
        printf("--->:cJSON_Parse is error:%s\n", pMsg);
        return ret;
    }
    cJSON * pSub = cJSON_GetObjectItem(pJson, "uuid");
    if(NULL != pSub){
        strcpy(scan->uuid, pSub->valuestring);
    }
    pSub = cJSON_GetObjectItem(pJson, "status");
    if(NULL != pSub){
        scan->type = pSub->valueint;
    }
    cJSON * idt = cJSON_GetObjectItem(pJson, "idt");
    if(NULL != idt){
        cJSON * prand = cJSON_GetObjectItem(idt, "app_rand");
        if(NULL != pSub){
            strcpy(scan->app_rand, prand->valuestring);
        }
    }

    ret = E_RET_OK;

    cJSON_Delete(pJson);
    return ret;
}

/**
 * brief: 
 *
 * @Param: retMsg
 * @Param: retCode
 * @Param: scan_type
 * @Param: dv
 *
 * @Returns: 
 */
char * 
joylink_package_scan(const char *retMsg, const int retCode,
        int scan_type, JLDevice_t *dv)
{
    if(NULL == retMsg ){
        return NULL;
    }
    cJSON *root;
    JLDevInfo_t *sdev = NULL;
    int count = 0;
    char *out  = NULL; 
    char *jsdev = NULL;

    root = cJSON_CreateObject();
    if(NULL == root){
        goto RET;
    }

    cJSON_AddNumberToObject(root, "code", retCode);
    cJSON_AddStringToObject(root, "msg", retMsg); 
    cJSON_AddStringToObject(root, "mac", dv->jlp.mac); 
    cJSON_AddStringToObject(root, "productuuid", dv->jlp.uuid); 
    cJSON_AddStringToObject(root, "feedid", dv->jlp.feedid); 
    cJSON_AddStringToObject(root, "devkey", dv->jlp.pubkeyS); 
    cJSON_AddNumberToObject(root, "lancon", dv->jlp.lancon); 
    cJSON_AddNumberToObject(root, "trantype", dv->jlp.cmd_tran_type); 
    cJSON_AddNumberToObject(root, "devtype", dv->jlp.devtype); 
    cJSON_AddStringToObject(root, "CID", dv->jlp.CID); 
    cJSON_AddStringToObject(root, "firmwareVersion", dv->jlp.firmwareVersion); 
    cJSON_AddStringToObject(root, "modelCode", dv->jlp.modelCode); 
    
    {
        cJSON *idt;
        idt = cJSON_CreateObject();
        if(NULL == idt){
            goto RET;
        }
        cJSON_AddNumberToObject(idt, "t", dv->idt.type);
        cJSON_AddStringToObject(idt, "d_p", dv->idt.pub_key);
        cJSON_AddStringToObject(idt, "d_s", dv->idt.sig);
        cJSON_AddStringToObject(idt, "f_p", dv->idt.f_pub_key);
        cJSON_AddStringToObject(idt, "f_s", dv->idt.f_sig);
        cJSON_AddStringToObject(idt, "d_r", dv->idt.rand);
        cJSON_AddStringToObject(idt, "d_as", dv->idt.a_rand_sig);
        cJSON_AddItemToObject(root,"d_idt", idt);
    }

    if(dv->jlp.devtype == E_JLDEV_TYPE_GW){
        sdev = joylink_dev_sub_devs_get(&count, scan_type);
        if(NULL != sdev){
            jsdev = joylink_package_subdev(sdev, count);

            cJSON *jsp_dev = cJSON_Parse(jsdev);
            if(jsp_dev){
                cJSON_AddItemToObject(root,"subdev", jsp_dev);
            }
        }
        if(sdev != NULL){
            free(sdev);
        }
        if(jsdev != NULL){
            free(jsdev);
        }
    }

    out=cJSON_Print(root);  
    cJSON_Delete(root); 
RET:
    return out;
}

/**
 * brief: 
 *
 * @Param: de
 * @Param: pMsg
 *
 * @Returns: 
 */
int 
joylink_parse_lan_write_key(DevEnable_t *de, const char * pMsg)
{
    int ret = -1;
    if(NULL == pMsg || NULL == de){
        printf("--->:ERROR: pMsg is NULL\n");
        return ret;
    }
    cJSON * pJson = cJSON_Parse(pMsg);

    if(NULL == pJson){
        printf("--->:cJSON_Parse is error:%s\n", pMsg);
      return ret;
    }

    cJSON * pData = cJSON_GetObjectItem(pJson, "data");
    if(NULL != pData){
        cJSON * pSub = cJSON_GetObjectItem(pData, "feedid");
        if(NULL != pSub){
            strcpy(de->feedid, pSub->valuestring);
        }
        pSub = cJSON_GetObjectItem(pData, "productuuid");
        if(NULL != pSub){
            strcpy(de->productuuid, pSub->valuestring);
        }
        pSub = cJSON_GetObjectItem(pData, "localkey");
        if(NULL != pSub){
            strcpy(de->localkey, pSub->valuestring);
        }
        pSub = cJSON_GetObjectItem(pData, "accesskey");
        if(NULL != pSub){
            strcpy(de->accesskey, pSub->valuestring);
        }
        pSub = cJSON_GetObjectItem(pData, "lancon");
        if(NULL != pSub){
            de->lancon = pSub->valueint;
        }                                                                                                         
        pSub = cJSON_GetObjectItem(pData, "trantype");
        if(NULL != pSub){
            de->cmd_tran_type = pSub->valueint;
        }                                                                                                         
        pSub = cJSON_GetObjectItem(pData, "joylink_server");
        if(NULL != pSub){
            if(cJSON_GetArraySize(pSub) > 0){
                cJSON *pv;
                pv = cJSON_GetArrayItem(pSub, 0);
                if(NULL != pv){
                    strcpy(de->joylink_server, pv->valuestring);
                }
            }
        }                                                                                                        
        cJSON *pOpt = cJSON_GetObjectItem(pData, "opt");
        char *out = NULL;
        if(NULL != pOpt){
            out = cJSON_Print(pOpt);
            if(NULL != out){
                strcpy(de->opt, out);
                free(out);
            }
        }
    }

    cJSON * idt = cJSON_GetObjectItem(pData, "c_idt");
    if(NULL != idt){
         cJSON * pSub = cJSON_GetObjectItem(idt, "cloud_sig");
        if(NULL != pSub){
            strcpy(de->cloud_sig, pSub->valuestring);
        }                                                                                                         
    }

    cJSON_Delete(pJson);
    ret = 0;
    return ret;
}

/**
 * brief: 
 *
 * @Param: feedid
 * @Param: pMsg
 *
 * @Returns: 
 */
int 
joylink_parse_json_ctrl(char *feedid, const char * pMsg)
{
    int ret = E_RET_ERROR;
    if(NULL == pMsg || NULL == feedid){
        printf("--->:ERROR: pMsg is NULL\n");
        return ret;
    }
    cJSON * pJson = cJSON_Parse(pMsg);
    if(NULL == pJson){
        printf("--->:cJSON_Parse is error:%s\n", pMsg);
      return ret;
    }
    cJSON * pSub = cJSON_GetObjectItem(pJson, "feedid");
    if(NULL != pSub){
        strcpy(feedid, pSub->valuestring);
    }
    cJSON_Delete(pJson);
    ret = E_RET_OK;

    return ret;
}

/**
 * brief: 
 *
 * @Param: otaOrder
 * @Param: pMsg
 *
 * @Returns: 
 */
int 
joylink_parse_server_ota_order_req(JLOtaOrder_t* otaOrder, const char * pMsg)
{
    int ret = -1;
    if(NULL == pMsg || NULL == otaOrder){
        log_error("--->:ERROR: pMsg is NULL\n");
        return ret;
    }
    cJSON * pJson = cJSON_Parse(pMsg);
    if(NULL == pJson){
        log_error("--->:cJSON_Parse is error:%s\n", pMsg);
      return ret;
    }
    cJSON * pData = cJSON_GetObjectItem(pJson, "serial");
    if(NULL != pData){
        otaOrder->serial = pData->valueint;
    }
    pData = cJSON_GetObjectItem(pJson, "data");
    if(NULL != pData){
        cJSON * pSub = cJSON_GetObjectItem(pData, "feedid");
        if(NULL != pSub){
            strcpy(otaOrder->feedid, pSub->valuestring);
        }
        pSub = cJSON_GetObjectItem(pData, "productuuid");
        if(NULL != pSub){
            strcpy(otaOrder->productuuid, pSub->valuestring);
        }
        pSub = cJSON_GetObjectItem(pData, "version");
        if(NULL != pSub){
            otaOrder->version = pSub->valueint;
        }                                                                                                         
        pSub = cJSON_GetObjectItem(pData, "versionname");
        if(NULL != pSub){
            strcpy(otaOrder->versionname, pSub->valuestring);
        }
        pSub = cJSON_GetObjectItem(pData, "crc32");
        if(NULL != pSub){
            otaOrder->crc32 = pSub->valueint;
        }
        pSub = cJSON_GetObjectItem(pData, "url");
        if(NULL != pSub){
            strcpy(otaOrder->url, pSub->valuestring);
        }
    }

    cJSON_Delete(pJson);
    ret = 0;
    return ret;
}

/**
 * brief: 
 *
 * @Param: otaUpload
 * @Param: pMsg
 *
 * @Returns: 
 */
int
joylink_parse_server_ota_upload_req(JLOtaUploadRsp_t* otaUpload, const char* pMsg)
{
    int ret = -1;
    if(NULL == pMsg || NULL == otaUpload){
        log_error("--->:ERROR: pMsg is NULL\n");
        return ret;
    }

    cJSON * pJson = cJSON_Parse(pMsg);
    if(NULL == pJson){
        log_error("--->:cJSON_Parse is error:%s\n", pMsg);
      return ret;
    }
    cJSON * pSub = cJSON_GetObjectItem(pJson, "code");
    if(NULL != pSub){
        otaUpload->code = pSub->valueint;
    }
    pSub = cJSON_GetObjectItem(pJson, "msg");
    if(NULL != pSub){
        strcpy(otaUpload->msg, pSub->valuestring);
    }

    cJSON_Delete(pJson);
    ret = 0;
    return ret;
}

/**
 * brief: 
 *
 * @Param: otaUpload
 *
 * @Returns: 
 */
char *
joylink_package_ota_upload(JLOtaUpload_t *otaUpload)
{
    if(NULL == otaUpload){
        return NULL;
    }
    cJSON *root, *data;
    char *out = NULL;
    root = cJSON_CreateObject();
    if(NULL == root){
        goto RET;
    }
    data = cJSON_CreateObject();
    if(NULL == data){
        cJSON_Delete(root);
        goto RET;
    }

    cJSON_AddStringToObject(root, "cmd", "otastat");
    cJSON_AddStringToObject(data, "feedid", otaUpload->feedid);
    cJSON_AddStringToObject(data, "productuuid", otaUpload->productuuid);
    cJSON_AddNumberToObject(data, "status", otaUpload->status);
    cJSON_AddStringToObject(data, "status_desc", otaUpload->status_desc);
    cJSON_AddNumberToObject(data, "progress", otaUpload->progress);
    cJSON_AddItemToObject(root,"data", data);

    out=cJSON_Print(root);
    cJSON_Delete(root);
RET:
    return out;
}

/**
 * brief: 
 *
 * @Param: pMsg
 * @Param: num
 *
 * @Returns: 
 */
char ** 
joylink_parse_ids(const char * pMsg, int32_t *num)
{
    char **ret = NULL;
    if(NULL == pMsg || NULL == num){
        printf("--->:ERROR: pMsg is NULL\n");
        return ret;
    }

    cJSON * pJson = cJSON_Parse(pMsg);
    int iSize = 0;
    int iCnt = 0;
    int sum = 0;

    if(NULL == pJson){
        printf("--->:cJSON_Parse is error:%s\n", pMsg);
        return ret;
    }
    if(NULL != pJson){
        iSize = cJSON_GetArraySize(pJson);
        ret = (char**)malloc(sizeof(char*) * iSize);
        iCnt = 0;

        for(iCnt = 0; iCnt < iSize; iCnt++){
            cJSON *pSub = cJSON_GetArrayItem(pJson, iCnt);
            if(NULL == pSub){
                continue;
            }
            ret[iCnt] = (char *)malloc(32);
            if(NULL != ret[iCnt]){
                strcpy(ret[iCnt], pSub->valuestring);
                sum ++;
            }
        }
    }
    
    *num = sum;
    cJSON_Delete(pJson);
    return ret;
}
