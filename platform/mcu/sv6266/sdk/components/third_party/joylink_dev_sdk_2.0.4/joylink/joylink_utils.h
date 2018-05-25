#ifndef _UTILS_H
#define _UTILS_H

#ifdef __LINUX_UB2__ 
#include <stdint.h>
#endif

#if defined(__MTK_7687__)
#include <stdint.h>
#include "lwip/sockets.h"
#elif defined(__ICOMM_6166__)
#include <stdint.h>
#include "lwip/sockets.h"
#else
#include <unistd.h>
#include <netinet/in.h>
#endif

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
joylink_util_cut_ip_port(const char *ipport, char *out_ip, int *out_port);

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
joylink_util_byte2hexstr(const uint8_t *pBytes, int srcLen, uint8_t *pDstStr, int dstLen);

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
joylink_util_hexStr2bytes(const char *hexStr, uint8_t *buf, int bufLen);

/**
 * brief: 
 *
 * @Param: pPeerAddr
 * @Param: str
 *
 * @Returns: 
 */
int 
joylink_util_get_ipstr(struct sockaddr_in* pPeerAddr, char* str);

/**
 * brief: 
 *
 * @Param: timestamp
 */
void 
joylink_util_timer_reset(uint32_t *timestamp);

/**
 * brief: 
 *
 * @Param: timestamp
 * @Param: timeout
 *
 * @Returns: 
 */
int 
joylink_util_is_time_out(uint32_t timestamp, uint32_t timeout);

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
joylink_util_print_buffer(const char *msg, int is_fmt, int num_line, const uint8_t *buff, int len);

#define JL_UTILS_P_FMT        (1)
#define JL_UTILS_P_NO_FMT     (0)

#define joylink_util_fmt_p(msg, buff, len) joylink_util_print_buffer(msg, JL_UTILS_P_FMT, 16, buff, len) 
        
#endif /* utils.h */
