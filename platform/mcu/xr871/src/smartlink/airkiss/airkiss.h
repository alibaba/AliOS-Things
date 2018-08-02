/*
 * airkiss.h
 *
 *  Created on: 2015-1-26
 *      Author: peterfan
 */

#ifndef _AIRKISS_H_
#define _AIRKISS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "smartlink/airkiss/wlan_airkiss.h"

/*
 * 定义AIRKISS_ENABLE_CRYPT为1以启用AirKiss加密功能
 */
#ifndef AIRKISS_ENABLE_CRYPT
#define AIRKISS_ENABLE_CRYPT 1
#endif

enum loglevel{
	OFF = 0,
	ERROR = 1,
	INFO = 2,
};

#define AIRKISS_DBG(level, fmt, args...) \
	do {		\
		if (level <= g_debuglevel)	\
			printf("[airkiss]"fmt,##args);	\
	} while (0)


#define AIRKISS_BUG_ON(v) do {if(v) {printf("BUG at %s:%d!\n", __func__, __LINE__); \
			      while (1);}} while (0)

typedef void* (*airkiss_memset_fn) (void* ptr, int value, unsigned int num);
typedef void* (*airkiss_memcpy_fn) (void* dst, const void* src, unsigned int num);
typedef int (*airkiss_memcmp_fn) (const void* ptr1, const void* ptr2, unsigned int num);
typedef int (*airkiss_printf_fn) (const char* format, ...);

/*
 * 对AirKiss库进行配置，目前仅定义了一些回调函数
 */
typedef struct
{
	/*
	 * 为尽量减少库文件体积，如下c标准库函数需要上层使用者提供
	 * 其中printf可以为NULL
	 */
	airkiss_memset_fn memset;
	airkiss_memcpy_fn memcpy;
	airkiss_memcmp_fn memcmp;
	airkiss_printf_fn printf;

} airkiss_config_t;



/*
 * AirKiss API工作需要的结构体，必须为全局变量或者通过malloc动态分配
 */
typedef struct
{
	int dummyap[26];
	int dummy[32];
} airkiss_context_t;



/*
 * AirKiss解码成功后的结果
 */
typedef struct
{
	char* pwd;		/* wifi密码，以'\0'结尾 */
	char* ssid;		/* wifi ssid，以'\0'结尾 */
	unsigned char pwd_len;	/* wifi密码长度 */
	unsigned char ssid_len;	/* wifi ssid长度 */
	unsigned char random;	/* 随机值，根据AirKiss协议，当wifi连接成功后，需要通过udp向10000端口广播这个随机值，这样AirKiss发送端（微信客户端或者AirKissDebugger）就能知道AirKiss已配置成功 */
	unsigned char reserved;	/* 保留值 */
} airkiss_result_t;


#if AIRKISS_ENABLE_CRYPT

/*
 * 设置解密key，最长可以为128bit，若传入的key不足128bit，则默认用0填充
 *
 * 返回值
 * 		< 0：出错，通常是参数错误
 * 		  0：成功
 */
int airkiss_set_key(airkiss_context_t* context, const unsigned char* key, unsigned int length);

#endif



/*
 * 获取AirKiss库版本信息
 */
const char* airkiss_version(void);



/*
 * 初始化AirKiss库，如要复用context，可以多次调用
 *
 * 返回值
 * 		< 0：出错，通常是参数错误
 * 		  0：成功
 */
int airkiss_init(airkiss_context_t* context, const airkiss_config_t* config);



/*
 * 开启WiFi Promiscuous Mode后，将收到的包传给airkiss_recv以进行解码
 *
 * 参数说明
 * 		frame：802.11 frame mac header(must contain at least first 24 bytes)
 * 		length：total frame length
 *
 * 返回值
 * 		 < 0：出错，通常是参数错误
 * 		>= 0：成功，请参考airkiss_status_t
 */
int airkiss_recv(airkiss_context_t* context, const void* frame, unsigned short length);



/*
 * 当airkiss_recv()返回AIRKISS_STATUS_COMPLETE后，调用此函数来获取AirKiss解码结果
 *
 * 返回值
 * 		< 0：出错，解码状态还不是AIRKISS_STATUS_COMPLETE
 * 		  0：成功
 */
int airkiss_get_result(airkiss_context_t* context, airkiss_result_t* result);


/*
 * 上层切换信道以后，可以调用一下本接口清缓存，降低锁定错信道的概率，注意调用的逻辑是在airkiss_init之后
 *
 * 返回值
 * 		< 0：出错，通常是参数错误
 * 		  0：成功
 */
int airkiss_change_channel(airkiss_context_t* context);

/*
 *
 * 以上是实现智能配置网络的相关API，以下是微信内网发现相关API
 *
 */

/*
 * airkiss_lan_recv()的返回值
 */
typedef enum
{
	/* 提供的数据缓冲区长度不足 */
	AIRKISS_LAN_ERR_OVERFLOW = -5,

	/* 当前版本不支持的指令类型 */
	AIRKISS_LAN_ERR_CMD = -4,

	/* 打包数据出错 */
	AIRKISS_LAN_ERR_PAKE = -3,

	/* 函数传递参数出错 */
	AIRKISS_LAN_ERR_PARA = -2,

	/* 报文数据错误 */
	AIRKISS_LAN_ERR_PKG = -1,

	/* 报文格式正确，但是不需要设备处理的数据包 */
	AIRKISS_LAN_CONTINUE = 0,

	/* 接收到发现设备请求数据包 */
	AIRKISS_LAN_SSDP_REQ = 1,

	/* 数据包打包完成 */
	AIRKISS_LAN_PAKE_READY = 2


} airkiss_lan_ret_t;


typedef enum
{
	AIRKISS_LAN_SSDP_REQ_CMD = 0x1,
	AIRKISS_LAN_SSDP_RESP_CMD = 0x1001,
	AIRKISS_LAN_SSDP_NOTIFY_CMD = 0x1002
} airkiss_lan_cmdid_t;

/*
 * 设备进入内网发现模式后，将收到的包传给airkiss_lan_recv以进行解析
 *
 * 参数说明
 * 		body：802.11 frame mac header(must contain at least first 24 bytes)
 * 		length：total frame length
 * 		config：AirKiss回调函数
 *
 * 返回值
 * 		 < 0：出错，请参考airkiss_lan_ret_t，通常是报文数据出错
 * 		>= 0：成功，请参考airkiss_lan_ret_t
 */
int airkiss_lan_recv(const void* body, unsigned short length, const airkiss_config_t* config);

/*
 * 设备要发送内网协议包时，调用本接口完成数据包打包
 *
 * 参数说明
 * 		body：802.11 frame mac header(must contain at least first 24 bytes)
 * 		length：total frame length
 * 		config：AirKiss回调函数
 *
 * 返回值
 * 		 < 0：出错，请参考airkiss_lan_ret_t，通常是报文数据出错
 * 		>= 0：成功，请参考airkiss_lan_ret_t
 */
int airkiss_lan_pack(airkiss_lan_cmdid_t ak_lan_cmdid, void* appid, void* deviceid, void* _datain, unsigned short inlength, void* _dataout, unsigned short* outlength, const airkiss_config_t* config);

typedef struct {
	char *app_id;
	char *device_id;
	uint32_t ack_period_ms;
} Airkiss_Online_Ack_Info;

#define AK_KEY_LEN 16
#define CHTABLESIZE 13
#define MAX_TIME 4294967295
#define AIRKISS_CYCLE_ACK_THREAD_STACK_SIZE	(1024 * 1)
#define AIRKISS_ONLINE_DIALOG_THREAD_STACK_SIZE	(1024 * 1)

#define AIRKISS_ACK_UDP_PORT 10000
#define AIRKISS_LAN_BUF_LEN 200

#define AK_ACK_TIME_OUT_MS 3000

#define AK_TASK_RUN     (1 << 0)
#define AK_TASK_STOP    (1 << 1)

typedef struct airkiss_priv {
	struct netif *nif;
	airkiss_context_t context;
	airkiss_result_t result;
	airkiss_status_t status;
#if AIRKISS_ENABLE_CRYPT
	char aes_key[AK_KEY_LEN];
#endif
	airkiss_config_t func;
	uint8_t waiting;
	uint8_t ack_run;
} airkiss_priv_t;

int airkiss_ack_start(airkiss_priv_t *priv, uint32_t random_num, uint32_t timeout_ms);
int airkiss_ack_stop(airkiss_priv_t *priv);

#ifdef __cplusplus
}
#endif

#endif /* _AIRKISS_H_ */
