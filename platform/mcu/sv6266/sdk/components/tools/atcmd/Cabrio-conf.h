/**
 *
 * @page ATCMD AT Command Manual V2.0
 *
 */
 
#ifndef __CABRIO_CONF_H__
#define __CABRIO_CONF_H__

#include "soc_types.h"
#include "atcmd_general_def.h"
#include "atcmd_wifi_def.h"
#include "atcmd_gpio_def.h"
#include "atcmd_socket_def.h"
#include "atcmd_rf_def.h"


/* ATE response command for sending to host uart */
#define	ATRSP_OK                    "+OK\r\n"
#define	ATRSP_ERROR                 "+ERROR:%d\r\n"
#define	ATRSP_ERROR_STR           	"+ERROR:%s\r\n"
#define	ATRSP_VERSION               "+VERSION:%s\r\n"
#define	ATRSP_MANUFACTURE           "+MF_INFO:%s\r\n"
#define	ATRSP_CONFIGURE             "+GET_CONFIG=%d,%s,%s,%d,%d,%d.%d.%d.%d,%d.%d.%d.%d,%d.%d.%d.%d\r\n"
#define	ATRSP_NETSTATUS             "+NETSTATUS=%d\r\n"
#define	ATRSP_NETSTATUS2             "+NETSTATUS=STA%d - %d\r\n"

#ifdef SMARTCONFIG_EN
#define ATCMD_SMARTCONFIG    "AT+SC"
#endif

#define ATCMD_USER_FLASH	"AT+USER_FLASH"

#endif /* __CABRIO_CONF_H__ */
