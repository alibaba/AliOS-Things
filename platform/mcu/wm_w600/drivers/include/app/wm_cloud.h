/**
 * @file    wm_cloud.h
 *
 * @brief   Cloud Interface API
 *
 * @author  winnermicro
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#ifndef __WM_CLOUD_H__
#define __WM_CLOUD_H__

#include "wm_config.h"
#if TLS_CONFIG_CLOUD
#include "wm_sockets.h"
#if TLS_CONFIG_HTTP_CLIENT_SECURE
#include "matrixsslApi.h"
#endif

#define SERVER_STATE_OFFLINE 0
#define SERVER_STATE_ONLINE 1

#define SOCKET_TCP 1
#define SOCKET_CLINET 2
#define SOCKET_SSL  4

/**
 * 开发者自定义socket的信息，目前sock_type不支持Tcp server，
 * 可支持udp、tcp client以及ssl的tcp连接
 */
typedef struct _SOCKET{
	int socket_num;  			/**< socket号，socket建立成功后，系统填入，
										开发者初始化时不必填写 */
	struct sockaddr * sock_addr;/**< socket地址，udp时，表示本地地址，
										tcp时，表示对端地址 */
	u8 sock_type; 				/**< socket类型定义，目前仅低三个bit有效，
										具体查看SOCKET_xxx定义 */
	u8 connected;  				/**< socket连接标志，仅tcp client有效，
										为1表示连接成功 */
	void * user_param;			/**< 开发者自定义信息，用来区分是否为开发者
										自定义socket */
#if TLS_CONFIG_HTTP_CLIENT_SECURE
	ssl_t * ssl; 				/**< ssl连接信息，仅SOCKET_SSL时有效 */
#endif
}SOCKET;

/** 开发者自定义socket接收数据的格式 */
typedef struct CReadData{
	SOCKET *socket;  			/**< socket相关信息 */
	char * read_data;			/**< 接收的数据 */
	u32     data_len;  			/**< 接收数据长度 */
	struct sockaddr_in sin_recv;/**< 对端socket地址，仅udp时有效 */
}CloudReadData;

#if TLS_CONFIG_CLOUD_KII
/** 云端回调数据格式 */
typedef struct _Cloud_Callback_data
{
	char* bucketName;/**< bucket名称 */
	char* objectId;  /**< 对象id */
	char* jsonObject;/**< 对象数据，通常用json格式表示 */
} CloudCallbackData, *PCloudCallbackData;
/** 设置上传云端回调，errcode为0表示上传成功，data为上传数据，
如果上传成功，可以从data->objectId得到上传云的数据对象id */
typedef void (*upload_callback)(PCloudCallbackData data, int errcode);
/** 上传云端的数据格式 */
typedef struct _CloudData
{
	char* bucketName;    /**< bucket名称 */
	char* jsonObject;    /**< 对象数据，通常用json格式表示 */
	upload_callback cb;  /**< 上传结果回调 */
} CloudData,*PCloudData;

/** 初始化云参数定义 */
typedef struct _Cloud_Init_arg
{
	char* site;       /**< 服务器地址，目前可传入"CN" "US" "JP" "SG" */
	char* appId;      /**< 应用ID */
	char* appKey;     /**< 应用Key */
	char* deviceType; /**< 设备类型 */
	char* venderId;   /**< 设备ID，通常为设备的MAC地址 */
	char* modelName;  /**< 模块名称，自定义字符串，OTA推送的bucketName格式为 */
	char* password;   /**< 设备密码 */
	char* recvDataBucket;    /**< 设备推送的bucketName */
} CloudInitArg, *PCloudInitArg;
#endif

#if TLS_CONFIG_CLOUD_JD
/** 云操作类型枚举 */
enum cloud_opt_type
{
	UPLOAD_REQ,    	/**< 上传数据请求 */
	UPLOAD_RESP,    /**< 上传数据响应 */
	CONTROL_REQ,    /**< 云端控制命令请求 */
	CONTROL_RESP,   /**< 控制命令响应 */
	SNAPSHOT_REQ,   /**< 云端抓取快照（状态）请求 */
	SNAPSHOT_RESP   /**< 快照（状态）响应 */
};

/**
 *  云端通信数据结构，需要注意一下两点:
 *	1)作为tls_cloud_upload_data方法参数:
 *	 上传数据请求、控制命令响应、快照（状态）响应接口，
 *	 对于上传数据请求，如果此接口返回后，data->opt为UPLOAD_RESP，
 *	 则实际上同步响应了上传命令，此时data->result即为上传数据后云端返回结果。
 *	 CloudData需要调用者自己分配内存，自己释放。
 *	2)作为cloud_callback回调参数:
 *	上传数据响应、云端控制命令下达、云端抓取快照（状态）请求时，
 *	cloud_callback回调接口被调用。
 *	对于云端控制命令请求、云端抓取快照请求，如果在此回调接口返回后，
 *	data->opt为CONTROL_RESP或SNAPSHOT_RESP，则实际上同步响应了此命令，
 *	此时data->result即为控制命令响应结果，data->values中包含了快照响应结果。
 *	同步响应SNAPSHOT_RESP时，names和values部分的内存以及同步响应CONTROL_RESP时，
 *	data->control_resp部分的内存由开发者申请，在云实现中释放。
 */
typedef struct _CloudData
{
	enum cloud_opt_type opt;/**< 操作类型枚举 */
	int result;    		/**< 云端响应上传数据或设备响应云端控制结果 */
	char* control_resp; /**< 云端控制命令失败原因 */
	char** names;  		/**< 上传数据请求、云端控制请求、
							 云端快照请求或快照响应的字段名 */
	char** values;    	/**< 上传数据请求、云端控制请求、快照响应的值 */
	int cnt;    		/**< 字段个数 */
	void * arg;    		/**< 云端发送过来的参数，需要原封不动回传给云端 */
} CloudData,*PCloudData, CloudCallbackData, *PCloudCallbackData;
/** 初始化云参数定义，应该传入product_uuid */
typedef struct _Cloud_Init_arg
{
	char *product_uuid;    /**< 产品UUID */
} CloudInitArg, *PCloudInitArg;
#endif
#if TLS_CONFIG_CLOUD_GAGENT

/** 云端通信数据结构 */
typedef struct _CloudData
{
    int32 totalcap;    /**< 全局缓存区的大小 4k+128byte */
    int32 remcap;      /**< 数据包头的冗余长度 128byte */
    uint8 *allbuf;     /**< 数据起始地址 */
    int32 bufcap;      /**< 数据区域的大小 4K */

    uint32 type;       /**< 消息体类型 */
    uint8 *phead;      /**< 数据包头起始地址 */
    uint8 *ppayload;   /**< 业务逻辑头起始地址 */
    uint8 *pend;       /**< 数据结束地址 */
} CloudData,*PCloudData, CloudCallbackData, *PCloudCallbackData;

/** 初始化云参数定义 */
typedef struct _Cloud_Init_arg
{
	char* product_key;    	/**< 产品标识码 */
	char* wifi_soft_ver;    /**< 软件版本号 */
	char* wifi_hard_ver;    /**< 硬件版本号 */
} CloudInitArg, *PCloudInitArg;
#endif
#if TLS_CONFIG_CLOUD_360
/**
 *          设置上传云端回调，errcode为0表示上传成功，user_param为
 *          调用tls_cloud_upload_data时传入的CloudData中的user_param
 */
typedef void (*upload_callback)(void* user_param, int errcode);

/** 云端回调数据格式 */
typedef struct _Cloud_Callback_data
{
    const uint8_t* data;    /**< 数据 */
    size_t datalen;    		/**< 数据长度 */
}CloudCallbackData, *PCloudCallbackData;

/** 上传云端的数据格式 */
typedef struct _CloudData
{
    void* user_param;    	/**< 用户参数，在上传回调中传入此参数 */
    const uint8_t* data;    /**< 数据 */
    size_t datalen;    		/**< 数据长度 */
    upload_callback cb;    	/**< 上传结果回调 */
} CloudData,*PCloudData;

/** 初始化云参数定义 */
typedef struct _Cloud_Init_arg
{
	int product_id;    				/**< 产品ID */
	unsigned char product_aes[16];  /**< 产品AES密钥 */
	char *device_name;    			/**< 设备名称 */
} CloudInitArg, *PCloudInitArg;
#endif

#if TLS_CONFIG_CLOUD_QQ

/**
 *          设置上传云端回调，errcode为0表示上传成功，cookie为自增
 *          的序号，和在调用tls_cloud_upload_data时得到的CloudData中
 *          的pCookie相同
 *
 */
typedef void (*upload_callback)(int32_t err_code, uint32_t cookie);

/** 云端回调数据格式 */
typedef struct  _Cloud_Callback_data {
    uint32_t property_id;    	   /**< 属性ID */
    uint8_t *property_value;       /**< 属性value，类型为字节数组 */
    uint32_t property_value_len;   /**< 属性value的长度 */
    uint32_t seq;    	/**< 操作序号 */
    int32_t  ret_code;  /**< 收到cc datapoint，回复ACK时设置此值供H5页面使用 */
} CloudCallbackData, *PCloudCallbackData;

/** 上传云端的数据格式 */
typedef struct _CloudData
{
	CloudCallbackData *datapoints;    	/**< 数据结构 */
	uint32_t datapoints_count;    		/**< 个数 */
	upload_callback cb;    				/**< 上传结果回调 */
	uint32_t *pCookie;    				/**< 自增的序号 */
}CloudData,*PCloudData;

/** 初始化云参数定义 */
typedef struct _Cloud_Init_arg{
    /* 硬件信息 */
    char*    device_name;    /**< 设备名称,'\0'结尾字符串长度不超过32字节，
    							  nettype为1时必须与网站注册的devicename一致 */
    char*    sn;    		/**< 设备序列号,'\0'结尾字符串长度必须是16字节 */
    char*    license;    	/**< 设备LICENSE */
    int32_t  product_id;    /**< 每一个厂商的每一种类型的设备对应的一个id */
    int32_t  product_version;    /**< 产品的版本信息，用于OTA，默认填0 */
    /** 密钥信息 */
    uint8_t* client_pub_key;    /**< client公钥，每台设备生成后烧录到设备 */
    int32_t  client_pub_key_len;
    uint8_t* auth_key;    		/**< authkey，每台设备生成后烧录到设备 */
    int32_t  auth_key_len;
} CloudInitArg, *PCloudInitArg;
#endif
#if TLS_CONFIG_CLOUD_ABLECLOUD

/** ABLECLOUD云端通信数据结构 */
typedef struct _CloudData
{
    u8 msg_id;    /**< 消息ID，响应消息同云端下发的消息邋ID，主动上传任意 */
    u8 msg_code;  /**< 消息Code，响应消息同云端下发的消息code，
    				   主动上传需要大于200 */
    u8 *data;    /**< 数据 */
    uint32 datalen;    /**< 数据长度 */
} CloudData,*PCloudData, CloudCallbackData, *PCloudCallbackData;

/** 初始化云参数定义 */
typedef struct _Cloud_Init_arg
{
	u32 major_domain;    	/**< 主域ID */
	u16 sub_domain;    		/**< 子域ID */
	u8 private_key[112];    /**< 产品私钥 */
	u32 product_version;    /**< 产品的版本信息，用于OTA */
} CloudInitArg, *PCloudInitArg;
#endif
#if TLS_CONFIG_CLOUD_ELIAN
enum
{
	DATA_TYPE_BINARY = 0,    /**< 二进制 */
	DATA_TYPE_TEXT = 1,      /**< 文本格式 */
	DATA_TYPE_NV = 2,        /**< 以空格分离的命令加参数的格式，
								  如：power 34。 */
	DATA_TYPE_EL_CMD = 3,    /**< 为4字节整数命令Id+4字节整数的值，
								  如：140 (电源状态上报，14为整数；
								  0也为整数，表示状态）。 */
	DATA_TYPE_INFRA = 6,     /**< 红外格式 */
	DATA_TYPE_STATUS_REQ = 7,/**< 设备状态请求 */
	DATA_TYPE_EL_CMD2 = 8    /**< 为4字节整数命令Id+字符串格式的值，
								  如：140 (电源状态上报，14为整数；
								  0为字符串，表示状态)。 */
};

/** 云端通信数据结构 */
typedef struct _CloudData
{
    const uint8_t* data;    /**< 数据 */
    size_t datalen;    		/**< 数据长度 */
    u8 datatype;
} CloudData,*PCloudData, CloudCallbackData, *PCloudCallbackData;

/** 初始化云参数定义 */
typedef struct _Cloud_Init_arg
{
	char*                 sn;    /**< 设备序列号,'\0'结尾字符串长度
									  必须是13字节 */
} CloudInitArg, *PCloudInitArg;
#endif

/** 设备在云端注册后得到的关于设备的信息 */
typedef struct _Cloud_Dev_info{
	u8* device_id;    	 /**< 设备ID */
	u8* access_token;    /**< 设备token，和设备ID一起构成设备访问云的凭证 */
}CloudDeviceInfo, *PCloudDeviceInfo;


/**
 * @brief          设置云的回调
 *
 * @param[in]      data    
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
typedef int (*cloud_callback)(PCloudCallbackData data);

/**
 * @brief          设置云状态的回调，status为1表示设备在线，status为0表示
                   设备不在线
 *
 * @param[in]      status    
 *
 * @retval         0 表示设备不在线
 * @retval         1 表示设备在线
 *
 * @note           None
 */
typedef void (*cloud_online_status_callback)(int status);

/**
 * @defgroup APP_APIs APP APIs
 * @brief APP APIs
 */

/**
 * @addtogroup APP_APIs
 * @{
 */

/**
 * @defgroup cloud_APIs cloud APIs
 * @brief cloud APIs
 */

/**
 * @addtogroup cloud_APIs
 * @{
 */

/**
 * @brief          初始化云，user task初始化时调用，
 				   对京东云，arg为product_uuid，
 				   对其它云arg请查看CloudInitArg结构定义
 *
 * @param[in]      arg
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
int tls_cloud_init(PCloudInitArg arg);

/**
 * @brief          注销云，设备下线时调用，销毁资源。block为0时，此函数立
                   即返回，注销云在后台执行，block为1时，云注销完成后此函数
                   返回。
 *
 * @param[in]      block   是否阻塞，为1表示，直到云注销完成返回，
 						   为0表示，立即返回，云注销在后台执行完成
 *
 * @return         None
 *
 * @note           None
 */
void tls_cloud_finish(u8 block);

/**
 * @brief          设备向云上传数据，包括对控制命令的响应，上传状态以及设
                   备快照等
 *
 * @param[in]      data
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
int tls_cloud_upload_data(PCloudData data);

/**
 * @brief          设置云的回调接口，是设备接收云的控制命令，推送消息等的
                   唯一接口
 *
 * @param[in]      callback
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
int tls_cloud_set_callback(cloud_callback callback);

/**
 * @brief          设置云状态的回调接口，设备上线，下线时回调此接口
 *
 * @param[in]      callback
 *
 * @return         None
 *
 * @note           None
 */
void tls_cloud_set_online_status_callback(cloud_online_status_callback callback);

/**
 * @brief          获得设备在云端注册后的设备相关信息，
 				   具体查看CloudDeviceInfo结构定义
 *
 * @param[in]      info
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
int tls_cloud_get_device_info(PCloudDeviceInfo info);

/*
* 以下extern开头的函数，需要开发者实现，具体实现查看wm_lsd_device_found.c，此为我们写的一个局域网设备发现的简单实现，
* 开发者可以根据需要，写自己的设备发现实现，或者是局域网通信控制等。
*/

/**
 * @brief          获得开发自定义的socket信息，count为最大支持的socket数，
                   根据不同的云此参数有所不同
 *
 * @param[in]      sockets
 * @param[in]      count
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
extern int tls_cloud_get_custom_sockets(SOCKET** sockets, int count);

/**
 * @brief          开发者自定义socket接收数据的处理函数，此函数在socket接
                   收到数据时被调用，
                   开发者可根据SOCKET中的user_param判断是否为自定义socket
                   ，然后做相应的处理
 *
 * @param[in]      data
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
extern int tls_cloud_socket_data_handler(CloudReadData* data);


/**
 * @brief          通过socket发送数据，其中to和tolen只有udp时有效并且必须
                   ，tcp时to参数传入NULL
 *
 * @param[in]      *sock
 * @param[in]      *data
 * @param[in]      size
 * @param[in]      flags
 * @param[in]      *to
 * @param[in]      tolen
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
int tls_cloud_socket_sendto(SOCKET *sock, const void *data, size_t size, int flags,
       const struct sockaddr *to, socklen_t tolen);

/**
 * @}
 */

/**
 * @}
 */

#endif //TLS_CONFIG_CLOUD
#endif

