#ifndef _ERROR_H_
#define _ERROR_H_

/* Error code definition */
enum {
	ERROR_MEMORY_FAILED                = -11,
	ERROR_GPIO_CONNECTION              = -10,
	ERROR_UDP_CONNECTION               = -9,
	ERROR_TCP_CONNECTION               = -8,
	ERROR_WIFI_CONNECTION_DEAUTH       = -7,
	ERROR_WIFI_CONNECTION_ASSOCIATION  = -6,
	ERROR_WIFI_CONNECTION_AUTH         = -5,
	ERROR_WIFI_CONNECTION              = -4,
	ERROR_UNKNOWN_COMMAND              = -3,
	ERROR_NOT_IMPLEMENT                = -2,
	ERROR_INVALID_PARAMETER            = -1,
	ERROR_SUCCESS                      =  0,
	ERROR_SUCCESS_NO_RSP               =  1,
};


#endif //_ERROR_H
