/* --------------------------------------------------
 * @brief: 
 *
 * @version: 1.0
 * --------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "joylink.h"
#include "joylink_packets.h"
#include "joylink_json.h"
#include "joylink_extern_json.h"

#define JL_MAX_SUB      (20)
int tail_index = 0;
/*
JLDevInfo_t _g_sub_dev[JL_MAX_SUB] = { 
    {.jlp.mac = "mac001", .type = E_JLDEV_TYPE_SUB,  .jlp.uuid = "jdsmart", .jlp.lancon = 1, .jlp.cmd_tran_type = 0, .state = 1}, 
    {.jlp.mac = "mac002", .type = E_JLDEV_TYPE_SUB,  .jlp.uuid = "jdsmart", .jlp.lancon = 1, .jlp.cmd_tran_type = 0, .state = 1}, 
    {.jlp.mac = "mac003", .type = E_JLDEV_TYPE_SUB,  .jlp.uuid = "jdsmart", .jlp.lancon = 1, .jlp.cmd_tran_type = 1, .state = 1}
};*/
JLDevInfo_t _g_sub_dev[JL_MAX_SUB] = { 
    {   .jlp.feedid="144739743326280531",.jlp.accesskey="9b5ccce967e4a818f1fb3f94f306d852",
        .jlp.mac = "mac001", .type = E_JLDEV_TYPE_SUB,  
        .jlp.uuid = "4GNIPK", .jlp.lancon = 1,
        .jlp.cmd_tran_type = 0, .state = 1}
};

/**
 * brief: 
 *
 * @Param: dev
 * @Param: num
 *
 * @Returns: 
 */
E_JLRetCode_t
joylink_dev_sub_add(JLDevInfo_t *dev, int num)
{
    /**
     *FIXME: todo
     */
    int ret = E_RET_OK;
    if(tail_index + 1 < JL_MAX_SUB){
        memcpy(&_g_sub_dev[++tail_index], dev, sizeof(JLDevInfo_t));
    }

    return ret;
}

/**
 * brief: 
 *
 * @Param: dev
 * @Param: num
 *
 * @Returns: 
 */
E_JLRetCode_t
joylink_sub_dev_del(JLDevInfo_t *dev, int num)
{
    /**
     *FIXME: todo
     */
    int ret = E_RET_OK;

    return ret;
}

/**
 * brief: 
 *
 * @Param: feedid
 * @Param: dev
 *
 * @Returns: 
 */
E_JLRetCode_t
joylink_dev_sub_get_by_feedid(char *feedid, JLDevInfo_t *dev)
{
    /**
     *FIXME: todo
     */
    int ret = E_RET_ERROR;
    int i; 

    for(i = 0; i < JL_MAX_SUB; i++){
        if(!strcmp(feedid, _g_sub_dev[i].jlp.feedid)){
            memcpy(dev, &_g_sub_dev[i], sizeof(JLDevInfo_t));
            ret = E_RET_OK;
        }
    }

    return ret;
}

/**
 * brief: 
 *
 * @Param: uuid
 * @Param: mac
 * @Param: dev
 *
 * @Returns: 
 */
E_JLRetCode_t
joylink_sub_dev_get_by_uuid_mac(char *uuid, char *mac, JLDevInfo_t *dev)
{
    /**
     *FIXME: todo
     */
    int ret = E_RET_OK;
    int i; 

    for(i = 0; i < JL_MAX_SUB; i++){
        if(!strcmp(uuid, _g_sub_dev[i].jlp.uuid) && 
                !strcmp(mac, _g_sub_dev[i].jlp.mac)){
            memcpy(dev, &_g_sub_dev[i], sizeof(JLDevInfo_t));
            ret = E_RET_OK;
        }
    }

    return ret;
}

/**
 * brief: 
 *
 * @Param: uuid
 * @Param: mac
 * @Param: dev
 *
 * @Returns: 
 */
E_JLRetCode_t
joylink_dev_sub_update_keys_by_uuid_mac(char *uuid, char *mac, JLDevInfo_t *dev)
{
    /**
     *FIXME: todo
     */
    int ret = E_RET_OK;
    int i; 

    for(i = 0; i < JL_MAX_SUB; i++){
        if(!strcmp(uuid, _g_sub_dev[i].jlp.uuid) && 
                !strcmp(mac, _g_sub_dev[i].jlp.mac)){

            memcpy(_g_sub_dev[i].jlp.accesskey, dev->jlp.accesskey, sizeof(dev->jlp.accesskey));
            memcpy(_g_sub_dev[i].jlp.localkey, dev->jlp.localkey, sizeof(dev->jlp.localkey));
            memcpy(_g_sub_dev[i].jlp.feedid, dev->jlp.feedid, sizeof(dev->jlp.feedid));
    
            _g_sub_dev[i].state = E_JLDEV_ST_CONFD;
            ret = E_RET_OK;
        }
    }

    return ret;
}

/**
 * brief: 
 *
 * @Param: count
 * @Param: scan_type
 *
 * @Returns: 
 */
JLDevInfo_t *
joylink_dev_sub_devs_get(int *count, int scan_type)
{
    /**
     *FIXME: todo must lock
     */
    int i, sum = 0; 
    JLDevInfo_t *devs = NULL;
    
    switch(scan_type){
        case 0: //All devs
            sum = tail_index + 1;
            devs = (JLDevInfo_t *)malloc(sizeof(JLDevInfo_t) * sum);
            bzero(devs, sizeof(JLDevInfo_t) * sum);
            if(devs != NULL){
                for(i = 0; i < JL_MAX_SUB && i < sum; i++){
                    memcpy(&devs[i], &_g_sub_dev[i], sizeof(JLDevInfo_t));
                }
            }
            break;
        case 1: //devs no conf
            for(i = 0; i < JL_MAX_SUB; i++){
                if(E_JLDEV_ST_NO_CONF == _g_sub_dev[i].state){
                    sum ++;
                }
            }
            devs = (JLDevInfo_t *)malloc(sizeof(JLDevInfo_t) * sum);
            if(devs != NULL){
                for(i = 0; i < JL_MAX_SUB && i < sum; i++){
                    if(E_JLDEV_ST_NO_CONF == _g_sub_dev[i].state){
                        memcpy(&devs[i], &_g_sub_dev[i], sizeof(JLDevInfo_t));
                    }
                }
            }
            break;
        case 2: //devs confd
            for(i = 0; i < JL_MAX_SUB; i++){
                if(E_JLDEV_ST_CONFD == _g_sub_dev[i].state){
                    sum ++;
                }
            }
            devs = (JLDevInfo_t *)malloc(sizeof(JLDevInfo_t) * sum);
            if(devs != NULL){
                for(i = 0; i < JL_MAX_SUB && i < sum; i++){
                    if(E_JLDEV_ST_CONFD == _g_sub_dev[i].state){
                        memcpy(&devs[i], &_g_sub_dev[i], sizeof(JLDevInfo_t));
                    }
                }
            }
            break;
        default:
            break;
    }

    *count = sum;

    return devs;
}

/**
 * brief: 
 *
 * @Param: cmd
 * @Param: cmd_len
 * @Param: feedid
 *
 * @Returns: 
 */
E_JLRetCode_t
joylink_dev_sub_ctrl(const char* cmd, int cmd_len, char* feedid)
{
    /**
     *FIXME: todo must lock
     */
    log_debug("cmd:%s:feedid:%s\n", cmd, feedid);
    int ret = E_RET_OK;

    return ret;
}

/**
 * brief: 
 *
 * @Param: feedid
 * @Param: out_len
 *
 * @Returns: 
 */
char *
joylink_dev_sub_get_snap_shot(char *feedid, int *out_len)
{
    /**
     *FIXME: todo must lock
     */
    log_debug("");
    char *tp ="{\"code\": 0,  \"streams\": [{ \"current_value\": 0, \"stream_id\": \"power\" }]}";

    char *ss = (char*)malloc(100);

    if(ss != NULL){
        memset(ss, 0, 100);
        memcpy(ss, tp, strlen(tp)); 
        *out_len = strlen(tp);
    }

    return ss;
}

/**
 * brief: 
 *
 * @Param: feedid
 *
 * @Returns: 
 */
E_JLRetCode_t
joylink_dev_sub_unbind(const char *feedid)
{
    /**
     *FIXME: todo must lock
     */
    int ret = E_RET_OK;
    log_debug("feedid:%s\n", feedid);

    return ret;
}
