/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "wifi_provision_internal.h"


#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

int awss_complete_token(char passwd[HAL_MAX_PASSWD_LEN], uint8_t *bssid, uint8_t bssid_len, 
                        uint8_t *token_in, uint8_t token_len, uint8_t token_out[AWSS_TOKEN_LEN])
{
    /*need to complete the token*/
    int ret = 0;
    if(token_len != 0 && token_len < 16 && token_in != NULL) { 
        int org_token_len = 0;
        unsigned char buff[128] = {0};
        unsigned char gen_token[32] = {0};
        uint8_t pwd_len = strlen(passwd);

        if(bssid != NULL) {
            memcpy(buff + org_token_len, bssid, bssid_len);
            org_token_len += bssid_len;
        }

        memcpy(buff + org_token_len, token_in, token_len);
        org_token_len += token_len;
        
        if(pwd_len != 0 && 128 >= pwd_len + org_token_len) {
            memcpy(buff + org_token_len, passwd, pwd_len);
            org_token_len += pwd_len; 
        }
        
        utils_sha256(buff, org_token_len, gen_token);
        memcpy(token_out, gen_token, AWSS_TOKEN_LEN);

    } else if (token_len == AWSS_TOKEN_LEN && token_in != NULL) {
       memcpy(token_out, token_in, AWSS_TOKEN_LEN); 
    } else {
        ret = -1;
    }

    return ret; 
}
int awss_connect(char ssid[HAL_MAX_SSID_LEN], char passwd[HAL_MAX_PASSWD_LEN], uint8_t *bssid, uint8_t bssid_len, 
                uint8_t *token, uint8_t token_len)
{
    unsigned char final_token[AWSS_TOKEN_LEN] = {0};
    unsigned char final_bssid[6] = {0};
    
    uint8_t has_bssid = 1;
    int ret;

    /*need to complete the token*/
    ret = awss_complete_token(passwd, bssid, bssid_len, token, token_len, final_token);

    if(ret == 0) {  
        awss_set_token(final_token);
    }
    
    /*need to complete the bssid */
    if(bssid_len > 0 && bssid_len < 6 && bssid != NULL) {
        if(zc_bssid != NULL) {
            memcpy(final_bssid, zc_bssid, 6); 
        }else {
            has_bssid = 0;
        }
    } else if (bssid_len == 6 && bssid != NULL){
        memcpy(final_bssid, bssid, 6); 
    } else {
        has_bssid = 0;
    }

    return HAL_Awss_Connect_Ap(WLAN_CONNECTION_TIMEOUT_MS, ssid, passwd, 0, 0, has_bssid ? final_bssid : NULL, 0);
}


#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
