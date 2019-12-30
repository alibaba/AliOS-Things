
//standard libary include
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <ssv_config.h>
//#include <ssv_lib.h>

#include "soc_types.h"
#include "atcmd.h"
#include "error.h"

#include "Cabrio-conf.h"

#include "atcmdlib.h"
#include "atcmd_icomm.h"  //these are all icomm-semi only debug command!!

#include "aos/cli.h"
//#include "../../common/drv/flash/drv_flash.h"	//FIXEDME for -I definition!!
//#include "drv_flash.h"	//FIXEDME for -I definition!!
//#include "drv_timer.h"    

#include "ieee80211_mgmt.h"
#include "softap_func.h"
#include "gpio/drv_gpio.h"
#include "pwm/drv_pwm.h"
#include "rf/rf_api.h"
#include "wifi_api.h"
#include "mac_cfg.h"
#include "sysconf_api.h"

#if defined(SUPPORT_PARTITION_CFG_TABLE)
#include "atcmd_cfg.h"
#endif
#include "drv_wdt.h"
#include "sw_version.h"
        
#ifdef SMARTCONFIG_EN
#include "joylink/joylink_main.h"
#include "airkiss/airkiss_main.h"
#endif

#ifdef OTA_EN
#include "ota_api.h"
#endif

static uint8_t bTXRun = 0;
static uint8_t ghttprun = 0;
static char *buffer_in_dynamic = NULL;
static char *buffer_out_dynamic = NULL;
extern const at_cmd_info atcmd_homekit_tbl[];
extern at_cmd_info atcmd_info_tbl[];

//static char atcmd_buffer[CLI_BUFFER_SIZE+1] = {0};
extern struct st_rf_table ssv_rf_table;

extern IEEE80211STATUS gwifistatus;
int bEfuseEnableWrite = 0;
    
#define RATE_NUM 39
static char *dataRateMapping[RATE_NUM] = 
{
"11b 1M",
"11b 2M",
"11b 5.5M",
"11b 11M",
"11b 2M SP",
"11b 5.5M SP",
"11b 11M SP",
"NON-HT 6M",
"NON-HT 9M",
"NON-HT 12M",
"NON-HT 18M",
"NON-HT 24M",
"NON-HT 36M",
"NON-HT 48M",
"NON-HT 54M",
"HT-MM MCS0",
"HT-MM MCS1",
"HT-MM MCS2",
"HT-MM MCS3",
"HT-MM MCS4",
"HT-MM MCS5",
"HT-MM MCS6",
"HT-MM MCS7",
"HT-MM SGI MCS0",
"HT-MM SGI MCS1",
"HT-MM SGI MCS2",
"HT-MM SGI MCS3",
"HT-MM SGI MCS4",
"HT-MM SGI MCS5",
"HT-MM SGI MCS6",
"HT-MM SGI MCS7",
"HT-GF MCS0",
"HT-GF MCS1",
"HT-GF MCS2",
"HT-GF MCS3",
"HT-GF MCS4",
"HT-GF MCS5",
"HT-GF MCS6",
"HT-GF MCS7"
};

#if 0
//#ifdef FUNC_SOFT_AP
int At_SET_AP_SSID (stParam *param)
{
    icomprintf(GROUP_ATCMD,LOG_INFO,"[%s] : +++\n",__func__);
    char *p_ssid = NULL;

    p_ssid = param->argv[0];

    gconfig_set_softap_ssid(p_ssid);

    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
S32 At_SAVE_CONFIG (stParam *param)
{
    S32 rlt = 0;
//    atcmdprintf("[%s] : +++\n",__func__);

    rlt = gconfig_save_config();

    if(rlt!=0){
        atcmdprintf("<Error>gconfig_save_config failed!!\n");
    }

    return rlt;
}
/*---------------------------------------------------------------------------*/
int At_Clean_Lists (stParam *param)
{
 //   atcmdprintf("[At_Clean_Lists] : +++\n");
    clear_ap_lists();

    return ERROR_SUCCESS;
}
#endif

/*---------------------------------------------------------------------------*/
const uint8_t hk_value_56byte_bin[] = {
  0xDD, 0x36, 0x00, 0xA0, 0x40,
  0x00, 0x07, 0x06, 0x90, 0x77, 0x97, 0x49, 0x5b, 0x29, 0x00, 0x03, 0x60,
  0x02, 0x40, 0x01, 0x14, 0x69, 0x55, 0x39, 0x2d, 0x53, 0x6d, 0x61, 0x72,
  0x74, 0x2d, 0x4f, 0x75, 0x74, 0x6c, 0x65, 0x74, 0x30, 0x32, 0x36, 0x44,
  0x03, 0x06, 0x46, 0x48, 0x48, 0x31, 0x30, 0x37, 0x02, 0x05, 0x4f, 0x70,
  0x72, 0x6f, 0x39
};

u8 get_current_rate(u8 wsid);

int ip2int (char *pStr, uip_ip4addr_t *ipaddr)
{	
	const char delimiters[] = " .";	
	char cp[16]={0};	
	char *pIp1=0, *pIp2=0, *pIp3=0, *pIp4=0;	
		
	strcpy (cp, pStr);	pIp1 = strtok (cp, delimiters);	
	pIp2 = strtok (NULL, delimiters);  	
	pIp3 = strtok (NULL, delimiters);	
	pIp4 = strtok (NULL, delimiters);	
	if (pIp1==0 || pIp2==0 || pIp3==0 ||pIp4==0)	
		return ERROR_INVALID_PARAMETER;
	
	ipaddr->u8[0] = atoi (pIp1);	
	ipaddr->u8[1] = atoi (pIp2);	
	ipaddr->u8[2] = atoi (pIp3);	
	ipaddr->u8[3] = atoi (pIp4);
	
	return ERROR_SUCCESS;
}
char* showDataRate(u8 idx)
{
	if(idx >=0 && idx <= 38 )
		return dataRateMapping[idx];
	else
		return "Unknown";
}
const char *securityString(u8 ver)
{
    switch(ver) {
    case NONE:
        return "OPEN";
        break;
    case WEP:
        return "WEP";
        break;
    case WPA:
        return "WPA";
        break;
    case WPA2:
        return "WPA2";
        break;
    case WPAWPA2:
        return "WPAWPA2";
        break;
    default:
        return "unknown security type";
    }
    return NULL;
}
/*---------------------------------------------------------------------------*/
const char *securitySubString(u8 ver)
{
    switch(ver) {
    case NOSECTYPE:
        return "NONE";
        break;
    case TKIP:
        return "TKIP";
        break;
    case CCMP:
        return "AES";
        break;
    case TKIP_CCMP:
        return "TKIP_CCMP";
        break;
    default:
        return "unknown security type";
    }
    return NULL;
}
int At_HKAP(stParam *param)
{
//    atcmdprintf("[At_HKAP] : +++\n");
    
    //softap_start();
	SOFTAP_CUSTOM_CONFIG isoftap_custom_config;

#if 0
	isoftap_custom_config.a_gw_digit[0] = 192;
	isoftap_custom_config.a_gw_digit[1] = 168;
	isoftap_custom_config.a_gw_digit[2] = 0;
#endif

	isoftap_custom_config.start_ip = 0xc0a80002;	//192.168.10.2
	isoftap_custom_config.end_ip = 0xc0a80005; 	//192.168.10.5
	isoftap_custom_config.gw = 0xc0a80001; 		//192.168.10.254
	isoftap_custom_config.subnet = 0xffffff00; 	//255.255.255.0

	isoftap_custom_config.max_sta_num = 1;
	isoftap_custom_config.encryt_mode = 0;

	isoftap_custom_config.beacon_interval = 50;

	isoftap_custom_config.keylen = strlen("1234567890");
	memcpy(&isoftap_custom_config.key[0],"1234567890",isoftap_custom_config.keylen);
	
	isoftap_custom_config.channel= 1;

	isoftap_custom_config.ssid_length = strlen("iU9-Smart-Outlet026D");
	memcpy(&isoftap_custom_config.ssid[0],"iU9-Smart-Outlet026D",isoftap_custom_config.ssid_length);
	
	softap_set_custom_conf(&isoftap_custom_config);

	homekit_softap_start( &hk_value_56byte_bin[0], sizeof(hk_value_56byte_bin));

    return ERROR_SUCCESS_NO_RSP;
}

/*---------------------------------------------------------------------------*/
int At_AP (stParam *param)
{
//    atcmdprintf("[At_AP] : +++\n");
    //softap_start();
    DUT_wifi_start(DUT_AP);
    return ERROR_SUCCESS_NO_RSP;
}
/*---------------------------------------------------------------------------*/
uint32_t htonlx(uint32_t x)
{
    uint8_t *s = (uint8_t *)&x;
    return (uint32_t)((s[0] << 24) | (s[1] << 16) | (s[2] << 8) | (s[3]));
}

int At_SET_APCONFIG (stParam *param)
{
    int32_t rlt = ERROR_INVALID_PARAMETER;
	SOFTAP_CUSTOM_CONFIG isoftap_custom_config;
	int loop_i;
	uint32_t tmp_ip0=0;
	uint32_t tmp_ip1=0;
	uint32_t tmp_ip2=0;
	uint32_t tmp_ip3=0;
	   
//	icomprintf(GROUP_ATCMD, LOG_ERROR,"[At_AP_EX] : +++\n");
	printf("\n[%s] : +++\n",__func__);

    if (param->argc < 9) {
        return ERROR_INVALID_PARAMETER;
    }

	isoftap_custom_config.start_ip = strtoul(param->argv[0], NULL, 16);
	isoftap_custom_config.end_ip = strtoul(param->argv[1], NULL, 16);
	isoftap_custom_config.gw = strtoul(param->argv[2], NULL, 16);
	isoftap_custom_config.subnet = strtoul(param->argv[3], NULL, 16);

    
    isoftap_custom_config.max_sta_num = strtoul(param->argv[4], NULL, 10);

	if(isoftap_custom_config.max_sta_num > 4){
		isoftap_custom_config.max_sta_num = 4;
	}

	if(isoftap_custom_config.max_sta_num < 1){
		isoftap_custom_config.max_sta_num = 1;
	}

	if(isoftap_custom_config.end_ip >  (isoftap_custom_config.start_ip + isoftap_custom_config.max_sta_num)){
		tmp_ip0 = isoftap_custom_config.end_ip;
		isoftap_custom_config.end_ip = isoftap_custom_config.start_ip + isoftap_custom_config.max_sta_num;
		tmp_ip1 = isoftap_custom_config.end_ip;
		printf("\n<Error> end_ip = %d.%d.%d.%d  over start_ip+max_stanum -----> new end_ip=%d.%d.%d.%d\n\n",(tmp_ip0>>24)&0xff,(tmp_ip0>>16)&0xff,(tmp_ip0>>8)&0xff,(tmp_ip0>>0)&0xff,(tmp_ip1>>24)&0xff,(tmp_ip1>>16)&0xff,(tmp_ip1>>8)&0xff,(tmp_ip1>>0)&0xff);
	}
	
    isoftap_custom_config.encryt_mode = strtoul(param->argv[5], NULL, 10);
    isoftap_custom_config.keylen = strtoul(param->argv[6], NULL, 10);
	
    if(isoftap_custom_config.keylen != strlen(param->argv[7])){
        rlt = -1;
        goto error_exit;
    }

    memset(isoftap_custom_config.key,0x0,sizeof(isoftap_custom_config.key));            
    memcpy(isoftap_custom_config.key,param->argv[7],isoftap_custom_config.keylen);
	
	isoftap_custom_config.channel = strtoul(param->argv[8], NULL, 10);
	isoftap_custom_config.ssid_length= strtoul(param->argv[9], NULL, 10);
	if(isoftap_custom_config.ssid_length != strlen(param->argv[10])){
		rlt = -2;
		goto error_exit;
	}

	memset(isoftap_custom_config.ssid,0x0,sizeof(isoftap_custom_config.ssid));			  
	memcpy(isoftap_custom_config.ssid,param->argv[10],isoftap_custom_config.ssid_length);

	tmp_ip0 = htonlx(isoftap_custom_config.start_ip);
	tmp_ip1 = htonlx(isoftap_custom_config.end_ip);
	tmp_ip2 = htonlx(isoftap_custom_config.gw);
	tmp_ip3 = htonlx(isoftap_custom_config.subnet);
	
	dbg_dump_ip("start_ip",&tmp_ip0);
	dbg_dump_ip("end_ip",&tmp_ip1);
	dbg_dump_ip("gateway",&tmp_ip2);
	dbg_dump_ip("netmask",&tmp_ip3);
	
    printf("(max_sta_num,encryt_mode,channel,hk_support)=(%d,%d,%d)\n",isoftap_custom_config.max_sta_num,isoftap_custom_config.encryt_mode,isoftap_custom_config.channel);
	printf("(keylen,key)=(%d,%s)\n",isoftap_custom_config.keylen,isoftap_custom_config.key);
	printf("(ssid_length,ssid)=(%d,%s)\n",isoftap_custom_config.ssid_length,isoftap_custom_config.ssid);

	isoftap_custom_config.beacon_interval = 50;
	
	rlt = softap_set_custom_conf(&isoftap_custom_config);

        if(rlt == -4)
            printf("Don't configure SoftAP when SoftAP is running. Please execute AT+AP_EXIT first\n");
        
    return rlt;
error_exit:
	for(loop_i=0;loop_i<param->argc;loop_i++){
		printf("argv[%d]=%s\n",loop_i,param->argv[loop_i]);
	}
    printf("<Error>At_SET_APCONFIG rlt = %d\n", rlt);
    return rlt;
}


#if 0
int At_GET_APCONFIG (stParam *param)
{
    uint8_t ssid[33], ssidlen = 32, key[65], keylen = 64, encryt = 0;
	

    if(get_apmode_config(ssid, &ssidlen, &encryt, key, &keylen) == 0)
    {
        ssid[ssidlen] = 0;
        key[keylen] = 0;
    }
    else 
	  return ERROR_INVALID_PARAMETER;

    atcmdprintf("AT+GET_APCONFIG=%s,%d,%d,%s,%d\n", ssid, ssidlen, encryt, key, keylen);


    return ERROR_SUCCESS;
}
#endif
int At_GET_APMODE (stParam *param)
{
    WIFI_OPMODE mode = get_DUT_wifi_mode();
    printf("\r\n");
    printf("AT+GET_APMODE=%s\n", mode == DUT_AP?"AP":"STA");	
    return 0;
}
/*---------------------------------------------------------------------------*/
int At_AP_EXIT (stParam *param)
{
    softap_exit();

    WIFI_OPMODE mode = get_DUT_wifi_mode();
    
    if( mode != DUT_STA )
        DUT_wifi_start(DUT_STA);
    return ERROR_SUCCESS_NO_RSP;
}
int At_ListStation (stParam *param)
{
    int ret = ERROR_SUCCESS;
    CONNSTAINFO connected_sta[4] = {0};
    u8 number_sta = 4;
    int i=0;
    
    if(get_DUT_wifi_mode() != DUT_AP )
    {
        printf("\nPlease Enter AP Mode First\n");
        return -1;
    }
    
    if( get_connectsta_info(&connected_sta, &number_sta) == 0 )
    {
        printf("\nConnected Station Number = %d\n", number_sta);
        for(i=0; i<number_sta; i++)
        {
            printf("STA%d MAC:%02x:%02x:%02x:%02x:%02x:%02x, IP:%d.%d.%d.%d\n", i+1, connected_sta[i].mac[0], connected_sta[i].mac[1], connected_sta[i].mac[2], connected_sta[i].mac[3], connected_sta[i].mac[4], connected_sta[i].mac[5], connected_sta[i].ipaddr[0], connected_sta[i].ipaddr[1], connected_sta[i].ipaddr[2], connected_sta[i].ipaddr[3]);
        }
    }
    else
    {
        ret = -1;
    }
    
    return ret;
}
int parseBuff2Param(char* bufCmd, stParam* pParam, uint8_t maxargu)
{
	int buflen, i;
	const char delimiters = ',';

    buflen = strlen (bufCmd);
	if (strlen (bufCmd) == 0) 
		return ERROR_INVALID_PARAMETER;

    if(maxargu == 0)
        maxargu = MAX_ARGUMENT;

    pParam->argc = 1;
    pParam->argv[pParam->argc - 1] = &bufCmd[0];
    i = 0;
    while(pParam->argc < maxargu)
    {
        for(; i < buflen ; i++)
        {
            if(bufCmd[i] == delimiters)
            {
                bufCmd[i] = 0;
                break;
            }
        }
        if(i == buflen)
			break;
        if(bufCmd[i + 1] != 0)
        {
            pParam->argc += 1;
            pParam->argv[pParam->argc - 1] = &bufCmd[i + 1];
        }
        else
            break;
    }
    
	return 0;
}


////RF AT Command
int At_RadioRFStart(stParam *param)
{	
    printf("\n");
    int ret = ERROR_SUCCESS;

    ret = rf_tx_start(atoi(param->argv[0]), atoi(param->argv[1])); 	
    return ret;
}
int At_RadioChannel(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    
    int ch = atoi(param->argv[0]);
    int ch_type = NL80211_CHAN_HT20;
    
    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;
    
    if(strcmp("+", param->argv[1]) == 0)
    {
        ch_type = NL80211_CHAN_HT40PLUS;
    }
    else if(strcmp("-", param->argv[1]) == 0)
    {
        ch_type= NL80211_CHAN_HT40MINUS;
    }

    ret = rf_set_ch(ch, 0, ch_type);

    return ret;
}
int At_RadioRFRate(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    
    int rate_id = 0;
    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;

    rate_id = atoi(param->argv[0]);
    ret = rf_set_rate(rate_id);

	return ret;
}
int At_RadioRFBandGain(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    
    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_band_gain(atoi(param->argv[0]));
    
    return ret;
}
int At_RadioRFGain(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    
    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;
    
    ret = write_reg_rf_gain(atoi(param->argv[0]));
        
    return ret;
}
int At_RadioRFRateGain(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    int gain_idx = 0;
    struct st_rate_gain rate_config = {0};
        
    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;
    
    gain_idx = atoi(param->argv[1]); 

    printf("argv0 = %s, \n", param->argv[0]);
    if( strncmp(param->argv[0], "b", 1) == 0 )
    {
        printf("set b mode\n");
        ret = write_reg_rate_gain_b(gain_idx);
    }
    else if( strncmp(param->argv[0], "g", 1) == 0 )
    {
        printf("set g mode\n");
        if(strcmp(param->argv[0], "g6") == 0)
        {
            rate_config.rate1 = gain_idx;
            ret = write_reg_rate_gain_g(rate_config, 0x8); //
        }
        else if(strcmp(param->argv[0], "g12") == 0)
        {
            rate_config.rate2 = gain_idx;
            ret = write_reg_rate_gain_g(rate_config, 0x4);
        }
        else if(strcmp(param->argv[0], "g24") == 0)
        {
            rate_config.rate3 = gain_idx;
            ret = write_reg_rate_gain_g(rate_config, 0x2);
        }
        else if(strcmp(param->argv[0], "g48") == 0)
        {
            rate_config.rate4 = gain_idx;
            ret = write_reg_rate_gain_g(rate_config, 0x1);
        }
        else
        {
            return ERROR_NOT_IMPLEMENT;
        }
    }    
    else if( strncmp(param->argv[0], "20n", 3) == 0 )
    {

        printf("set 20n mode\n");
        if(strcmp(param->argv[0], "20n0") == 0)
        {
            rate_config.rate1 = gain_idx;
            ret = write_reg_rate_gain_20n(rate_config, 0x8);
        }
        else if(strcmp(param->argv[0], "20n1") == 0)
        {
            rate_config.rate2 = gain_idx;
            ret = write_reg_rate_gain_20n(rate_config, 0x4);            
        }
        else if(strcmp(param->argv[0], "20n3") == 0)
        {
            rate_config.rate3 = gain_idx;
            ret = write_reg_rate_gain_20n(rate_config, 0x2);
        }
        else if(strcmp(param->argv[0], "20n5") == 0)
        {
            rate_config.rate4 = gain_idx;
            ret = write_reg_rate_gain_20n(rate_config, 0x1);
        }
        else
        {
            return ERROR_NOT_IMPLEMENT;
        }
    }
    else if( strncmp(param->argv[0], "40n", 3) == 0 )
    {

        printf("set 40n mode\n");

        if(strcmp(param->argv[0], "40n0") == 0)
        {
            rate_config.rate1 = gain_idx;
            ret = write_reg_rate_gain_40n(rate_config, 0x8);
        }
        else if(strcmp(param->argv[0], "40n1") == 0)
        {
            rate_config.rate2 = gain_idx;
            ret = write_reg_rate_gain_40n(rate_config, 0x4);
        }
        else if(strcmp(param->argv[0], "40n3") == 0)
        {
            rate_config.rate3 = gain_idx;
            ret = write_reg_rate_gain_40n(rate_config, 0x2);
        }
        else if(strcmp(param->argv[0], "40n5") == 0)
        {
            rate_config.rate4 = gain_idx;
            ret = write_reg_rate_gain_40n(rate_config, 0x1);
        }
        else
        {
            return ERROR_NOT_IMPLEMENT;
        }
    }
    else
    {
        return ERROR_NOT_IMPLEMENT;
    }
    
    return ret;
}
int At_RadioRFReadTempcs(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    int value = 0;
    get_current_temperature(&value);

    printf("temperature = %d\n", value);
    return ret;
}
int At_RadioRFStop(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	
    
    ret = rf_tx_stop();
    
    return ret;
}
int At_RadioRFReset(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    ret = rf_rx_reset();

    return ret;
}
int At_RadioRFCount(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    uint32_t err_rxpkt = 0;
    uint32_t total_rxpkt = 0;

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;

    if( atoi(param->argv[0]) == 1 )
    {
        printf("B Mode Count:\n");
        rf_bmode_count(&total_rxpkt, &err_rxpkt);
    }
    else
    {
        printf("G/N Mode Count:\n");
        rf_gmode_count(&total_rxpkt, &err_rxpkt);
    }
        
    printf("crc count = '%ld'\n", err_rxpkt);
    printf("total count = '%ld'\n", total_rxpkt);	

    return ret;
}
int At_RadioEnableTCSR(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;
    
    ret = rf_enable_tcsr(atoi(param->argv[0]));
    return ret;
}
int At_RadioRFDump(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    dump_rf_table();
    return ret;
}
int At_RfCaliDPD(stParam *param)    
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;
    
   ret = write_reg_padpd_cali(atoi(param->argv[0]));
     
    return ret;    

}
int At_RfManuDPD(stParam *param)    
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    ret = rf_manu_padpd_cali();
     
    return ret;    

}
int At_RadioRFFreqoffset(stParam *param)    
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_freq_xi_xo(atoi(param->argv[0]), atoi(param->argv[0]), 0x3);
        
    return ret;
}
int At_RadioRFFreqoffset_readxi(stParam *param)    
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    uint32_t xi_value = 0;
    ret = rf_freqoffset_readxi(&xi_value);

    printf("%s=%ld\n", ATCMD_RF_FREQOFFSET_READXI, xi_value);
         
    return ret;
}    
int At_RadioRFFreqoffset_readxo(stParam *param)    
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    uint32_t xo_value = 0;
    ret = rf_freqoffset_readxo(&xo_value);
    
    printf("%s=%ld\n", ATCMD_RF_FREQOFFSET_READXO, xo_value);
         
    return ret;
}    

int At_RadioRF2Gpa1(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_2gpa(atoi(param->argv[0]), 0, 0, 0, 0x8);
        
    return ret;
}    
int At_RadioRF2Gpa2(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_2gpa(0, atoi(param->argv[0]), 0, 0, 0x4);
        
    return ret;
}   
int At_RadioRF2Gpa3(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_2gpa(0, 0, atoi(param->argv[0]), 0, 0x2);
        
    return ret;
}   
int At_RadioRF2GpaBias(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_2gpa(0, 0, 0, atoi(param->argv[0]), 0x1);
        
    return ret;
}   
int At_RadioRF2GpaCell(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_2gpa_cell(atoi(param->argv[0]));
        
    return ret;
}   
int At_RadioRF2GpaCap(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_2gpa_cap(atoi(param->argv[0]));
        
    return ret;
}   

int At_RadioRFLdo(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_rxafe(atoi(param->argv[0]));
        
    return ret;
}     
int At_RadioRFDcdc(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_dcdc(atoi(param->argv[0]));
        
    return ret;
} 
int At_RadioRFDcdcv(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_ldo(atoi(param->argv[0]), 0, 0x2);
        
    return ret;
} 
int At_RadioRFDldov(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_ldo(0, atoi(param->argv[0]), 0x1);
        
    return ret;
}     
int At_RadioRFXoldo(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_xoldo(atoi(param->argv[0]));
        
    return ret;
}     
int At_RadioRFRxPadCh13(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_rxpad_ch13(atoi(param->argv[0]));
        
    return ret;
}      
int At_RadioRFTempBoundary(stParam *param)
{

    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;

    set_temper_value(atoi(param->argv[0]), atoi(param->argv[1]));
    
    save_rf_table_to_flash();
    
    return ret;
}
int At_RfTableRT(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;
    
    //printf("arg1 = %s, arg2 = %s, ... , arg18 = %s\n", param->argv[0], param->argv[1], param->argv[17]);
    
    if(strcmp("?" , param->argv[0])==0)
    {
        printf("%s=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", ATCMD_RF_TABLE_RT
                                                                                                                                     , ssv_rf_table.rt_config.band_gain[0]
                                                                                                                                     , ssv_rf_table.rt_config.band_gain[1]
                                                                                                                                     , ssv_rf_table.rt_config.band_gain[2]
                                                                                                                                     , ssv_rf_table.rt_config.band_gain[3]
                                                                                                                                     , ssv_rf_table.rt_config.band_gain[4]
                                                                                                                                     , ssv_rf_table.rt_config.band_gain[5]
                                                                                                                                     , ssv_rf_table.rt_config.band_gain[6]
                                                                                                                                     , ssv_rf_table.rt_config.freq_xi
                                                                                                                                     , ssv_rf_table.rt_config.freq_xo
                                                                                                                                     , ssv_rf_table.rt_config.ldo_rxafe
                                                                                                                                     , ssv_rf_table.rt_config.ldo_dcdcv
                                                                                                                                     , ssv_rf_table.rt_config.ldo_dldov
                                                                                                                                     , ssv_rf_table.rt_config.pa_vcas1
                                                                                                                                     , ssv_rf_table.rt_config.pa_vcas2
                                                                                                                                     , ssv_rf_table.rt_config.pa_vcas3
                                                                                                                                     , ssv_rf_table.rt_config.pa_bias
                                                                                                                                     , ssv_rf_table.rt_config.pa_cap
                                                                                                                                     , ssv_rf_table.rt_config.padpd_cali
                                                                                                                                     );
    }
    else
    {
        struct st_tempe_table tempe_config;
        tempe_config.band_gain[0] = atoi(param->argv[0]);
        tempe_config.band_gain[1] = atoi(param->argv[1]);
        tempe_config.band_gain[2] = atoi(param->argv[2]);
        tempe_config.band_gain[3] = atoi(param->argv[3]);
        tempe_config.band_gain[4] = atoi(param->argv[4]);
        tempe_config.band_gain[5] = atoi(param->argv[5]);
        tempe_config.band_gain[6] = atoi(param->argv[6]);
        
        tempe_config.freq_xi = atoi(param->argv[7]);
        tempe_config.freq_xo = atoi(param->argv[8]);
        tempe_config.ldo_rxafe = atoi(param->argv[9]);
        tempe_config.ldo_dcdcv = atoi(param->argv[10]);
        tempe_config.ldo_dldov = atoi(param->argv[11]);

        tempe_config.pa_vcas1 = atoi(param->argv[12]);
        tempe_config.pa_vcas2 = atoi(param->argv[13]);
        tempe_config.pa_vcas3 = atoi(param->argv[14]);
        tempe_config.pa_bias = atoi(param->argv[15]);
        tempe_config.pa_cap = atoi(param->argv[16]);
        tempe_config.padpd_cali = atoi(param->argv[17]);

        if( get_current_tempe_state( ssv_rf_table.low_boundary, ssv_rf_table.high_boundary ) == 0 )
            ret = write_reg_tempe_table(tempe_config);

        if(ret != 0)
            return ret;

        memcpy(&ssv_rf_table.rt_config, &tempe_config, sizeof(tempe_config) );
        save_rf_table_to_flash();
    }
        
    return ret;
}      
int At_RfTableHT(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;
    
    //printf("arg1 = %s, arg2 = %s, ... , arg18 = %s\n", param->argv[0], param->argv[1], param->argv[17]);
    
    if(strcmp("?" , param->argv[0])==0)
    {
        printf("%s=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", ATCMD_RF_TABLE_HT
                                                                                                                                     , ssv_rf_table.ht_config.band_gain[0]
                                                                                                                                     , ssv_rf_table.ht_config.band_gain[1]
                                                                                                                                     , ssv_rf_table.ht_config.band_gain[2]
                                                                                                                                     , ssv_rf_table.ht_config.band_gain[3]
                                                                                                                                     , ssv_rf_table.ht_config.band_gain[4]
                                                                                                                                     , ssv_rf_table.ht_config.band_gain[5]
                                                                                                                                     , ssv_rf_table.ht_config.band_gain[6]
                                                                                                                                     , ssv_rf_table.ht_config.freq_xi
                                                                                                                                     , ssv_rf_table.ht_config.freq_xo
                                                                                                                                     , ssv_rf_table.ht_config.ldo_rxafe
                                                                                                                                     , ssv_rf_table.ht_config.ldo_dcdcv
                                                                                                                                     , ssv_rf_table.ht_config.ldo_dldov
                                                                                                                                     , ssv_rf_table.ht_config.pa_vcas1
                                                                                                                                     , ssv_rf_table.ht_config.pa_vcas2
                                                                                                                                     , ssv_rf_table.ht_config.pa_vcas3
                                                                                                                                     , ssv_rf_table.ht_config.pa_bias
                                                                                                                                     , ssv_rf_table.ht_config.pa_cap
                                                                                                                                     , ssv_rf_table.ht_config.padpd_cali
                                                                                                                                     );
    }
    else
    {
        struct st_tempe_table tempe_config;
        tempe_config.band_gain[0] = atoi(param->argv[0]);
        tempe_config.band_gain[1] = atoi(param->argv[1]);
        tempe_config.band_gain[2] = atoi(param->argv[2]);
        tempe_config.band_gain[3] = atoi(param->argv[3]);
        tempe_config.band_gain[4] = atoi(param->argv[4]);
        tempe_config.band_gain[5] = atoi(param->argv[5]);
        tempe_config.band_gain[6] = atoi(param->argv[6]);
        
        tempe_config.freq_xi = atoi(param->argv[7]);
        tempe_config.freq_xo = atoi(param->argv[8]);
        tempe_config.ldo_rxafe = atoi(param->argv[9]);
        tempe_config.ldo_dcdcv = atoi(param->argv[10]);
        tempe_config.ldo_dldov = atoi(param->argv[11]);

        tempe_config.pa_vcas1 = atoi(param->argv[12]);
        tempe_config.pa_vcas2 = atoi(param->argv[13]);
        tempe_config.pa_vcas3 = atoi(param->argv[14]);
        tempe_config.pa_bias = atoi(param->argv[15]);
        tempe_config.pa_cap = atoi(param->argv[16]);
        tempe_config.padpd_cali = atoi(param->argv[17]);

        if( get_current_tempe_state( ssv_rf_table.low_boundary, ssv_rf_table.high_boundary ) == 1 )
            ret = write_reg_tempe_table(tempe_config);

        if(ret != 0)
            return ret;

        memcpy(&ssv_rf_table.ht_config, &tempe_config, sizeof(tempe_config) );
        save_rf_table_to_flash();
    }
    return ret;
}      
int At_RfTableLT(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;
    
    //printf("arg1 = %s, arg2 = %s, ... , arg18 = %s\n", param->argv[0], param->argv[1], param->argv[17]);
    
    if(strcmp("?" , param->argv[0])==0)
    {
        printf("%s=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", ATCMD_RF_TABLE_LT
                                                                                                                                     , ssv_rf_table.lt_config.band_gain[0]
                                                                                                                                     , ssv_rf_table.lt_config.band_gain[1]
                                                                                                                                     , ssv_rf_table.lt_config.band_gain[2]
                                                                                                                                     , ssv_rf_table.lt_config.band_gain[3]
                                                                                                                                     , ssv_rf_table.lt_config.band_gain[4]
                                                                                                                                     , ssv_rf_table.lt_config.band_gain[5]
                                                                                                                                     , ssv_rf_table.lt_config.band_gain[6]
                                                                                                                                     , ssv_rf_table.lt_config.freq_xi
                                                                                                                                     , ssv_rf_table.lt_config.freq_xo
                                                                                                                                     , ssv_rf_table.lt_config.ldo_rxafe
                                                                                                                                     , ssv_rf_table.lt_config.ldo_dcdcv
                                                                                                                                     , ssv_rf_table.lt_config.ldo_dldov
                                                                                                                                     , ssv_rf_table.lt_config.pa_vcas1
                                                                                                                                     , ssv_rf_table.lt_config.pa_vcas2
                                                                                                                                     , ssv_rf_table.lt_config.pa_vcas3
                                                                                                                                     , ssv_rf_table.lt_config.pa_bias
                                                                                                                                     , ssv_rf_table.lt_config.pa_cap
                                                                                                                                     , ssv_rf_table.lt_config.padpd_cali
                                                                                                                                     );
    }
    else
    {
        struct st_tempe_table tempe_config;
        tempe_config.band_gain[0] = atoi(param->argv[0]);
        tempe_config.band_gain[1] = atoi(param->argv[1]);
        tempe_config.band_gain[2] = atoi(param->argv[2]);
        tempe_config.band_gain[3] = atoi(param->argv[3]);
        tempe_config.band_gain[4] = atoi(param->argv[4]);
        tempe_config.band_gain[5] = atoi(param->argv[5]);
        tempe_config.band_gain[6] = atoi(param->argv[6]);
        
        tempe_config.freq_xi = atoi(param->argv[7]);
        tempe_config.freq_xo = atoi(param->argv[8]);
        tempe_config.ldo_rxafe = atoi(param->argv[9]);
        tempe_config.ldo_dcdcv = atoi(param->argv[10]);
        tempe_config.ldo_dldov = atoi(param->argv[11]);

        tempe_config.pa_vcas1 = atoi(param->argv[12]);
        tempe_config.pa_vcas2 = atoi(param->argv[13]);
        tempe_config.pa_vcas3 = atoi(param->argv[14]);
        tempe_config.pa_bias = atoi(param->argv[15]);
        tempe_config.pa_cap = atoi(param->argv[16]);
        tempe_config.padpd_cali = atoi(param->argv[17]);

        if( get_current_tempe_state( ssv_rf_table.low_boundary, ssv_rf_table.high_boundary ) == 2 )
            ret = write_reg_tempe_table(tempe_config);

        if(ret != 0)
            return ret;

        memcpy(&ssv_rf_table.lt_config, &tempe_config, sizeof(tempe_config) );
        save_rf_table_to_flash();
    }
    return ret;
}      
int At_RfTableGain(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;
    
    if(strcmp("?" , param->argv[0])==0)
    {
        printf("RF Gain = %d\n", ssv_rf_table.rf_gain );
    }
    else
    {
        uint8_t rf_gain = atoi(param->argv[0]);
        ret = write_reg_rf_gain(rf_gain);
        if(ret != 0)
            return ret;
        ssv_rf_table.rf_gain = rf_gain;
        save_rf_table_to_flash(); 
    }
    return ret;
}
int At_RfTableBRateGain(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;
    
    if(strcmp("?" , param->argv[0])==0)
    {
        printf("B Mode Rate Gain = %d\n", ssv_rf_table.rate_gain_b);
    }
    else
    {
        uint8_t rate_gain = atoi(param->argv[0]);
        ret = write_reg_rate_gain_b(rate_gain);
        if(ret != 0)
            return ret;
        ssv_rf_table.rate_gain_b = rate_gain;
        save_rf_table_to_flash();
    }
    return ret;
}   
int At_RfTableGRateGain(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;
    
    if(strcmp("?" , param->argv[0])==0)
    {
        printf("G Mode Rate Gain = %d,%d,%d,%d\n", ssv_rf_table.rate_config_g.rate1, 
                                                                              ssv_rf_table.rate_config_g.rate2, 
                                                                              ssv_rf_table.rate_config_g.rate3, 
                                                                              ssv_rf_table.rate_config_g.rate4 );
    }
    else
    {
        struct st_rate_gain g_rate_gain = {0};
        g_rate_gain.rate1 = atoi(param->argv[0]);
        g_rate_gain.rate2 = atoi(param->argv[1]);
        g_rate_gain.rate3 = atoi(param->argv[2]);
        g_rate_gain.rate4 = atoi(param->argv[3]);
        
        ret = write_reg_rate_gain_g(g_rate_gain, 0xf);
        if(ret != 0)
            return ret;

        memcpy(&ssv_rf_table.rate_config_g, &g_rate_gain, sizeof(g_rate_gain));
        save_rf_table_to_flash(); 
    }
    return ret;
}   
int At_RfTable20NRateGain(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;
    
    if(strcmp("?" , param->argv[0])==0)
    {
        printf("20N Mode Rate Gain = %d,%d,%d,%d\n", ssv_rf_table.rate_config_20n.rate1, 
                                                                              ssv_rf_table.rate_config_20n.rate2, 
                                                                              ssv_rf_table.rate_config_20n.rate3, 
                                                                              ssv_rf_table.rate_config_20n.rate4 );
    }
    else
    {
        struct st_rate_gain rate_gain_20n = {0};
        rate_gain_20n.rate1 = atoi(param->argv[0]);
        rate_gain_20n.rate2 = atoi(param->argv[1]);
        rate_gain_20n.rate3 = atoi(param->argv[2]);
        rate_gain_20n.rate4 = atoi(param->argv[3]);
        
        ret = write_reg_rate_gain_20n(rate_gain_20n, 0xf);
        if(ret != 0)
            return ret;

        memcpy(&ssv_rf_table.rate_config_20n, &rate_gain_20n, sizeof(rate_gain_20n));
        save_rf_table_to_flash(); 
    }
    return ret;
}   
int At_RfTable40NRateGain(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;
    
    if(strcmp("?" , param->argv[0])==0)
    {
        printf("40N Mode Rate Gain = %d,%d,%d,%d\n", ssv_rf_table.rate_config_40n.rate1, 
                                                                              ssv_rf_table.rate_config_40n.rate2, 
                                                                              ssv_rf_table.rate_config_40n.rate3, 
                                                                              ssv_rf_table.rate_config_40n.rate4 );
    }
    else
    {
        struct st_rate_gain rate_gain_40n = {0};
        rate_gain_40n.rate1 = atoi(param->argv[0]);
        rate_gain_40n.rate2 = atoi(param->argv[1]);
        rate_gain_40n.rate3 = atoi(param->argv[2]);
        rate_gain_40n.rate4 = atoi(param->argv[3]);
        
        ret = write_reg_rate_gain_40n(rate_gain_40n, 0xf);
        if(ret != 0)
            return ret;

        memcpy(&ssv_rf_table.rate_config_40n, &rate_gain_40n, sizeof(rate_gain_40n));
        save_rf_table_to_flash(); 
    }
    return ret;
}   
int At_RadioWriteMAC1(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;

    if( strlen(param->argv[0]) != 17 )
    {
        printf("EX:%s=00:23:45:67:89:AB\n", ATCMD_RADIO_WRITE_MAC1);
        return ERROR_INVALID_PARAMETER;
    }
    
    void *cfg_handle = wifi_cfg_init();
    if (cfg_handle == NULL) {
        return ERROR_MEMORY_FAILED;
    }
   
    wifi_cfg_replace_mem_addr1(cfg_handle, param->argv[0]);
    wifi_cfg_write_cfg(cfg_handle);
    wifi_cfg_deinit(cfg_handle);

    return ret;
}
int At_RadioReadMAC1(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    void *cfg_handle = wifi_cfg_init();
    char mac_addr[6] = {0};
    wifi_cfg_get_addr1(cfg_handle, mac_addr);
    wifi_cfg_deinit(cfg_handle);
    
    printf("%s=%02X:%02X:%02X:%02X:%02X:%02X\n", ATCMD_RADIO_READ_MAC1, (uint8_t)mac_addr[0], (uint8_t)mac_addr[1], (uint8_t)mac_addr[2], (uint8_t)mac_addr[3], (uint8_t)mac_addr[4], (uint8_t)mac_addr[5]);
    return ret;
}
int At_RadioWriteMAC2(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;

    if( strlen(param->argv[0]) != 17 )
    {
        printf("EX:%s=00:23:45:67:89:AB\n", ATCMD_RADIO_WRITE_MAC2);
        return ERROR_INVALID_PARAMETER;
    }
    
    void *cfg_handle = wifi_cfg_init();
    if (cfg_handle == NULL) {
        return ERROR_MEMORY_FAILED;
    }
   
    wifi_cfg_replace_mem_addr2(cfg_handle, param->argv[0]);
    wifi_cfg_write_cfg(cfg_handle);
    wifi_cfg_deinit(cfg_handle);

    return ret;
}
int At_RadioReadMAC2(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    void *cfg_handle = wifi_cfg_init();
    char mac_addr[6] = {0};
    wifi_cfg_get_addr2(cfg_handle, mac_addr);
    wifi_cfg_deinit(cfg_handle);
    
    printf("%s=%02X:%02X:%02X:%02X:%02X:%02X\n", ATCMD_RADIO_READ_MAC2, (uint8_t)mac_addr[0], (uint8_t)mac_addr[1], (uint8_t)mac_addr[2], (uint8_t)mac_addr[3], (uint8_t)mac_addr[4], (uint8_t)mac_addr[5]);
    return ret; 
}
int At_EfuseEnableWrite(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    bEfuseEnableWrite = 1;
    printf("Now you can write efuse\n");
    
    return ret; 
}
int At_EfuseWriteMAC(stParam *param)
{
    int ret = ERROR_SUCCESS;
    int i = 0;
    const char delimiters[] = ":";
    char *pMac = 0, *ptmp;
    unsigned long int toul_val;
    uint8_t efuse_mac[6] = {0};

    printf("\n");
    
    if(param->argc <1)
        return ERROR_INVALID_PARAMETER;
    
    if( strlen(param->argv[0]) != 17 )
    {
        printf("EX:%s=00:23:45:67:89:AB\n", ATCMD_EFUSE_WRITE_MAC);
        return ERROR_INVALID_PARAMETER;
    }

    pMac = param->argv[0];

    if(pMac != NULL) 
    {
        ptmp = strtok (pMac, delimiters);
        for(i=0; i<=5; i++) 
        {
            toul_val =  strtoul(ptmp, NULL, 16);
            efuse_mac[i] = toul_val;
            ptmp = strtok (NULL, delimiters);
        }
    }

    printf("Parse mac -> %02X:%02X:%02X:%02X:%02X:%02X\n", efuse_mac[0], efuse_mac[1], efuse_mac[2], efuse_mac[3], efuse_mac[4], efuse_mac[5]);

    if(bEfuseEnableWrite == 0)
    {
        printf("Please run %s first\n", ATCMD_EFUSE_ENABLE_WRITE);
        return ERROR_NOT_IMPLEMENT;
    }
    bEfuseEnableWrite = 0;

    ret = efuse_write_mac(efuse_mac);

    return ret;
}
int At_EfuseReadMAC(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    uint8_t efuse_mac[6] = {0};

    ret = efuse_read_mac(efuse_mac);
    
    if(ret == 0)
    {
        printf("%s=%02X:%02X:%02X:%02X:%02X:%02X\n", ATCMD_EFUSE_READ_MAC, efuse_mac[0], efuse_mac[1], efuse_mac[2], efuse_mac[3], efuse_mac[4], efuse_mac[5]);
     
        //efuse_read_mac_increase(efuse_mac);
        //printf("MAC INCREASE=%02X:%02X:%02X:%02X:%02X:%02X\n", efuse_mac[0], efuse_mac[1], efuse_mac[2], efuse_mac[3], efuse_mac[4], efuse_mac[5]);
    }
    else
    {
        printf("No MAC in Efuse\n");
    }
    
    return ret; 
}    
int At_EfuseDump(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    int len = 0;
    uint8_t efuse_mac[6] = {0};
    uint16_t usb_vid = 0;
    uint16_t usb_pid = 0;
    uint8_t power_tx1 = 0;
    uint8_t power_tx2 = 0;
    uint8_t xtal = 0;

    len = efuse_dump_data();

    printf("efuse free bits = %d\n", efuse_get_free_bits());
    
    if( efuse_read_mac(efuse_mac) == 0 )
    {
        printf("MAC = %02x:%02x:%02x:%02x:%02x:%02x\n", efuse_mac[0], efuse_mac[1], efuse_mac[2], efuse_mac[3], efuse_mac[4], efuse_mac[5]);
    }
    else
    {
        printf("NO MAC\n");
    }

    if( efuse_read_usb_vid(&usb_vid) == 0 )
    {
        printf("VID = 0x%x\n", usb_vid);
    }
    else
    {
        printf("NO VID\n");
    }

    if( efuse_read_usb_pid(&usb_pid) == 0 )
    {
        printf("PID = 0x%x\n", usb_pid);
    }
    else
    {
        printf("NO PID\n");
    }
    
    if( efuse_read_tx_power1(&power_tx1) == 0 )
    {
        printf("TX Power1 = 0x%x\n", power_tx1);
    }
    else
    {
        printf("NO TX Power1\n");
    }
    
    if( efuse_read_tx_power2(&power_tx2) == 0 )
    {
        printf("TX Power1 = 0x%x\n", power_tx2);
    }
    else
    {
        printf("NO TX Power2\n");
    }

     if( efuse_read_xtal(&xtal) == 0 )
    {
        printf("XTAL = 0x%x\n", xtal);
    }
    else
    {
        printf("NO XTAL\n");
    }   
   
    return ret; 
}
//5G
int At_Rf5GBBScale(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	
    
    if(param->argc < 4)
        return ERROR_INVALID_PARAMETER;
  
    if( write_reg_5g_bbscale_5100(atoi(param->argv[0])) != 0)
        return -1;
    if( write_reg_5g_bbscale_5500(atoi(param->argv[1])) != 0)
        return -1;
    if( write_reg_5g_bbscale_5700(atoi(param->argv[2])) != 0)
        return -1;
    if( write_reg_5g_bbscale_5900(atoi(param->argv[3])) != 0)
        return -1;

    return ret;
}
int At_Rf5GBias1(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    if(param->argc < 1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_5g_bias1(strtoul(param->argv[0], NULL, 16));
        
    return ret;
}
int At_Rf5GBias2(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    if(param->argc < 1)
        return ERROR_INVALID_PARAMETER;

    ret = write_reg_5g_bias2(strtoul(param->argv[0], NULL, 16));
        
    return ret;
}
int At_Rf5GTableRT(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(strcmp("?" , param->argv[0])==0)
    {
        printf("%s=%d,%d,%d,%d\n", ATCMD_RF_5GTABLE_RT, ssv_rf_table.rt_5g_config.bbscale_band0
                                                                                           , ssv_rf_table.rt_5g_config.bbscale_band1
                                                                                           , ssv_rf_table.rt_5g_config.bbscale_band2
                                                                                           , ssv_rf_table.rt_5g_config.bbscale_band3);
        return ret;
    }
    
    if(param->argc < 4)
        return ERROR_INVALID_PARAMETER;

    struct st_tempe_5g_table tempe_config = {0};
     
    tempe_config.bbscale_band0 = atoi(param->argv[0]);
    tempe_config.bbscale_band1 = atoi(param->argv[1]);
    tempe_config.bbscale_band2 = atoi(param->argv[2]);
    tempe_config.bbscale_band3 = atoi(param->argv[3]);
    tempe_config.bias1 = ssv_rf_table.rt_5g_config.bias1;
    tempe_config.bias2 = ssv_rf_table.rt_5g_config.bias2;

    if( get_current_tempe_state( ssv_rf_table.low_boundary, ssv_rf_table.high_boundary ) == 0 )
        ret = write_reg_tempe_5g_table(tempe_config);

    if(ret != 0)
        return ret;
        
    memcpy(&ssv_rf_table.rt_5g_config, &tempe_config, sizeof(tempe_config) );    
    save_rf_table_to_flash(); 

    return ret;    
}
int At_Rf5GTableHT(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(strcmp("?" , param->argv[0])==0)
    {
        printf("%s=%d,%d,%d,%d\n", ATCMD_RF_5GTABLE_HT, ssv_rf_table.ht_5g_config.bbscale_band0
                                                                                           , ssv_rf_table.ht_5g_config.bbscale_band1
                                                                                           , ssv_rf_table.ht_5g_config.bbscale_band2
                                                                                           , ssv_rf_table.ht_5g_config.bbscale_band3);
        return ret;
    }
    
    if(param->argc < 4)
        return ERROR_INVALID_PARAMETER;

    struct st_tempe_5g_table tempe_config = {0};
     
    tempe_config.bbscale_band0 = atoi(param->argv[0]);
    tempe_config.bbscale_band1 = atoi(param->argv[1]);
    tempe_config.bbscale_band2 = atoi(param->argv[2]);
    tempe_config.bbscale_band3 = atoi(param->argv[3]);
    tempe_config.bias1 = ssv_rf_table.ht_5g_config.bias1;
    tempe_config.bias2 = ssv_rf_table.ht_5g_config.bias2;

    if( get_current_tempe_state( ssv_rf_table.low_boundary, ssv_rf_table.high_boundary ) == 1 )
        ret = write_reg_tempe_5g_table(tempe_config);

    if(ret != 0)
        return ret;
        
    memcpy(&ssv_rf_table.ht_5g_config, &tempe_config, sizeof(tempe_config) );    
    save_rf_table_to_flash(); 

    return ret;    
}
int At_Rf5GTableLT(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	

    if(strcmp("?" , param->argv[0])==0)
    {
        printf("%s=%d,%d,%d,%d\n", ATCMD_RF_5GTABLE_LT, ssv_rf_table.lt_5g_config.bbscale_band0
                                                                                          , ssv_rf_table.lt_5g_config.bbscale_band1
                                                                                          , ssv_rf_table.lt_5g_config.bbscale_band2
                                                                                          , ssv_rf_table.lt_5g_config.bbscale_band3);
        return ret;
    }
    
    if(param->argc < 4)
        return ERROR_INVALID_PARAMETER;

    struct st_tempe_5g_table tempe_config = {0};
     
    tempe_config.bbscale_band0 = atoi(param->argv[0]);
    tempe_config.bbscale_band1 = atoi(param->argv[1]);
    tempe_config.bbscale_band2 = atoi(param->argv[2]);
    tempe_config.bbscale_band3 = atoi(param->argv[3]);
    tempe_config.bias1 = ssv_rf_table.lt_5g_config.bias1;
    tempe_config.bias2 = ssv_rf_table.lt_5g_config.bias2;

    if( get_current_tempe_state( ssv_rf_table.low_boundary, ssv_rf_table.high_boundary ) == 2 )
        ret = write_reg_tempe_5g_table(tempe_config);

    if(ret != 0)
        return ret;
        
    memcpy(&ssv_rf_table.lt_5g_config, &tempe_config, sizeof(tempe_config) );    
    save_rf_table_to_flash(); 

    return ret; 
}
int At_Rf5GTableBIAS1(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	
    uint32_t rt_bias1 = 0, ht_bias1 = 0, lt_bias1 = 0;

    if(strcmp("?" , param->argv[0])==0)
    {
        printf("%s=0x%x,0x%x,0x%x\n", ATCMD_RF_5GTABLE_BIAS1, ssv_rf_table.rt_5g_config.bias1
                                                                                           , ssv_rf_table.ht_5g_config.bias1
                                                                                           , ssv_rf_table.lt_5g_config.bias1);
        return ret;
    }
    
    if(param->argc < 3)
        return ERROR_INVALID_PARAMETER;

    rt_bias1 = strtoul(param->argv[0], NULL, 16);
    ht_bias1 = strtoul(param->argv[1], NULL, 16);
    lt_bias1 = strtoul(param->argv[2], NULL, 16);
    
    ret = write_reg_5g_three_bias(rt_bias1, ht_bias1, lt_bias1);

    if(ret != 0)
        return ret;

    ssv_rf_table.rt_5g_config.bias1 = rt_bias1;
    ssv_rf_table.ht_5g_config.bias1 = ht_bias1;
    ssv_rf_table.lt_5g_config.bias1 = lt_bias1;
    
    save_rf_table_to_flash(); 

    return ret;    
}
int At_Rf5GTableBIAS2(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;	
    uint32_t rt_bias2 = 0, ht_bias2 = 0, lt_bias2 = 0;

    if(strcmp("?" , param->argv[0])==0)
    {
        printf("%s=0x%x,0x%x,0x%x\n", ATCMD_RF_5GTABLE_BIAS2, ssv_rf_table.rt_5g_config.bias2
                                                                                           , ssv_rf_table.ht_5g_config.bias2
                                                                                           , ssv_rf_table.lt_5g_config.bias2);
        return ret;
    }
    
    if(param->argc < 3)
        return ERROR_INVALID_PARAMETER;

    rt_bias2 = strtoul(param->argv[0], NULL, 16);
    ht_bias2 = strtoul(param->argv[1], NULL, 16);
    lt_bias2 = strtoul(param->argv[2], NULL, 16);
    
    ret = write_reg_5g_three_bias(rt_bias2, ht_bias2, lt_bias2);

    if(ret != 0)
        return ret;

    ssv_rf_table.rt_5g_config.bias2 = rt_bias2;
    ssv_rf_table.ht_5g_config.bias2 = ht_bias2;
    ssv_rf_table.lt_5g_config.bias2 = lt_bias2;
    
    save_rf_table_to_flash(); 

    return ret; 
}
int At_Rf5GBandThreshold(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;
    uint16_t thr_f0, thr_f1, thr_f2;

    if(strcmp("?" , param->argv[0])==0)
    {
        printf("%s=%d,%d,%d\n", ATCMD_RF_5GTABLE_BAND_THRESHOLD, ssv_rf_table.band_f0_threshold, ssv_rf_table.band_f1_threshold, ssv_rf_table.band_f2_threshold);
        return ret;
    }
    
    if(param->argc < 3)
        return ERROR_INVALID_PARAMETER;

    struct st_tempe_5g_table tempe_config = {0};
     
    thr_f0 = strtoul(param->argv[0], NULL, 10);
    thr_f1 = strtoul(param->argv[1], NULL, 10);
    thr_f2 = strtoul(param->argv[2], NULL, 10);
    
    ret = write_reg_5g_band_threshold(thr_f0, thr_f1, thr_f2);

    if(ret != 0)
        return ret;
        
    ssv_rf_table.band_f0_threshold = thr_f0;
    ssv_rf_table.band_f1_threshold = thr_f1;
    ssv_rf_table.band_f2_threshold = thr_f2;
    
    save_rf_table_to_flash(); 

    return ret; 
}

void At_RfSingleTone(stParam *param)
{
    printf("\n");
    int ret = ERROR_SUCCESS;

    
    if(param->argc < 1)
        return ERROR_INVALID_PARAMETER;

    ret = rf_tone_gen(atoi(param->argv[0]));

    return ret; 
}
void scan_cbfunc()
{
    u8 i;

    printf("\nCount:%d\n", getAvailableIndex());
    for(i = 0; i < getAvailableIndex(); i++)
    {
        printf("%2d - name:%32s, rssi:-%2d CH:%2d mac:%02x-%02x-%02x-%02x-%02x-%02x\n", i, ap_list[i].name, ap_list[i].rssi, ap_list[i].channel
            , ap_list[i].mac[0], ap_list[i].mac[1], ap_list[i].mac[2], ap_list[i].mac[3], ap_list[i].mac[4], ap_list[i].mac[5]);
    }
    printf("end\n");
}
void atwificbfunc(WIFI_RSP *msg)
{
    uint8_t dhcpen;
    u8 mac[6];
    uip_ipaddr_t ipaddr, submask, gateway, dnsserver;

    if(msg->wifistatus == 1)
    {
        printf("%s OK\n", ATCMD_CONNECT);
        if(msg->id == 0)
            get_if_config_2("et0", &dhcpen, (u32*)&ipaddr, (u32*)&submask, (u32*)&gateway, (u32*)&dnsserver, mac, 6);
        else
            get_if_config_2("et1", &dhcpen, (u32*)&ipaddr, (u32*)&submask, (u32*)&gateway, (u32*)&dnsserver, mac, 6);
        printf("STA%d:\n", msg->id);
        printf("mac addr        - %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        printf("ip addr         - %d.%d.%d.%d\n", ipaddr.u8[0], ipaddr.u8[1], ipaddr.u8[2], ipaddr.u8[3]);
        printf("netmask         - %d.%d.%d.%d\n", submask.u8[0], submask.u8[1], submask.u8[2], submask.u8[3]);
        printf("default gateway - %d.%d.%d.%d\n", gateway.u8[0], gateway.u8[1], gateway.u8[2], gateway.u8[3]);
        printf("DNS server      - %d.%d.%d.%d\n", dnsserver.u8[0], dnsserver.u8[1], dnsserver.u8[2], dnsserver.u8[3]);


        recordAP();

    }
    else
    {
        printf("%s OK\n", ATCMD_DISCONNECT);
    }
}

int At_Reboot (stParam *param)
{
    int sec = 0;
    if(param->argc >=1 )
        sec = atoi(param->argv[0]);

    printf("\r\n");
    if( sec == 0 )
    {
        printf("System will reboot after %d seconds\n", sec);
        drv_wdt_init();
        drv_wdt_enable(SYS_WDT, 100);
    }
    else
    {
        printf("System will reboot after %d seconds\n", sec);
        drv_wdt_init();
        drv_wdt_enable(SYS_WDT, sec*1000);
    }
    return ERROR_SUCCESS;
}

int At_NetScan (stParam *param)
{
    if((get_DUT_wifi_mode() == DUT_NONE) || (get_DUT_wifi_mode() == DUT_SNIFFER))
        printf("\nPlease run AT+DUT_START=1 first\n");
    
    if(scan_AP(scan_cbfunc))
        return ERROR_INVALID_PARAMETER;
    
    return ERROR_SUCCESS_NO_RSP;
}
int At_Connect2 (stParam *param)
{
    u8 id, *pid;
    int ret;
    printf("\r\n");

    pid = (u8*)param->argv[0];
    id = *pid - 0x30;
    if((param->argc < 1) || (id != 0 && id != 1))
    {
        return ERROR_INVALID_PARAMETER;
    }

    ret = wifi_connect_2(id, atwificbfunc);
    if(ret != 0)
    {
        return ERROR_INVALID_PARAMETER;
    }

    return ERROR_SUCCESS_NO_RSP;
}
int At_Connect (stParam *param)
{
    wifi_connect(atwificbfunc);
    return ERROR_SUCCESS_NO_RSP;
}

int At_ConnectActive (stParam *param)
{
    const char delimiters[] = ":";
    char *pSsid = 0, *pWebkey = 0, *ptmp;
    int ssid_len, keylen = 0;
    int ret;
    int8_t loop_i = 0;
	
    unsigned long int toul_val;
    int i = 0;
       
    if (param->argc < 1) {
        return ERROR_INVALID_PARAMETER;
    }

    pSsid  = param->argv[0];
    pWebkey = param->argv[1];
    ssid_len = strlen (pSsid);

    if(pWebkey)
        keylen = strlen(pWebkey);

    DUT_wifi_start(DUT_STA);
    ret = wifi_connect_active ( pSsid, ssid_len, pWebkey, keylen, atwificbfunc);

    return ret;
}

int At_Disconnect2 (stParam *param)
{
    u8 id, *pid;
    printf("\r\n");

    pid = (u8*)param->argv[0];
    id = *pid - 0x30;
    if((param->argc < 1) || (id != 0 && id != 1))
    {
        return ERROR_INVALID_PARAMETER;
    }

    wifi_disconnect_2(id, atwificbfunc);
    return ERROR_SUCCESS_NO_RSP;
}
int At_Disconnect (stParam *param)
{
    wifi_disconnect(atwificbfunc);
    return ERROR_SUCCESS_NO_RSP;
}
int At_SetWifiConfig2 (stParam *param)
{
    const char delimiters[] = ":";
	char *pid, *pMode= 0, *pSsid = 0, *pWebkey = 0, *pMac = 0, *ptmp;
	int ssid_len, keylen = 0;
	int ret;
	int8_t loop_i = 0;
	uint8_t mac[6];
    u8 id;
	unsigned long int toul_val;
       
	if (param->argc < 3) {
		return ERROR_INVALID_PARAMETER;
	}

	pid = param->argv[0];
	pMode = param->argv[1];
	pSsid = param->argv[2];
	pWebkey = param->argv[3];
	pMac = param->argv[4];
	ssid_len = strlen (pSsid);

    id = *pid - 0x30;
	if (id != 0 && id != 1) {
		return ERROR_INVALID_PARAMETER;
	}

    if(pWebkey)
        keylen = strlen(pWebkey);

	if(pMac != NULL) {
        ptmp = strtok (pMac, delimiters);
		for(loop_i=0;loop_i<=5;loop_i++) {
            toul_val =  strtoul(ptmp, NULL, 16);
			mac[loop_i] = toul_val;
			ptmp = strtok (NULL, delimiters);
		}
	}

    if(pMac)
    {
        ret = set_wifi_config_3(id, (u8*)pSsid, ssid_len, (u8*)pWebkey, keylen, mac, 6, 5);
    }
    else
    {
        ret = set_wifi_config_3(id, (u8*)pSsid, ssid_len, (u8*)pWebkey, keylen, NULL, 0, 5);
    }
        
    if(ret != 0)	
        return ERROR_INVALID_PARAMETER;

	return ERROR_SUCCESS;
}
int At_SetWifiConfig (stParam *param)
{
        const char delimiters[] = ":";
	char *pMode= 0, *pSsid = 0, *pWebkey = 0, *pMac = 0, *ptmp;
	int ssid_len, keylen = 0;
	int ret;
	int8_t loop_i = 0;
	uint8_t mac[6];
	unsigned long int toul_val;
        int i = 0;
       
	if (param->argc < 2) {
		return ERROR_INVALID_PARAMETER;
	}

	pMode = param->argv[0];
	pSsid = param->argv[1];
	pWebkey = param->argv[2];
	pMac = param->argv[3];
	ssid_len = strlen (pSsid);

    if(pWebkey)
        keylen = strlen(pWebkey);

	if(pMac != NULL) {
        ptmp = strtok (pMac, delimiters);
		for(loop_i=0;loop_i<=5;loop_i++) {
            toul_val =  strtoul(ptmp, NULL, 16);
			mac[loop_i] = toul_val;
			ptmp = strtok (NULL, delimiters);
		}
	}
#if 1    
    for(i = 0; i < getAvailableIndex(); i++)
    {
        if( strcmp(ap_list[i].name, pSsid) == 0 )
        {
            if(ap_list[i].security_type == 0 && keylen==0)
            {
                memcpy(mac, ap_list[i].mac, 6);
                pMac = mac;
                break;
            }
            else if(ap_list[i].security_type =! 0 && keylen > 0)
            {
                memcpy(mac, ap_list[i].mac, 6);
                pMac = mac;
                break;
            }
            
        }
    }  
 #endif
 
    if(pMac)
    {
        ret = set_wifi_config((u8*)pSsid, ssid_len, (u8*)pWebkey, keylen, mac, 6);
    }
    else
    {
        ret = set_wifi_config((u8*)pSsid, ssid_len, (u8*)pWebkey, keylen, NULL, 0);
    }
        
    if(ret != 0)	
        return ERROR_INVALID_PARAMETER;

	return ERROR_SUCCESS;
}

int At_ShowConnectAP (stParam *param)
{	
    printf("\r\n");
    if (get_wifi_status() == 1) 
    {
		const int ssid_max_len =32+1;
        u8 ap_ssid[ssid_max_len];
        memset(ap_ssid, 0, ssid_max_len);
        memcpy(ap_ssid, gwifistatus.connAP[0].ssid, gwifistatus.connAP[0].ssid_len>ssid_max_len?ssid_max_len:gwifistatus.connAP[0].ssid_len);
        printf("%s", ap_ssid);
        printf(", ch: %d", gwifistatus.connAP[0].channel);
        printf(", rssi: -%d dBm", gwifistatus.connAP[0].rssi);
        printf(", mac: %x:%x:%x:%x:%x:%x", gwifistatus.connAP[0].mac[0],gwifistatus.connAP[0].mac[1],gwifistatus.connAP[0].mac[2],
                                         gwifistatus.connAP[0].mac[3],gwifistatus.connAP[0].mac[4],gwifistatus.connAP[0].mac[5]);
        printf(", security_type = %s", securityString(gwifistatus.connAP[0].security_type));
        printf("/%s ", securitySubString(gwifistatus.connAP[0].security_subType));
        printf(", %s\r\n", showDataRate(get_current_rate(0)) );
    } else {
        printf("please connect STA0 to ap\r\n");
    }

    if (gwifistatus.opmode == DUT_TWOSTA) 
    {
        if(get_wifi_status_2(1) == 1)
        {
            const int ssid_max_len =32+1;
            u8 ap_ssid[ssid_max_len];
            memset(ap_ssid, 0, ssid_max_len);
            memcpy(ap_ssid, gwifistatus.connAP[1].ssid, gwifistatus.connAP[1].ssid_len>ssid_max_len?ssid_max_len:gwifistatus.connAP[1].ssid_len);
            printf("%s", ap_ssid);
            printf(", ch: %d", gwifistatus.connAP[1].channel);
            printf(", rssi: -%d dBm", gwifistatus.connAP[1].rssi);
            printf(", mac: %x:%x:%x:%x:%x:%x", gwifistatus.connAP[1].mac[1],gwifistatus.connAP[1].mac[1],gwifistatus.connAP[1].mac[2],
                                             gwifistatus.connAP[1].mac[3],gwifistatus.connAP[1].mac[4],gwifistatus.connAP[1].mac[5]);
            printf(", security_type = %s", securityString(gwifistatus.connAP[1].security_type));
            printf("/%s ", securitySubString(gwifistatus.connAP[1].security_subType));
            printf(", %s\r\n", showDataRate(get_current_rate(1)) );
        }
        else
        {
            printf("please connect STA1 to ap\r\n");
        }
    }
    
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_ShowAllAP (stParam *param)
{	
    int i;
    uint8_t apcnt;
    TAG_AP_INFO *aplist;

    apcnt = get_ap_lsit_total_num();
    if (apcnt > 0) 
    {
        aplist = malloc(apcnt * sizeof(TAG_AP_INFO));
        if(aplist)
        {
            get_ap_list(aplist, &apcnt);
            for (i=0; i<apcnt; i++) 
            {
                printf("[%d]%s\n",i, aplist[i].name);
                printf("        ch: %2d", aplist[i].channel);
                printf(", rssi: -%3d dBm", aplist[i].rssi);
                printf(", mac:  %x:%x:%x:%x:%x:%x", aplist[i].mac[0],aplist[i].mac[1],aplist[i].mac[2],
                                                     aplist[i].mac[3],aplist[i].mac[4],aplist[i].mac[5]);
                printf(", security_type = %s", securityString(aplist[i].security_type));
                printf("/%s\r\n", securitySubString(aplist[i].security_subType));
            }
            free(aplist);
        }
    }
    else {
        printf("please scan ap then you can show all ap info\r\n");
    }
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_GetVersion (stParam *param)
{
    printf("\r\n");
    printf(ATRSP_VERSION, version);        
    return ERROR_SUCCESS;
}
int At_GetManufactureInfo (stParam *param)
{
    printf("\r\n");
    printf(ATRSP_MANUFACTURE, "iComm Company");
    return ERROR_SUCCESS;
}
int At_SetIfConfigure2 (stParam *param)
{

    char *pid, *pdhcp = NULL, *pIp = NULL, *pSubmask = NULL, *pGateway = NULL;
    u8 dhcpen, id;
    uip_ip4addr_t ipaddr, submask, gateway, dnsserver;
    int ret;

    if (param->argc < 2) 
    {
        return ERROR_INVALID_PARAMETER;
    }

    pid = param->argv[0];
    pdhcp = param->argv[1];
    pIp = param->argv[2];
    pSubmask = param->argv[3];
    pGateway = param->argv[4];

    id = *pid - 0x30;
    dhcpen = *pdhcp - 0x30;
    if(dhcpen == 0 && param->argc < 5 || (id != 0 && id != 1))
    {
        return ERROR_INVALID_PARAMETER;
    }

    memset(&dnsserver, 0, sizeof(uip_ipaddr_t));
    if (dhcpen == 0) 
    {
        if (ERROR_SUCCESS != ip2int (pIp, &ipaddr)) 
        {
            return ERROR_INVALID_PARAMETER;
        }
	 
        if (ERROR_SUCCESS != ip2int (pSubmask, &submask)) 
        {
            return ERROR_INVALID_PARAMETER;
        }
		
        if (ERROR_SUCCESS != ip2int (pGateway, &gateway)) 
        {
            return ERROR_INVALID_PARAMETER;
        }
    }
    else
    {
        memset(&ipaddr, 0, sizeof(uip_ipaddr_t));
        memset(&submask, 0, sizeof(uip_ipaddr_t));
        memset(&gateway, 0, sizeof(uip_ipaddr_t));
    }

    ret = set_if_config_2(id, dhcpen, ipaddr.u32, submask.u32, gateway.u32, dnsserver.u32);
    
    if(ret != 0)
    {
        return ERROR_INVALID_PARAMETER;
    }
    return ERROR_SUCCESS;
}
int At_SetIfConfigure (stParam *param)
{

    char *pdhcp = NULL, *pIp = NULL, *pSubmask = NULL, *pGateway = NULL;
    u8 dhcpen;
    uip_ip4addr_t ipaddr, submask, gateway, dnsserver;

    if (param->argc < 1) 
    {
        return ERROR_INVALID_PARAMETER;
    }

    pdhcp = param->argv[0];
    pIp = param->argv[1];
    pSubmask = param->argv[2];
    pGateway = param->argv[3];

    dhcpen = *pdhcp - 0x30;
    if(dhcpen == 0 && param->argc < 4)
    {
        return ERROR_INVALID_PARAMETER;
    }

    memset(&dnsserver, 0, sizeof(uip_ipaddr_t));
    if (dhcpen == 0) 
    {
        if (ERROR_SUCCESS != ip2int (pIp, &ipaddr)) 
        {
            return ERROR_INVALID_PARAMETER;
        }
	 
        if (ERROR_SUCCESS != ip2int (pSubmask, &submask)) 
        {
            return ERROR_INVALID_PARAMETER;
        }
		
        if (ERROR_SUCCESS != ip2int (pGateway, &gateway)) 
        {
            return ERROR_INVALID_PARAMETER;
        }
    }
    else
    {
        memset(&ipaddr, 0, sizeof(uip_ipaddr_t));
        memset(&submask, 0, sizeof(uip_ipaddr_t));
        memset(&gateway, 0, sizeof(uip_ipaddr_t));
    }

    set_if_config(dhcpen, ipaddr.u32, submask.u32, gateway.u32, dnsserver.u32);
    
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/	
int At_GetConfigure (stParam *param)
{   
    uint8_t ssid[33], ssidlen = 32, key[65], keylen = 64, mac[6], dhcpen;
    uip_ipaddr_t ipaddr, submask, gateway, dnsserver;

    if(get_wifi_config(ssid, &ssidlen, key, &keylen, mac, 6) == 0)
    {
        ssid[ssidlen] = 0;
        key[keylen] = 0;
    }
    
    get_if_config(&dhcpen, (u32*)&ipaddr, (u32*)&submask, (u32*)&gateway, (u32*)&dnsserver);
    printf("\r\n");
	printf (ATRSP_CONFIGURE, 0, ssid, key, keylen, dhcpen, 
		ipaddr.u8[0], ipaddr.u8[1], ipaddr.u8[2], ipaddr.u8[3],
		submask.u8[0], submask.u8[1], submask.u8[2], submask.u8[3],
		gateway.u8[0], gateway.u8[1], gateway.u8[2], gateway.u8[3]);

	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_GetNetStatus2 (stParam *param)
{
    u8 id, *pid;
    printf("\r\n");

    pid = (u8*)param->argv[0];
    id = *pid - 0x30;
    if((param->argc < 1) || (id != 0 && id != 1))
    {
        return ERROR_INVALID_PARAMETER;
    }

    printf (ATRSP_NETSTATUS2, id, get_wifi_status_2(id));	
    return ERROR_SUCCESS;
}

int At_GetNetStatus (stParam *param)
{
    printf("\r\n");
    printf (ATRSP_NETSTATUS, get_wifi_status());	
    return ERROR_SUCCESS;
}

int At_DUTSTART (stParam *param)
{
    u8 mode, *pmode;
    printf("\r\n");

    if(param->argc < 1)
    {
        return ERROR_INVALID_PARAMETER;
    }
    
    pmode = (u8*)param->argv[0];
    mode = *pmode - 0x30;
    if(mode > 5)
    {
        return ERROR_INVALID_PARAMETER;
    }

    DUT_wifi_start(mode);	
    return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
#ifdef SMARTCONFIG_EN
int At_SmartConfig(stParam *param)
{
    u8 enable, sc_len, key[16];
    u8 *sc = NULL;
    if (param->argc < 1)  {
        printf("\nAT+SC [joylink/ smartlink/ airkiss /rtlink],[xxx]\n");
        printf("ex: \n");
        printf("\tAT+SC joylink,0123456789123456 => start joylink with key_code\n");
        printf("\tAT+SC smartlink,1              => start smartlink\n");
        return ERROR_INVALID_PARAMETER;
    }

    printf("\n");
    sc_len = sizeof(u8)*strlen(param->argv[0]);
    sc = (u8 *) malloc(sc_len);
    memcpy(sc, param->argv[0], sc_len);
    sc[sc_len] = '\0';

    if (!memcmp(sc, "joylink", sc_len)) {
        memcpy(key, param->argv[1], sizeof(key));
        if (strlen(param->argv[1]) == 16) {
            joylink_init(key);    
        } 
        else {
            joylink_stop();    
        }
    }
    else if (!memcmp(sc, "airkiss", sc_len)) {
        memcpy(key, param->argv[1], sizeof(key));
        if (strlen(param->argv[1]) == 16) {
            airkiss_start(key);    
        }
        else {
            airkiss_stop();
        }
    }
#if 1
    else if (!memcmp(sc, "rtlink", sc_len)) {
        memcpy(key, param->argv[1], sizeof(key));
        if (strlen(param->argv[1]) == 16) {
            if (strcmp(param->argv[2],"1") == 0) {
                RTSmntSetBand(1);
            } else if (strcmp(param->argv[2],"2") == 0) {
                RTSmntSetBand(2);
            }
            RTInit(key);    
        }
        else {
            RTStop();    
        } 
    }
#endif
    if (sc) {
        free(sc);
        sc = NULL;    
    }
    
    return ERROR_SUCCESS;
}
#endif

int At_SetAutoConnect(stParam *param)
{
    if (param->argc < 1)  {
        return ERROR_INVALID_PARAMETER;
    }    
    set_auto_connect_flag(atoi(param->argv[0]));
    return ERROR_SUCCESS;
}

#ifdef OTA_EN
int At_OTASetServer (stParam *param)
{
	char *pIp = 0, *pport = 0;

	if (get_wifi_status() != 1)
		return ERROR_WIFI_CONNECTION;

    if (param->argc < 3) {
        return ERROR_INVALID_PARAMETER;
    }

	pIp = param->argv[0];
	pport = param->argv[1];
	
	if (atoi(pport) == 0) 
	{
		return ERROR_INVALID_PARAMETER;
	}

	ota_set_server(atoi(param->argv[2]), pIp, atoi(pport));

	return ERROR_SUCCESS;
}
/*---------------------------------------------------------------------------*/
int At_OTASetParam (stParam *param)
{
	char *pFilename = 0, *pPathname = 0;

	pFilename = param->argv[0];
	pPathname = param->argv[1];

	if(pFilename == 0)
		return 1;
	if(pPathname == NULL)
		ota_set_parameter(pFilename, "");
	else
		ota_set_parameter(pFilename, pPathname);
	return ERROR_SUCCESS;	
}
/*---------------------------------------------------------------------------*/
int At_OTAStart (stParam *param)
{
	ota_update();
	return ERROR_SUCCESS;
}
#endif

#ifdef JD_CLOUD_EN
int At_JDDeviceRemove (stParam *param)
{
    joylink_remove_conf();
    return ERROR_SUCCESS;
}
int At_JDStart (stParam *param)
{
    printf("joylink_main_start\n");
    joylink_main_start();
    OS_MsDelay(1000);
    printf("joylink smartlink start\n");
    joylink_init("9CTGIVN6AEK77EGI");    
    return ERROR_SUCCESS;
}
#endif

#include "user_cfg.h"
#if defined(SUPPORT_UASCENT_USER_CFG)
int At_User_Flsh(stParam *param)
{
	u8 action;
	u8 write_offset = 0;

	if(param->argc == 0 )
	{
		return ERROR_INVALID_PARAMETER;
	}
	printf("\n");
	if( *(param->argv[0]) == 'w' )//write
	{
		action = 1;
		if(param->argc != 3 )
		{
			printf("argc =%d\n",param->argc);
			return ERROR_INVALID_PARAMETER;
		}

		write_offset = atoi(param->argv[1]);
		if( write_offset < 0 || write_offset >3)
		{
			printf("write_offset error %d\n",write_offset);
			return ERROR_INVALID_PARAMETER; 
		}
		if(strlen(param->argv[2]) != RW_BLOCK_SIZE)
		{
			printf("write data Error.Len=%d\n",strlen(param->argv[2]));
			return ERROR_INVALID_PARAMETER; 
		}
	}
	else if( *(param->argv[0]) == 'r')//read
	{
		action = 0;
	}
	return do_At_User_Flsh(action, write_offset ,param->argv[2]);

}
#endif

int At_SetGpioTriState (stParam *param) {
	char *pid = 0;
    int8_t ret = 0;

	pid = param->argv[0];

	if ((param->argc < 1) || (atoi(pid) > 22)) {
        printf("\n");
        printf("Usage   : AT+SET_GPIO_TRI_STATE=<id>\n");
        printf("<id>    : gpio id(0-22)\n");
		return ERROR_INVALID_PARAMETER;
	}
    
    ret = drv_gpio_set_mode(atoi(pid), 1);

    printf("\n");
    printf("gpio_id = %d\n", atoi(pid));
    
	return ERROR_SUCCESS;
}

int At_SetGpioLogic (stParam *param) {
	char *pid = 0, *logic = 0;
    int8_t ret = 0;
    
	pid = param->argv[0];
	logic = param->argv[1];

	if ((param->argc < 2) || (atoi(pid) > 22) || (atoi(logic) > 1)) {
        printf("\n");
        printf("Usage   : AT+SET_GPIO_LOGIC=<id>,<logic>\n");
        printf("<id>    : gpio id(0-22)\n");
        printf("<logic> : 0:low ; 1:high\n");
		return ERROR_INVALID_PARAMETER;
	}

    ret = drv_gpio_set_mode(atoi(pid), 0);
    ret = drv_gpio_set_dir(atoi(pid), 1);
    ret = drv_gpio_set_logic(atoi(pid), atoi(logic));

    printf("\n");
    printf("gpio_id = %d\n", atoi(pid));
    printf("gpio_logic = %d\n", atoi(logic));
    
	return ERROR_SUCCESS;
}

int At_SetGpioPull (stParam *param) {
	char *pid = 0, *gpio_pull = 0;
    int8_t ret = 0;
    
	pid = param->argv[0];
	gpio_pull = param->argv[1];

	if ((param->argc < 2) || (atoi(pid) > 22) || (atoi(gpio_pull) > 2)) {
        printf("\n");
        printf("Usage   : AT+SET_GPIO_PULL=<id>,<pull>\n");
        printf("<id>    : gpio id(0-22)\n");
        printf("<pull>  : 0:down ; 1:up ; 2:none\n");
		return ERROR_INVALID_PARAMETER;
	}
    
    ret = drv_gpio_set_mode(atoi(pid), 0);
    ret = drv_gpio_set_dir(atoi(pid), 0);
    ret = drv_gpio_set_pull(atoi(pid), atoi(gpio_pull));

    printf("\n");
    printf("gpio_id = %d\n", atoi(pid));
    printf("gpio_pull = %d\n", atoi(gpio_pull));
    printf("gpio_input_get = %d\n", drv_gpio_get_logic(atoi(pid)));
    
	return ERROR_SUCCESS;
}

int At_ReadGpio (stParam *param) {
	char *pid = 0;
    int8_t ret = 0;

	pid = param->argv[0];

	if ((param->argc < 1) || (atoi(pid) > 22)) {
        printf("\n");
        printf("Usage   : AT+READ_GPIO=<id>\n");
        printf("<id>    : gpio id(0-22)\n");
		return ERROR_INVALID_PARAMETER;
	}

    ret = drv_gpio_get_logic(atoi(pid));

    printf("\n");
    printf("gpio_id = %d\n", atoi(pid));
    printf("gpio_logic = %d\n", ret);
    
	return ERROR_SUCCESS;
}


uint8_t gpio_intc_id = 0x0;
uint8_t gpio_intc_mode = 0x0;

void irq_test_gpio_ipc(uint32_t irq_num) {
    int8_t ret = 0x0;
    
    ret = drv_gpio_intc_clear(gpio_intc_id);
    
    if(2 == gpio_intc_mode) {
        printf("\nLine = %d\nFunc = %s\nGPIO_INTC_HIGH_LEVEL\n", __LINE__, __FUNCTION__);
    }
    else if(3 == gpio_intc_mode) {
        printf("\nLine = %d\nFunc = %s\nGPIO_INTC_RISING_EDGE\n", __LINE__, __FUNCTION__);
    }
    else if(4 == gpio_intc_mode) { 
        printf("\nLine = %d\nFunc = %s\nGPIO_INTC_LOW_LEVEL\n", __LINE__, __FUNCTION__);
    }
    else {
        printf("\nLine = %d\nFunc = %s\nGPIO_INTC_FALLING_EDGE\n", __LINE__, __FUNCTION__);
    }    
}

int At_SetGpioInt (stParam *param) {
	char *pid = 0, *int_mode = 0;
    int8_t ret = 0;
    
	pid = param->argv[0];
	int_mode = param->argv[1];

	if ((param->argc < 2) || (atoi(pid) > 22) || (atoi(int_mode) > 5) || (atoi(int_mode) < 2)) {
        printf("\n");
        printf("Usage   : AT+SET_GPIO_INT=<id>,<mode>\n");
        printf("<id>    : gpio id(0-22)\n");
        printf("<mode>  : 2:high_level ; 3:rising_edge ; 4:low_level ; 5:falling_edge\n");
		return ERROR_INVALID_PARAMETER;
	}

    gpio_intc_id = atoi(pid);
    gpio_intc_mode = atoi(int_mode);
    
    ret = drv_gpio_set_mode(gpio_intc_id, 0);
	ret = drv_gpio_set_dir(gpio_intc_id, 0);
    ret = drv_gpio_intc_trigger_mode(gpio_intc_id, gpio_intc_mode);
    ret = drv_gpio_register_isr(gpio_intc_id, irq_test_gpio_ipc);

    printf("\n");
    printf("gpio_id = %d\n", gpio_intc_id);
    printf("gpio_interrupt_mode = %d\n", gpio_intc_mode);
    
	return ERROR_SUCCESS;
}

int At_SetPWM (stParam *param) {
	char *pwm_id = 0, *freq_hz = 0, *duty = 0, *is_invert = 0;
    int8_t ret = 0;
    
	pwm_id = param->argv[0];
	freq_hz = param->argv[1];
    duty = param->argv[2];
    is_invert = param->argv[3];

	if ((param->argc < 4) || (atoi(pwm_id) > 4) || (atoi(freq_hz) < 5) || (atoi(freq_hz) > 4000000) || (atoi(duty) > 100) || (atoi(is_invert) > 1)) {
        printf("\n");
        printf("Usage   : AT+SET_PWM=<id>,<freq>,<duty>,<inv>\n");
        printf("<id>    : pwm id(0-4)\n");
        printf("<freq>  : freq_hz(5-4M)\n");
        printf("<duty>  : duty(0-100)\n");
        printf("<inv>   : 0:none ; 1:invert the PWM polarity\n");
		return ERROR_INVALID_PARAMETER;
	}

    ret = drv_pwm_init(atoi(pwm_id));
    ret = drv_pwm_config(atoi(pwm_id), atoi(freq_hz), atoi(duty), atoi(is_invert));
    ret = drv_pwm_enable(atoi(pwm_id));
    
    
    printf("\n");
    printf("pwm_id      = %d\n", atoi(pwm_id));
    printf("freq_hz     = %d\n", atoi(freq_hz));
    printf("duty        = %d\n", atoi(duty));
    printf("is_invert   = %d\n", atoi(is_invert));
    
	return ERROR_SUCCESS;
}

int At_SetPWMDisable (stParam *param) {
	char *pwm_id = 0;
    int8_t ret = 0;

	pwm_id = param->argv[0];

	if ((param->argc < 1) || (atoi(pwm_id) > 4)) {
        printf("\n");
        printf("Usage   : AT+SET_PWM_DISABLE=<id>\n");
        printf("<id>    : pwm_id(0-4)\n");
		return ERROR_INVALID_PARAMETER;
	}

    ret = drv_pwm_disable(atoi(pwm_id));

    printf("\n");
    printf("pwm_id      = %d\n", atoi(pwm_id));
    
	return ERROR_SUCCESS;
}

int At_SetPWMEnable (stParam *param) {
	char *pwm_id = 0;
    int8_t ret = 0;

	pwm_id = param->argv[0];

	if ((param->argc < 1) || (atoi(pwm_id) > 4)) {
        printf("\n");
        printf("Usage   : AT+SET_PWM_ENABLE=<id>\n");
        printf("<id>    : pwm_id(0-4)\n");
		return ERROR_INVALID_PARAMETER;
	}

    ret = drv_pwm_enable(atoi(pwm_id));

    printf("\n");
    printf("pwm_id      = %d\n", atoi(pwm_id));
    
	return ERROR_SUCCESS;
}

int At_SetPWMReconfig (stParam *param) {
	char *pwm_id = 0, *freq_hz = 0, *duty = 0, *is_invert = 0;
    int8_t ret = 0;
    
	pwm_id = param->argv[0];
	freq_hz = param->argv[1];
    duty = param->argv[2];
    is_invert = param->argv[3];

	if ((param->argc < 4) || (atoi(pwm_id) > 4) || (atoi(freq_hz) < 5) || (atoi(freq_hz) > 4000000) || (atoi(duty) > 100) || (atoi(is_invert) > 1)) {
        printf("\n");
        printf("Usage   : AT+SET_PWM_RECONFIG=<id>,<freq>,<duty>,<inv>\n");
        printf("<id>    : pwm id(0-4)\n");
        printf("<freq>  : freq_hz(5-4M)\n");
        printf("<duty>  : duty(0-100)\n");
        printf("<inv>   : 0:none ; 1:invert the PWM polarity\n");
		return ERROR_INVALID_PARAMETER;
	}
    
    ret = drv_pwm_config(atoi(pwm_id), atoi(freq_hz), atoi(duty), atoi(is_invert));
    
    printf("\n");
    printf("pwm_id      = %d\n", atoi(pwm_id));
    printf("freq_hz     = %d\n", atoi(freq_hz));
    printf("duty        = %d\n", atoi(duty));
    printf("is_invert   = %d\n", atoi(is_invert));
    
	return ERROR_SUCCESS;
}

int At_CmdSetCountryCode(stParam *param) 
{
    int ret = ERROR_SUCCESS;	
    if( param->argc < 1 )
    {
        return ERROR_INVALID_PARAMETER;
    }
 
    ret = config_channel(atoi(param->argv[0]));
    return ret;
}

int At_FIXRATE(stParam *param)
{
    int rate;
    u8 wsid=0;

    
    if(param->argc!=2)
    {
        printf("usage:\n");
        printf("AT+FR=[wsid] [rate]\n");
        return ERROR_SUCCESS;    
    }
    
    wsid = atoi(param->argv[0]);

    rate = atoi(param->argv[1]);


    wifi_set_fix_drate(wsid, (u8)rate);
    
	return ERROR_SUCCESS;
}

int At_RCMASK(stParam *param)
{
    u16 rc_mask=0;

    
    if(param->argc!=1)
    {
        printf("usage:\n");
        printf("rc_mask=[rate mask]\n");
        return ERROR_SUCCESS;    
    }
    
    rc_mask = strtoul(param->argv[0], NULL, 16);;



    wifi_set_rc_mask(rc_mask);
    
	return ERROR_SUCCESS;
}


int At_MacHWQueue(stParam *param) 
{
    drv_mac_hw_queue_status();
    return ERROR_SUCCESS;
}

int At_MacHWMIB(stParam *param) 
{
    if(param->argc!=1)
    {
        goto USAGE;        
    }

    if(0==strcmp(param->argv[0],"on"))
    {
        drv_mac_mib_on();
    }
    else if(0==strcmp(param->argv[0],"off"))
    {
        drv_mac_mib_off();
    }
    else if(0==strcmp(param->argv[0],"reset"))
    {
        drv_mac_mib_reset();    
    }
    else if(0==strcmp(param->argv[0],"tx"))
    {
        drv_mac_mib_tx();    
    }
    else if(0==strcmp(param->argv[0],"rx"))
    {
        drv_mac_mib_rx();    
    }
    else if(0==strcmp(param->argv[0],"txrx"))
    {
        drv_mac_mib_tx();    
        drv_mac_mib_rx();        
    }
    else
    {
        goto USAGE;
    }

    return ERROR_SUCCESS;
    
USAGE:
    printf("mib=on\r\n");
    printf("mib=off\r\n");        
    printf("mib=reset\r\n");        
    printf("mib=tx\r\n");        
    printf("mib=rx\r\n");        
    printf("mib=txrx\r\n");        
    return ERROR_SUCCESS;
}

int At_UartFWUpgrade(stParam *param)
{
    sys_uart_fw_upgrade();
    return ERROR_SUCCESS;
}

/*---------------------------------------------------------------------------*/
int At_CmdList (stParam *param);

/*---------------------------------------------------------------------------*/
const at_cmd_info atcmdicomm_info_tbl[] =
{
    {ATCMD_REBOOT ,            At_Reboot,             0},
    {ATCMD_HELP,               At_Help,               0},
#if 1    
    {ATCMD_VERSION,            At_GetVersion,         0},
    {ATCMD_MINFO ,            At_GetManufactureInfo,             0},       
    {ATCMD_NETSCAN,            At_NetScan,            0},
    {ATCMD_SETWIFICONFIG,      At_SetWifiConfig,      0},
    {ATCMD_SETWIFICONFIG2,      At_SetWifiConfig2,      0},
    {ATCMD_GETCONFIG,      At_GetConfigure,      0},                                                 
    {ATCMD_CONNECT,            At_Connect,            0},
    {ATCMD_CONNECT2,            At_Connect2,            0},
    {ATCMD_DISCONNECT,         At_Disconnect,         0},
    {ATCMD_DISCONNECT2,         At_Disconnect2,         0},
    {ATCMD_SETIFCONFIG ,            At_SetIfConfigure,             0},
    {ATCMD_SETIFCONFIG2 ,            At_SetIfConfigure2,             0},
    {ATCMD_NETSTATUS ,            At_GetNetStatus,             0},        
    {ATCMD_NETSTATUS2 ,            At_GetNetStatus2,             0},        
    {ATCMD_SHOWCONNECTAP,      At_ShowConnectAP,      0},
    {ATCMD_SHOWALLAP,          At_ShowAllAP,          0},        
    {ATCMD_DUT_START,          At_DUTSTART,      0},
    {ATCMD_CONNECT_ACTIVE,          At_ConnectActive,      0},    
#endif            
    {ATCMD_TEST_PERIPHERAL,    At_TEST_PERIPHERAL,    0},
    {ATCMD_SYSINFO,            At_SYSINFO,            0},
    {ATCMD_MEMINFO,            At_MEMINFO,            0},
    {ATCMD_MEMDUMP,            At_MEMDUMP,            2},    
    {ATCMD_IPERF,    		   At_IPERF,              0},
    {ATCMD_PING,               At_Ping,               0},    
    {ATCMD_WRITE_REG,          At_WriteReg32,         0},
    {ATCMD_READ_REG,           At_ReadReg32,          2},        
#if 0
//    {ATCMD_MEM_READ,           At_TestMemRead,        0},
    {ATCMD_FLASH_WRITE,        At_TestFlashWrite,     0},
    {ATCMD_FLASH_ERASE,        At_TestFlashErase,     0},
    {ATCMD_FLASH_ERASE32K,        At_TestFlashErase32KB,  0},
    {ATCMD_FLASH_READ,         At_TestFlashRead,      0},
//    {ATCMD_BENCH_FLASH_READ,   At_BenchFlashRead,     0},
//    {ATCMD_BENCH_FLASH_WRITE,  At_BenchFlashWrite,    0},
#endif


//#ifdef FUNC_SOFT_AP     
    {ATCMD_AP_EXIT,            At_AP_EXIT,            0},       //warning: due to current flow, ATCMD_AP_EXIT should always be in front of ATCMD_AP 
    {ATCMD_SET_APCONFIG,       At_SET_APCONFIG,       0},
    {ATCMD_AP,                 At_AP,                 0},
    {ATCMD_GET_APMODE,         At_GET_APMODE,         0},
    {ATCMD_HKAP,               At_HKAP,               0},
    {ATCMD_LIST_STA,         At_ListStation,         0},

#if 0    
    {ATCMD_IPCONF,             At_GetIPconf,          0},
    {ATCMD_APSTATUS,           At_GetApStatus,        0},
    {ATCMD_SOFTAP_STATUS,      At_GetSoftapStatus,    0},
    {ATCMD_WIFISTATUS,         At_GetWifiStatus,      0},
    {ATCMD_MRX,                At_GetMRX,             0},    
    {ATCMD_DBG_LEASETIME,      At_DBG_LEASETIME,      0},
    {ATCMD_GET_APCONFIG,       At_GET_APCONFIG,       0},
    {ATCMD_SET_AP_SSID,        At_SET_AP_SSID,        0},
#endif    

    {ATCMD_TCPCONNECT,         At_TCPConnect,         0},
    {ATCMD_TCPSEND,            At_TCPSend,            2},
#if (LWIP_IPV6==1)     
    {ATCMD_TCPSEND6,            At_TCPSend6,            2},    
#endif
    {ATCMD_TCPDISCONNECT,      At_TCPDisconnect,      0},
    {ATCMD_TCPLISTEN,          At_TCPListen,          0},
    {ATCMD_TCPUNLISTEN,        At_TCPUnlisten,        0},
#if (LWIP_IPV6==1) 
    {ATCMD_TCPLISTEN6,          At_TCPListen6,          0},
#endif
    {ATCMD_UDPCREATE,          At_UDPCreate,          0},
    {ATCMD_UDPSEND,            At_UDPSend,            4},
    {ATCMD_UDPCLOSE,           At_UDPClose,           0},
#if defined(SUPPORT_PARTITION_CFG_TABLE)
    {ATCMD_CFGPARSER,          At_CfgParser,            0},
    {ATCMD_CFG_TESTER,          At_CfgTester,            0},
    {ATCMD_CFG_MAC_TESTER,          At_CfgWifiTester,            0},        
#endif
#if defined(SUPPORT_UASCENT_USER_CFG)
	{ATCMD_USER_FLASH,			At_User_Flsh,		0},
#endif
#if 1	//RF Test ATCMD
    {ATCMD_RF,            At_ShowRfCommand,            0},
    {ATCMD_RADIO_RF_START,            At_RadioRFStart,            2},
    {ATCMD_RADIO_CHANNEL,            At_RadioChannel,            0},
    {ATCMD_RADIO_RF_RATE,            At_RadioRFRate,            1},
    {ATCMD_RADIO_RF_BANDGAIN,            At_RadioRFBandGain,            1},
    {ATCMD_RADIO_RF_GAIN,            At_RadioRFGain,            1},
    {ATCMD_RADIO_RF_RATEGAIN,            At_RadioRFRateGain,            2},
    {ATCMD_RADIO_RF_READ_TEMPCS,            At_RadioRFReadTempcs,            0},
    {ATCMD_RADIO_RF_STOP,            At_RadioRFStop,            0},
    {ATCMD_RADIO_RF_RESET,            At_RadioRFReset,            0},
    {ATCMD_RADIO_RF_COUNT,            At_RadioRFCount,            1},
    {ATCMD_RADIO_RF_ENABLE_TCSR,   At_RadioEnableTCSR,     1},
    {ATCMD_RADIO_RF_DUMP,            At_RadioRFDump,            0},
    
    {ATCMD_RF_CALI_DPD,            At_RfCaliDPD,            1},
    {ATCMD_RF_MANU_DPD,            At_RfManuDPD,            0},        
    {ATCMD_RF_FREQOFFSET,            At_RadioRFFreqoffset,            2},
    {ATCMD_RF_FREQOFFSET_READXO,            At_RadioRFFreqoffset_readxo,            0},
    {ATCMD_RF_FREQOFFSET_READXI,            At_RadioRFFreqoffset_readxi,            0},

    {ATCMD_RADIO_RF_2GPA1,            At_RadioRF2Gpa1,            1},
    {ATCMD_RADIO_RF_2GPA2,            At_RadioRF2Gpa2,            1},
    {ATCMD_RADIO_RF_2GPA3,            At_RadioRF2Gpa3,            1},
    {ATCMD_RADIO_RF_2GPABIAS,            At_RadioRF2GpaBias,            1},
    {ATCMD_RADIO_RF_2GPACELL,            At_RadioRF2GpaCell,            1},
    {ATCMD_RADIO_RF_2GPACAP,            At_RadioRF2GpaCap,            1},    

    {ATCMD_RADIO_RF_LDO,            At_RadioRFLdo,            1},
    {ATCMD_RADIO_RF_DCDC,            At_RadioRFDcdc,            1},
    {ATCMD_RADIO_RF_DCDCV,            At_RadioRFDcdcv,            1},
    {ATCMD_RADIO_RF_DLDOV,            At_RadioRFDldov,            1},
    {ATCMD_RADIO_RF_XOLDO,            At_RadioRFXoldo,            1},    
    {ATCMD_RADIO_RF_RXPAD_CH13,            At_RadioRFRxPadCh13,            1},          

    {ATCMD_RADIO_TEMP_BOUNDARY,            At_RadioRFTempBoundary,            2},
    {ATCMD_RF_TABLE_RT,            At_RfTableRT,            18},
    {ATCMD_RF_TABLE_HT,            At_RfTableHT,            18},
    {ATCMD_RF_TABLE_LT,            At_RfTableLT,            18},
    {ATCMD_RF_TABLE_GAIN,            At_RfTableGain,            18},
    {ATCMD_RF_TABLE_BRATE_GAIN,            At_RfTableBRateGain,            1},
    {ATCMD_RF_TABLE_GRATE_GAIN,            At_RfTableGRateGain,            4},
    {ATCMD_RF_TABLE_20NRATE_GAIN,            At_RfTable20NRateGain,            4},
    {ATCMD_RF_TABLE_40NRATE_GAIN,            At_RfTable40NRateGain,            4},        

    {ATCMD_RADIO_WRITE_MAC1,            At_RadioWriteMAC1,            1},
    {ATCMD_RADIO_READ_MAC1,            At_RadioReadMAC1,            1},
    {ATCMD_RADIO_WRITE_MAC2,            At_RadioWriteMAC2,            1},
    {ATCMD_RADIO_READ_MAC2,            At_RadioReadMAC2,            1},    
    
    {ATCMD_EFUSE_ENABLE_WRITE,            At_EfuseEnableWrite,            0},
    {ATCMD_EFUSE_WRITE_MAC,            At_EfuseWriteMAC,            1},
    {ATCMD_EFUSE_READ_MAC,            At_EfuseReadMAC,            1},        
    {ATCMD_EFUSE_DUMP,            At_EfuseDump,            0},       

    {ATCMD_RF_5G_BB_SCALE,            At_Rf5GBBScale,            0},   
    {ATCMD_RF_5G_BIAS1,            At_Rf5GBias1,            0},   
    {ATCMD_RF_5G_BIAS2,            At_Rf5GBias2,            0},   
    
    {ATCMD_RF_5GTABLE_RT,            At_Rf5GTableRT,            0},   
    {ATCMD_RF_5GTABLE_HT,            At_Rf5GTableHT,            0},   
    {ATCMD_RF_5GTABLE_LT,            At_Rf5GTableLT,            0},    
    {ATCMD_RF_5GTABLE_BIAS1,            At_Rf5GTableBIAS1,            0},      
    {ATCMD_RF_5GTABLE_BIAS2,            At_Rf5GTableBIAS2,            0},          
    {ATCMD_RF_5GTABLE_BAND_THRESHOLD,            At_Rf5GBandThreshold,            0},
    
    {ATCMD_RADIO_RF_SINGLE_TONE,            At_RfSingleTone,            0},
#endif  
#if 1
    {ATCMD_POWERSAVE,          At_POWERSAVE,          0},
#endif
#if 0
    {ATCMD_NSLOOKUP,           At_NSLookup,           0},

    {ATCMD_ENABLE_DCDC,            At_EnableDCDC,            1},    

    {ATCMD_USERSMART_DEMO,     AT_USERSMART_DEMO,     0},
    {ATCMD_WIFIUART_DEMO,      AT_WIFIUART_DEMO,      0},
    {ATCMD_SEND_RAW_INIT,      AT_SendRawInit,        0},
    {ATCMD_SEND_RAW_PACKET,    AT_SendRawPacket,      0},
    {ATCMD_SET_RAW_REMOTE_MAC, AT_SetRawRemoteMac,    0},
    {ATCMD_SET_GPIO,           At_SetGpio,            0},
    {ATCMD_WRITE_GPIO,         At_WriteGpio,          0},
    {ATCMD_READ_GPIO,          At_ReadGpio,           0},
    {ATCMD_SET_PWM,            At_SetPWM,             0},
//    {ATCMD_SET_PWMSPEC,        At_SetPWMspecial,      0},
    {ATCMD_TCPST,           At_tcp_dbg,      0},
#ifdef IPERF_ENABLE    
    {ATCMD_IPERF,        At_iperf_main,      0},
#endif
#ifdef FUNC_CLOUD_ON
    {ATCMD_BUKET_CREATE,      At_BUKET_CREATE,      0},
    {ATCMD_BUKET_DELETE,     At_BUKET_DELETE,     0},
    {ATCMD_OBJECT_DOWNLOAD,     At_OBJECT_DOWNLOAD,     0},
    {ATCMD_OBJECT_UPDATE,    At_OBJECT_UPDATE,    0},
#endif
#endif
#ifdef SSL_EN
#ifdef MBED_EN
    {ATCMD_SSLTEST,            At_SSLTest,            0},
#else
    {ATCMD_SSLCONNECT,         At_SSLConnect,         0},
    {ATCMD_SSLSEND,            At_SSLSend,            0},
    {ATCMD_SSLDISCONNECT,      At_SSLDisconnect,      0},
#endif
#endif
#ifdef SMARTCONFIG_EN
    {ATCMD_SMARTCONFIG,      At_SmartConfig,      0},
#endif

    {ATCMD_SET_AUTO_CONNECT,      At_SetAutoConnect,      1},
#ifdef OTA_EN
    {ATCMD_OTASETSERVER,       At_OTASetServer,       0},
    {ATCMD_OTASETPARAM,        At_OTASetParam,        0},
    {ATCMD_OTASTART,           At_OTAStart,           0},
#endif
#ifdef JD_CLOUD_EN
    {ATCMD_JD_DEVICE_REMOVE,       At_JDDeviceRemove,       0},
    {ATCMD_JD_START,        At_JDStart,        0},
#endif
    {ATCMD_SET_GPIO_TRI_STATE ,At_SetGpioTriState   ,0},
    {ATCMD_SET_GPIO_LOGIC     ,At_SetGpioLogic      ,0},
    {ATCMD_SET_GPIO_PULL      ,At_SetGpioPull       ,0},
    {ATCMD_READ_GPIO          ,At_ReadGpio          ,0},
    {ATCMD_SET_GPIO_INT       ,At_SetGpioInt        ,0},
    {ATCMD_SET_PWM            ,At_SetPWM            ,0},
    {ATCMD_SET_PWM_DISABLE    ,At_SetPWMDisable     ,0},
    {ATCMD_SET_PWM_ENABLE     ,At_SetPWMEnable      ,0},
    {ATCMD_SET_PWM_RECONFIG   ,At_SetPWMReconfig    ,0},
    {ATCMD_FIXRATE            ,At_FIXRATE            ,0},   
    {ATCMD_RC_MASK            ,At_RCMASK           ,0},   
    {ATCMD_SET_COUNTRY_CODE   ,At_CmdSetCountryCode ,0},
    {ATCMD_MAC_HW_QUEUE       ,At_MacHWQueue        ,0},
    {ATCMD_MAC_HW_MIB         ,At_MacHWMIB          ,0},
#if (SETTING_UART_FW_UPGRADE == 0)
    {ATCMD_UART_FW_UPGRADE    ,At_UartFWUpgrade     ,0},
#endif
    {ATCMD_LIST               ,At_CmdList           ,0},
};

/*---------------------------------------------------------------------------*/
int At_ShowRfCommand (stParam *param)
{
    int i = 0;
    printf("\r\n");
    int num = sizeof(atcmdicomm_info_tbl)/sizeof(at_cmd_info);
    for(i = 0;i<num-1;i++)
    {
        if( strncmp(atcmdicomm_info_tbl[i].atCmd, "AT+RF", strlen("AT+RF")) == 0 )
            printf("%s\n", atcmdicomm_info_tbl[i].atCmd);
    }

    return ERROR_SUCCESS;
}
int At_CmdList (stParam *param)
{
    int i = 0;
    printf("\r\n");
    int num = sizeof(atcmdicomm_info_tbl)/sizeof(at_cmd_info);
    for(i = 0;i<num-1;i++)
    {
        if( strncmp(atcmdicomm_info_tbl[i].atCmd, "AT+", strlen("AT+")) == 0 )
            printf("%s\n", atcmdicomm_info_tbl[i].atCmd);
    }
    
    return ERROR_SUCCESS;
}

int At_Parser (char *buff, int len)
{
	int i = 0;
	int	nRet = ERROR_UNKNOWN_COMMAND;
    char cmd[MAX_CMD_LEN], operat = 0; //cmd len 32 should be enough
    stParam param;

#if 0
	memset(atcmd_buffer,0 , CLI_BUFFER_SIZE+1);
	memcpy(atcmd_buffer, buff, len);
	buff = atcmd_buffer;
#endif

#if 1
    buff[len] = 0x0;  //chomp!! replace \r\n with null string
    len++;
#endif


    memset(&param, 0, sizeof(stParam));
    if( (1==len) && (buff[0]=='\r' || buff[0]=='\n')){
        nRet = ERROR_SUCCESS;
        goto exit;
    }
    
	if (0 == len) {
		printf (ATRSP_ERROR, ERROR_INVALID_PARAMETER);
		return ERROR_INVALID_PARAMETER;
	}


    for(i = 0; i < MAX_CMD_LEN; i++)
    {
        if(buff[i] == 0 || buff[i] == '=' || buff[i] == ' ')
        {
            memcpy(cmd, buff, i);
            operat = buff[i];
            cmd[i] = 0;
            break;
        }
    }

    for(i = 0; i < sizeof(atcmdicomm_info_tbl)/sizeof(at_cmd_info); i++)
    {
#if 0    
        printf("atCmd = %s\n", atcmdicomm_info_tbl[i].atCmd);
        printf("buff=%s\n",buff);
        printf("cmd=%s\n",cmd);
        printf("strlen=%d\n",strlen(atcmdicomm_info_tbl[i].atCmd));
#endif
    
        //if(strncmp(atcmdicomm_info_tbl[i].atCmd, cmd, strlen(atcmdicomm_info_tbl[i].atCmd)) == 0)    
        if(strcmp(atcmdicomm_info_tbl[i].atCmd, cmd) == 0)
        {
            if(operat != 0)
                parseBuff2Param(buff + strlen(atcmdicomm_info_tbl[i].atCmd) + 1, &param, atcmdicomm_info_tbl[i].maxargu);

            nRet = atcmdicomm_info_tbl[i].pfHandle(&param);
            goto exit_rsp;
        }
    }
#ifdef WAC_EN
    for(i = 0; i < get_atcmd_homekit_size()/sizeof(at_cmd_info); i++)
    {
        if(strncmp(atcmd_homekit_tbl[i].atCmd, cmd, strlen(atcmd_homekit_tbl[i].atCmd)) == 0)
        {
            if(operat != 0)
                parseBuff2Param(buff + strlen(atcmd_homekit_tbl[i].atCmd) + 1, &param, atcmd_homekit_tbl[i].maxargu);
            nRet = atcmd_homekit_tbl[i].pfHandle(&param);
            goto exit_rsp;
        }
    }
#endif


exit_rsp:
	if (ERROR_SUCCESS > nRet){
		//atcmdprintf (ATRSP_ERROR, nRet);
		printf (ATRSP_ERROR, nRet);
    }else if(ERROR_SUCCESS == nRet)
        At_RespOK(cmd);

exit:
	return nRet;
}

