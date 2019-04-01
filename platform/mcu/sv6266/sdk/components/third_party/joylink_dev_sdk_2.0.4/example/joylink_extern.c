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
#include "joylink.h"
#include "joylink_packets.h"
#include "joylink_json.h"
#include "joylink_extern_json.h"
#include "joylink_auth_crc.h"

#ifdef __MTK_7687__
#include "mtk_ctrl.h"
#include "joylink_porting_layer.h"
#endif

#ifdef __ICOMM_6166__
#include "fsal.h"
#include "wifi_api.h"
#include "mac_cfg.h"
#endif

//icomm joylink conf
extern spiffs* fs_handle;
typedef struct {
	char feedid[JL_MAX_FEEDID_LEN];
	char accesskey[33];
    char localkey[33];
}JLPConf_t;




extern int joylink_parse_jlp(JLPInfo_t *jlp, char * pMsg);

typedef struct _light_manage_{
	int conn_st;	
    JLPInfo_t jlp;
    jl2_d_idt_t idt;
	LightCtrl_t lightCtrl;
}LightManage_t;

LightManage_t _g_lightMgr = {
	.conn_st = -1,	
	.jlp.mac= "00:23:45:67:89:12",
    .jlp.devtype = E_JLDEV_TYPE_NORMAL,
    .jlp.version = 1,
    .jlp.uuid = "673694",	//87B580
    .jlp.lancon = E_LAN_CTRL_ENABLE,
    .jlp.cmd_tran_type = E_CMD_TYPE_LUA_SCRIPT,
    .jlp.accesskey = "",//78cc4064756c3d2c82d709fa2e51b443
    .jlp.localkey = "",//25cbd2e832f89c3951fcc34c8bfed11c
    .jlp.feedid = "",//152042399903816876

    .jlp.joylink_server = "sbdevicegw.jd.com",
    .jlp.server_port = 2002,

    .idt.type = 0,
    .idt.cloud_pub_key = "0214496DDFDE2558CA4BFAFABFDF43B982EF3D634719106ADEBBA085318119986C", //0241A5170B0299294D39B0676D3D85BE732EE3EC664A0DCFA43C871A0D85192371
    .idt.pub_key = "01234567890123456789012345678901",
    .idt.sig = "01234567890123456789012345678901",
    .idt.f_sig = "01234567890123456789012345678901",
    .idt.f_pub_key = "01234567890123456789012345678901",
	
	.lightCtrl.cmd = LIGHT_CMD_NONE,
    .lightCtrl.para_value = LIGHT_CTRL_OFF
};

LightManage_t *_g_pLightMgr = &_g_lightMgr;

int 
joylink_util_cut_ip_port(const char *ipport, char *out_ip, int *out_port);

/**
 * brief: 
 *
 * @Param:
 *
 * @Returns: 
 */
int
joylink_dev_get_random()
{
    /**
     *FIXME:must to do
     */
    return xTaskGetTickCount();
}

/**
 * brief: 
 *
 * @Returns: 
 */
E_JLRetCode_t
joylink_dev_is_net_ok()
{
    /**
     *FIXME:must to do
     */
    int ret = E_RET_TRUE;
    
    if(get_wifi_status() == 1)
        ret = E_RET_TRUE;
    else
        ret = E_RET_FAIL;
        
    return ret;
}

/**
 * brief: 
 *
 * @Param: st
 *
 * @Returns: 
 */
E_JLRetCode_t
joylink_dev_set_connect_st(int st)
{
    /**
     *FIXME:must to do
     */
    char buff[64] = {0};
    int ret = 0;
    sprintf(buff, "{\"conn_status\":\"%d\"}", st);
	log_info("--set_connect_st:%s\n", buff);
	
	// Save the state.
	_g_pLightMgr->conn_st = st;
	
    return ret;
}

/**
 * brief: 
 *
 * @Param: jlp
 *
 * @Returns: 
 */
E_JLRetCode_t
joylink_dev_set_attr_jlp(JLPInfo_t *jlp)
{
    if(NULL == jlp){
        return E_RET_ERROR;
    }
    /**
     *FIXME:must to do
     *Must save jlp info to flash 
     */
    int ret = E_RET_ERROR;

#if defined(__MTK_7687__)
	char buff[256];

	JLPInfo_t *pJLPInfo = &(_g_pLightMgr->jlp);
	
    bzero(buff, sizeof(buff));
    if(strlen(jlp->feedid)){
		strcpy(pJLPInfo->feedid, jlp->feedid);		
        sprintf(buff, "{\"feedid\":\"%s\"}", jlp->feedid);
        log_debug("--set buff:%s", buff);		
		if(joylink_set_feedid(buff)){
			log_error("set error");
			goto RET;
		}
    }
	
	bzero(buff, sizeof(buff));
    if(strlen(jlp->accesskey)){
		strcpy(pJLPInfo->accesskey, jlp->accesskey);
        sprintf(buff, "{\"accesskey\":\"%s\"}", jlp->accesskey);
        log_debug("--set buff:%s", buff);
		if(joylink_set_accesskey(buff)){
			log_error("set error");
			goto RET;
		}
    }

    bzero(buff, sizeof(buff));
    if(strlen(jlp->localkey)){
		strcpy(pJLPInfo->localkey, jlp->localkey);
        sprintf(buff, "{\"localkey\":\"%s\"}", jlp->localkey);
        log_debug("--set buff:%s", buff);
		if(joylink_set_localkey(buff)){
			log_error("set error");
			goto RET;
		}
    }

    bzero(buff, sizeof(buff));
    if(strlen(jlp->joylink_server)){
		strcpy(pJLPInfo->joylink_server, jlp->joylink_server);
		pJLPInfo->server_port = jlp->server_port;
		
        sprintf(buff, "%s:%d", jlp->joylink_server, jlp->server_port);
        log_debug("--set buff:%s", buff);
		if(joylink_set_server_info(buff)){
			log_error("set error");
			goto RET;
		}
    }

	pJLPInfo->version = jlp->version;
	
    bzero(buff, sizeof(buff));
    sprintf(buff, "{\"version\":%d}", jlp->version);
    log_debug("--set buff:%s", buff);	
	if(joylink_set_version(buff)){
		log_error("set error");
		goto RET;
	}
#elif defined(__ICOMM_6166__)
    //save jip to flash
#if 1
    memcpy(&_g_pLightMgr->jlp, jlp, sizeof(JLPInfo_t));

    if( strlen(jlp->feedid) > 0 && strlen(jlp->accesskey) > 0 && strlen(jlp->localkey) > 0)
    {
        SSV_FILE fd = FS_open(fs_handle, "joylink.conf", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
        if (fd >= 0) 
        {
            JLPConf_t jlpConf = {"","",""};
            memcpy(jlpConf.feedid, jlp->feedid, JL_MAX_FEEDID_LEN);
            memcpy(jlpConf.accesskey, jlp->accesskey, 33);
            memcpy(jlpConf.localkey, jlp->localkey, 33);
           
            if( FS_write(fs_handle, fd, &jlpConf, sizeof(JLPConf_t)) >= 0 )
            {
                log_notice("joylink config write success\n");   
            }
            else
            {
                log_notice("joylink config write fails\n");   
            }

            printf("Save joylink config into flash!!!\n");
            FS_close(fs_handle, fd);
        }
    }
    else
    {
        log_notice("feedid is empty!\n");
    }
#endif

#else
    memcpy(&_g_pLightMgr->jlp, jlp, sizeof(JLPInfo_t));
#endif	
    ret = E_RET_OK;

#if defined(__MTK_7687__)
RET:
#endif

    return ret;
}

/**
 * brief: 
 *
 * @Param: jlp
 *
 * @Returns: 
 */
E_JLRetCode_t
joylink_dev_get_idt(jl2_d_idt_t *pidt)
{
    if(NULL == pidt){
        return E_RET_ERROR; 
    }

    strcpy(pidt->pub_key, _g_pLightMgr->idt.pub_key);
    strcpy(pidt->sig, _g_pLightMgr->idt.sig);
    strcpy(pidt->rand, _g_pLightMgr->idt.rand);
    strcpy(pidt->f_sig, _g_pLightMgr->idt.f_sig);
    strcpy(pidt->f_pub_key, _g_pLightMgr->idt.f_pub_key);
    strcpy(pidt->cloud_pub_key, _g_pLightMgr->idt.cloud_pub_key);

    return E_RET_OK;
}

/**
 * brief: 
 *
 * @Param: jlp
 *
 * @Returns: 
 */
E_JLRetCode_t
joylink_dev_get_jlp_info(JLPInfo_t *jlp)
{
    if(NULL == jlp){
        return -1;
    }
    /**
     *FIXME:must to do
     *Must get jlp info from flash 
     */
    int ret = E_RET_OK;
    
    /**
     *FIXME: not use memcpy, it will cover jlp->pubkeyS
     *   memcpy(jlp, &_g_pLightMgr->jlp, sizeof(JLPInfo_t));
     */
#if defined(__MTK_7687__)
	char buff[256];

    bzero(buff, sizeof(buff));
	if(!joylink_get_accesskey(buff, sizeof(buff))){
		log_debug("-->accesskey:%s\n", buff);            
		joylink_parse_jlp(jlp, buff);
	}else{
		log_error("get accesskey error");
	}
	
	bzero(buff, sizeof(buff));
	if(!joylink_get_localkey(buff, sizeof(buff))){
		log_debug("-->localkey:%s\n", buff);
		joylink_parse_jlp(jlp, buff);
	}else{
		log_error("get localkey error");
	}
	
	bzero(buff, sizeof(buff));
	if(!joylink_get_feedid(buff, sizeof(buff))){
		log_debug("-->feedid:%s\n", buff);
		joylink_parse_jlp(jlp, buff);
	}else{
		log_error("get feedid error");
	}
	
	bzero(buff, sizeof(buff));
	if(!joylink_get_server_info(buff, sizeof(buff))){
		log_info("-->server_info:%s\n", buff);
		joylink_util_cut_ip_port(buff, jlp->joylink_server, &jlp->server_port);
	}else{
		log_error("get server_info error");
	}
	
	bzero(buff, sizeof(buff));
	if(!joylink_get_version(buff, sizeof(buff))){
		log_info("-->version:%s\n", buff);
		joylink_parse_jlp(jlp, buff);
	}else{
		log_error("get version error");
	}		
#else

#if 1
	jlp->devtype = _g_pLightMgr->jlp.devtype;
    jlp->lancon = _g_pLightMgr->jlp.lancon;
    jlp->cmd_tran_type = _g_pLightMgr->jlp.cmd_tran_type;
    //strcpy(jlp->mac, _g_pLightMgr->jlp.mac);

    unsigned char mac1[6] = {0};
    void *cfg_handle = wifi_cfg_init();
    wifi_cfg_get_addr1(cfg_handle, (char*)mac1);
    wifi_cfg_deinit(cfg_handle);
    
    sprintf(jlp->mac, "%02X:%02X:%02X:%02X:%02X:%02X", mac1[0], mac1[1], mac1[2], mac1[3], mac1[4], mac1[5]);
    
    strcpy(jlp->uuid, _g_pLightMgr->jlp.uuid);
   
    JLPConf_t jlpConf = {"", "", ""};
    SSV_FILE fd = FS_open(fs_handle, "joylink.conf", SPIFFS_RDWR, 0);
    if (fd < 0) 
    {
        log_notice("joylink config open error\n");
        return ret;
    }
    
    if (FS_read(fs_handle, fd, &jlpConf, sizeof(JLPConf_t)) < 0) 
    {
        log_notice("joylink config read error\n");
    }
    else
    {
        log_notice("Load joylink config from flash!!!\n");
        memcpy(jlp->feedid, jlpConf.feedid, JL_MAX_FEEDID_LEN);
        memcpy(jlp->accesskey, jlpConf.accesskey, JL_MAX_FEEDID_LEN);
        memcpy(jlp->localkey, jlpConf.localkey, JL_MAX_FEEDID_LEN);
        
        memcpy(jlp->joylink_server, "sbdevicegw.jd.com", strlen("sbdevicegw.jd.com"));
        jlp->server_port = 2002;
        
        log_notice("-->feedid:%s\n", jlp->feedid);
        log_notice("-->.accesskey :%s\n", jlp->accesskey );
        log_notice("-->localkey:%s\n", jlp->localkey);
              
    }
    FS_close(fs_handle, fd);
   
#endif

#endif

	jlp->devtype = _g_pLightMgr->jlp.devtype;
    jlp->lancon = _g_pLightMgr->jlp.lancon;
    jlp->cmd_tran_type = _g_pLightMgr->jlp.cmd_tran_type;
    strcpy(jlp->mac, _g_pLightMgr->jlp.mac);
    strcpy(jlp->uuid, _g_pLightMgr->jlp.uuid);

    return ret;
}

/**
 * brief: 
 *
 * @Param: out_modelcode
 * @Param: out_max
 *
 * @Returns: 
 */
int
joylink_dev_get_modelcode(char *out_modelcode, int32_t out_max)
{
    if(NULL == out_modelcode || out_max < 0){
        return 0;
    }
    /**
     *FIXME:must to do
     */
    int len = 0;
	LightCtrl_t *pCtrl = &(_g_pLightMgr->lightCtrl);
	
    char *packet_data =  joylink_dev_modelcode_info(0, pCtrl);
    if(NULL !=  packet_data){
        len = strlen(packet_data);
        printf("------>%s:len:%d\n", packet_data, len);
        if(len < out_max){
            memcpy(out_modelcode, packet_data, len); 
        }else{
            len = 0;
        }
    }

    if(NULL !=  packet_data){
        free(packet_data);
    }

    return len;
}

/**
 * brief: 
 *
 * @Param: out_snap
 * @Param: out_max
 *
 * @Returns: 
 */
int
joylink_dev_get_snap_shot(char *out_snap, int32_t out_max)
{
    if(NULL == out_snap || out_max < 0){
        return 0;
    }
    /**
     *FIXME:must to do
     */
    int len = 0;
	LightCtrl_t *pCtrl = &(_g_pLightMgr->lightCtrl);
	
    char *packet_data =  joylink_dev_package_info(0, pCtrl);
    if(NULL !=  packet_data){
        len = strlen(packet_data);
 //       printf("------>%s:len:%d\n", packet_data, len);
        if(len < out_max){
            memcpy(out_snap, packet_data, len); 
        }else{
            len = 0;
        }
    }

    if(NULL !=  packet_data){
        free(packet_data);
    }

    return len;
}

/**
 * brief: 
 *
 * @Param: out_snap
 * @Param: out_max
 * @Param: code
 * @Param: feedid
 *
 * @Returns: 
 */
int
joylink_dev_get_json_snap_shot(char *out_snap, int32_t out_max, int code, char *feedid)
{
    /**
     *FIXME:must to do
     */
    sprintf(out_snap, "{\"code\":%d, \"feedid\":\"%s\"}", code, feedid);

    return strlen(out_snap);
}

/**
 * brief: 
 *
 * @Param: json_cmd
 *
 * @Returns: 
 */
E_JLRetCode_t 
joylink_dev_lan_json_ctrl(const char *json_cmd)
{
    /**
     *FIXME:must to do
     */
    log_debug("json ctrl:%s", json_cmd);

    return E_RET_OK;
}

/**
 * brief: 
 *
 * @Param: src
 * @Param: src_len
 * @Param: ctr
 * @Param: from_server
 *
 * @Returns: 
 */
E_JLRetCode_t 
joylink_dev_script_ctrl(const char *src, int src_len, JLContrl_t *ctr, int from_server)
{
    if(NULL == src || NULL == ctr){
        return -1;
    }
    /**
     *FIXME:must to do
     */
    int ret = -1;
    ctr->biz_code = (int)(*((int *)(src + 4)));
    ctr->serial = (int)(*((int *)(src +8)));

	LightCtrl_t *pCtrl = &(_g_pLightMgr->lightCtrl);
	
    time_t tt = xTaskGetTickCount();//time(NULL);
    log_info("bcode:%d:server:%d:time:%ld", ctr->biz_code, from_server,(long)tt);

    if(ctr->biz_code == JL_BZCODE_GET_SNAPSHOT){
        /*
         *Nothing to do!
         */
        ret = 0;
    }else if(ctr->biz_code == JL_BZCODE_CTRL){
        joylink_dev_parse_ctrl(pCtrl, src + 12);
		
#ifdef __MTK_7687__
		gpio_ligt_ctrl(pCtrl->para_value);
#endif
		if(pCtrl->cmd == LIGHT_CMD_POWER){      
			if(pCtrl->para_value == LIGHT_CTRL_ON){ //Open light.
				log_info("LIGHT_CTRL--[%d]--\n", pCtrl->para_value);
                            printf("Light Open!!!\n");
			}else{							 //Close light.
				log_info("LIGHT_CTRL--[%d]--\n", pCtrl->para_value);
                            printf("Light Close!!!\n");
			}
		}
		
        ret = 0;
    }else{
        log_error("unKown biz_code:%d", ctr->biz_code);
    }
    
    return ret;
}

/**
 * brief: 
 *
 * @Param: otaOrder
 *
 * @Returns: 
 */
E_JLRetCode_t
joylink_dev_ota(JLOtaOrder_t *otaOrder)
{
    if(NULL == otaOrder){
        return -1;
    }
    int ret = E_RET_OK;

    log_debug("serial:%d | feedid:%s | productuuid:%s | version:%d | versionname:%s | crc32:%d | url:%s\n",
     otaOrder->serial, otaOrder->feedid, otaOrder->productuuid, otaOrder->version, 
     otaOrder->versionname, otaOrder->crc32, otaOrder->url);

#ifdef __MTK_7687__
    strcpy(_g_fota_ctx.download_url, (const char*)otaOrder->url);
	//strcpy(_g_fota_ctx.download_url, (const char*)"http://192.168.1.96/mt7687_iot_sdk.bin");
    _g_fota_ctx.crc32 = otaOrder->crc32;
	joylink_fota_download_package();
#else
    char cmd_download[1024] = {0};
    char *folder = "/home/yn/workspace/";
    sprintf(cmd_download, "wget -b %s -P %s", otaOrder->url, folder);
    system(cmd_download);
#endif

    return ret;
}

/**
 * brief: 
 */
void
joylink_dev_ota_status_upload()
{
    JLOtaUpload_t otaUpload;    
    strcpy(otaUpload.feedid, _g_pdev->jlp.feedid);
    strcpy(otaUpload.productuuid, _g_pdev->jlp.uuid);	

#ifdef __MTK_7687__
    otaUpload.status = _g_fota_ctx.upgrade_status;
    otaUpload.progress = _g_fota_ctx.progress;
    strcpy(otaUpload.status_desc, joylink_fota_get_status_desc(FOTA_ERROR_CODE_NONE));
#endif
    joylink_server_ota_status_upload_req(&otaUpload);   
}


//brief: remove joylink conf in flash
void joylink_remove_conf()
{
    SSV_FILE fd = FS_remove(fs_handle, "joylink.conf");
}
