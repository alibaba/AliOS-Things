#include <stdio.h>
#include <string.h>

#if defined(__MTK_7687__)
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/netif.h"
#include "wifi_api.h"
#include "os_util.h"
#elif defined(__ICOMM_6166__)
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/netif.h"
#else
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "joylink_utils.h"
#include "joylink.h"

/**
 * brief: 
 *
 * @Param: pBytes
 * @Param: srcLen
 * @Param: pDstStr
 * @Param: dstLen
 *
 * @Returns: 
 */
int 
joylink_util_byte2hexstr(const uint8_t *pBytes, int srcLen, uint8_t *pDstStr, int dstLen)
{
	const char tab[] = "0123456789abcdef";
	int i = 0;

	memset(pDstStr, 0, dstLen);

	if(dstLen < srcLen * 2){
		srcLen = (dstLen - 1) / 2;
    }

	for(i = 0; i < srcLen; i++){
		*pDstStr++ = tab[*pBytes >> 4];
		*pDstStr++ = tab[*pBytes & 0x0f];
		pBytes++;
	}
	*pDstStr++ = 0;

	return srcLen * 2;
}

/**
 * brief: 
 *
 * @Param: hexStr
 * @Param: buf
 * @Param: bufLen
 *
 * @Returns: 
 */
int 
joylink_util_hexStr2bytes(const char *hexStr, uint8_t *buf, int bufLen)
{
	int i;
	int len;

	if(NULL == hexStr){
		len = 0;
	}else{
		len = (int)strlen(hexStr) / 2;
		if(bufLen < len){
			len = bufLen;
        }
	}
	memset(buf, 0, bufLen);

	for(i = 0; i < len; i++){
		char ch1, ch2;
		int val;

		ch1 = hexStr[i * 2];
		ch2 = hexStr[i * 2 + 1];
		if(ch1 >= '0' && ch1 <= '9'){
			val = (ch1 - '0') * 16;
        }else if (ch1 >= 'a' && ch1 <= 'f'){
			val = ((ch1 - 'a') + 10) * 16;
        }else if (ch1 >= 'A' && ch1 <= 'F'){
			val = ((ch1 - 'A') + 10) * 16;
        }else{
			return -1;
        }

		if(ch2 >= '0' && ch2 <= '9'){
			val += ch2 - '0';
        }else if (ch2 >= 'a' && ch2 <= 'f'){
			val += (ch2 - 'a') + 10;
        }else if (ch2 >= 'A' && ch2 <= 'F'){
			val += (ch2 - 'A') + 10;
        }else{
			return -1;
        }

		buf[i] = val & 0xff;
	}

	return 0;
}

/**
 * brief: 
 *
 * @Param: pPeerAddr
 * @Param: str
 *
 * @Returns: 
 */
int 
joylink_util_get_ipstr(struct sockaddr_in* pPeerAddr, char* str)
{
	struct sockaddr_in* pAddr = (struct sockaddr_in*)pPeerAddr;

	int n = sprintf(str, "%s:%d",
		(char *)inet_ntoa(pAddr->sin_addr),
		pAddr->sin_port
		);

	return n;
}

/**
 * brief: 
 *
 * @Param: ipport
 * @Param: out_ip
 * @Param: out_port
 *
 * @Returns: 
 */
int 
joylink_util_cut_ip_port(const char *ipport, char *out_ip, int *out_port)
{
    if(NULL == ipport || 
            NULL == out_ip ||
            NULL == out_port){
        return E_RET_ERROR;
    }
    int ret = E_RET_OK;
    int offset = 0;
    int len = strlen(ipport);

    while(ipport[offset] != ':' && offset < len){
        offset++;
    }
    
    if(offset < len){
        memcpy(out_ip, ipport, offset);
        out_ip[offset] = '\0';
        *out_port = atoi(ipport + offset + 1);
    }else{
        ret = E_RET_ERROR;
    }

	return ret;
}

/**
 * brief: 
 *
 * @Returns: 
 */
static uint32_t 
joylink_util_getsys_ms(void)
{
	uint64_t ret;

#ifdef __MRVL_MW300__
	ret = os_get_timestamp() / 1000UL;
#elif defined(__LINUX__)
    #include <time.h>  
    #include <sys/time.h>
      
    struct timeval	now;

	gettimeofday(&now,NULL);
	ret = 1000UL * now.tv_sec + (now.tv_usec / 1000UL);
#elif defined(_WIN32)
	ret = GetTickCount();
#elif defined(__MTK_7687__)
    struct timeval now;
    gettimeofday(&now, NULL); 
	return  1000UL * now.tv_sec + (now.tv_usec / 1000UL);
#elif defined(__ICOMM_6166__)
    uint32_t sys_tick = xTaskGetTickCount();
    return sys_tick;
#else
#error("System time error!\r\n");
#endif
	return ret;
}

/**
 * brief: 
 *
 * @Param: timestamp
 */
void 
joylink_util_timer_reset(uint32_t *timestamp)
{
	uint32_t unix_time_value = joylink_util_getsys_ms() & 0x7FFFFFFF;
	*timestamp = unix_time_value;
}

/**
 * brief: 
 *
 * @Param: timestamp
 * @Param: timeout
 *
 * @Returns: 
 */
int 
joylink_util_is_time_out(uint32_t timestamp, uint32_t timeout)
{
	uint32_t unix_time_value = joylink_util_getsys_ms() & 0x7FFFFFFF;
    int32_t ret = (timestamp + timeout) - unix_time_value;
    return ret <= 0? 1:0;
}

/**
 * brief: 
 *
 * @Param: msg
 * @Param: is_fmt
 * @Param: num_line
 * @Param: buff
 * @Param: len
 */
void
joylink_util_print_buffer(const char *msg, int is_fmt, int num_line, const uint8_t *buff, int len)
{
    if(NULL == msg || NULL == buff){
        return;
    }
    int cut_num = 4;
    int i = 0;
    printf("------:%s\n", msg);
    for(i =0; i < len; i++){
        printf("%02x ", (int)buff[i]);
        if(is_fmt && !(num_line%cut_num)){
            if(!((i + 1)%cut_num)){
                printf("| ");
            }
        }

        if(!((i + 1)%num_line)){
            printf("\n");
        }
    }
    printf("\n");
}
