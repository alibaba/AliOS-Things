#include <stdio.h>
#include "joylink_packets.h"
#include "joylink_crypt.h"
#include "joylink_auth_uECC.h"
#include "joylink_auth_crc.h"
#include "joylink.h"
#include "joylink_dev.h"
#include "joylink_utils.h"

#ifdef __LINUX_UB2__
#include <stdint.h>
#endif

#include <unistd.h>

/**
 * brief: 
 *
 * @Param: scan
 *
 * @Returns: 
 */
int 
joylink_packet_lan_scan_rsp(DevScan_t *scan)
{
    int len = 0;
    if(!strcmp(_g_pdev->idt.rand, "")){
        /*1 signature the app_random*/
        /*2 get a dev random*/
        int ran[8];
        int i;
        ran[0] = joylink_dev_get_random();
        ran[1] = xTaskGetTickCount();//time(NULL);
        for(i = 2; i < 8; i++){
           ran[i] = ran[i-1] ^ ran[i-0]; 
        }

        joylink_util_byte2hexstr((uint8_t *)&ran[0], 
                        sizeof(ran), 
                        (uint8_t *)(&_g_pdev->idt.rand[0]), 
                        sizeof(_g_pdev->idt.rand) - 1);
    }

    printf("vvvvvvvvvvvvvv  %s\n", _g_pdev->idt.rand);
    char * scan_rsp_data = joylink_package_scan("scan ok", 
            0,
            scan->type,
            _g_pdev);

    char *pout = NULL;
    int max_out = 0;

    if(scan_rsp_data != NULL){
        _g_pdev->payload_total = strlen(scan_rsp_data); 
    
        if(_g_pdev->payload_total < JL_MAX_CUT_PACKET_LEN){
            pout = (char*)_g_pdev->send_buff;
            max_out = JL_MAX_PACKET_LEN;
        }else{
            _g_pdev->send_p = (char *)malloc(_g_pdev->payload_total + JL_MAX_AES_EXTERN);
            if(_g_pdev->send_p == NULL){
                goto RET;
            }

            pout = _g_pdev->send_p;
            max_out = _g_pdev->payload_total + JL_MAX_AES_EXTERN;
        }
        log_debug("new scan rsp data:%s", scan_rsp_data);

        len = joylink_encrypt_lan_basic(
                (uint8_t*)pout, 
                max_out, ET_NOTHING, 
                PT_SCAN, (uint8_t*)1, 
                (uint8_t*)scan_rsp_data, 
                strlen(scan_rsp_data));

    }

RET:
    if(scan_rsp_data != NULL){
        free(scan_rsp_data);
    }

	return len;
}

/**
 * brief: 
 *
 * @Returns: 
 */
int 
joylink_packet_lan_write_key_rsp(int code, char *msg)
{
    char data[100] = {0};
    sprintf(data, "{\"code\":%d, \"msg\":\"%s\"}", code, msg);

	int len = joylink_encrypt_lan_basic(
            _g_pdev->send_buff, 
            JL_MAX_PACKET_LEN, ET_NOTHING, 
            PT_WRITE_ACCESSKEY, 
            NULL, 
            (uint8_t*)&data, 
            strlen(data));

	return len;
}

/**
 * brief: 
 *
 * @Returns: 
 */
int 
joylink_packet_server_auth_rsp(void)
{
	JLAuth_t auth;
    bzero(&auth, sizeof(auth));

	auth.random_unm = 1;
    auth.timestamp = xTaskGetTickCount();//time(NULL);
    
    log_debug("accesskey key:%s",
            _g_pdev->jlp.accesskey);

	int len = joylink_encypt_server_rsp(
            _g_pdev->send_buff,
            JL_MAX_PACKET_LEN, PT_AUTH, 
            (uint8_t*)_g_pdev->jlp.accesskey, 
            (uint8_t*)&auth, sizeof(auth));

	return len;
}

/**
 * brief: 
 *
 * @Returns: 
 */
int 
joylink_packet_server_hb_req(void)
{
	JLHearBeat_t heartbeat;
    bzero(&heartbeat, sizeof(heartbeat));

	heartbeat.rssi = 1;
	heartbeat.verion = _g_pdev->jlp.version;
	heartbeat.timestamp = xTaskGetTickCount();//time(NULL);

	int len = joylink_encypt_server_rsp(
            _g_pdev->send_buff, 
            JL_MAX_PACKET_LEN, PT_BEAT, 
            _g_pdev->jlp.sessionKey, 
            (uint8_t*)&heartbeat, 
            sizeof(JLHearBeat_t));

	return len;
}

/**
 * brief: 
 *
 * @Returns: 
 */
int 
joylink_packet_server_upload_req(void)
{
	JLDataUpload_t data = { 0 };
	data.timestamp = 10;

	int len = joylink_encypt_server_rsp(
            _g_pdev->send_buff, 
            JL_MAX_PACKET_LEN, PT_UPLOAD, 
            _g_pdev->jlp.sessionKey, 
            (uint8_t*)&data, 
            sizeof(data));

	return len;
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
joylink_packet_script_ctrl_rsp(char *data, int max, JLContrl_t *ctrl)
{
    if(NULL == data || NULL == ctrl){
        return -1;
    }
    int ret = -1;
    if(ctrl->biz_code == JL_BZCODE_GET_SNAPSHOT){
        ctrl->biz_code = 104;
    }else if(ctrl->biz_code == JL_BZCODE_CTRL){
        ctrl->biz_code = 102;
    }

    int time_v = xTaskGetTickCount();//time(NULL);
    memcpy(data, &time_v, 4);
    memcpy(data + 4, &ctrl->biz_code , 4);
    memcpy(data + 8, &ctrl->serial, 4);

    ret = joylink_dev_get_snap_shot(data + 12, JL_MAX_PACKET_LEN - 12);

    //printf("XXXX :len:%d:%s\n", ret, data+12);
   
    return ret > 0 ? ret + 12 : 0;
}

/**
 * brief: 
 *
 * @Param: otaOrder
 *
 * @Returns: 
 */
int
joylink_packet_server_ota_order_rsp(JLOtaOrder_t *otaOrder)
{
    int len = 0;
    char data[100] = {0};    
    time_t tt = xTaskGetTickCount();//time(NULL);
    bzero(_g_pdev->dev_detail, sizeof(_g_pdev->dev_detail));

    memcpy((char*)_g_pdev->dev_detail, &tt, 4);
    
    sprintf(data, "{\"code\":0,\"serial\":%d,\"msg\":\"success\"}", otaOrder->serial);
    memcpy((char*)_g_pdev->dev_detail + 4, data, strlen(data));

    len = joylink_encypt_server_rsp(_g_pdev->send_buff, JL_MAX_PACKET_LEN,
            PT_OTA_ORDER, (uint8_t*)_g_pdev->jlp.sessionKey, _g_pdev->dev_detail, strlen(data) + 4);

    return len;
}

/**
 * brief: 
 *
 * @Param: otaUpload
 *
 * @Returns: 
 */
int 
joylink_package_ota_upload_req(JLOtaUpload_t *otaUpload)
{    
    time_t tt = xTaskGetTickCount();//time(NULL);
    char * ota_upload_rsp_data = joylink_package_ota_upload(otaUpload);
    bzero(_g_pdev->dev_detail, sizeof(_g_pdev->dev_detail));
    bzero(_g_pdev->send_buff, sizeof(_g_pdev->send_buff));
    
    memcpy((char *)_g_pdev->dev_detail, &tt, 4);
    if(ota_upload_rsp_data != NULL){
        memcpy((char *)_g_pdev->dev_detail + 4, ota_upload_rsp_data, strlen(ota_upload_rsp_data));
        log_debug("new ota upload rsp data:%s", (char *)_g_pdev->dev_detail + 4);
    }

    int len = joylink_encypt_server_rsp(_g_pdev->send_buff, JL_MAX_PACKET_LEN, 
        PT_OTA_UPLOAD, (uint8_t*)_g_pdev->jlp.sessionKey, 
        _g_pdev->dev_detail, strlen(ota_upload_rsp_data) + 4);

    if(ota_upload_rsp_data != NULL){
        free(ota_upload_rsp_data);
        ota_upload_rsp_data = NULL;
    }

    return len;
}
