#ifndef __WIFI_SIMPLE_CONFIG_H
#define __WIFI_SIMPLE_CONFIG_H
/*****************************wifi_simple_config.h****************************/
enum sc_result {
	SC_ERROR = -1,	/* default error code*/
	SC_NO_CONTROLLER_FOUND = 1, /* cannot get sta(controller) in the air which starts a simple config session */
	SC_CONTROLLER_INFO_PARSE_FAIL, /* cannot parse the sta's info  */
	SC_TARGET_CHANNEL_SCAN_FAIL, /* cannot scan the target channel */
	SC_JOIN_BSS_FAIL, /* fail to connect to target ap */
	SC_DHCP_FAIL, /* fail to get ip address from target ap */
	 /* fail to create udp socket to send info to controller. note that client isolation
		must be turned off in ap. we cannot know if ap has configured this */
	SC_UDP_SOCKET_CREATE_FAIL,
	SC_TERMINATE,
	SC_SUCCESS,	/* default success code */

};
int SC_send_simple_config_ack(u8 round);

#endif //__WIFI_SIMPLE_CONFIG_H	
