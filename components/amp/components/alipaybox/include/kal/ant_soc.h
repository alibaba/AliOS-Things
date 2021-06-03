/**
 * ant_soc.h
 *
 * 网络套接字接口
 */
#ifndef __ANT_SOC_H__
#define __ANT_SOC_H__

#include "ant_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ANT_SOC_AF_INET         (0)
#define ANT_SOC_PF_INET         (0)
#define ANT_SOC_IPPROTO_IP      (0)
#define ANT_SOC_IPPROTO_ICMP    (1)
#define ANT_MAX_SOCKET_NUM      (10)
#define ANT_MAX_SOCK_ADDR_LEN   (28)
#define ANT_MAX_HOST_NAME_LEN   (64)
#define ANT_RESOLVE_HOST_NUM    (5)
#define ANT_DNS_HOST_NUM        (10)

/* Socket Type */ 
typedef enum {
    ANT_SOC_TYPE_TCP = 0,  /* TCP */ 
    ANT_SOC_TYPE_UDP,      /* datagram socket, UDP */ 
    ANT_SOC_TYPE_RAW,      /* RAW socket, ICMP, .. */     
} ant_sock_type_e;

typedef enum
{
    ANT_SOC_READ    = 0x01,  /* Notify for read */
    ANT_SOC_WRITE   = 0x02,  /* Notify for write */
    ANT_SOC_ACCEPT  = 0x04,  /* Notify for accept */
    ANT_SOC_CONNECT = 0x08,  /* Notify for connect */
    ANT_SOC_CLOSE   = 0x10   /* Notify for close */
} ant_soc_event_e;

/****************************************************************************
 * Return Codes Definition for TCP/IP
 ***************************************************************************/
typedef enum {
    ANT_SOC_SUCCESS             = 0,
    ANT_SOC_ERROR               = -1,
    ANT_SOC_WOULDBLOCK          = -2,
    ANT_SOC_LIMIT_RESOURCE      = -3,    /* limited resource */
    ANT_SOC_INVALID_SOCKET      = -4,    /* invalid socket */
    ANT_SOC_INVALID_ACCOUNT     = -5,    /* invalid account id */
    ANT_SOC_NAMETOOLONG         = -6,    /* address too long */
    ANT_SOC_ALREADY             = -7,    /* operation already in progress */
    ANT_SOC_OPNOTSUPP           = -8,    /* operation not support */
    ANT_SOC_CONNABORTED         = -9,    /* Software caused connection abort */
    ANT_SOC_INVAL               = -10,   /* invalid argument */
    ANT_SOC_PIPE                = -11,   /* broken pipe */
    ANT_SOC_NOTCONN             = -12,   /* socket is not connected */
    ANT_SOC_MSGSIZE             = -13,   /* msg is too long */
    ANT_SOC_BEARER_FAIL         = -14,   /* bearer is broken */
    ANT_SOC_CONNRESET           = -15,   /* TCP half-write close, i.e., FINED */
    ANT_SOC_DHCP_ERROR          = -16,
    ANT_SOC_IP_CHANGED          = -17,
    ANT_SOC_ADDRINUSE           = -18,
    ANT_CANCEL_ACT_BEARER       = -19,   /* cancel the activation of bearer */
    ANT_SOC_TIMEOUT             = -20,
    ANT_SOC_EINTR               = -100
} ant_soc_errcode_e;

/* Socket Options */
typedef enum
{
    ANT_SOC_OOBINLINE     = 0x01 << 0,  /* not support yet */
    ANT_SOC_LINGER        = 0x01 << 1,  /* linger on close */
    ANT_SOC_NBIO          = 0x01 << 2,  /* Nonblocking */
    ANT_SOC_ASYNC         = 0x01 << 3,  /* Asynchronous notification */   
    ANT_SOC_NODELAY       = 0x01 << 4,  /* disable Nagle algorithm or not */
    ANT_SOC_KEEPALIVE     = 0x01 << 5,  /* enable/disable the keepalive */
    ANT_SOC_RCVBUF        = 0x01 << 6,  /* set the socket receive buffer size */
    ANT_SOC_SENDBUF       = 0x01 << 7,  /* set the socket send buffer size */
    ANT_SOC_NREAD         = 0x01 << 8,  /* no. of bytes for read, only for soc_getsockopt */
    ANT_SOC_PKT_SIZE      = 0x01 << 9,  /* datagram max packet size */
    ANT_SOC_SILENT_LISTEN = 0x01 << 10, /* SOC_SOCK_SMS property */
    ANT_SOC_QOS           = 0x01 << 11, /* set the socket qos */
    ANT_SOC_TCP_MAXSEG    = 0x01 << 12, /* set the max segmemnt size */
    ANT_SOC_IP_TTL        = 0x01 << 13, /* set the IP TTL value */
    ANT_SOC_LISTEN_BEARER = 0x01 << 14, /* enable listen bearer */
    ANT_SOC_UDP_ANY_FPORT = 0x01 << 15, /* enable UDP any foreign port */
    ANT_SOC_WIFI_NOWAKEUP = 0x01 << 16, /* send packet in power saving mode */
    ANT_SOC_UDP_NEED_ICMP = 0x01 << 17, /* deliver NOTIFY(close) for ICMP error */
    ANT_SOC_IP_HDRINCL    = 0x01 << 18, /* IP header included for raw sockets */
    ANT_SOC_IPSEC_POLICY  = 0x01 << 19, /* ip security policy */
    ANT_SOC_TCP_ACKED_DATA  = 0x01 << 20, /* TCPIP acked data */
    ANT_SOC_TCP_DELAYED_ACK = 0x01 << 21, /* TCP delayed ack */
    ANT_SOC_TCP_SACK        = 0x01 << 22, /* TCP selective ack */
    ANT_SOC_TCP_TIME_STAMP  = 0x01 << 23, /* TCP time stamp */
    ANT_SOC_TCP_ACK_MSEG    = 0x01 << 24  /* TCP ACK multiple segment */
} ant_soc_option_e;

typedef struct {
    char *h_name;
    char **h_aliases;
    int  h_addrtype;
    int  h_length;
    char **h_addr_list;
} ant_hostent;

typedef struct {
    ant_u8     sin_len;
    ant_u8     sin_family;
    ant_u16    sin_port;
    ant_u32    sin_addr;
    ant_u8     sin_zero[8];
} ant_sockaddr;

typedef struct 
{
    ant_u32 tv_sec;  /* select timeout: no. of second */
    ant_u32 tv_usec; /* select timeout: no. of microsecond */
} ant_timeval;

typedef struct {
    ant_bool  onoff;       /* ANT_TRUE: enable linger timeout ANT_FALSE: disable linger timeout */
    ant_u16   linger_time; /* linger timeout (unit in seconds) */   
} ant_soc_linger;

typedef struct {
    ant_u8  fds_bits[ANT_MAX_SOCKET_NUM]; /* select set based on socket id */
} ant_fd_set;

#define ANT_SOC_FD_ZERO(p)    memset(p, 0x00, sizeof(*(p)))
#define ANT_SOC_FD_SET(s,p)   ((p)->fds_bits[s] |= 0x01)
#define ANT_SOC_FD_CLR(s,p)   ((p)->fds_bits[s] &= ~(0x01))
#define ANT_SOC_FD_ISSET(s,p) ((p)->fds_bits[s] & 0x02)

typedef struct {
    void (*callback_socket_connect)(ant_s32 sock, ant_s32 errcode, void *param);
    void (*callback_socket_close)(ant_s32 sock, ant_s32 errcode, void *param);
    void (*callback_socket_accept)(ant_s32 sock, ant_s32 errcode, void *param);
    void (*callback_socket_read)(ant_s32 sock, ant_s32 errcode, void *param);
    void (*callback_socket_write)(ant_s32 sock, ant_s32 errcode, void *param);
} ant_soc_callback_t;

/**
 * 网络初始化
 *
 * @return  0: 初始化成功
 *         -1: 初始化失败
 */
ant_s32 ant_network_init(void);

/**
 * 获取主机ip地址
 *
 * @param host_name 主机域名
 * @param addr 32位IP地址，网络字节序
 * @param time_out 超时时间，单位为毫秒
 * @return    0： 成功，获取到主机IP地址
 *         小于0: 失败，取值参见ant_soc_errcode_e定义
 */
ant_s32 ant_soc_gethostbyname(const ant_char *host_name, ant_u32 *addr, ant_u32 time_out);

/**
 * 注册socket操作回调函数
 *
 * @param sock socket句柄
 * @param cb 回调函数
 * @param param 回调函数用户参数
 * @return    0： 成功，注册成功
 *         小于0: 失败，取值参见ant_soc_errcode_e定义
 */
ant_s32 ant_soc_register_callback(ant_s32 sock, ant_soc_callback_t cb, void *param);

/**
 * 创建socket
 *
 * @param stream_type socket类型，取值参见ant_sock_type_e
 * @param protocol 协议类型，当前固定值为0
 *
 * @return 大于等于0： 创建成功，返回socket句柄
 *            小于0: 失败，取值参见ant_soc_errcode_e定义
 */
ant_s32 ant_soc_create(ant_u32 stream_type, ant_u32 protocol);
ant_s32 ant_soc_create_async(ant_u32 stream_type, ant_u32 protocol);

/**
 * 设置socket属性
 *
 * @param sock socket句柄
 * @param option 要设置的socket属性，取值参见ant_soc_option_e定义
 * @param val 属性值缓存地址
 * @param val_size 属性值缓存大小
 * @return   0： 成功
 *         小于0: 失败，取值参见ant_soc_errcode_e定义
 */
ant_s32 ant_soc_setsockopt(ant_s32 sock, ant_u32 option, void *val, ant_u32 val_size);

/**
 * 获取socket属性
 *
 * @param sock socket句柄
 * @param option 要获取的socket属性，取值参见ant_soc_option_e定义
 * @param val 属性值缓存地址
 * @param val_size 属性值缓存大小
 * @return   0： 成功
 *         小于0: 失败，取值参见ant_soc_errcode_e定义
 */
ant_s32 ant_soc_getsockopt(ant_s32 sock, ant_u32 option, void *val, ant_u32 val_size);

/**
 * 连接主机
 *
 * @param sock socket句柄
 * @param addr 主机地址
 * @return    0: 成功
 *         小于0: 失败，取值参见ant_soc_errcode_e定义
 */
ant_s32 ant_soc_connect(ant_s32 sock, ant_sockaddr *addr);

/**
 * 发送数据(TCP)
 *
 * @param sock socket句柄
 * @param data 发送数据缓存地址
 * @param len 发送数据的长度
 * @return 大于等于0: 发送成功，返回已发送数据的长度
 *            小于0: 发送失败，取值参见ant_soc_errcode_e定义
 */
ant_s32 ant_soc_send(ant_s32 sock, const ant_u8 *data, ant_s32 len);

/**
 * 向指定的主机发送数据(UDP)
 *
 * @param sock socket句柄
 * @param data 发送数据缓存地址
 * @param len 发送数据的长度
 * @param sock_addr 指定的主机地址缓存
 * @param size 指定的主机地址长度
 * @return 大于等于0: 发送成功，返回已发送数据的长度
 *            小于0: 发送失败，取值参见ant_soc_errcode_e定义
 */
ant_s32 ant_soc_sendto(ant_s32 sock, ant_u8 *data, ant_s32 len, 
                       ant_sockaddr *sock_addr, ant_u32 size);

/**
 * 接收数据（TCP）
 *
 * @param sock socket句柄
 * @param data 接收数据缓存地址
 * @param len 接收数据缓存的长度
 * @return 大于等于0: 接收成功，返回已接收数据的长度
 *            小于0: 接收失败，取值参见ant_soc_errcode_e定义
 */
ant_s32 ant_soc_recv(ant_s32 sock, ant_u8 *data, ant_s32 len);

/**
 * 从指定的主机接收数据(UDP)
 *
 * @param sock socket句柄
 * @param data 接收数据缓存地址
 * @param len 接收数据的长度
 * @param sock_addr 指定的主机地址缓存
 * @param size 指定的主机地址长度
 * @return 大于等于0: 接收成功，返回已发送数据的长度
 *            小于0: 接收失败，取值参见ant_soc_errcode_e定义
 */
ant_s32 ant_soc_recvfrom(ant_s32 sock, ant_u8 *data, ant_s32 len, 
                       ant_sockaddr *sock_addr, ant_u32 size);

/**
 * 绑定主机地址(端口号)
 *
 * @param sock socket句柄
 * @param sock_addr 绑定的主机地址(端口号)缓存
 * @param len 绑定的主机地址(端口号)缓存长度
 * @return 大于等于0: 绑定成功
 *            小于0: 绑定失败，取值参见ant_soc_errcode_e定义
 */
ant_s32 ant_soc_bind(ant_s32 sock, ant_sockaddr *sock_addr, ant_u32 len);

/**
 * 监听socket
 *
 * @param sock socket句柄
 * @return    0: 成功
 *         小于0: 失败，取值参见ant_soc_errcode_e定义
 */
ant_s32 ant_soc_listen(ant_s32 sock);

/**
 * 接受主机连接
 *
 * @param sock 监听socket句柄
 * @param sock_addr 如果成功，返回对端主机地址
 *
 * @return 大于等于0: 接受成功，返回已连接的socket句柄
 *            小于0: 接收失败，取值参见ant_soc_errcode_e定义
 */
ant_s32 ant_soc_accept(ant_s32 sock, ant_sockaddr *sock_addr);

/**
 * 关闭socket
 *
 * @param sock socket句柄
 * @return    0: 成功
 *         小于0: 失败，取值参见ant_soc_errcode_e定义
 */
ant_s32 ant_soc_close(ant_s32 sock);

/**
 * 轮询socket事件
 *
 * @param ndesc  socket句柄最大值
 * @param in 轮询可读的文件句柄
 * @param out 轮询可写的文件句柄
 * @param ex 轮询发生错误的文件句柄
 * @param timeout 超时时间，单位为毫秒
 * @return   0: 成功
 *         小于0: 失败，取值参见ant_soc_errcode_e定义
 */
ant_s32 ant_soc_select(ant_u8 ndesc, 
                       ant_fd_set *in, 
                       ant_fd_set *out,
                       ant_fd_set *ex,
                       ant_timeval *timeout);

/**
 * 获取socket上次发生的错误
 *
 * @param sock socket句柄
 * @param error 返回错误码，取值参见ant_soc_errcode_e定义
 * @return    0: 成功
 *         小于0: 失败，取值参见ant_soc_errcode_e定义
 */
ant_s32 ant_soc_get_last_error(ant_s32 sock, ant_s32 *error);

/**
 * 设置socket上次发生的错误
 *
 * @param sock socket句柄
 * @param error 设置的错误码，取值参见ant_soc_errcode_e定义
 * @return    0: 成功
 *         小于0: 失败，取值参见ant_soc_errcode_e定义
 */
ant_s32 ant_soc_set_last_error(ant_s32 sock, ant_s32 error);

/**
 * 将socket错误码转换成字符串
 *
 * @param errcode 错误码，取值参见ant_soc_errcode_e定义
 * @return  非空: 返回错误码字符串
 *         NULL: 失败
 */
ant_char* ant_soc_strerr(ant_s32 errcode);

/**
 * 等待socket可读
 *
 * @param sock socket句柄
 * @param timeout 超时时间，单位为毫秒
 * @return    0: 成功，socket可读
 *         小于0: 失败，取值参见ant_soc_errcode_e定义
 */
ant_s32 ant_soc_wait_for_read(ant_s32 sock, ant_u32 timeout);

/**
 * 等待socket可写
 *
 * @param sock socket句柄
 * @param timeout 超时时间，单位为毫秒
 * @return    0: 成功，socket可写
 *         小于0: 失败，取值参见ant_soc_errcode_e定义
 */
ant_s32 ant_soc_wait_for_write(ant_s32 sock, ant_u32 timeout);

#ifdef __cplusplus
}
#endif

#endif

