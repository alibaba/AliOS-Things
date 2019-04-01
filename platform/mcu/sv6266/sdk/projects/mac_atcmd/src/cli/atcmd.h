#ifndef _ATCMD_H_
#define _ATCMD_H_

//#include "contiki.h"
//#include "Cabrio-conf.h"
#include <stdio.h> /* For printf() */
#include "console.h"
#include "atcmdlib.h"

////Customer AT Command
#define MAX_CMD_LEN (32)
#define MAX_ARGUMENT 24
typedef struct _stParam
{
	char*	argv[MAX_ARGUMENT];
	int	argc;
} stParam;

typedef int (*PF_handle) (stParam *param);
typedef struct _at_cmd_info
{
	char*		atCmd;
	PF_handle	pfHandle;
    uint8_t maxargu;
} at_cmd_info;

int At_ParserInd (EVENTMSG_ID msgid, unsigned char *buff, int len) ;
int At_Parser (char *buff, int len);
int At_SSLConnect(stParam *param);
int At_SSLSend(stParam *param);
int At_SSLDisconnect(stParam *param);
int At_SSLTest(stParam *param);

int At_Parser (char *buff, int len);
int At_TCPConnect (stParam *param);
int At_TCPDisconnect (stParam *param);
int At_TCPSend (stParam *param);
int At_TCPSend6 (stParam *param);
int At_TCPListen (stParam *param);
int At_TCPListen6 (stParam *param);
int At_TCPUnlisten (stParam *param);
int At_UDPCreate (stParam *param);
int At_UDPSend (stParam *param);
int At_UDPClose (stParam *param);
int At_NSLookup(stParam *param);
int At_tcp_dbg(stParam *param);

#ifdef IPERF_ENABLE
int At_iperf_main(stParam *param);
#endif

#endif
