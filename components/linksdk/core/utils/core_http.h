#ifndef _CORE_HTTP_H_
#define _CORE_HTTP_H_


#if defined(__cplusplus)
extern "C" {
#endif

#include "core_stdinc.h"
#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "core_string.h"
#include "core_log.h"
#include "core_auth.h"
#include "aiot_http_api.h"

typedef enum {
    CORE_HTTP_SM_READ_HEADER,
    CORE_HTTP_SM_READ_BODY
} core_http_sm_t;

typedef struct {
    core_http_sm_t sm;
    uint32_t body_total_len;
    uint32_t body_read_len;
} core_http_session_t;

typedef struct {
    uint32_t code;
    uint8_t *content;
    uint32_t content_len;
    uint32_t content_total_len;
} core_http_response_t;

typedef struct {
    aiot_sysdep_portfile_t *sysdep;
    void *network_handle;
    char *host;
    uint16_t port;
    char *product_key;
    char *device_name;
    char *device_secret;
    char *extend_devinfo;
    uint32_t connect_timeout_ms;
    uint32_t send_timeout_ms;
    uint32_t recv_timeout_ms;
    uint32_t auth_timeout_ms;
    uint32_t deinit_timeout_ms;
    uint32_t header_line_max_len;
    uint32_t body_buffer_max_len;
    aiot_sysdep_network_cred_t *cred;
    char *token;
    uint8_t long_connection;
    uint8_t exec_enabled;
    uint32_t exec_count;
    uint8_t core_exec_enabled;
    uint32_t core_exec_count;
    void *data_mutex;
    void *send_mutex;
    void *recv_mutex;
    core_http_session_t session;
    aiot_http_event_handler_t event_handler;
    aiot_http_recv_handler_t recv_handler;
    aiot_http_recv_handler_t core_recv_handler;
    void *userdata;
    void *core_userdata;
} core_http_handle_t;

#define CORE_HTTP_MODULE_NAME "HTTP"
#define CORE_HTTP_DEINIT_INTERVAL_MS               (100)

#define CORE_HTTP_DEFAULT_CONNECT_TIMEOUT_MS       (10 * 1000)
#define CORE_HTTP_DEFAULT_AUTH_TIMEOUT_MS          (5 * 1000)
#define CORE_HTTP_DEFAULT_SEND_TIMEOUT_MS          (5 * 1000)
#define CORE_HTTP_DEFAULT_RECV_TIMEOUT_MS          (5 * 1000)
#define CORE_HTTP_DEFAULT_HEADER_LINE_MAX_LEN      (128)
#define CORE_HTTP_DEFAULT_BODY_MAX_LEN             (128)
#define CORE_HTTP_DEFAULT_DEINIT_TIMEOUT_MS        (2 * 1000)

typedef enum {
    CORE_HTTPOPT_HOST,                  /* 数据类型: (char *), 服务器域名, 默认值: iot-as-http.cn-shanghai.aliyuncs.com        */
    CORE_HTTPOPT_PORT,                  /* 数据类型: (uint16_t), 服务器端口号, 默认值: 443                                     */
    CORE_HTTPOPT_NETWORK_CRED,          /* 数据类型: (aiot_sysdep_network_cred_t *), 网络安全凭证, 默认值: NULL                */
    CORE_HTTPOPT_CONNECT_TIMEOUT_MS,    /* 数据类型: (uint32_t), 建立网络连接的超时时间 */
    CORE_HTTPOPT_SEND_TIMEOUT_MS,       /* 数据类型: (uint32_t), 网络发送超时时间(单位ms), 默认值: 5000ms                      */
    CORE_HTTPOPT_RECV_TIMEOUT_MS,       /* 数据类型: (uint32_t), 网络接受超时时间(单位ms), 默认值: 5000ms                      */
    CORE_HTTPOPT_DEINIT_TIMEOUT_MS,     /* 数据类型: (uint32_t), 销毁http实例时, 等地啊其他api执行完毕的时间                   */
    CORE_HTTPOPT_HEADER_LINE_MAX_LEN,   /* 数据类型: (uint32_t), http协议中单行header的最大长度                                */
    CORE_HTTPOPT_BODY_BUFFER_MAX_LEN,   /* 数据类型: (uint32_t), 每次读取的body最大长度                                        */
    CORE_HTTPOPT_EVENT_HANDLER,         /* 数据类型: (aiot_http_event_handler_t), 用户事件回调函数, 默认值: NULL               */
    /* 以上选项配置的数据与 AIOT_HTTPOPT_XXX 共用 */
    CORE_HTTPOPT_USERDATA,              /* 数据类型: (void *), 用户上下文数据指针, 默认值: NULL                                */
    CORE_HTTPOPT_RECV_HANDLER,          /* 数据类型: (aiot_http_event_handler_t), 用户数据接受回调函数, 默认值: NULL           */
    CORE_HTTPOPT_MAX
} core_http_option_t;

typedef struct {
    char       *method;             /* HTTP请求方法, 可为"POST", "GET"等 */
    char       *path;               /* HTTP请求的路径 */
    char       *header;             /* HTTP请求的头部, 必须以单个\r\n结尾, 无需包含Content-Length */
    uint8_t    *content;            /* 指向用户待发送Content的指针 */
    uint32_t    content_len;        /* 用户待发送Content的长度 */
} core_http_request_t;

/**
 * @brief 初始化一个HTTP实例, 并返回实例句柄
 *
 * @return void*
 * @retval NotNull core HTTP句柄
 * @retval Null 初始化HTTP实例失败
 */
void *core_http_init(void);

/**
 * @brief 设置HTTP实例选项
 *
 * @param[in] handle HTTP句柄
 * @param option 配置选项, 可查看枚举类型 @ref core_http_option_t
 * @param[in] data 配置数据, 每个选项对应的数据类型可查看 @ref core_http_option_t
 * @return int32_t
 * @retval STATE_SUCCESS, 成功
 * @retval STATE_HTTP_HANDLE_IS_NULL, HTTP句柄为NULL
 * @retval STATE_USER_INPUT_OUT_RANGE, 用户输入参数无效
 * @retval STATE_SYS_DEPEND_MALLOC_FAILED, 内存分配失败
 *
 */
int32_t core_http_setopt(void *handle, core_http_option_t option, void *data);

/**
 * @brief 建立网络连接
 *
 * @param handle HTTP句柄
 * @return int32_t
 * @retval STATE_SUCCESS 网络连接建立成功
 * @retval <STATE_SUCCESS 网络连接建立失败
 */
int32_t core_http_connect(void *handle);

/**
 * @brief 发送HTTP请求
 *
 * @param[in] handle HTTP句柄
 * @param request 请求结构体, 查看 @ref core_http_request_t
 * @return int32_t
 * @retval > 0, 已发送的数据长度
 * @retval STATE_HTTP_HANDLE_IS_NULL, HTTP句柄为NULL
 * @retval STATE_USER_INPUT_NULL_POINTER, 用户输入参数为NULL
 * @retval STATE_USER_INPUT_MISSING_HOST, 用户未配置Host
 * @retval STATE_SYS_DEPEND_MALLOC_FAILED, 内存分配失败
 * @retval STATE_SYS_DEPEND_NWK_EST_FAILED, 网络建立连接失败
 * @retval STATE_SYS_DEPEND_NWK_CLOSED, 网络连接已关闭
 * @retval STATE_SYS_DEPEND_NWK_WRITE_LESSDATA, 网络发送超时
 */
int32_t core_http_send(void *handle, const core_http_request_t *request);

/**
 * @brief 接受HTTP应答数据, 内部将解析状态码和Header并通过回调函数通知用户, 若应答中有body则保存到用户缓冲区中
 *
 * @param[in] handle HTTP句柄
 * @param buffer 指向存放接受
 * @param buffer_len
 * @return int32_t
 * @retval >= 0, 接受到的HTTP body数据长度
 * @retval STATE_HTTP_HANDLE_IS_NULL, HTTP句柄为NULL
 * @retval STATE_USER_INPUT_NULL_POINTER, 用户输入参数为NULL
 * @retval STATE_USER_INPUT_OUT_RANGE, buffer_len为0
 * @retval STATE_SYS_DEPEND_NWK_CLOSED, 网络连接已关闭
 * @retval STATE_SYS_DEPEND_NWK_READ_OVERTIME, 网络接收超时
 * @retval STATE_HTTP_RECV_LINE_TOO_LONG, HTTP单行数据过长, 内部无法解析
 * @retval STATE_HTTP_PARSE_STATUS_LINE_FAILED, 无法解析状态码
 * @retval STATE_HTTP_GET_CONTENT_LEN_FAILED, 获取Content-Length失败
 *
 */
int32_t core_http_recv(void *handle);

/**
 * @brief 销毁参数p_handle所指定的HTTP实例
 *
 * @param[in] p_handle 指向HTTP句柄的指针
 * @return int32_t
 * @retval STATE_SUCCESS 成功
 * @retval STATE_USER_INPUT_NULL_POINTER 参数p_handle为NULL或者p_handle指向的句柄为NULL
 */
int32_t core_http_deinit(void **p_handle);

#if defined(__cplusplus)
}
#endif

#endif /* #ifndef _CORE_HTTP_H_ */

