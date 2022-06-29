/**
 * @file aiot_ota_api.h
 * @brief OTA模块头文件, 提供设备获取固件升级和远程配置的能力
 * @date 2019-12-27
 *
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 *
 * @details
 *
 * OTA模块可用于配合阿里云平台的固件升级服务, 在[推送固件到设备](https://help.aliyun.com/document_detail/58328.html)页面有介绍OTA的控制操作流程
 *
 * + 参考[设备端OTA升级](https://help.aliyun.com/document_detail/58328.html)页面, 了解设备端配合OTA升级时的网络交互流程
 *
 *     + 设备要使用OTA服务, 必须先以@ref aiot_ota_report_version 向云端上报当前的固件版本号, 否则无法接收到固件信息推送
 *     + 设备下载固件时, 设备自行用@ref aiot_download_send_request 从云端获取, SDK不会自动下载固件
 *     + 设备下载中或者下载完成后, 设备需自行将内存buffer中的固件内容写到Flash上, SDK不含有固件烧录的逻辑
 *     + 设备升级完成后, 需以@ref aiot_ota_report_version 接口, 向云端上报升级后的新固件版本号, 否则云端不会认为升级完成
 *     + 设备从@ref aiot_ota_recv_handler_t 收到升级任务时, 如果忽略了该任务, 可以通过调用@ref aiot_ota_query_firmware 再次收到该任务
 *
 * + 用户在控制台推送固件的下载地址等信息给设备时, 是通过MQTT通道, 所以设备OTA升级的前提是成功建立并保持MQTT长连接通道在线
 *
 */

#ifndef __AIOT_OTA_API_H__
#define __AIOT_OTA_API_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief 云端下行的OTA消息的类型, 分为固件升级和远程配置两种
 *
 * @details
 * 传入@ref aiot_ota_recv_handler_t 的MQTT报文类型
 *
 */
typedef enum {

    /**
     * @brief 收到的OTA消息为固件升级消息
     *
     */
    AIOT_OTARECV_FOTA,

    /**
     * @brief 收到的OTA消息为远程配置消息
     *
     */
    AIOT_OTARECV_COTA
} aiot_ota_recv_type_t;

/**
 * @brief OTA过程中使用的digest方法类型, 分为MD5和SHA256两种
 *
 */

typedef enum {

    /**
     * @brief 收到的OTA固件的digest方法为MD5
     *
     */
    AIOT_OTA_DIGEST_MD5,

    /**
     * @brief 收到的OTA固件的digest方法为SHA256
     *
     */
    AIOT_OTA_DIGEST_SHA256,
    AIOT_OTA_DIGEST_MAX
} aiot_ota_digest_type_t;

/**
 * @brief 云端下推的固件升级任务的描述信息, 包括url, 大小, 签名等
 *
 */
typedef struct {

    /**
     * @brief 待升级设备的product_key
     *
     */
    char       *product_key;

    /**
     * @brief 待升级设备的device_name
     *
     */
    char       *device_name;

    /*
     * @brief 下载固件所需的链接
     *
     */
    char       *url;

    /*
     * @brief 固件的大小, 单位为Byte
     *
     */
    uint32_t    size_total;

    /*
     * @brief 云端对的固件进行数字签名的方式, 具体见@ref aiot_ota_digest_type_t
     *
     */
    uint8_t     digest_method;

    /*
     * @brief 云端对固件计算数字签名得出来的结果
     *
     */
    char       *expect_digest;

    /**
     * @brief 固件的版本信息. 如果为固件信息, 则这个version字段为固件的版本号. 如果为远程配置消息, 则为配置的configId
     *
     */
    char       *version;

    /**
     * @brief 当前固件所都对应的模块
     *
     */
    char       *module;

    /**
     * @brief  *固件升级过程中需用往云端上报消息时需要用到的mqtt句柄
     */
    void       *mqtt_handle;

    /**
     * @brief 当前下载信息中的扩展内容
     *
     */
    char       *extra_data;
} aiot_download_task_desc_t;

/**
* @brief 云端下行的OTA消息, 包括其消息类型(固件升级/远程配置)和升级任务的具体描述
*
*/
typedef struct {

    /**
     * @brief 云端下行的OTA消息类型, 更多信息请参考@ref aiot_ota_recv_type_t
     */
    aiot_ota_recv_type_t        type;

    /**
     * @brief 云端下推的固件升级任务的描述信息, 包括url, 大小, 签名等, 更多信息参考@ref aiot_download_task_desc_t
     */
    aiot_download_task_desc_t   *task_desc;
} aiot_ota_recv_t;

/**
 * @brief 设备收到OTA的mqtt下行报文时的接收回调函数.用户在这个回调函数中可以看到待升级固件的版本号, 决定升级策略(是否升级, 何时升级等)

 * @param[in] handle OTA实例句柄
 * @param[in] msg 云端下行的OTA消息
 * @param[in] userdata 用户上下文
 *
 * @return void
 */
typedef void (* aiot_ota_recv_handler_t)(void *handle, const aiot_ota_recv_t *const msg, void *userdata);

/**
* @brief 下载固件过程中收到的分片的报文的类型
*
*/
typedef enum {

    /**
    * @brief 基于HTTP传输的固件分片报文
    *
    */
    AIOT_DLRECV_HTTPBODY
} aiot_download_recv_type_t;

/**
* @brief 下载固件过程中收到的分片的报文的描述, 包括类型, 以及所存储的buffer地址, buffer的长度, 以及当前的下载进度
*
*/
typedef struct {

    /**
    * @brief 下载固件过程中收到的分片的报文的类型, 具体见@ref aiot_download_recv_type_t
    *
    */
    aiot_download_recv_type_t type;

    struct {

        /**
        * @brief 下载固件过程中, SDK分配出来的存储云端下行的固件内容的buffer地址.在回调函数结束后SDK就会主动释放.用户需要自行将报文拷贝保存.
        *
        */
        uint8_t *buffer;

        /**
        * @brief 下载固件过程中, SDK分配出来的存储云端下行的固件内容的buffer的大小, 用户可以通过@ref AIOT_DLOPT_BODY_BUFFER_MAX_LEN 来调整
        *
        */
        uint32_t len;

        /**
        * @brief 当前的下载进度的百分比
        *
        */
        int32_t  percent;
    } data;
} aiot_download_recv_t;


/**
 * @brief 升级开始后, 设备收到分成一段段的固件内容时的收包回调函数.当前默认是通过https报文下推分段后的固件内容.

 * @param[in] handle download实例句柄
 * @param[in] packet 云端下行的分段后的固件的报文
 * @param[in] userdata 用户上下文
 *
 * @return void
 */
typedef void (* aiot_download_recv_handler_t)(void *handle, const aiot_download_recv_t *packet,
        void *userdata);

/**
 * @brief 与云端约定的OTA过程中的错误码, 云端据此知道升级过程中出错在哪个环节
 *
 */

typedef enum {

    /**
     * @brief 与云端约定的设备升级出错的错误描述
     */
    AIOT_OTAERR_UPGRADE_FAILED = -1,

    /**
     * @brief 与云端约定的设备下载出错的错误码描述
     */
    AIOT_OTAERR_FETCH_FAILED = -2,

    /**
     * @brief 与云端约定的固件校验数字签名时出错的错误码描述
     */
    AIOT_OTAERR_CHECKSUM_MISMATCH = -3,

    /**
     * @brief 与云端约定的烧写固件出错的错误码描述
     */
    AIOT_OTAERR_BURN_FAILED = -4
} aiot_ota_protocol_errcode_t;

/**
 * @brief 调用 @ref aiot_ota_setopt 接口时, option参数的可用值
 *
 */

typedef enum {

    /**
     * @brief 设置处理OTA消息的用户回调函数
     *
     * @details
     *
     * 在该回调中, 用户可能收到两种消息: 固件升级消息或者远程配置消息.
     *
     * 无论哪种消息, 都包含了url, version, digest method, sign等内容.
     *
     * 用户需要在该回调中决定升级策略, 包括是否升级和何时升级等. 如果需要升级, 则需要调用aiot_download_init初始化一个download实例句柄.
     * 具体见demos目录下的fota_xxx_xxx.c的用例
     *
     * 数据类型: (void *)
     */
    AIOT_OTAOPT_RECV_HANDLER,

    /**
     * @brief 设置MQTT的handle
     *
     * @details
     *
     * OTA过程中使用MQTT的通道能力, 用以向云端上报版本号, 进度, 以及错误码
     *
     * 数据类型: (void *)
     */
    AIOT_OTAOPT_MQTT_HANDLE,

    /**
      * @brief 用户需要SDK暂存的上下文
      *
      * @details
      *
      * 该上下文会在@ref AIOT_OTAOPT_RECV_HANDLER 中传回给用户
      *
      * 数据类型: (void *)
      */
    AIOT_OTAOPT_USERDATA,

    /**
      * @brief 如果当前ota是针对某个外接模块(mcu等), 需要通过该字段设置模块名
      *
      * @details
      *
      * OTA可能会针对某个外接的模块进行, 在上报版本号的时候, 需要知道模块的名称.
      * 模块的名称通过该字段来设置.
      *
      * 数据类型: (void *)
      */
    AIOT_OTAOPT_MODULE,
    AIOT_OTAOPT_MAX
} aiot_ota_option_t;

/**
 * @brief 调用 @ref aiot_download_setopt 接口时, option参数的可用值
 *
 */

typedef enum {

    /**
     * @brief 设备通过HTTP与固件服务器建联时, 网络使用的安全凭据
     *
     * @details
     *
     * 该配置项用于为底层网络配置@ref aiot_sysdep_network_cred_t 安全凭据数据
     *
     * 1. 若该选项不配置, 那么HTTP将以tcp方式直接建联
     *
     * 2. 若@ref aiot_sysdep_network_cred_t 中option配置为@ref AIOT_SYSDEP_NETWORK_CRED_NONE , HTTP将以tcp方式直接建联
     *
     * 3. 若@ref aiot_sysdep_network_cred_t 中option配置为@ref AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA , HTTP将以tls方式建联
     *
     * 4. 若@ref aiot_sysdep_network_cred_t 中option配置为@ref AIOT_SYSDEP_NETWORK_CRED_SVRCERT_PSK , HTTP将以tls psk方式建联
     *
     * 数据类型: (aiot_sysdep_network_cred_t *)
     */
    AIOT_DLOPT_NETWORK_CRED,

    /**
     * @brief 设备通过HTTP访问固件下载服务器的端口号
     *
     * @details
     *
     * 如果使用的是tcp或者tls证书方式, 端口号设置为443
     *
     * 数据类型: (uint16_t *)
     */
    AIOT_DLOPT_NETWORK_PORT,

    /**
     * @brief 通过HTTP接收固件内容时, 在协议栈花费的最长时间
     *
     * @details
     *
     * 数据类型: (uint32_t *) 默认值: (5 * 1000) ms
     */
    AIOT_DLOPT_RECV_TIMEOUT_MS,

    /**
     * @brief HTTP 数据接收回调函数
     *
     * @details
     *
     * 数据类型: (aiot_download_recv_handler_t)
     */
    AIOT_DLOPT_RECV_HANDLER,

    /**
     * @brief 用户需要SDK暂存的上下文
     *
     * @details
     *
     * 当接收到HTTP数据时, 该上下文会从 @ref aiot_download_recv_handler_t 的 userdata 参数给出
     *
     * 数据类型: (void *)
     */
    AIOT_DLOPT_USERDATA,

    /**
     * @brief 设置download实例句柄所包含下载任务的具体内容
     *
     * @details
     *
     * 用户在收到OTA的mqtt消息后, 如果决定升级, 则需要通过该选项, 在download实例句柄中开辟内存,
     * 将OTA消息中携带的url, version, digest method, sign等信息复制过来, 有了这些信息后才能开始下载任务
     *
     * 数据类型: (aiot_download_task_desc_t *)
     *
     **/
    AIOT_DLOPT_TASK_DESC,

    /**
     * @brief 设置按照range下载的起始地址
     *
     * @details
     *
     * HTTP 范围请求(range requests)特性中, 表示从第该byte开始下载
     * 如果指定从头开始下载, 则start的值为0
     *
     * 数据类型: (uint32_t *)
     *
     **/
    AIOT_DLOPT_RANGE_START,

    /**
     * @brief 设置按照range下载的结束地址
     *
     * @details
     * HTTP 范围请求(range requests)特性中, 表示下载到该byte后结束.
     * 如果指定从头开始下载到10个byte后结束,
     * 则需要指定start = 0, end = 9, 这样总共10个byte
     *
     * 数据类型: (uint32_t *)
     *
     **/
    AIOT_DLOPT_RANGE_END,

    /**
    * @brief 当设备从固件下载服务器接收返回的http报文时, 每次从 @ref aiot_download_recv_handler_t 回调函数中给出的body最大长度
    *
    * @details
    * 如果收到的数据没能达到这个长度, 则以aiot_download_recv_handler_t回调函数给出的长度是设备实际接收到的长度
    *
    * 数据类型: (uint32_t *) 默认值: (2 *1024) Bytes
    */
    AIOT_DLOPT_BODY_BUFFER_MAX_LEN,
    AIOT_DLOPT_MAX
} aiot_download_option_t;

/**
 * @brief 设备端主动向云端查询升级任务
 *
 * @details
 * 设备上线后, 云端如果部署了OTA任务, SDK会通过@ref aiot_ota_recv_handler_t 将该任务透给用户.
 * 出于当前业务繁忙等原因, 这个OTA任务可能被暂时忽略, 那么用户可以在业务空闲的时候通过调用本api来让云端再次下推这个OTA任务.
 * 同样地, SDK会从@ref aiot_ota_recv_handler_t 将该OTA任务透给用户
 *
 * @return int32_t
 * @retval STATE_SUCCESS 发送请求成功
 * @retval STATE_OTA_QUERY_FIRMWARE_HANDLE_IS_NULL 作为入参的handle句柄没有经过初始化, 需要调用@ref aiot_ota_init 来进行初始化
 *
 */
int32_t aiot_ota_query_firmware(void *handle);

/**
 * @brief 创建一个OTA实例
 *
 * @return void*
 * @retval 非NULL ota实例句柄
 * @retval NULL 初始化失败, 或者是因为没有设置portfile, 或者是内存分配失败导致
 *
 */
void   *aiot_ota_init();

/**
 * @brief 销毁ota实例句柄
 *
 * @param[in] handle 指向ota实例句柄的指针
 *
 * @return int32_t
 * @retval STATE_OTA_DEINIT_HANDLE_IS_NULL handle或者handle所指向的地址为空
 * @retval STATE_SUCCESS 执行成功
 *
 */
int32_t aiot_ota_deinit(void **handle);

/**
 * @brief 上报普通设备(非网关中的子设备)的版本号
 *
 * @details
 *
 * 如果云端不知道某台设备当前的固件版本号, 就不会为其提供OTA服务, 而如果不知道设备的新版本号, 也不会认为它升级成功
 *
 * 所以OTA要正常工作, 一般会要求设备在每次开机之后, 就调用这个接口, 将当前运行的固件版本号字符串上报给云端
 *
 * 参数的handle通过@ref aiot_ota_init 得到, 比如, 上报当前版本号为"1.0.0"的代码写作
 *
 * ```c
 * handle = aiot_ota_init();
 * ...
 * aiot_ota_report_version(handle, "1.0.0");
 * ```
 *
 * @param[in] handle 指向ota实例句柄的指针
 * @param[in] version 待上报的版本号
 *
 * @return int32_t
 * @retval STATE_OTA_REPORT_HANDLE_IS_NULL ota句柄为空
 * @retval STATE_OTA_REPORT_VERSION_IS_NULL 用户输入的版本号为空
 * @retval STATE_OTA_REPORT_MQTT_HANDLE_IS_NULL ota_handle句柄中的mqtt句柄为空
 * @retval STATE_OTA_REPORT_FAILED 中止执行上报
 * @retval STATE_SUCCESS 执行成功
 *
 */
int32_t aiot_ota_report_version(void *handle, char *version);

/**
 * @brief 用于网关中的子设备上报版本号
 *
 * @param[in] handle  ota实例句柄
 * @param[in] product_key 设备的product_key
 * @param[in] device_name 设备的名称
 * @param[in] version 版本号
 *
 * @return int32_t
 * @retval STATE_SUCCESS 上报成功
 * @retval STATE_OTA_REPORT_EXT_HANELD_IS_NULL ota句柄为空
 * @retval STATE_OTA_REPORT_EXT_VERSION_NULL 用户输入的版本号为空
 * @retval STATE_OTA_REPORT_EXT_PRODUCT_KEY_IS_NULL 子设备的product_key输入为空
 * @retval STATE_OTA_REPORT_EXT_DEVICE_NAME_IS_NULL 子设备的device_name输入为空
 * @retval STATE_OTA_REPORT_EXT_MQTT_HANDLE_IS_NULL ota句柄中的mqtt_handle为空
 * @retval STATE_OTA_REPORT_FAILED 中止执行上报
 *
 */
int32_t aiot_ota_report_version_ext(void *handle, char *product_key, char *device_name, char *version);

/**
 * @brief 设置ota句柄的参数
 *
 * @details
 *
 * 对OTA会话进行配置, 常见的配置选项包括
 *
 * + `AIOT_OTAOPT_MQTT_HANDLE`: 把 @ref aiot_mqtt_init 返回的MQTT会话句柄跟OTA会话关联起来
 * + `AIOT_OTAOPT_RECV_HANDLER`: 设置OTA消息的数据处理回调, 这个用户回调在有OTA消息的时候, 会被 @ref aiot_mqtt_recv 调用到
 *
 * @param[in] handle ota句柄
 * @param[in] option 配置选项, 更多信息请参考@ref aiot_ota_option_t
 * @param[in] data   配置选项数据, 更多信息请参考@ref aiot_ota_option_t
 *
 * @return int32_t
 * @retval STATE_OTA_SETOPT_HANDLE_IS_NULL ota句柄为空
 * @retval STATE_OTA_SETOPT_DATA_IS_NULL 参数data字段为空
 * @retval STATE_USER_INPUT_UNKNOWN_OPTION option不支持
 * @retval STATE_SUCCESS 参数设置成功
 *
 */
int32_t aiot_ota_setopt(void *handle, aiot_ota_option_t option, void *data);

/**
 * @brief 初始化download实例并设置默认参数
 *
 * @return void*
 * @retval 非NULL download实例句柄
 * @retval NULL 初始化失败, 或者是没有设置portfile, 或者是内存不足无法分配download或者http实例
 *
 */
void   *aiot_download_init();

/**
 * @brief 释放download实例句柄的资源
 *
 * @param[in] handle 指向download实例句柄的指针
 *
 * @return int32_t
 * @retval STATE_DOWNLOAD_DEINIT_HANDLE_IS_NULL handle或者handle指向的内容为空
 * @retval STATE_SUCCESS 执行成功
 *
 */
int32_t aiot_download_deinit(void **handle);

/**
 * @brief 通过download实例句柄下载一段buffer
 *
 * @details
 *
 * 用户解析完OTA消息, 知道了固件的下载地址之后, 就可以用这个接口以HTTP下载固件内容
 *
 * 被下载到的内容, 会通过回调函数传递给用户, 用户调用 @ref aiot_download_setopt 把自己的数据处理回调函数设置给SDK
 *
 * @param[in] handle 指向download实例句柄的指针
 *
 * @return int32_t
 * @retval >STATE_SUCCESS 表示下载到的字节数
 * @retval STATE_DOWNLOAD_HTTPRSP_CODE_ERROR 下载所使用的url链接不可访问, 返回的code并非200或者206
 * @retval STATE_DOWNLOAD_FINISHED 整个固件包下载完成
 * @retval STATE_DOWNLOAD_RANGE_FINISHED 分段下载的时候, 单个分段下载完成
 * @retval STATE_DOWNLOAD_HTTPRSP_HEADER_ERROR 访问下载链接后http的回复报文中并没有Content-Length字段
 * @retval STATE_DOWNLOAD_RECV_HANDLE_IS_NULL download句柄为空
 * @retval STATE_DOWNLOAD_RENEWAL_REQUEST_SENT 进行断点续传往固件服务器重新发送了下载请求
 * @retval 其他出错信息 请参考@ref aiot_state_api.h
 *
 */
int32_t aiot_download_recv(void *handle); /* 返回条件: 网络出错 | 校验出错 | 读到EOF | buf填满 */

/**
 * @brief 设置download句柄参数
 *
 * @details
 *
 * 配置固件下载会话的选项, 常见需要设置的选项包括
 *
 * + `AIOT_DLOPT_RECV_HANDLER`: 用户告诉SDK, 当SDK收到固件内容的时候, 调用哪个用户函数来传出固件内容缓冲区
 * + `AIOT_DLOPT_NETWORK_CRED`: 可以配置是走HTTP还是走HTTPS下载固件内容
 * + `AIOT_DLOPT_BODY_BUFFER_MAX_LEN`: 这是个缓冲区的长度, SDK下载中, 每当填满这个长度就调用一次用户回调, 所以这里设置的越大, 下载越快, 内存开销也越大
 *
 * @param[in] handle download句柄
 * @param[in] option 配置选项, 更多信息请参考@ref aiot_download_option_t
 * @param[in] data   配置选项数据, 更多信息请参考@ref aiot_download_option_t
 *
 * @return int32_t
 * @retval STATE_SUCCESS 参数设置成功
 * @retval STATE_DOWNLOAD_SETOPT_HANDLE_IS_NULL download句柄为空
 * @retval STATE_DOWNLOAD_SETOPT_DATA_IS_NULL data字段为空
 * @retval STATE_DOWNLOAD_SETOPT_COPIED_DATA_IS_NULL 拷贝task_desc失败
 * @retval STATE_DOWNLOAD_SETOPT_MALLOC_SHA256_CTX_FAILED  为shs256算法的context分配内存失败
 * @retval STATE_DOWNLOAD_SETOPT_MALLOC_MD5_CTX_FAILED     为MD5算法的context分配内存失败
 * @retval 其他出错信息 请参考@ref aiot_state_api.h
 *
 */
int32_t aiot_download_setopt(void *handle, aiot_download_option_t option, void *data);

/**
 * @brief 上报下载完成度的百分比或者错误码
 *
 * @details
 *
 * 在设备开始下载固件的过程之后, 都可以用这个接口向云端上报进展情况, 包括下载进度或出错信息
 *
 * + 如果下载是正常的, 可以整数形式上报, 当前已下载的内容占固件整体大小的百分比, percent参数, SDK会自动计算好, 在回调中传给用户
 * + 如果下载异常或者下载之后固件的烧录异常了, 也可以用这个接口把异常告诉云端, 设备和云端的协议错误码约定见 @ref aiot_ota_protocol_errcode_t
 * + 通过 @ref aiot_download_report_progress 上报的内容都会影响控制台的显示, 比如显示OTA升级进度, 显示OTA升级失败等
 *
 * @param[in] handle download句柄
 * @param[in] percent 当前所下载内容完成度的百分比或者错误码
 *
 * @return int32_t
 * @retval STATE_SUCCESS 参数设置成功
 * @retval STATE_DOWNLOAD_REPORT_HANDLE_IS_NULL 上报时handle为空
 * @retval STATE_DOWNLOAD_REPORT_TASK_DESC_IS_NULL 上报时task_desc为空, 无法找到相应的product_key, device_name
 * @retval 其他出错信息 请参考@ref aiot_state_api.h
 *
 */
int32_t aiot_download_report_progress(void *handle, int32_t percent);

/**
 * @brief 向云端发送GET固件报文请求
 *
 * @details
 *
 * 设备通过OTA消息回调函数知道了固件下载地址后, 就可以调用这个接口, 下载一段固件
 *
 * + 这段长度可由`AIOT_DLOPT_RANGE_START`和`AIOT_DLOPT_RANGE_END`选项设置
 * + 如果不做设置, 则默认SDK会整段去请求固件内容, 但每到填满1次用户buffer就会通知用户1次, buffer长度用`AIOT_DLOPT_BODY_BUFFER_MAX_LEN`选项配置
 *
 * @param[in] handle download句柄, 包含了固件的url等信息
 *
 * @return int32_t
 * @retval STATE_SUCCESS 请求发送成功
 * @retval STATE_DOWNLOAD_REQUEST_HANDLE_IS_NULL 发送GET请求的时候handle为空
 * @retval STATE_DOWNLOAD_REQUEST_URL_IS_NULL 发送GET请求的时候task_desc不为空, 但是其中的url为空
 * @retval STATE_DOWNLOAD_SEND_REQUEST_FAILED 发送GET请求的时候http底层发包逻辑报错
 * @retval STATE_DOWNLOAD_REQUEST_TASK_DESC_IS_NULL 发送GET请求的时候task_desc字段为空
 * @retval 其他出错信息 请参考@ref aiot_state_api.h
 */
int32_t aiot_download_send_request(void *handle);

/**
 * @brief -0x0900~-0x09FF表达SDK在OTA模块内的状态码, 也包含下载时使用的`STATE_DOWNLOAD_XXX`
 *
 */
#define STATE_OTA_BASE                                              (-0x0900)

/**
 * @brief OTA固件下载已完成, 校验和验证成功
 *
 */
#define STATE_OTA_DIGEST_MATCH                                      (-0x0901)

/**
 * @brief OTA下载进度消息或固件版本号消息上报到服务器时遇到失败
 *
 */
#define STATE_OTA_REPORT_FAILED                                     (-0x0902)

/**
 * @brief OTA模块收取固件内容数据时出现错误
 *
 */
#define STATE_DOWNLOAD_RECV_ERROR                                   (-0x0903)

/**
 * @brief OTA模块下载固件时出现校验和签名验证错误
 *
 * @details
 *
 * 固件的md5或者sha256计算结果跟云端通知的期望值不匹配所致的错误
 *
 */
#define STATE_OTA_DIGEST_MISMATCH                                   (-0x0904)

/**
 * @brief OTA模块解析服务器下推的MQTT下行JSON报文时出错
 *
 * @details
 *
 * 从云端下行的JSON报文中, 无法找到目标的key, 从而无法找到相应的value
 *
 */
#define STATE_OTA_PARSE_JSON_ERROR                                  (-0x0905)

/**
 * @brief OTA模块发送HTTP报文, 请求下载固件时遇到失败
 *
 * @details
 *
 * OTA模块往存储固件的服务器发送GET请求失败
 *
 */
#define STATE_DOWNLOAD_SEND_REQUEST_FAILED                          (-0x0906)

/**
 * @brief OTA模块下载固件内容已到达之前设置的range末尾, 不会继续下载
 *
 * @details
 *
 * 按照range下载的时候已经下载到了range_end字段指定的地方. 如果用户此时还是继续尝试去下载, SDK返回返回错误码提示用户
 *
 */
#define STATE_DOWNLOAD_RANGE_FINISHED                               (-0x0907)

/**
 * @brief OTA模块为解析JSON报文而申请内存时, 未获取到所需内存而解析失败
 *
 */
#define STATE_OTA_PARSE_JSON_MALLOC_FAILED                          (-0x0908)

/**
 * @brief 销毁OTA会话实例时, 发现会话句柄为空, 中止销毁动作
 *
 */
#define STATE_OTA_DEINIT_HANDLE_IS_NULL                             (-0x0909)

/**
 * @brief 配置OTA会话实例时, 发现会话句柄为空, 中止配置动作
 *
 */
#define STATE_OTA_SETOPT_HANDLE_IS_NULL                             (-0x090A)

/**
 * @brief 配置OTA会话实例时, 发现配置数据为空, 中止配置动作
 *
 */
#define STATE_OTA_SETOPT_DATA_IS_NULL                               (-0x090B)

/**
 * @brief 销毁下载会话实例时, 发现会话句柄为空, 中止销毁动作
 *
 */
#define STATE_DOWNLOAD_DEINIT_HANDLE_IS_NULL                        (-0x090C)

/**
 * @brief 配置下载会话实例时, 发现会话句柄为空, 中止配置动作
 *
 */
#define STATE_DOWNLOAD_SETOPT_HANDLE_IS_NULL                        (-0x090D)

/**
 * @brief 配置下载会话实例时, 发现配置数据为空, 中止配置动作
 *
 */
#define STATE_DOWNLOAD_SETOPT_DATA_IS_NULL                          (-0x090E)

/**
 * @brief 配置下载会话实例时, 从OTA会话同步配置发生内部错误, 中止配置动作
 *
 */
#define STATE_DOWNLOAD_SETOPT_COPIED_DATA_IS_NULL                   (-0x090F)

/**
 * @brief 直连设备上报版本号时, OTA句柄为空, 中止执行上报
 *
 */
#define STATE_OTA_REPORT_HANDLE_IS_NULL                             (-0x0910)

/**
 * @brief 直连设备上报版本号时, 版本号字符串为空, 中止执行上报
 *
 */
#define STATE_OTA_REPORT_VERSION_IS_NULL                            (-0x0911)

/**
 * @brief 直连设备上报版本号时, MQTT句柄为空, 中止执行上报
 *
 */
#define STATE_OTA_REPORT_MQTT_HANDLE_IS_NULL                        (-0x0912)

/**
 * @brief 网关为子设备上报版本号时, OTA句柄为空, 中止执行上报
 *
 */
#define STATE_OTA_REPORT_EXT_HANELD_IS_NULL                         (-0x0913)

/**
 * @brief 网关为子设备上报版本号时, 版本号字符串为空, 中止执行上报
 *
 */
#define STATE_OTA_REPORT_EXT_VERSION_NULL                           (-0x0914)

/**
 * @brief 网关为子设备上报版本号时, 子设备productKey为空, 中止执行上报
 *
 */
#define STATE_OTA_REPORT_EXT_PRODUCT_KEY_IS_NULL                    (-0x0915)

/**
 * @brief 网关为子设备上报版本号时, 子设备deviceName为空, 中止执行上报
 *
 */
#define STATE_OTA_REPORT_EXT_DEVICE_NAME_IS_NULL                    (-0x0916)

/**
 * @brief 网关为子设备上报版本号时, MQTT会话句柄为空, 中止执行上报
 *
 */
#define STATE_OTA_REPORT_EXT_MQTT_HANDLE_IS_NULL                    (-0x0917)

/**
 * @brief 上报下载进度或OTA错误码时, 下载会话句柄为空, 中止执行上报
 *
 */
#define STATE_DOWNLOAD_REPORT_HANDLE_IS_NULL                        (-0x0918)

/**
 * @brief 上报下载进度或OTA错误码时, 任务描述数据结构为空, 中止执行上报
 *
 */
#define STATE_DOWNLOAD_REPORT_TASK_DESC_IS_NULL                     (-0x0919)

/**
 * @brief 调用aiot_download_recv接收固件内容时, 处理接收数据的用户回调为空值, 中止执行
 *
 */
#define STATE_DOWNLOAD_RECV_HANDLE_IS_NULL                          (-0x091A)

/**
 * @brief 调用aiot_download_send_request发送固件下载请求时, 下载会话的句柄为空, 中止执行
 *
 */
#define STATE_DOWNLOAD_REQUEST_HANDLE_IS_NULL                       (-0x091B)

/**
 * @brief 调用aiot_download_send_request发送固件下载请求时, 任务描述为空, 中止执行
 *
 */
#define STATE_DOWNLOAD_REQUEST_TASK_DESC_IS_NULL                    (-0x091C)

/**
 * @brief 调用aiot_download_send_request发送固件下载请求时, 任务描述中的固件URL为空, 中止执行
 *
 */
#define STATE_DOWNLOAD_REQUEST_URL_IS_NULL                          (-0x091D)

/**
 * @brief 解析通知OTA的MQTT下行报文时, 其中的digest方法并非md5或sha256, SDK不支持
 *
 */
#define STATE_OTA_UNKNOWN_DIGEST_METHOD                             (-0x091E)

/**
 * @brief 整个固件(而不是单独一次的下载片段)收取已完成, 收取的累计字节数与固件预期字节数一致
 *
 */
#define STATE_DOWNLOAD_FINISHED                                     (-0x091F)

/**
 * @brief 设备向固件服务器发出GET请求时, 服务器返回的HTTP报文中Status Code错误, 既非200, 也非206
 *
 */
#define STATE_DOWNLOAD_HTTPRSP_CODE_ERROR                           (-0x0920)

/**
 * @brief 设备向固件服务器发出GET请求时, 服务器返回的HTTP报文header里, 没有说明Content-Length
 *
 */
#define STATE_DOWNLOAD_HTTPRSP_HEADER_ERROR                         (-0x0921)

/**
 * @brief OTA固件下载失败后, 正在进行断点续传, SDK向服务端重新发起了下载请求
 *
 */
#define STATE_DOWNLOAD_RENEWAL_REQUEST_SENT                         (-0x0922)

/**
 * @brief OTA模块为计算固件的SHA256校验和申请内存时遇到失败
 *
 */
#define STATE_DOWNLOAD_SETOPT_MALLOC_SHA256_CTX_FAILED              (-0x0923)

/**
 * @brief OTA模块为计算固件的MD5校验和内存时遇到失败
 *
 */
#define STATE_DOWNLOAD_SETOPT_MALLOC_MD5_CTX_FAILED                 (-0x0924)

/**
 * @brief OTA模块从任务描述数据结构中解析固件下载URL时, 遇到HOST字段为空, 解析失败
 *
 */
#define STATE_OTA_PARSE_URL_HOST_IS_NULL                            (-0x0925)

/**
 * @brief OTA模块从任务描述数据结构中解析固件下载URL时, 遇到PATH字段为空, 解析失败
 *
 */
#define STATE_OTA_PARSE_URL_PATH_IS_NULL                            (-0x0926)

/**
 * @brief OTA模块分多段下载固件时, 多段累计的总和大小超过了固件预期的值
 *
 * @details 可能的一个原因是用户将固件划分了多个range来下载, 但是由于不同的range之间存在重叠等原因, 导致最终的下载总量超出了固件的总大小
 *
 */
#define STATE_DOWNLOAD_FETCH_TOO_MANY                               (-0x0927)

/**
 * @brief 向云端查询OTA的升级任务时, OTA句柄为空
 *
 * @details 需要先调用aiot_ota_init函数来初始化一个OTA句柄, 再把这个句柄传给aiot_ota_query_firmware. 如果这个句柄没有被初始化, 或者
 * 初始化不成功, 就会报出这个错误
 *
 */
#define STATE_OTA_QUERY_FIRMWARE_HANDLE_IS_NULL                     (-0x0928)

/**
 * @brief OTA下行报文中url字段中的host字段超出长度限制
 *
 * @details 在OTA的下行报文中, 包含了存储固件的url. url中包含host字段, 如果host字段超出限制(当前限1024个字节), 就会报出这个错误
 *
 */
#define STATE_OTA_HOST_STRING_OVERFLOW                              (-0x0929)

/**
 * @brief OTA下行报文中url字段中的path字段超出长度限制
 *
 * @details 在OTA的下行报文中, 包含了存储固件的url. url中包含path字段, 如果path字段超出限制(当前限1024个字节), 就会报出这个错误
 *
 */
#define STATE_OTA_PATH_STRING_OVERFLOW                              (-0x092A)

#if defined(__cplusplus)
}
#endif

#endif  /* #ifndef __AIOT_OTA_API_H__ */

