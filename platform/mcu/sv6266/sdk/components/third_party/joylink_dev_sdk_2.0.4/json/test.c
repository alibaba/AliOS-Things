/* --------------------------------------------------
 * @file: test.c
 *
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
#include "joylink_json.h"

#if 0 
int joy_test_dev_enable() 
{
    char text1[] = "{\
        \"data\":{\"feedid\": \"1234567890123456\",\
            \"accesskey\": \"25e35c9691ee91296facd5d28a4822bc\",\
            \"server\": [\"live.smart.jd.com:2001\"],\
            \"joylink_server\" : [\"live.smart.jd.com:6001\"],\
            \"tcpaes\": [ \"live.smart.jd.com:2014\"],\
            \"lancon\":1,\
            \"cmd_tran_type\":1,\
            \"opt\":{ \"type\":1,\"std\":1} \
        }\
    }";

    DevEnable_t de;
    bzero(&de, sizeof(de));
    joylink_parse_lan_write_key(&de, text1);
   
    printf("--->feedid:%s\n", de.feedid);
    printf("--->accesskey:%s\n", de.accesskey);
    printf("--->lancon:%d\n", de.lancon);
    printf("--->cmd_tran_type:%d\n", de.cmd_tran_type);
    printf("--->server_num:%d\n", de.server_num);
    int i;
    for(i = 0; i < de.server_num; i++){
        printf("--->server:%s\n", de.server[i]);
    }

    return 0;
}

WIFICtrl_t wc;
int joy_test_dev_ctrl() 
{
    /*char devCtr[] = "{ \"streams\":[ \*/
            /*{\*/
                /*\"stream_id\": \"wifi_24g_info\",\*/
                /*\"value\": {\*/
                    /*\"disabled\": 1,\*/
                    /*\"hidden\": 0,\*/
                    /*\"ssid\": \"huangcm_test\",\*/
                    /*\"encryption\": \"none\",\*/
                    /*\"key\":\"1234567\"\*/
                /*},\*/
                /*\"at\": \"2015-09-22T14:39:41+0800\"\*/
            /*},\*/
            /*{\*/
                /*\"stream_id\": \"wifi_24g\",\*/
                /*\"value\": 1,\*/
                /*\"at\": \"2015-09-22T14:39:41+0800\"\*/
            /*}\*/
        /*]\*/
    /*}";*/

#if 0
    char tttdevCtr[] = "{\"snapshot\":[{\"current_value\":\"0\",\"stream_id\":\"wifi_24g\"},{\"current_value\":\"2\",\"stream_id\":\"wifi_5g\"},{\"current_value\":\"500\",\"stream_id\":\"downspeed\"},{\"current_value\":\"600\",\"stream_id\":\"upspeed\"},{\"at\":\"\",\"current_value\":{\"ssid\":\"tesssid\",\"disable\":0,\"encryption\":\"mixed-psk\",\"hidden\":0,\"key\":\"tessskey\"},\"stream_id\":\"wifi_24g_info\"},{\"at\":\"\",\"current_value\":{\"ssid\":\"5gtesssid\",\"disable\":0,\"encryption\":\"mixed-psk\",\"hidden\":0,\"key\":\"5gtessskey\"},\"stream_id\":\"wifi_5g_info\"},{\"at\":\"2015-09-22T14:39:41+0800\",\"current_value\":[{\"lastest_online_time\":\"2015-09-22 00:00\",\"mac\":\"f8:bc:12:89:b4:ee\",\"device_name\":\"HiWiFi-PC\",\"down\":\"0\",\"ip\":\"192.168.199.126\",\"up\":\"0\"},{\"lastest_online_time\":\"2015-09-22 13:48\",\"mac\":\"d0:22:be:02:40:7d\",\"device_name\":\"黄传民\",\"down\":\"0\",\"ip\":\"192.168.199.197\",\"up\":\"0\"}],\"stream_id\":\"devlist\"}],\"streams\":[{\"current_value\":{\"ssid\":\"tesssideeeeegg\",\"encryption\":\"mixed-psk\",\"key\":\"tessskey\"},\"stream_id\":\"wifi_24g_info\"}]}";
#endif

    char *devCtr = "{\"streams\":[{\"current_value\":{\"ssid\":\"tesssideeeeegg\",\"encryption\":\"mixed-psk\",\"key\":\"tessskey\"},\"stream_id\":\"wifi_24g_info\"},{\"current_value\":1,\"stream_id\":\"wifi_24g\"}]}";
    printf("org msg:%s", devCtr);
    bzero(&wc, sizeof(wc));
    wc._24g.on = -1;
    joylink_dev_parse_wifi_ctrl(&wc, devCtr);
    printf("key:%s\n", wc._24g.key);
    printf("wifi_on:%d\n", wc._24g.on);
    printf("wifi_ssid:%s\n", wc._24g.ssid);
    printf("wifi_enc:%s\n", wc._24g.encryption);

    char buff[1024] = {0};
    packageWIFIInfo(&wc._24g, sizeof(buff), buff, WIFI_INFO_24G);
    printf("buff:%s\n", buff);

    return 0;
}

int joy_test_dev_info() 
{
#if 0
    char *jinfo = "{\
                    \"uuid\":\"uuid1000000\",\
                    \"feedid\":\"feedid1000000\"\
            }";
    char *w24g = "{\
                    \"enabled\":\"1\",\
                    \"type\":\"1\",\
                    \"bssid\":\"11:22:33:44:55:66\",\
                    \"ssid\":\"abc\",\
                    \"passphrase\":\"12345678\"\
            }";

    char *ttwspeed = "{\
                    \"dlspeed\":\"10000\",\
                    \"ulspeed\":\"20000\"\
                    }";

    char *wspeed= "{\"dlspeed \":\"426\" , \"ulspeed \":\"0\"}";
    char *ssubdev = "[\
  {\
    \"hostname\":\"XXX_iphone\",\
    \"mac\":\"C8:3A:35:11:22:33\",\
    \"ip\":\"192.168.0.100\", \
    \"linktype\":\"1\", \
    \"dlspeed\":\"1000\",\
    \"ulspeed\":\"10\",\
    \"lastonlinetime\":\"2015-10-10 10:10\" \
  },\
  {\
    \"hostname\":\"XXX_iphone\",\
    \"mac\":\"C8:3A:35:11:22:33\",\
    \"ip\":\"192.168.0.100\", \
    \"linktype\":\"1\", \
    \"dlspeed\":\"1000\",\
    \"ulspeed\":\"10\",\
    \"lastonlinetime\":\"2015-10-10 10:10\" \
   }\
 ]";

#endif

#if 0
    char *subdev = "[{\"hostname\":\"Unknown\",\"mac\":\"68:F7:28:35:F0:5A\",\"ip\":\"192.168.0.10\",\"linktype\":0,\"dlspeed\":0,\"ulspeed\":0,\"lastonlinetime\":\"2015-10-23 15:50\"},{\"hostname\":\"Unknown\",\"mac\":\"68:F7:28:35:F0:5A\",\"ip\":\"192.168.0.10\",\"linktype\":0,\"dlspeed\":0,\"ulspeed\":0,\"lastonlinetime\":\"2015-10-23 15:50\"}]";
#endif

#if 0
    WIFICtrl_t wifi;
    bzero(&wifi, sizeof(wifi));

    /*jl_parse_24g(&wifi, w24g);*/
    jl_parse_wan_speed(&wifi, wspeed);

    printf("-->key:%s\n", wifi._24g.key);
    printf("-->ssid:%s\n", wifi._24g.ssid);
    printf("-->enable:%d\n", wifi._24g.on);
    printf("-->down:%d\n", wifi.downspeed);
    printf("-->up:%d\n", wifi.upspeed);

    JLPInfo_t jlp;
    bzero(&jlp, sizeof(jlp));
    joylink_parse_jlp(&jlp, jinfo);
    printf("-->feedid:%s\n", jlp.feedid);
    printf("-->uuidd:%s\n", jlp.uuid);

#endif
    /*char *devCtr = "{\"streams\":[{\"current_value\":{\"ssid\":\"tesssideeeeegg\",\"encryption\":\"mixed-psk\",\"key\":\"tessskey\"},\"stream_id\":\"wifi_24g_info\"},{\"current_value\":1,\"stream_id\":\"wifi_24g\"}]}";*/
    /*printf("org msg:%s", devCtr);*/
    /*bzero(&wc, sizeof(wc));*/
    /*wc._24g.on = -1;*/

    /*joylink_dev_parse_wifi_ctrl(&wc, devCtr);*/
    /*printf("key:%s\n", wc._24g.key);*/
    /*printf("wifi_on:%d\n", wc._24g.on);*/
    /*printf("wifi_ssid:%s\n", wc._24g.ssid);*/
    /*printf("wifi_enc:%s\n", wc._24g.encryption);*/

#if 0
    SubDev_t devs[10];
    int ret, i;
    bzero(devs, sizeof(devs));

    extern int joylink_parse_client_list(SubDev_t *sdev, int max, char * pMsg);

    ret = joylink_parse_client_list(devs, 10, subdev);
    for(i = 0; i < ret; i++){
        printf("--dev name:%s\n", devs[i].device_name); 
        printf("--mac:%s\n", devs[i].mac); 
        printf("--time:%s\n", devs[i].lastest_online_time); 
        printf("--up:%d\n", devs[i].up); 
    }

    extern char * joylink_package_client_list_(SubDev_t *sdev, int count);

    char *p =  joylink_package_client_list_(devs, ret);
    printf("___:%s\n", p);


    char *rsp = packageGWInfo("", 0, &wc, p);
    
    printf("___:%s\n", rsp);
#endif
    return 0;
}

void test_wifi_server()
{
    char *devCtr = "{\"streams\":[{\"current_value\":\"{\\\"ssid\\\":\\\"tendau\\\",\\\"encryption\\\":\\\"none\\\"}\",\"stream_id\":\"wifi_24g_info\"}]}";
    printf("org msg:%s\n", devCtr);
    bzero(&wc, sizeof(wc));
    wc._24g.on = -1;

    joylink_dev_parse_wifictrl_from_server(&wc, devCtr);
    printf("key:%s\n", wc._24g.key);
    printf("wifi_on:%d\n", wc._24g.on);
    printf("wifi_ssid:%s\n", wc._24g.ssid);
    printf("wifi_enc:%s\n", wc._24g.encryption);
}

void test_sub_dev()
{
#if 0
    {
        "cmd":"addsd",
            "data":[{"sdevmac": "mac","productuuid": "xxxxxx1","protocol": 1 },
            {"sdevmac": "mac","productuuid": "XXXXXX2","protocol": 1 },
            {"sdevmac": "mac","productuuid": "XXXXXX3","protocol": 1 }
        ]
    }
    }
#endif
    char * sub_add = "{\
        \"cmd\":\"addsd\",\
            \"data\":[{\"sdevmac\": \"mac1\",\"productuuid\": \"xxxxxx1\",\"protocol\": 1 },\
            {\"sdevmac\": \"mac2\",\"productuuid\": \"XXXXXX2\",\"protocol\": 1 },\
            {\"sdevmac\": \"mac3\",\"productuuid\": \"XXXXXX3\",\"protocol\": 1 }\
        ]\
    }";

#if 0
{
    "data":[{
        "feedid": "1234567890123456",  
            "accesskey": "25e35c9691ee91296facd5d28a4822bc", 
            "localkey": "25e35c9691ee91296facd5d28a4822bc", 
            "sdevmac": "XXX…XXX",     
            "productuuid":"XXXXXX1"},
        {
            "feedid": "1234567890123456",  
            "accesskey": "25e35c9691ee91296facd5d28a4822bc", 
            "localkey": "25e35c9691ee91296facd5d28a4822bc", 
            "sdevmac": "XXX…XXX",     
            "productuuid":"XXXXXX2"}
    ]
}
#endif
#if 1
    char *sub_auth= "{\
        \"data\":[{\
            \"feedid\":\"1234567890123456\",\
            \"accesskey\":\"25e35c9691ee91296facd5d28a4822bc\",\
            \"localkey\":\"24e35c9691ee91296facd5d28a4822bc\",\
            \"sdevmac\":\"XXX…XXX\",\
            \"productuuid\":\"XXXXXX1\"},\
        {\
            \"feedid\":\"1234567890123456\",\
            \"accesskey\":\"25e35c9691ee91296facd5d28a4822bc\",\
            \"localkey\":\"25e35c9691ee91296facd5d28a4822bc\",\
            \"sdevmac\":\"XXX…XXX\",\
            \"productuuid\":\"XXXXXX2\"}\
        ]\
    }";
#endif
    char rsp[1400] = {0};
    int add_n, auth_n;
    int len;

    JLDevInfo_t *p_add = joylink_parse_sub_add((uint8_t *)sub_add, &add_n);
    len = joylink_packet_sub_add_rsp(p_add, add_n, rsp);
    printf("len:%d:rsp:%s\n", len, rsp);

    JLDevInfo_t *p_auth = joylink_parse_sub_auth((uint8_t*)sub_auth, &auth_n);
    len = joylink_packet_sub_add_rsp(p_auth, auth_n, rsp);
    printf("len:%d:rsp:%s\n", len, rsp);
}

void
test_pacage_scan()
{   
    JLDevice_t dv;
    dv.jlp.devtype = E_JLDEV_TYPE_GW;
    char * p = joylink_package_scan("hh", 1, 3, &dv);
    if(p != NULL){
        printf("scan:%s\n", p);
    }
}

void
test_joylink_parse_server_ota_order_req()
{
    char pMsg[1024] = "{\"cmd\":\"ota\",\"serial\":1,\"data\":{\"feedid\":\"1234567890123456\",\"productuuid\":\"XXXXXX\",\"version\":1,\"versionname\":\"versionname\",\"crc32\":1234566,\"url\":\"www.baidu.com\"}}";
    printf("%s\n", pMsg);
    JLOtaOrder_t otaOrder;
    bzero(&otaOrder, sizeof(JLOtaOrder_t));
    joylink_parse_server_ota_order_req(&otaOrder, pMsg);
    printf("serial:%d | feedid:%s | productuuid:%s | version:%d | versionname:%s | crc32:%0x | url:%s\n",
        otaOrder.serial, otaOrder.feedid, otaOrder.productuuid, otaOrder.version, otaOrder.versionname, otaOrder.crc32, otaOrder.url);
}

void
test_joylink_parse_server_ota_upload_req()
{
    char *pMsg = "{\"code\":0,\"msg\":\"success\"}";
    JLOtaUploadRsp_t otaUpload;
    bzero(&otaUpload, sizeof(JLOtaUploadRsp_t));
    joylink_parse_server_ota_upload_req(&otaUpload, pMsg);
    printf("code:%d | msg:%s\n", otaUpload.code, otaUpload.msg);
}

void
test_joylink_package_ota_upload()
{
    JLOtaUpload_t otaUpload = {"1234567890123456", "prod", 1, 100};
    char *out = joylink_package_ota_upload(&otaUpload);
    printf("out=%s\n", out);

}
#endif

void
test_act_t()
{
    char *tp = "{\"srv\":\"1.1.1\",\"act\":\"get\", \"param\":[[\"temp\", \"hue\"],[ \[{\"val\":25,\"idx\":1}], [{\"val\":20,\"idx\":1}]]]}";

    char template[20] = {0x01,0x23,0x34,0x45,0x01,0x23,0x34,0x45,0x34,0x45,0x01,0x23,0x34,0x45,0x01,0x23,0x34,0x45,0x34,0x45};
    printf("----tp:%s :%s\n", tp, template);
}

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

void
test_parse_ids()
{
    int num= 0;
    char *ids ="[\"12345\",\"23456\"]";
    char **ret = joylink_parse_ids(ids, &num);
    printf("-----:%d\n",  num);
    int i;
    for(i = 0; i < num; i++){
        printf("----:%s\n", ret[i]);
    }

    char buff[1024] =  {0};
    int offset = 0;
    buff[offset++] = '[';
    for(i = 0; i < num; i++){
        buff[offset++] = '"';
        strcpy(&buff[offset], ret[i]);
        offset += strlen(ret[i]);
        buff[offset++] = '"';
        buff[offset++] = ',';
    }
    buff[offset - 1] = ']';
   
    printf("FFFFFFf:%s\n", buff);

    for(i = 0; i < num; i++){
        if(NULL != ret[i]){
            free(ret[i]);
        }
    }

    if(NULL != ret){
        free(ret);
    }
}

/* Create a bunch of objects as demonstration. */
int main(int arvc , char **argv)
{
    /*joy_test_dev_enable();*/
    /*joy_test_dev_ctrl();*/
    /*char buff[512] = {0};*/
    /*char *p =  packageGWInfo("ok", 0, &wc, buff);*/

    /*printf("packet:%s\n", p);*/
    /*free(p);*/

    /*joy_test_dev_info();*/
    /*joy_test_dev_ctrl();*/
    /*test_wifi_server();*/

    /*test_sub_dev();*/
    /*test_pacage_scan();*/
    /*test_act_t();*/

    test_parse_ids();
    return 0;
}
