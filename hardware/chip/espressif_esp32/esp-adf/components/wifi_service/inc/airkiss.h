/*
 * airkiss.h
 *
 *  Created on: 2015-1-26
 *      Author: peterfan
 */

#ifndef AIRKISS_H_
#define AIRKISS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 定义AIRKISS_ENABLE_CRYPT为1以启用AirKiss加密功能
 */
#ifndef AIRKISS_ENABLE_CRYPT
#define AIRKISS_ENABLE_CRYPT 1
#endif


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
	char* pwd;						/* wifi密码，以'\0'结尾 */
	char* ssid;						/* wifi ssid，以'\0'结尾 */
	unsigned char pwd_length;		/* wifi密码长度 */
	unsigned char ssid_length;		/* wifi ssid长度 */
	unsigned char random;			/* 随机值，根据AirKiss协议，当wifi连接成功后，需要通过udp向10000端口广播这个随机值，这样AirKiss发送端（微信客户端或者AirKissDebugger）就能知道AirKiss已配置成功 */
	unsigned char reserved;			/* 保留值 */
} airkiss_result_t;



/*
 * airkiss_recv()正常情况下的返回值
 */
typedef enum
{
	/* 解码正常，无需特殊处理，继续调用airkiss_recv()直到解码成功 */
	AIRKISS_STATUS_CONTINUE = 0,

	/* wifi信道已经锁定，上层应该立即停止切换信道 */
	AIRKISS_STATUS_CHANNEL_LOCKED = 1,

	/* 解码成功，可以调用airkiss_get_result()取得结果 */
	AIRKISS_STATUS_COMPLETE = 2

} airkiss_status_t;



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
 * 		body：通过监听端口接收到的UDP报文
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
 * 		ak_lan_cmdid：要发送的数据包类型，查看airkiss_lan_cmdid_t
 * 		appid：对应device_type
 * 		deviceid：当前设备的ID
 * 		_datain：要发送的数据内容
 * 		inlength：要发送的数据内容长度
 * 		_dataout：输出数据缓冲区
 * 		outlength：传入时应赋值为输出数据缓冲区的大小，函数返回后会赋值为要发送的数据大小
 * 		config：AirKiss回调函数
 *
 * 返回值
 * 		 < 0：出错，请参考airkiss_lan_ret_t，通常是报文数据出错
 * 		>= 0：成功，请参考airkiss_lan_ret_t
 */
int airkiss_lan_pack(airkiss_lan_cmdid_t ak_lan_cmdid, void* appid, void* deviceid, void* _datain, unsigned short inlength, void* _dataout, unsigned short* outlength, const airkiss_config_t* config);

#ifdef __cplusplus
}
#endif

#endif /* AIRKISS_H_ */
