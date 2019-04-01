/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

//==============================================================================
//                                INCLUDE FILES
//==============================================================================
//#include <stdint.h>
#include <stdio.h>
#include <string.h>

// HTTPD
#include "os_port.h"
#include "axhttp.h"
#include "cgi_api.h"

// FreeRTOS
//#include "nvdm.h"
//#include "wifi_api.h"

//#include "hal_sys.h"
//==============================================================================
//                                    MACROS
//==============================================================================

//==============================================================================
//                               TYPE DEFINITIONS
//==============================================================================

//==============================================================================
//                               Porting from APSoC
//==============================================================================


//==============================================================================
//                               LOCAL VARIABLES
//==============================================================================


//==============================================================================
//                          LOCAL FUNCTION PROTOTYPES
//==============================================================================
typedef struct cgi_cmd_t {
    char *cmd;
    int (*func)(struct connstruct *cn);
} cgi_cmd;

int CGI_set_sta_info(struct connstruct *cn);
int CGI_set_ap_info(struct connstruct *cn);
int CGI_set_common_info(struct connstruct *cn);
int CGI_sys_reboot(struct connstruct *cn);
const cgi_cmd ps_cgi_cmds[] = {
    { "CMD1", &CGI_set_sta_info},
    { "CMD2", &CGI_set_ap_info},
    { "CMD3", &CGI_sys_reboot},
};




//==============================================================================
//                              EXTERNAL FUNCTIONS
//==============================================================================

//------------------------------------------------------------------------------
// FUNCTION
//
//  CFG_get_str
//
//------------------------------------------------------------------------------
// DESCRIPTION
//      get the config value from the cfgobj list, and format in string
//
//------------------------------------------------------------------------------
// PARAMETERS
//      id : the obj id, val: the return value
//
//------------------------------------------------------------------------------
// RETURN
//     =0 : ok ,  < 0 : not found
//------------------------------------------------------------------------------
int CFG_get_str(int id, char *val)
{
#if 0
    int rc;

    switch (CFG_ID_TYPE(id)) {
        case CFG_TYPE_STR:
            rc = CFG_get(id, val);
            break;

        case CFG_TYPE_MAC: {
            struct ether_addr maca;
            rc = CFG_get(id, &maca);
            sprintf(val , "%s", ether_ntoa(&maca));
        }
        break;

        case CFG_TYPE_INT: {
            int t;
            rc = CFG_get(id, &t);
            sprintf(val, "%d", t);
        }
        break;

        case CFG_TYPE_IP: {
            struct in_addr addr;
            rc = CFG_get(id, &addr);
            sprintf(val , "%s", inet_ntoa(addr));
        }
        break;

        default:
            val[0] = '\0';  // null
            rc = CFG_ETYPE;
            break;
    }
    if (!rc) {
        cfg_log("CFG_get_str: id=%x, str=%s\n", id, val);
    } else {
        val[0] = '\0';    // default is null
    }
#endif
    return 0;
}

//------------------------------------------------------------------------------
// FUNCTION
//
//
// DESCRIPTION
//
//
// PARAMETERS
//
//
// RETURN
//
//
//------------------------------------------------------------------------------
void sc_convert(char *instr)
{
    char *p;

    p = instr;
    while (*p) {
        if ((*p == '"') || (*p == '\'') || (*p == '\\')) {
            memmove(p + 1, p, strlen(p) + 1);
            *p = '\\';
            p++;
        }
        p++;
    }
}

void CGI_var_map(struct connstruct *cn, char *name, int id)
{
    char val[512];
    CFG_get_str(id, val);
    sc_convert(val);
    WEB_printf(cn, "addCfg('%s',0x%08x,'%s');\n", name, id, val);
}

void CGI_var_map_array(struct connstruct *cn, char *name, int id, int num)
{
    char val[512];
    int i;
    for (i = 0; i < num ; i++) {
        val[0] = '\0';
        CFG_get_str(id + i, val);
        sc_convert(val);
        WEB_printf(cn, "addCfg('%s%d',0x%08x,'%s');\n", name, i + 1, id + i, val);
    }
}

char *CGI_put_var(struct connstruct *cn, char *var)
{
    char *val;
    val = WEB_query(cn, var);
    if (val) {
        WEB_printf(cn, "%s", val);
    }
    return val;
}

int CGI_do_cmd(struct connstruct *cn)
{
    int i;
    int rc = -2;
    char *cmd;

    // TODO: whether auth_check?
    printf("[HTTPD] CGI_do_cmd() - cn = %p\n", cn);
    cmd = WEB_query(cn, "CMD");
    if (!cmd) {
        goto bad;
    }

    if (cmd) {
        for (i = 0; i < (sizeof(ps_cgi_cmds) / sizeof(cgi_cmd)) ; i++) {
            if (!strcmp(ps_cgi_cmds[i].cmd, cmd)) {
                if (ps_cgi_cmds[i].func) {
                    rc = (ps_cgi_cmds[i].func)(cn);

                    if (rc < 0) {
                        break;
                    }
                }
            }
        }
    }

bad:

    printf("[HTTPD] CGI_do_cmd() - cgi result=%d\n", rc);
    return rc;
}

int CGI_set_sta_info(struct connstruct *cn)
{
    printf("\n=============CGI_set_sta_info===============\n");
    char *Ssid, *WpaPsk;
    char *key1 = "Ssid";
    char *key2 = "WpaPsk";
    printf("[HTTPD] CGI_set_sta_info()\n");

    if ((Ssid = WEB_query(cn, key1)) == 0) {
        return -1;
    } else {

        //wifi_config_set_ssid(WIFI_PORT_STA, (uint8_t *)Ssid, strlen(Ssid));
    }
    if ((WpaPsk = WEB_query(cn, key2)) == 0) {
        return -1;
    } else {

        //wifi_config_set_wpa_psk_key(WIFI_PORT_STA, (uint8_t *)WpaPsk, strlen(WpaPsk));
    }
  
    //wifi_config_set_security_mode(WIFI_PORT_STA, WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK,  WIFI_ENCRYPT_TYPE_TKIP_AES_MIX);
    printf("[HTTPD] CGI_set_sta_info() - Ssid = %s, Psk = %s\n", Ssid,  WpaPsk);
    return 0;
}

int CGI_set_ap_info(struct connstruct *cn)
{
    printf("\n=============CGI_set_ap_info===============\n");
    char *Ssid, *WpaPsk;
    char *key1 = "Ssid";
    char *key2 = "WpaPsk";
    printf("[HTTPD] CGI_set_ap_info()\n");

    if ((Ssid = WEB_query(cn, key1)) == 0) {
        return -1;
    } else {
        //wifi_config_set_ssid(WIFI_PORT_AP, (uint8_t *)Ssid, strlen(Ssid));
    }

    if ((WpaPsk = WEB_query(cn, key2)) == 0) {
        return -1;
    } else {

        //wifi_config_set_wpa_psk_key(WIFI_PORT_AP, (uint8_t *)WpaPsk, strlen(WpaPsk));
    }

    //wifi_config_set_security_mode(WIFI_PORT_AP, WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK, WIFI_ENCRYPT_TYPE_TKIP_AES_MIX);
    printf("[HTTPD] CGI_set_ap_info() - Ssid = %s, Psk = %s\n", Ssid,  WpaPsk);
    return 0;
}


void CGI_get_ap_ssid(struct connstruct *cn)
{
    printf("[HTTPD] CGI_get_ap_ssid() - cn = %p\n", cn);
    char ap_ssid[33] = {0};
    uint8_t len = sizeof(ap_ssid);
    //wifi_config_get_ssid(WIFI_PORT_STA, (uint8_t *)ap_ssid, (uint8_t *)&len);
    if (strlen(ap_ssid)) {
        WEB_printf(cn, ap_ssid);
    }
}

void CGI_get_ap_passwd(struct connstruct *cn)
{

    printf("[HTTPD] CGI_get_ap_passwd() - cn = %p\n", cn);
    char ap_pwd[65] = {0} ;
    uint8_t pwdlen = 0;
    //wifi_config_get_wpa_psk_key(WIFI_PORT_STA, (uint8_t *)ap_pwd, (uint8_t *)&pwdlen);
    if (strlen(ap_pwd)) {
        WEB_printf(cn, ap_pwd);
    }

}


void CGI_get_mt7687ap_ssid(struct connstruct *cn)
{
    printf("[HTTPD] CGI_get_mt7687ap_ssid() - cn = %p\n", cn);
    char ap_ssid[33] = {0} ;
    uint8_t len = sizeof(ap_ssid);
    //wifi_config_get_ssid(WIFI_PORT_AP, (uint8_t *)ap_ssid, (uint8_t *)&len);
    if (strlen(ap_ssid)) {
        WEB_printf(cn, ap_ssid);
    }

}

void CGI_get_mt7687ap_passwd(struct connstruct *cn)
{

    printf("[HTTPD] CGI_get_mt7687ap_passwd() - cn = %p\n", cn);
    char ap_pwd[65] = {0} ;
    uint8_t pwdlen = 0;
    //wifi_config_get_wpa_psk_key(WIFI_PORT_AP, (uint8_t *)ap_pwd, (uint8_t *)&pwdlen);
    if (strlen(ap_pwd)) {
        WEB_printf(cn, ap_pwd);
    }

}


int CGI_sys_reboot(struct connstruct *cn)
{
    printf("[HTTPD] CGI_config_sys_reload() - cn = %p\n", cn);
    //wifi_config_reload_setting();
    return 0;
}

