#ifndef _SOFT_AP_DEF_H
#define _SOFT_AP_DEF_H

#define ENABLE_HK (1)	//in original cabrio-rtos,it's in soft_ap.h

typedef enum _SOFTAP_SECURITY
{
	SECURE_NONE = 0,
	SECURE_WEP,
	SECURE_WPA,
	SECURE_WPA2
} SOFTAP_SECURITY;


typedef enum {
       SOFTAP_OFF = 0,                  
	SOFTAP_NO_CONNECTION,   //softap function being enabled and waiting for sta's connection
	SOFTAP_PROBING_RESP,
	SOFTAP_AUTHENTICATING,
	SOFTAP_ASSOCIATING_RESP,
	SOFTAP_DHCP_RACK,
	//SOFTAP_ARP_RESPONSE,
	SOFTAP_REASSOCIATING_RESP,
	SOFTAP_HANDSHAKE,
	SOFTAP_CONNECTED,
} softap_state;

typedef enum {
    eSOFTAP_NULL_DATA_NO_RESP,
    eSOFTAP_NULL_DATA_ACK
} SOFTAP_NULL_DATA_ACTION;


#endif //_SOFT_AP_DEF_H
