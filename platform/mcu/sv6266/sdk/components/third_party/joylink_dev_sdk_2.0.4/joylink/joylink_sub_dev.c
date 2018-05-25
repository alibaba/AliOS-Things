#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(__MTK_7687__)
#include <stdint.h>
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#elif defined(__ICOMM_6166__)
#include <stdint.h>
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#else
#include <stdarg.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <ifaddrs.h>
#include <netdb.h>
#endif

#include "joylink.h"
#include "joylink_utils.h"
#include "joylink_packets.h"
#include "joylink_crypt.h"
#include "joylink_json.h"
#include "joylink_sub_dev.h"
#include "joylink_dev.h"

/**
 *1 parse json 
 *2 add sub dev, save sub dev info 
 *3 send ack
 */
E_JLRetCode_t 
joylink_proc_lan_sub_add(uint8_t *src, struct sockaddr_in *sin_recv, socklen_t addrlen)
{
    int ret = E_RET_ERROR;
    int len = 0;
    int num = 0;
    int en_len = 0;
    char data[JL_MAX_PACKET_LEN] = {0};
    JLDevInfo_t *devs = NULL;

    if(NULL == (devs = joylink_parse_sub_add(src, &num))){
        log_error("parse error");
        return ret;
    }

    if(E_RET_OK  != joylink_dev_sub_add(devs, num)){
    }else{
        devs[0].state = 0;
        len = joylink_packet_sub_add_rsp(devs, num, data);
    }

    log_info("rsp data:%s:len:%d", data + 4, len);
    if(devs != NULL){
        free(devs);
    }
    en_len = joylink_encrypt_lan_basic(_g_pdev->send_buff, JL_MAX_PACKET_LEN,
            ET_ACCESSKEYAES, PT_SUB_ADD,
            (uint8_t *)_g_pdev->jlp.localkey, (const uint8_t*)data, len);
    if(en_len > 0 && en_len < JL_MAX_PACKET_LEN){
        ret = sendto(_g_pdev->lan_socket, _g_pdev->send_buff, en_len, 0,
                (SOCKADDR*)sin_recv, addrlen);
        if(ret < 0){
            log_error("send error ret:%d", ret);
        }else{
            log_info("send len:%d",ret);
        }
        log_debug("send len:%d",ret);
    }else{
        log_error("packet error ret:%d", ret);
    }

    return E_RET_OK;
}

/**
 *1 parse json 
 *2 save feedid accesskey localkey 
 *3 send ack
 */
E_JLRetCode_t 
joylink_proc_lan_sub_auth(uint8_t *src, struct sockaddr_in *sin_recv, socklen_t addrlen)
{
    int ret = E_RET_ERROR;
    int len = 0;
    char data[JL_MAX_PACKET_LEN] = {0};

    JLDevInfo_t dev;
    bzero(&dev, sizeof(dev));

    if(E_RET_OK != joylink_parse_sub_auth(src, &dev)){
        return ret;
    }
    if(E_RET_OK == joylink_dev_sub_update_keys_by_uuid_mac(
                dev.jlp.uuid, dev.jlp.mac, &dev)){
        dev.state = 0;
        strcpy(dev.msg, "sucess");
    }else{
        dev.state = -1;
        strcpy(dev.msg, "failed");
    }
    len = joylink_packet_sub_auth_rsp(&dev, data);
    log_info("rsp data:%s:len:%d", data + 4, len);
    int en_len = 0;

    en_len = joylink_encrypt_lan_basic(_g_pdev->send_buff, JL_MAX_PACKET_LEN,
            ET_NOTHING, PT_SUB_AUTH,
            NULL, (const uint8_t*)data, len);
    if(en_len > 0 && en_len < JL_MAX_PACKET_LEN){
        ret = sendto(_g_pdev->lan_socket, _g_pdev->send_buff, en_len, 0,
                (SOCKADDR*)sin_recv, addrlen);
        if(ret < 0){
            log_error("send error ret:%d", ret);
        }else{
            log_info("send len:%d",ret);
        }
    }else{
        log_error("packet error ret:%d", ret);
    }

    return E_RET_OK;
}

/**
 * brief: 
 *
 * @Param: src
 * @Param: src_len
 * @Param: sin_recv
 * @Param: addrlen
 *
 * @Returns: 
 */
E_JLRetCode_t 
joylink_proc_lan_sub_script_ctrl(uint8_t *src, int src_len,
        struct sockaddr_in *sin_recv, socklen_t addrlen)
{
    int ret = -1;
    int len = 0;
    int en_len = 0;
    int offset = 0;
    int snapshot_len = 0;
    char *ss = NULL;
    char data[JL_MAX_PACKET_LEN] = {0};
    JLContrl_t ctr;
    JLDevInfo_t dev;

    bzero(&ctr, sizeof(ctr));
    bzero(&dev, sizeof(dev));

    memcpy(&ctr.timestamp, src + offset, 4);
    offset += 4;
    memcpy(&ctr.biz_code, src + offset, 4);
    offset += 4;
    memcpy(&ctr.serial, src + offset, 4);
    offset += 4;
    memcpy(&ctr.feedid, src + offset, 32);
    offset += 32;

    if(E_RET_OK != joylink_dev_sub_get_by_feedid(ctr.feedid, &dev)){
        return  E_RET_ERROR;
    }
    ret =  joylink_dencrypt_sub_dev_data(src + offset, src_len - offset, 
            (uint8_t *)dev.jlp.localkey, ET_ACCESSKEYAES,
            (uint8_t *)data, sizeof(data));

    if(ret > 0){
        joylink_dev_sub_ctrl((const char *)data, ret, ctr.feedid);
    }
    ss = joylink_dev_sub_get_snap_shot(ctr.feedid, &snapshot_len);

    /**
     * 1 Encypt sub dev snapshot only with sub dev localkey.
     */
    if(ss != NULL){
        en_len = joylink_encrypt_sub_dev_data(
                (uint8_t*)(data + 44), 
                sizeof(data) - 44, 
                ET_ACCESSKEYAES, 
                (uint8_t*)dev.jlp.localkey, 
                (uint8_t*)ss, 
                snapshot_len);
    }

    ctr.biz_code = 102;

    ctr.timestamp = xTaskGetTickCount();//time(NULL);
    offset = 0;
    memcpy(data, &ctr.timestamp, 4);
    offset += 4; 
    memcpy(data, &ctr.biz_code, 4);
    offset += 4;
    memcpy(data, &ctr.serial, 4);
    offset += 4;
    memcpy(data, &ctr.feedid, 32);
    offset += 32;

    len = en_len + offset;

    /**
     * 2 Encypt all rsp data with GW localkey. 
     */
    en_len = 0;
    en_len = joylink_encrypt_lan_basic(_g_pdev->send_buff, JL_MAX_PACKET_LEN,
            ET_ACCESSKEYAES, PT_SUB_LAN_SCRIPT,
            (uint8_t*)_g_pdev->jlp.localkey, (const uint8_t*)data, len);

    if(en_len > 0 && en_len < JL_MAX_PACKET_LEN){ 
        ret = sendto(_g_pdev->lan_socket, _g_pdev->send_buff, en_len, 0,
                (SOCKADDR*)sin_recv, addrlen);

        if(ret < 0){
            log_error("send error ret:%d", ret);
        }else{
            log_info("rsp send to ip:%s len:%d", _g_pdev->jlp.ip, ret);
        }
    }else{
        log_error("packet error ret:%d", ret);
    }

    return E_RET_OK;
}

/**
 * brief: 
 *
 * @Returns: 
 */
int
joylink_packet_server_sub_hb_req(void)
{
    int i;
    int count = 0;
    int scan_type = 0;
    int offset = 0;
    char *rsp_data = NULL;
    int len, en_len = 0;

    JLDevInfo_t *alldevinfo = NULL;

    JLSubHearBeat_t dev_heartbeat;
    bzero(&dev_heartbeat, sizeof(dev_heartbeat));    
    
    dev_heartbeat.verion = _g_pdev->jlp.version;
    dev_heartbeat.timestamp = xTaskGetTickCount();//time(NULL);

    alldevinfo = joylink_dev_sub_devs_get(&count, scan_type);
    
    if(count <= 0){
        goto RET;
    }
    len = sizeof(JLSubDevInfo_t) * count + sizeof(JLSubHearBeat_t);
    rsp_data = (char *)malloc(len);
    if(NULL == rsp_data){
        goto RET;
    }
    
    dev_heartbeat.num = count;
    memcpy(rsp_data + offset, &dev_heartbeat, 10);
    offset += 10;

    for(i = 0; i < count ; i++){
        memcpy(rsp_data + offset, &alldevinfo[i].jlp.version, 2); 
        offset += 2;
        /*no rssi only offset + 2*/
        offset += 2;
        memcpy(rsp_data + offset, alldevinfo[i].jlp.feedid, 32); 
        offset += 32;
        memcpy(rsp_data + offset, &alldevinfo[i].state, 1); 
        offset += 1;
    }
    
    en_len = joylink_encypt_server_rsp(
            _g_pdev->send_buff, 
            JL_MAX_PACKET_LEN, PT_SUB_HB, 
            _g_pdev->jlp.sessionKey, 
            (uint8_t*)rsp_data, 
            offset
            );

    if(en_len > JL_MAX_PACKET_LEN){
        log_error("packet data is too long than JL_MAX_PACKET_LEN");
        en_len = 0;
    }
RET:
    if(alldevinfo != NULL){
        free(alldevinfo);
    }
    if(rsp_data != NULL){
        free(rsp_data);
    }

    return en_len;
}

/**
 * brief: 
 *
 * @Param: recPainText
 * @Param: ctr
 * @Param: payloadlen
 *
 * @Returns: 
 */
int 
joylink_subdev_script_ctrl(uint8_t* recPainText, JLSubContrl_t *ctr, unsigned short payloadlen)
{
    if(NULL == recPainText || NULL == ctr){
        return -1;
    }
    int ret = -1;
    char data[JL_MAX_PACKET_LEN] = {0};
    int offset = 0;
    JLDevInfo_t dev;
    bzero(&dev, sizeof(dev));

    memcpy(ctr, recPainText, 44);
    offset += 44;
    log_info("bcode:%d:serial:%d:feeid:%s", 
            ctr->biz_code, ctr->serial, ctr->feedid);

    switch(ctr->biz_code){
        case JL_BZCODE_GET_SNAPSHOT:
        case JL_BZCODE_CTRL:
            if(E_RET_OK != joylink_dev_sub_get_by_feedid(ctr->feedid, &dev)){
                return  E_RET_ERROR;
            }
            ret =  joylink_dencrypt_sub_dev_data(recPainText + offset, payloadlen - offset, 
                (uint8_t *)dev.jlp.accesskey, ET_ACCESSKEYAES,
                (uint8_t *)data, sizeof(data));
            if(ret > 0){
                ctr->cmd = (unsigned char*)malloc(sizeof(unsigned char) * (ret + 1));
                bzero(ctr->cmd, (ret + 1));
                memcpy(ctr->cmd, (const char *)data, ret);
                ctr->cmd[ret] = 0;
                log_info("feedid:%s:cmd:%s:ret:%d\n", ctr->feedid, ctr->cmd, ret);
            }else{
                log_error("cmd dencrypt error:%s:%s\n", ctr->feedid, recPainText + 44); 
            }

            ret = joylink_dev_sub_ctrl((const char*)ctr->cmd, ret, ctr->feedid);
            break;
        default:
            log_error("unknow bizcode!\n");
            break;
    }

    if(ctr->cmd != NULL){
        free(ctr->cmd);
    }

    return ret;
}

/**
 * brief: 
 *
 * @Param: data
 * @Param: max
 * @Param: ctrl
 *
 * @Returns: 
 */
int 
joylink_packet_subdev_script_ctrl_rsp(char *data, int max, JLSubContrl_t *ctrl)
{
    if(NULL == data || NULL == ctrl){
        return -1;
    }
    int ret = -1;
    int offset = 0;
    JLDevInfo_t dev;
    bzero(&dev, sizeof(dev));
    int snapshot_len = 0;
    char *ss;
    int time_v = xTaskGetTickCount();//time(NULL);

    memcpy(data + offset, &time_v, 4);
    offset += 4;
    
    if(ctrl->biz_code == JL_BZCODE_GET_SNAPSHOT || ctrl->biz_code == JL_BZCODE_CTRL){
        if(ctrl->biz_code == JL_BZCODE_GET_SNAPSHOT)
            ctrl->biz_code = 104;
        if(ctrl->biz_code == JL_BZCODE_CTRL)
            ctrl->biz_code = 102;
        memcpy(data + offset, &ctrl->biz_code , 4);
        offset += 4;
        memcpy(data + offset, &ctrl->serial, 4);
        offset += 4;
        memcpy(data + offset, ctrl->feedid, 32);
        offset += 32;

        if(E_RET_OK != joylink_dev_sub_get_by_feedid(ctrl->feedid, &dev)){
            return  E_RET_ERROR;
        }
        
        ss = joylink_dev_sub_get_snap_shot(ctrl->feedid, &snapshot_len);
        if(ss != NULL){
            ret = joylink_encrypt_sub_dev_data(
                    (uint8_t*)(data + 44), 
                    sizeof(data) - 44,
                    ET_ACCESSKEYAES, 
                    (uint8_t*)dev.jlp.accesskey, 
                    (uint8_t*)ss, 
                    snapshot_len);
            free(ss);
        }

        if(ret > max){
            log_error("data of length is too large!\n");
            return 0;
        }
        log_debug("XXXX :len:%d\n", ret);

        return ret > 0 ? ret + 44 : 0;
    }else{
        log_error("unknow biz_code\n");
        return 0;
    }
}

/**
 * brief: 
 *
 * @Param: recPainText
 * @Param: unbind
 *
 * @Returns: 
 */
int
joylink_subdev_unbind(uint8_t* recPainText, JLSubUnbind_t* unbind)
{
    if(NULL == recPainText && NULL == unbind){
        return -1;
    }
    int i = 0;
    int offset = 0;

    memcpy(unbind, recPainText, 14);
    offset += 14;
    log_info("timestamp:%dbcode:%d:serial:%d:num:%d:", 
        unbind->timestamp, unbind->biz_code, 
        unbind->serial, unbind->num);

    if(unbind->biz_code == JL_BZCODE_UNBIND){
        unbind->info = (JLSubDevInfo_t*)malloc(sizeof(JLSubDevInfo_t) * (unbind->num));
        for(i = 0; i < unbind->num; i++){
            memcpy(&unbind->info[i].version, (recPainText + offset), 2);
            offset += 2;
            memcpy(&unbind->info[i].rssi, (recPainText + offset), 2);
            offset += 2;
            memcpy(unbind->info[i].feedid, (recPainText + offset), 32);
            offset += 32;
            memcpy(&unbind->info[i].state, (recPainText + offset), 1);
            offset += 1;
            log_info("the %d subdev:version:%d:rssi:%d:feedid:%s:state:%d\n", 
                i, unbind->info[i].version, unbind->info[i].rssi,
                unbind->info[i].feedid, unbind->info[i].state);

            joylink_dev_sub_unbind((const char*)unbind->info[i].feedid);
        }
    }
    return 0;
}

/**
 * brief: 
 *
 * @Param: data
 * @Param: max
 * @Param: unbind
 *
 * @Returns: 
 */
int
joylink_packet_subdev_unbind_rsp(char* data, int max, JLSubUnbind_t* unbind)
{
    if(NULL == data || NULL == unbind){
        return -1;
    }
    int i = 0;
    int time_v = xTaskGetTickCount();//time(NULL);
    int offset = 0;
    
    if(unbind->biz_code == JL_BZCODE_UNBIND){
        memcpy(data + offset, &time_v, 4);
        offset += 4;

        unbind->biz_code = 160;
        memcpy(data + offset, &unbind->biz_code , 4);
        offset += 4;

        memcpy(data + offset, &unbind->serial, 4);
        offset += 4;
        
        memcpy(data + offset, &unbind->num, 2);
        offset += 2;

        for(i = 0; i < unbind->num; i++){
            memcpy(data + offset, &unbind->info[i].version, 2);
            offset += 2;
            memcpy(data + offset, &unbind->info[i].rssi, 2);
            offset += 2;
            memcpy(data + offset, unbind->info[i].feedid, 32);
            offset += 32;
            memcpy(data + offset, &unbind->info[i].state, 1);
            offset += 1;
        }
    }else{
        log_error("unknow biz_code\n");
        
    }

    return offset;
}
