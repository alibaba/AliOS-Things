/*
    FreeRTOS V6.0.4 - Copyright (C) 2010 Real Time Engineers Ltd.

    ***************************************************************************
    *                                                                         *
    * If you are:                                                             *
    *                                                                         *
    *    + New to FreeRTOS,                                                   *
    *    + Wanting to learn FreeRTOS or multitasking in general quickly       *
    *    + Looking for basic training,                                        *
    *    + Wanting to improve your FreeRTOS skills and productivity           *
    *                                                                         *
    * then take a look at the FreeRTOS eBook                                  *
    *                                                                         *
    *        "Using the FreeRTOS Real Time Kernel - a Practical Guide"        *
    *                  http://www.FreeRTOS.org/Documentation                  *
    *                                                                         *
    * A pdf reference manual is also available.  Both are usually delivered   *
    * to your inbox within 20 minutes to two hours when purchased between 8am *
    * and 8pm GMT (although please allow up to 24 hours in case of            *
    * exceptional circumstances).  Thank you for your support!                *
    *                                                                         *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    ***NOTE*** The exception to the GPL is included to allow you to distribute
    a combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public 
    License and the FreeRTOS license exception along with FreeRTOS; if not it 
    can be viewed here: http://www.freertos.org/a00114.html and also obtained 
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/*
    Implements a simplistic WEB server.  Every time a connection is made and
    data is received a dynamic page that shows the current TCP/IP statistics
    is generated and returned.  The connection is then closed.

    This file was adapted from a FreeRTOS lwIP slip demo supplied by a third
    party.
*/

/* ------------------------ System includes ------------------------------- */


/* ------------------------ FreeRTOS includes ----------------------------- */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* ------------------------ lwIP includes --------------------------------- */
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/ip.h"
#include "lwip/memp.h"
#include "lwip/stats.h"
#include "netif/loopif.h"

/* ------------------------ Project includes ------------------------------ */
#include <string.h>
#include "main.h"

#include "webserver.h"
#include "wlan_intf.h"


#define CONFIG_READ_FLASH	1


#ifdef CONFIG_READ_FLASH

#ifndef CONFIG_PLATFORM_AMEBA_X

#include <flash/stm32_flash.h>
#if defined(STM32F2XX)
#include <stm32f2xx_flash.h>
#elif defined(STM32F4XX)	 
#include <stm32f4xx_flash.h>
#elif defined(STM32f1xx)	 
#include <stm32f10x_flash.h>
#endif

#else
#include "flash_api.h"
#include "device_lock.h"
#define DATA_SECTOR     AP_SETTING_SECTOR
#define BACKUP_SECTOR	(0x00008000)

#endif
#endif
/* ------------------------ Defines --------------------------------------- */
/* The size of the buffer in which the dynamic WEB page is created. */
#define webMAX_PAGE_SIZE       (3200/* 2800 */) /*FSL: buffer containing array*/
#define LOCAL_BUF_SIZE        800
#define AP_SETTING_ADDR			AP_SETTING_SECTOR
/* Standard GET response. */
#define webHTTP_OK  "HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n"

/* The port on which we listen. */
#define webHTTP_PORT            ( 80 )

/* Delay on close error. */
#define webSHORT_DELAY          ( 10 )

#define USE_DIV_CSS 1

#if USE_DIV_CSS

/* Format of the dynamic page that is returned on each connection. */
#define webHTML_HEAD_START \
"<html>\
<head>\
"
/*
<meta http-equiv=\"Content-Type\" content=\"text/html;charset=gb2312>\
<meta http-equiv=\"Cache-Control\" CONTENT=\"no-cache\">\
<meta http-equiv=\"Expires\" CONTENT=\"0\">\
*/

#define webHTML_TITLE \
"<title>Realtek SoftAP Config UI</title>"

#define webHTML_BODY_START \
"</head>\
<body  onLoad=\"onChangeSecType()\">\
<form method=\"post\" onSubmit=\"return onSubmitForm()\" accept-charset=\"utf-8\">\
<div class=\"wrapper\">\
<div class=\"header\">\
Realtek SoftAP Configuration\
</div>"



#define webHTML_CSS \
"<style>\
body {\
text-align:center;\
font-family: 'Segoe UI';\
}\
.wrapper {\
text-align:left;\
margin:0 auto;\
margin-top:200px;\
border:#000;\
width:500px;\
}\
.header {\
background-color:#CF9;\
font-size:18px;\
line-height:50px;\
text-align:center;\
}\
.oneline {\
width:100%;\
border-left:#FC3 10px;\
font-size:15px;\
height:30px;\
margin-top:3px;\
}\
.left {\
background-color:#FF0;\
line-height:30px;\
height:100%;\
width:40%;\
float:left;\
padding-left:20px;\
}\
.right {\
margin-left:20px;\
}\
\
.box {\
width:40%;\
height:28px;\
margin-left:20px;\
\
}\
\
.btn {\
background-color:#CF9;\
height:40px;\
text-align:center;\
}\
\
.btn input {\
font-size:16px;\
height:30px;\
width:150px;\
border:0px;\
line-height:30px;\
margin-top:5px;\
border-radius:20px;\
background-color:#FFF;\
}\
.btn input:hover{\
cursor:pointer;\
background-color:#FB4044;\
}\
\
.foot {\
text-align:center;\
font-size:15px;\
line-height:20px;\
border:#CCC;\
}\
#pwd {\
display:none;\
}\
</style>"



#define webHTML_END \
" <div class=\"oneline btn\">\
<input  type=\"submit\" value=\"Submit\">\
</div>\
<div class=\"oneline foot\">\
Copyright &copy;realtek.com\
</div>\
 </div>\
 </form>\
</body>\
</html>\
"

#define webWaitHTML_START \
"<html location.href='wait.html'>\
<head>\
"
#define webWaitHTML_END \
"</head>\
<BODY>\
<p>\
<h2>SoftAP is now restarting!</h2>\
<h2>Please wait a moment and reconnect!</h2>\
</p>"\
"</BODY>\r\n" \
"</html>"

#define onChangeSecType \
"<script>\
function onChangeSecType()\
{\
x=document.getElementById(\"sec\");\
y=document.getElementById(\"pwd\");\
if(x.value == \"open\"){\
y.style.display=\"none\";\
}else{\
y.style.display=\"block\";\
}\
}\
</script>"

#define onSubmitForm \
"<script>\
function onSubmitForm()\
{\
x=document.getElementById(\"Ssid\");\
y=document.getElementById(\"pwd\");\
z=document.getElementById(\"pwd_val\");\
if(x.value.length>32)\
{\
alert(\"SoftAP SSID is too long!(1-32)\");\
return false;\
}\
/*if(!(/^[A-Za-z0-9]+$/.test(x.value)))\
{\
alert(\"SoftAP SSID can only be [A-Za-z0-9]\");\
return false;\
}*/\
if(y.style.display == \"block\")\
{\
if((z.value.length < 8)||(z.value.length>64))\
{\
alert(\"Password length is between 8 to 64\");\
return false;\
}\
}\
}\
</script>"



#else


/* Format of the dynamic page that is returned on each connection. */
#define webHTML_HEAD_START \
"<html>\
<head>\
"
/*
<meta http-equiv=\"Content-Type\" content=\"text/html;charset=gb2312>\
<meta http-equiv=\"Cache-Control\" CONTENT=\"no-cache\">\
<meta http-equiv=\"Expires\" CONTENT=\"0\">\
*/

#define webHTML_BODY_START \
"</head>\
<BODY onLoad=\"onChangeSecType()\">\
\r\n\r\n<form name=\"form\" method=\"post\" onsubmit=\"return onSubmitForm()\" accept-charset=\"utf-8\">\
<table width=\"500\">\
<tr>\
<td colspan=\"2\" style=\"background-color:#FFA500;text-align:center;\">\
<h2>Realtek SoftAP Configuration</h2>\
</td>\
</tr>"

#define webHTML_END \
"<tr>\
<td colspan=\"2\" style=\"background-color:#FFD700;text-align:center;height:40px\">\
<input type=\"submit\" value=\"Submit\"><br></td>\
</tr>\
<tr>\
<td colspan=\"2\" style=\"background-color:#FFA500;text-align:center;\">\
Copyright ?realtek.com</td>\
</tr>\
</table>\
\r\n</form>" \
"</BODY>\r\n" \
"</html>"

#define webWaitHTML_START \
"<html location.href='wait.html'>\
<head>\
"
#define webWaitHTML_END \
"</head>\
<BODY>\
<p>\
<h2>SoftAP is now restarting!</h2>\
<h2>Please wait a moment and reconnect!</h2>\
</p>"\
"</BODY>\r\n" \
"</html>"

#define onChangeSecType \
"<script>\
function onChangeSecType()\
{\
x=document.getElementById(\"sec\");\
y=document.getElementById(\"pwd_row\");\
if(x.value == \"open\"){\
y.style.display=\"none\";\
}else{\
y.style.display=\"block\";\
}\
}\
</script>"

#define onSubmitForm \
"<script>\
function onSubmitForm()\
{\
x=document.getElementById(\"Ssid\");\
y=document.getElementById(\"pwd_row\");\
z=document.getElementById(\"pwd\");\
if(x.value.length>32)\
{\
alert(\"SoftAP SSID is too long!(1-32)\");\
return false;\
}\
if(!(/^[A-Za-z0-9]+$/.test(x.value)))\
{\
alert(\"SoftAP SSID can only be [A-Za-z0-9]\");\
return false;\
}\
if(y.style.display == \"block\")\
{\
if((z.value.length < 8)||(z.value.length>64))\
{\
alert(\"Password length is between 8 to 64\");\
return false;\
}\
}\
}\
</script>"




#endif




/*
alert(\"Please enter your password!\");\
return false;\
}\
if(z.value.length < 8)\
{\
alert(\"Your password is too short!(8-64)\");\
return false;\
}\
if(z.value.length>64)\
{\
alert(\"Your password is too long!(8-64)\");\
*/

#define MAX_SOFTAP_SSID_LEN      32
#define MAX_PASSWORD_LEN          64
#define MAX_CHANNEL_NUM             13

#if INCLUDE_uxTaskGetStackHighWaterMark
	static volatile unsigned portBASE_TYPE uxHighWaterMark_web = 0;
#endif

/* ------------------------ Prototypes ------------------------------------ */
static void     vProcessConnection( struct netconn *pxNetCon );

/*------------------------------------------------------------------------------*/
/*                            GLOBALS                                          */
/*------------------------------------------------------------------------------*/
rtw_wifi_setting_t wifi_setting = {RTW_MODE_NONE, {0}, 0, RTW_SECURITY_OPEN, {0}};




#ifndef WLAN0_NAME
  #define WLAN0_NAME		"wlan0"
#endif

#ifndef WLAN1_NAME
  #define WLAN1_NAME      	"wlan1"
#endif 

static void LoadWifiSetting()
{
    const char *ifname = WLAN0_NAME;

    if(rltk_wlan_running(WLAN1_IDX))
    {//STA_AP_MODE
    	ifname = WLAN1_NAME;
    }

    wifi_get_setting(ifname, &wifi_setting);

    //printf("\r\nLoadWifiSetting(): wifi_setting.ssid=%s\n", wifi_setting.ssid); 
    //printf("\r\nLoadWifiSetting(): wifi_setting.channel=%d\n", wifi_setting.channel);
    //printf("\r\nLoadWifiSetting(): wifi_setting.security_type=%d\n", wifi_setting.security_type); 
    //printf("\r\nLoadWifiSetting(): wifi_setting.password=%s\n", wifi_setting.password); 
}

#if CONFIG_READ_FLASH
#ifndef CONFIG_PLATFORM_AMEBA_X
void LoadWifiConfig()
{
    rtw_wifi_config_t local_config;
    uint32_t address;
#ifdef STM32F10X_XL
    address = 0x08080000;   //bank2 domain
#else
    uint16_t sector_nb = FLASH_Sector_11;
    address = flash_SectorAddress(sector_nb);
#endif
    printf("\r\nLoadWifiConfig(): Read from FLASH!\n"); 
    flash_Read(address, (char *)&local_config, sizeof(local_config));
    
    printf("\r\nLoadWifiConfig(): local_config.boot_mode=0x%x\n", local_config.boot_mode); 
    printf("\r\nLoadWifiConfig(): local_config.ssid=%s\n", local_config.ssid); 
    printf("\r\nLoadWifiConfig(): local_config.channel=%d\n", local_config.channel);
    printf("\r\nLoadWifiConfig(): local_config.security_type=%d\n", local_config.security_type); 
    printf("\r\nLoadWifiConfig(): local_config.password=%s\n", local_config.password);

    if(local_config.boot_mode == 0x77665502)
    {
        wifi_setting.mode = RTW_MODE_AP;
        if(local_config.ssid_len > 32)
            local_config.ssid_len = 32;
        memcpy(wifi_setting.ssid, local_config.ssid, local_config.ssid_len);
        wifi_setting.ssid[local_config.ssid_len] = '\0';
        wifi_setting.channel = local_config.channel;
        wifi_setting.security_type = local_config.security_type;
        if(local_config.password_len > 64)
            local_config.password_len = 64;
        memcpy(wifi_setting.password, local_config.password, local_config.password_len);
        wifi_setting.password[local_config.password_len] = '\0';
    }
    else
    {
        LoadWifiSetting();
    }
}

int StoreApInfo()
{
	rtw_wifi_config_t wifi_config;
	uint32_t address;
#ifdef STM32F10X_XL
	address = 0x08080000;	//bank2 domain
#else
	uint16_t sector_nb = FLASH_Sector_11;
	address = flash_SectorAddress(sector_nb);
#endif
	// clean wifi_config first
	memset(&wifi_config, 0x00, sizeof(rtw_wifi_config_t));

	wifi_config.boot_mode = 0x77665502;
	memcpy(wifi_config.ssid, wifi_setting.ssid, strlen((char*)wifi_setting.ssid));
	wifi_config.ssid_len = strlen((char*)wifi_setting.ssid);
	wifi_config.security_type = wifi_setting.security_type;
	memcpy(wifi_config.password, wifi_setting.password, strlen((char*)wifi_setting.password));
	wifi_config.password_len= strlen((char*)wifi_setting.password);
	wifi_config.channel = wifi_setting.channel;

	printf("\n\rWritting boot mode 0x77665502 and Wi-Fi setting to flash ...");
#ifdef STM32F10X_XL
	FLASH_ErasePage(address);
#else
	flash_EraseSector(sector_nb);
#endif
	flash_Wrtie(address, (char *)&wifi_config, sizeof(rtw_wifi_config_t));

	return 0;
}


#else

void LoadWifiConfig()
{


    flash_t flash;

    rtw_wifi_config_t local_config;
    uint32_t address;

    address = DATA_SECTOR;

    
    //memset(&local_config,0,sizeof(rtw_wifi_config_t));
    printf("\r\nLoadWifiConfig(): Read from FLASH!\n"); 
   // flash_Read(address, &local_config, sizeof(local_config));

    device_mutex_lock(RT_DEV_LOCK_FLASH);
    flash_stream_read(&flash, address, sizeof(rtw_wifi_config_t),(uint8_t *)(&local_config));
	device_mutex_unlock(RT_DEV_LOCK_FLASH);

    printf("\r\nLoadWifiConfig(): local_config.boot_mode=0x%x\n", local_config.boot_mode); 
    printf("\r\nLoadWifiConfig(): local_config.ssid=%s\n", local_config.ssid); 
    printf("\r\nLoadWifiConfig(): local_config.channel=%d\n", local_config.channel);
    printf("\r\nLoadWifiConfig(): local_config.security_type=%d\n", local_config.security_type); 
    printf("\r\nLoadWifiConfig(): local_config.password=%s\n", local_config.password);

    if(local_config.boot_mode == 0x77665502)
    {
        wifi_setting.mode = RTW_MODE_AP;
        if(local_config.ssid_len > 32)
            local_config.ssid_len = 32;
        memcpy(wifi_setting.ssid, local_config.ssid, local_config.ssid_len);
        wifi_setting.ssid[local_config.ssid_len] = '\0';
        wifi_setting.channel = local_config.channel;
        if(local_config.security_type == 1)
          wifi_setting.security_type = RTW_SECURITY_WPA2_AES_PSK;
        else
          wifi_setting.security_type = RTW_SECURITY_OPEN;
        if(local_config.password_len > 64)
            local_config.password_len = 64;
        memcpy(wifi_setting.password, local_config.password, local_config.password_len);
        wifi_setting.password[local_config.password_len] = '\0';
    }
    else
    {
        LoadWifiSetting();
    }

}

int StoreApInfo()
{

    flash_t flash;

    rtw_wifi_config_t wifi_config;
    uint32_t address;
    uint32_t data,i = 0;

    // clean wifi_config first
    memset(&wifi_config, 0x00, sizeof(rtw_wifi_config_t));

    address = DATA_SECTOR;

    wifi_config.boot_mode = 0x77665502;
    memcpy(wifi_config.ssid, wifi_setting.ssid, strlen((char*)wifi_setting.ssid));
    wifi_config.ssid_len = strlen((char*)wifi_setting.ssid);
    wifi_config.security_type = wifi_setting.security_type;
    if(wifi_setting.security_type !=0)
        wifi_config.security_type = 1;
    else
        wifi_config.security_type = 0;
    memcpy(wifi_config.password, wifi_setting.password, strlen((char*)wifi_setting.password));
    wifi_config.password_len= strlen((char*)wifi_setting.password);
    wifi_config.channel = wifi_setting.channel;
    printf("\n\rWritting boot mode 0x77665502 and Wi-Fi setting to flash ...");
    //printf("\n\r &wifi_config = 0x%x",&wifi_config);

   flash_read_word(&flash,address,&data);

   
    if(data == ~0x0){
	  device_mutex_lock(RT_DEV_LOCK_FLASH);
      flash_stream_write(&flash, address,sizeof(rtw_wifi_config_t), (uint8_t *)&wifi_config);
	  device_mutex_unlock(RT_DEV_LOCK_FLASH);
    }else{
    //flash_EraseSector(sector_nb);
      
        device_mutex_lock(RT_DEV_LOCK_FLASH);
        flash_erase_sector(&flash,BACKUP_SECTOR);
        for(i = 0; i < 0x1000; i+= 4){
            flash_read_word(&flash, DATA_SECTOR + i, &data);
            if(i < sizeof(rtw_wifi_config_t))
            {
                 memcpy(&data,(char *)(&wifi_config) + i,4);
                 //printf("\n\r Wifi_config + %d = 0x%x",i,(void *)(&wifi_config + i));
                 //printf("\n\r Data = %d",data);
            }
            flash_write_word(&flash, BACKUP_SECTOR + i,data);
        }
        flash_read_word(&flash,BACKUP_SECTOR + 68,&data);
        //printf("\n\r Base + BACKUP_SECTOR + 68 wifi channel = %d",data);
        //erase system data
        flash_erase_sector(&flash, DATA_SECTOR);
        //write data back to system data
        for(i = 0; i < 0x1000; i+= 4){
            flash_read_word(&flash, BACKUP_SECTOR + i, &data);
            flash_write_word(&flash, DATA_SECTOR + i,data);
        }
                  //erase backup sector
           flash_erase_sector(&flash, BACKUP_SECTOR);
        }
        device_mutex_unlock(RT_DEV_LOCK_FLASH);
	//flash_Wrtie(address, (char *)&wifi_config, sizeof(rtw_wifi_config_t));
	//flash_stream_write(&flash, address,sizeof(rtw_wifi_config_t), (uint8_t *)&wifi_config);
	//flash_stream_read(&flash, address, sizeof(rtw_wifi_config_t),data);
        //flash_stream_read(&flash, address, sizeof(rtw_wifi_config_t),data);
	//printf("\n\r Base + 0x000FF000 +4  wifi config  = %s",data[4]);
        //printf("\n\r Base + 0x000FF000 +71 wifi channel = %d",data[71]);

	return 0;
}

int EraseApinfo(){
	flash_t flash;
	uint32_t address;

	address = DATA_SECTOR;
	device_mutex_lock(RT_DEV_LOCK_FLASH);
	flash_erase_sector(&flash, address);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
	return 0;
}
#endif

#endif

static void RestartSoftAP()
{
	//printf("\r\nRestartAP: ssid=%s", wifi_setting.ssid);
	//printf("\r\nRestartAP: ssid_len=%d", strlen((char*)wifi_setting.ssid));
	//printf("\r\nRestartAP: security_type=%d", wifi_setting.security_type);
	//printf("\r\nRestartAP: password=%s", wifi_setting.password);
	//printf("\r\nRestartAP: password_len=%d", strlen((char*)wifi_setting.password));
	//printf("\r\nRestartAP: channel=%d\n", wifi_setting.channel);
	wifi_restart_ap(wifi_setting.ssid,
					wifi_setting.security_type,
					wifi_setting.password,
					strlen((char*)wifi_setting.ssid),
					strlen((char*)wifi_setting.password),
					wifi_setting.channel);
}


u32 web_atoi(char* s)
{
	int num=0,flag=0;
	int i;

	for(i=0;i<=strlen(s);i++)
	{
		if(s[i] >= '0' && s[i] <= '9')
			num = num * 10 + s[i] -'0';
		else if(s[0] == '-' && i==0) 
			flag =1;
		else 
			break;
	}

	if(flag == 1)
		num = num * -1;

	return(num); 
}

static void CreateSsidTableItem(char *pbuf, u8_t *ssid, u8_t ssid_len)
{
    char local_ssid[MAX_SOFTAP_SSID_LEN+1];

    if(ssid_len > MAX_SOFTAP_SSID_LEN)
        ssid_len = MAX_SOFTAP_SSID_LEN;
    memcpy(local_ssid, ssid, ssid_len);
    local_ssid[ssid_len] = '\0';

#if USE_DIV_CSS
    sprintf(pbuf, "<div class=\"oneline\"><div class=\"left\">SoftAP SSID:</div> <div class=\"right\">" \
			"<input class=\"box\" type=\"text\" name=\"Ssid\" id=\"Ssid\" value=\"%s\"></div></div>",
                        local_ssid);
#else
    sprintf(pbuf, "<tr>"
                        "<td style=\"background-color:#FFD700;width:100px;\">"
                        "<b>SoftAP SSID:</b><br>"
                        "</td>"
                        "<td style=\"background-color:#eeeeee;height:30px;width:400px;\">"
                        "<input type=\"text\" name=\"Ssid\" id=\"Ssid\" value=\"%s\"><br>"
                        "</td>"
                        "</tr>", 
                        local_ssid);

#endif
    //printf("\r\nstrlen(SsidTableItem)=%d\n", strlen(pbuf));	
}

static void CreateSecTypeTableItem(char *pbuf, u32_t sectype)
{
    u8_t flag[2] = {0, 0};

    if(sectype == RTW_SECURITY_OPEN)
	flag[0] = 1;
    else if(sectype == RTW_SECURITY_WPA2_AES_PSK)
        flag[1] = 1;
    else	
        return;

#if USE_DIV_CSS
    sprintf(pbuf, "<div class=\"oneline\"><div class=\"left\">Security Type: </div><div class=\"right\">"\
             	"<select  class=\"box\" name=\"Security Type\"  id=\"sec\" onChange=onChangeSecType()>"\
             	"<option value=\"open\" %s>OPEN</option><option value=\"wpa2-aes\" %s>WPA2-AES</option>"\
             	"</select></div></div>",
                        flag[0]?"selected":"",
                        flag[1]?"selected":"");
#else
    sprintf(pbuf, "<tr>"
                        "<td style=\"background-color:#FFD700;width:100px;\">"
                        "<b>Security Type:</b><br>"
                        "</td>"
                        "<td style=\"background-color:#eeeeee;height:30px;\">"
                        "<select name=\"Security Type\"  id=\"sec\" onChange=onChangeSecType()>"
                        "<option value=\"open\" %s>OPEN</option>"
                        "<option value=\"wpa2-aes\" %s>WPA2-AES</option>"
                        "</select>"
                        "</td>"
                        "</tr>",
                        flag[0]?"selected":"",
                        flag[1]?"selected":"");


#endif

    //printf("\r\nstrlen(SecTypeTableItem)=%d\n", strlen(pbuf));	
}

static void CreatePasswdTableItem(char *pbuf, u8_t *password, u8_t passwd_len)
{
    char local_passwd[MAX_PASSWORD_LEN+1];

    if(passwd_len > MAX_PASSWORD_LEN)
        passwd_len = MAX_PASSWORD_LEN;
    if(passwd_len > 0)
    {
        memcpy(local_passwd, password, passwd_len);
        local_passwd[passwd_len] = '\0';
    }

#if USE_DIV_CSS

     sprintf(pbuf,	"<div class=\"oneline\" id=\"pwd\"><div class=\"left\">Password: </div>"\
				"<div class=\"right\" >"\
				"<input  class=\"box\" id=\"pwd_val\" type=\"text\" name=\"Password\" value=\"%s\" >"\
				" </div></div>",
                        passwd_len?local_passwd:"");
#else
    sprintf(pbuf, "<tr id=\"pwd_row\">"
                        "<td style=\"background-color:#FFD700;width:100px;\">"
                        "<b>Password:</b><br>"
                        "</td>"
                        "<td style=\"background-color:#eeeeee;height:30px;\">"
                        "<input type=\"text\" name=\"Password\" id=\"pwd\" value=\"%s\" ><br>"
                        "</td>"
                        "</tr>", 
                        passwd_len?local_passwd:"");


#endif


    //printf("\r\nstrlen(passwordTableItem)=%d\n", strlen(pbuf));	
}

static void CreateChannelTableItem(char *pbuf, u8_t channel)
{
    u8_t flag[MAX_CHANNEL_NUM+1] = {0};

    if(channel > MAX_CHANNEL_NUM){
        printf("Channel(%d) is out of range!\n", channel);
        channel = 1;
    }
    flag[channel] = 1;

#if USE_DIV_CSS

   sprintf(pbuf, "<div class=\"oneline\"><div class=\"left\">Channel: </div>"
			   "<div class=\"right\"><select  class=\"box\" name=\"Channel\">"
                        "<option value=\"1\" %s>1</option>"
                        "<option value=\"2\" %s>2</option>"
                        "<option value=\"3\" %s>3</option>"
                        "<option value=\"4\" %s>4</option>"
                        "<option value=\"5\" %s>5</option>"
                        "<option value=\"6\" %s>6</option>"
                        "<option value=\"7\" %s>7</option>"
                        "<option value=\"8\" %s>8</option>"
                        "<option value=\"9\" %s>9</option>"
                        "<option value=\"10\" %s>10</option>"
                        "<option value=\"11\" %s>11</option>"
			   "</select> </div> </div>",
				
                        flag[1]?"selected":"",
                        flag[2]?"selected":"",
                        flag[3]?"selected":"",
                        flag[4]?"selected":"",
                        flag[5]?"selected":"",
                        flag[6]?"selected":"",
                        flag[7]?"selected":"",
                        flag[8]?"selected":"",
                        flag[9]?"selected":"",
                        flag[10]?"selected":"",
                        flag[11]?"selected":"");
#else
  	 sprintf(pbuf, "<tr>"
                        "<td style=\"background-color:#FFD700;width:100px;\">"
                        "<b>Channel:</b><br>"
                        "</td>"
                        "<td style=\"background-color:#eeeeee;height:30px;\">"
                        "<select name=\"Channel\">"
                        "<option value=\"1\" %s>1</option>"
                        "<option value=\"2\" %s>2</option>"
                        "<option value=\"3\" %s>3</option>"
                        "<option value=\"4\" %s>4</option>"
                        "<option value=\"5\" %s>5</option>"
                        "<option value=\"6\" %s>6</option>"
                        "<option value=\"7\" %s>7</option>"
                        "<option value=\"8\" %s>8</option>"
                        "<option value=\"9\" %s>9</option>"
                        "<option value=\"10\" %s>10</option>"
                        "<option value=\"11\" %s>11</option>"
                        "</select>"
                        "</td>"
                        "</tr>",
                        flag[1]?"selected":"",
                        flag[2]?"selected":"",
                        flag[3]?"selected":"",
                        flag[4]?"selected":"",
                        flag[5]?"selected":"",
                        flag[6]?"selected":"",
                        flag[7]?"selected":"",
                        flag[8]?"selected":"",
                        flag[9]?"selected":"",
                        flag[10]?"selected":"",
                        flag[11]?"selected":"");


#endif

    //printf("\r\nstrlen(ChannelTableItem)=%d\n", strlen(pbuf));	
}

static void GenerateIndexHtmlPage(portCHAR* cDynamicPage, portCHAR *LocalBuf)
{
        /* Generate the page index.html...
           ... First the page header. */
        strcpy( cDynamicPage, webHTML_HEAD_START );
        
        /* Add script */
        strcat( cDynamicPage, onChangeSecType );
        strcat( cDynamicPage, onSubmitForm);
#if USE_DIV_CSS
        /* add css */
        strcat( cDynamicPage, webHTML_CSS);

        strcat( cDynamicPage, webHTML_TITLE);
#endif
        /* Add Body start */
        strcat( cDynamicPage, webHTML_BODY_START );
        
        /* Add SSID */
        CreateSsidTableItem(LocalBuf, wifi_setting.ssid, strlen((char*)wifi_setting.ssid));
        strcat( cDynamicPage, LocalBuf );
        
        /* Add SECURITY TYPE  */
        CreateSecTypeTableItem(LocalBuf, wifi_setting.security_type);
        strcat( cDynamicPage, LocalBuf );

        /* Add PASSWORD */
        CreatePasswdTableItem(LocalBuf, wifi_setting.password, strlen((char*)wifi_setting.password));
        strcat( cDynamicPage, LocalBuf );
       
        /* Add CHANNEL  */
        CreateChannelTableItem(LocalBuf, wifi_setting.channel);
        strcat( cDynamicPage, LocalBuf );

        /* ... Finally the page footer. */
        strcat( cDynamicPage, webHTML_END );
        //printf("\r\nGenerateIndexHtmlPage(): %s\n", cDynamicPage);
        printf("\r\nGenerateIndexHtmlPage Len: %d\n", strlen( cDynamicPage ));
}

static void GenerateWaitHtmlPage(portCHAR* cDynamicPage)
{
        /* Generate the dynamic page...
           ... First the page header. */
        strcpy( cDynamicPage, webWaitHTML_START );

        /* ... Finally the page footer. */
        strcat( cDynamicPage, webWaitHTML_END);

        //printf("\r\nGenerateWaitHtmlPage(): %s\n",  cDynamicPage);
        //printf("\r\nGenerateWaitHtmlPage Len: %d\n", strlen( cDynamicPage ));
}

static void http_translate_url_encode(char *ptr)
{

	char *data = ptr;
	char tmp_data[3] = {0};
	char outdata[33] = {0};
	int buffer;
	char *outdata_ptr = outdata;

	while (*data != '\0') {

		if (*data == '%') {
			if ((*(data + 1) != 0) &&  (*(data + 2) != 0)) {
				tmp_data[0] = *(data + 1);
				tmp_data[1] = *(data + 2);
				sscanf(tmp_data, "%x", &buffer);
				*outdata_ptr = (char)buffer;

				/* destroy data */
				*data  = 0;
				*(data+1)  = 0;
				*(data+2)  = 0;
				
				data += 2;
				outdata_ptr++;
			}
			
		} else {
		  *outdata_ptr = *data;
		  if (*data == '+')
		    *outdata_ptr = ' ';
		  outdata_ptr++;
		}
		data++;
	}
	strcpy(ptr, outdata);

}

static u8_t ProcessPostMessage(struct netbuf  *pxRxBuffer, portCHAR *LocalBuf)
{
    struct pbuf *p;
    portCHAR *pcRxString, *ptr;
    unsigned portSHORT usLength;
    u8_t bChanged = 0;
    rtw_security_t secType;
    u8_t channel;
    u8_t len = 0;
    
    pcRxString = LocalBuf;
    p = pxRxBuffer->p;
    usLength = p->tot_len;
    //printf("\r\n !!!!!!!!!POST!p->tot_len =%d p->len=%d\n", p->tot_len, p->len);            
    while(p)
    {
        memcpy(pcRxString, p->payload, p->len);
        pcRxString += p->len;
        p = p->next;
    }
    pcRxString = LocalBuf;
    pcRxString[usLength] = '\0';
    //printf("\r\n usLength=%d pcRxString = %s\n", usLength, pcRxString);

    ptr = (char*)strstr(pcRxString, "Ssid=");
    if(ptr)
    {
	//printf("ssid passed = %s\n", ptr);
        pcRxString = (char*)strstr(ptr, "&");
        *pcRxString++ = '\0';
        ptr += 5;
		http_translate_url_encode(ptr);
        if(strcmp((char*)wifi_setting.ssid, ptr))
        {
            bChanged = 1;
            len = strlen(ptr);
            if(len > MAX_SOFTAP_SSID_LEN){
                len = MAX_SOFTAP_SSID_LEN;
                ptr[len] = '\0';
            }
            strcpy((char*)wifi_setting.ssid, ptr);
        }
    }

	
    printf("\r\n get wifi_config.ssid = %s\n", wifi_setting.ssid);
    ptr = (char*)strstr(pcRxString, "Security+Type=");
    if(ptr)
    {
        pcRxString = (char*)strstr(ptr, "&");
        *pcRxString++ = '\0';
        ptr += 14;
        if(!strcmp(ptr, "open"))
           secType = RTW_SECURITY_OPEN;
        else if(!strcmp(ptr, "wpa2-aes"))
            secType = RTW_SECURITY_WPA2_AES_PSK;
        else
            secType = RTW_SECURITY_OPEN;
        if(wifi_setting.security_type != secType)
        {
            bChanged = 1;
            wifi_setting.security_type = secType;
        }
    }
    
    //printf("\r\n wifi_config.security_type = %d\n", wifi_setting.security_type);
    if(wifi_setting.security_type > RTW_SECURITY_OPEN)
    {
        ptr = (char*)strstr(pcRxString, "Password=");
        if(ptr)
        {
            pcRxString = (char*)strstr(ptr, "&");
            *pcRxString++ = '\0';
            ptr += 9;
            if(strcmp((char*)wifi_setting.password, ptr))
            {
                bChanged = 1;
                len = strlen(ptr);
                if(len > MAX_PASSWORD_LEN){
                    len = MAX_PASSWORD_LEN;
                    ptr[len] = '\0';
                }
                strcpy((char*)wifi_setting.password, ptr);
            }
        }
        //printf("\r\n wifi_config.password = %s\n", wifi_setting.password);
    }
    ptr = (char*)strstr(pcRxString, "Channel=");
    if(ptr)
    {
        ptr += 8;
        channel = web_atoi(ptr);
        if((channel>MAX_CHANNEL_NUM)||(channel < 1))
            channel = 1;
        if(wifi_setting.channel !=channel)
        {
            bChanged = 1;
            wifi_setting.channel = channel;
        }
    }
    //printf("\r\n wifi_config.channel = %d\n", wifi_setting.channel);
            
    return bChanged;
}

struct netconn *pxHTTPListener = NULL;
static void vProcessConnection( struct netconn *pxNetCon )
{
    static portCHAR cDynamicPage[webMAX_PAGE_SIZE];
    struct netbuf  *pxRxBuffer, *pxRxBuffer1 = NULL;
    portCHAR       *pcRxString;
    unsigned portSHORT usLength;
    static portCHAR LocalBuf[LOCAL_BUF_SIZE];
    u8_t bChanged = 0;
    int ret_recv = ERR_OK;
    int ret_accept = ERR_OK;
    char *ptr = NULL;

    /* Load WiFi Setting*/
    LoadWifiSetting();

    /* We expect to immediately get data. */
    port_netconn_recv( pxNetCon , pxRxBuffer, ret_recv);

    if( pxRxBuffer != NULL && ret_recv == ERR_OK)
    {
         /* Where is the data? */
        netbuf_data( pxRxBuffer, ( void * )&pcRxString, &usLength );

        //printf("\r\nusLength=%d pcRxString = \n%s\n", usLength, pcRxString);
	/* Is this a GET?  We don't handle anything else. */
        if( !strncmp( pcRxString, "GET", 3 ) )
        {
            //printf("\r\nusLength=%d pcRxString=%s \n", usLength, pcRxString);
            //pcRxString = cDynamicPage;

            /* Write out the HTTP OK header. */            
            netconn_write( pxNetCon, webHTTP_OK, ( u16_t ) strlen( webHTTP_OK ), NETCONN_COPY );

            /* Generate index.html page. */
            GenerateIndexHtmlPage(cDynamicPage, LocalBuf);
            
            /* Write out the dynamically generated page. */
            netconn_write( pxNetCon, cDynamicPage, ( u16_t ) strlen( cDynamicPage ), NETCONN_COPY );
        }
        else if(!strncmp( pcRxString, "POST", 4 ) )
        {
            /* Write out the HTTP OK header. */            
            netconn_write( pxNetCon, webHTTP_OK, ( u16_t ) strlen( webHTTP_OK ), NETCONN_COPY );
			
            bChanged = ProcessPostMessage(pxRxBuffer, LocalBuf);
            if(bChanged == 0){
                    port_netconn_recv( pxNetCon , pxRxBuffer1, ret_recv);
                    if(pxRxBuffer != NULL && ret_recv == ERR_OK){
                        bChanged = ProcessPostMessage(pxRxBuffer1, LocalBuf);
                        netbuf_delete( pxRxBuffer1 );
                    }
            }
            if(bChanged)
            {
                GenerateWaitHtmlPage(cDynamicPage);
                
                /* Write out the generated page. */
                netconn_write( pxNetCon, cDynamicPage, ( u16_t ) strlen( cDynamicPage ), NETCONN_COPY );

#if CONFIG_READ_FLASH
                StoreApInfo();
#endif
            }
            else
            {
                /* Generate index.html page. */
                GenerateIndexHtmlPage(cDynamicPage, LocalBuf);
                
                /* Write out the generated page. */
                netconn_write( pxNetCon, cDynamicPage, ( u16_t ) strlen( cDynamicPage ), NETCONN_COPY );
            }
        }
        netbuf_delete( pxRxBuffer );
    }
    netconn_close( pxNetCon );

    if(bChanged)
    {
        struct netconn *pxNewConnection;
        vTaskDelay(200/portTICK_RATE_MS);
        //printf("\r\n%d:before restart ap\n", xTaskGetTickCount());
        RestartSoftAP();
        //printf("\r\n%d:after restart ap\n", xTaskGetTickCount());
        pxHTTPListener->recv_timeout = 1;		
        port_netconn_accept( pxHTTPListener , pxNewConnection, ret_accept);
        if( pxNewConnection != NULL && ret_accept == ERR_OK)
        {
            //printf("\r\n%d: got a conn\n", xTaskGetTickCount());
            netconn_close( pxNewConnection );
            while( netconn_delete( pxNewConnection ) != ERR_OK )
            {
                vTaskDelay( webSHORT_DELAY );
            }
        }
        //printf("\r\n%d:end\n", xTaskGetTickCount());
        pxHTTPListener->recv_timeout = 0;		
    }
}

/*------------------------------------------------------------*/
xTaskHandle webs_task = NULL;
xSemaphoreHandle webs_sema = NULL;
u8_t webs_terminate = 0;
void vBasicWEBServer( void *pvParameters )
{
    struct netconn *pxNewConnection;
    //struct ip_addr  xIpAddr, xNetMast, xGateway;
    extern err_t ethernetif_init( struct netif *netif );
    int ret = ERR_OK;
    /* Parameters are not used - suppress compiler error. */
    ( void )pvParameters;

    /* Create a new tcp connection handle */
    pxHTTPListener = netconn_new( NETCONN_TCP );
    ip_set_option(pxHTTPListener->pcb.ip, SOF_REUSEADDR);
    netconn_bind( pxHTTPListener, NULL, webHTTP_PORT );
    netconn_listen( pxHTTPListener );

#if CONFIG_READ_FLASH
  /* Load wifi_config */
    LoadWifiConfig();
    RestartSoftAP();
#endif
    //printf("\r\n-0\n");

    /* Loop forever */
    for( ;; )
    {	
        if(webs_terminate)
            break;

        //printf("\r\n%d:-1\n", xTaskGetTickCount());
        /* Wait for connection. */
        port_netconn_accept( pxHTTPListener , pxNewConnection, ret);
        //printf("\r\n%d:-2\n", xTaskGetTickCount());

        if( pxNewConnection != NULL && ret == ERR_OK)
        {
            /* Service connection. */
            vProcessConnection( pxNewConnection );
            while( netconn_delete( pxNewConnection ) != ERR_OK )
            {
                vTaskDelay( webSHORT_DELAY );
            }
        }
        //printf("\r\n%d:-3\n", xTaskGetTickCount());
    }
    //printf("\r\n-4\n");
    if(pxHTTPListener)
    {
        netconn_close(pxHTTPListener);
        netconn_delete(pxHTTPListener);
        pxHTTPListener = NULL;
    }

    //printf("\r\nExit Web Server Thread!\n");
    xSemaphoreGive(webs_sema);
}

#define STACKSIZE				512
void start_web_server()
{
    	printf("\r\nWEB:Enter start web server!\n");
	webs_terminate = 0;
	if(webs_task == NULL)
	{
		if(xTaskCreate(vBasicWEBServer, (const char *)"web_server", STACKSIZE, NULL, tskIDLE_PRIORITY + 1, &webs_task) != pdPASS)
			printf("\n\rWEB: Create webserver task failed!\n");
	}
	if(webs_sema == NULL)
	{
		webs_sema = xSemaphoreCreateCounting(0xffffffff, 0);	//Set max count 0xffffffff
	}
    	//printf("\r\nWEB:Exit start web server!\n");
}

void stop_web_server()
{
    	//printf("\r\nWEB:Enter stop web server!\n");
	webs_terminate = 1;
   	if(pxHTTPListener)
		netconn_abort(pxHTTPListener);
	if(webs_sema)
	{
		if(xSemaphoreTake(webs_sema, 15 * configTICK_RATE_HZ) != pdTRUE)
		{
			if(pxHTTPListener)
			{
				netconn_close(pxHTTPListener);
  				netconn_delete(pxHTTPListener);
				pxHTTPListener = NULL;
			}
			printf("\r\nWEB: Take webs sema(%p) failed!!!!!!!!!!!\n", webs_sema);
		}
		vSemaphoreDelete(webs_sema);
		webs_sema = NULL;
	}
	if(webs_task)
	{
		vTaskDelete(webs_task);
		webs_task = NULL;
	}
    	printf("\r\nWEB:Exit stop web server!\n");		
}
