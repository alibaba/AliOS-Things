/**
 * @file aiot_mqtt_upload_api.h
 * @brief MQTT UPLOAD模块头文件, 提供通过mqtt进行文件上传的能力
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
#ifndef __AIOT_MQTT_UPLOAD_API_H__
#define __AIOT_MQTT_UPLOAD_API_H__

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief -0x2100~-0x21FF表达SDK在mqtt_upload模块内的状态码
 */
#define STATE_MQTT_UPLOAD_BASE                                    (-0x2100)

/**
 * @brief MQTT UPLOAD Handle 为空
 */
#define STATE_MQTT_UPLOAD_MQTT_HANDLE_IS_NULL                     (-0x2101)

/**
 * @brief MQTT UPLOAD 请求上传文件时，文件名是空的
 *
 */
#define STATE_MQTT_UPLOAD_FILENAME_IS_NULL                        (-0x2102)

/**
 * @brief MQTT UPLOAD PUB到服务端的时候参数为空
 *
 */
#define STATE_MQTT_UPLOAD_PARAMS_IS_NULL                          (-0x2103)

/**
 * @brief MQTT UPLOAD上传文件数据为空
 *
 */
#define STATE_MQTT_UPLOAD_FILE_DATA_IS_NULL                       (-0x2104)

/**
 * @brief MQTT portfile是空的
 */
#define STATE_MQTT_UPLOAD_MQTT_SYSDEP_IS_NULL                     (-0x2105)

/**
 * @brief MQTT UPLOAD 解析服务端初始化应答的Json数据失败
 */
#define STATE_MQTT_UPLOAD_PARSE_INIT_FAILED                       (-0x2106)

/**
 * @brief MQTT UPLOAD 解析服务端文件数据上传应答的Json数据失败
 */
#define STATE_MQTT_UPLOAD_PARSE_UPLOAD_REPLY_FAILED               (-0x2107)

/**
 * @brief MQTT UPLOAD 解析服务端取消应答的Json数据失败
 */
#define STATE_MQTT_UPLOAD_PARSE_CANCEL_FAILED                     (-0x2108)

/**
 * @brief MQTT UPLOAD 解析服务端应答的Json数据失败
 */
#define STATE_MQTT_UPLOAD_PARSE_JSON_FAILED                       (-0x2109)

/**
 * @brief MQTT UPLOAD 接收到的服务端应答文件信息不对
 */
#define STATE_MQTT_UPLOAD_RECV_FILE_INFO_ERROR                    (-0x210A)

/**
 * @brief MQTT UPLOAD uploadId不匹配或者设备端为初始化请求
 */
#define STATE_MQTT_UPLOAD_RECV_UPLOADID_ERROR                     (-0x210B)

/**
 * @brief MQTT UPLOAD malloc失败
 */
#define STATE_MQTT_UPLOAD_MALLOC_FAILED                           (-0x210C)

/**
 * @brief MQTT UPLOAD 初始化文件upload的句柄时失败
 */
#define STATE_MQTT_UPLOAD_UPTASK_IS_NULL                           (-0x210D)

/**
 * @brief MQTT UPLOAD 收到服务端应答的文件上传的偏移量和分包大小错误，与上次上传的不相同
 */
#define STATE_MQTT_UPLOAD_UPLOAD_REPLY_ERROR                       (-0x210E)

/**
 * @brief  MQTT UPLOAD 反初始化
 */
#define STATE_MQTT_UPLOAD_HANDLE_DEINIT                            (-0x210F)

/**
 * @brief 重新启动文件上传失败
 */
#define STATE_MQTT_UPLOAD_FILE_RESTART_FAILED                      (-0x2110)

/**
 * @brief MQTT UPLOAD 配置分包上传的大小错误，不在256~128K范围内，继续保持默认的2K
 *
 */
#define STATE_MQTT_UPLOAD_BLOCK_SIZE_ERROR                         (-0x2111)

/**
 * @brief MQTT UPLOAD 请求文件上传，回调函数为空
 *
 */
#define STATE_MQTT_UPLOAD_READ_DATA_HANDLER_IS_NULL                (-0x2112)

/**
 * @brief  MQTT UPLOAD 初始化
 */
#define STATE_MQTT_UPLOAD_HANDLE_INIT                              (-0x2113)

/**
 * @brief MQTT UPLOAD 处理服务端应答的回调时失败
 */
#define STATE_MQTT_UPLOAD_PROCESS_REPLY_ERROR                      (-0x2114)

/**
 * @brief 无任何状态，未开始进行文件上传的处理
 *
 */
#define STATE_MQTT_UPLOAD_NONE                                      (-0x2115)

/**
 * @brief 请求服务端进行文件上传，等待服务端返回结果
 *
 */
#define STATE_MQTT_UPLOAD_REQUEST_INIT                               (-0x2116)

/**
 * @brief 正在执行文件上传任务
 *
 */
#define STATE_MQTT_UPLOAD_IS_UPLOADING                               (-0x2117)

/**
 * @brief 通知服务端取消文件上传。
 *
 */
#define STATE_MQTT_UPLOAD_REQUEST_CANCEL                             (-0x2118)

/**
 * @brief 文件上传，取消上传任务成功，取消成功后会销毁对应的upload task释放内存资源
 *
    */
#define STATE_MQTT_UPLOAD_CANCEL_SUCCESS                             (-0x2119)

/**
 * @brief 文件上传，取消上传任务失败
 *
 */
#define STATE_MQTT_UPLOAD_CANCEL_FAILED                               (-0x2120)

/**
 * @brief 文件上传失败
 *
 */
#define STATE_MQTT_UPLOAD_FAILED                                    (-0x2121)

/**
 * @brief 文件上传超时失败
 *
 */
#define STATE_MQTT_UPLOAD_FAILED_TIMEOUT                           (-0x2122)

/**
 * @brief 文件完整性校验失败
 *
 */
#define STATE_MQTT_UPLOAD_FAILED_WHOLE_CHECK                       (-0x2124)

/**
 * @brief 已经完成文件上传的任务，完成文件上传后会销毁对应的upload task释放内存资源
 *
 */
#define STATE_MQTT_UPLOAD_FINISHED                                 (-0x2125)

/**
 * @brief 没有文件上传任务，无法执行取消操作
 *
 */
#define STATE_MQTT_UPLOAD_NO_UPLOAD_TASK                            (-0x2126)

/**
 * @brief 同名的文件的处理策略 overwrite: 覆盖模式，服务端删除之前的同名文件，原有文件无法找回
 */
#define AIOT_MQTT_UPLOAD_FILE_MODE_OVERWRITE        "overwrite"

/**
 * @brief 同名的文件的处理策略 append:文件追加模式
 *  有同名文件未完成上传：在服务端文件未完成上传的情况下，云端返回当前已上传文件的信息，以便设备端继续上传后续的文件内
 *      容，未完成的文件云端保留24H，超过24小时后删除未完成的文件，下次请求时重新建立
 *  有同名已完成上传：返回错误码 UPLOAD_FILE_FAILED_SAME_FILE ，请求上传失败
 *  无同名文件：服务端新建文件信息
 */
#define AIOT_MQTT_UPLOAD_FILE_MODE_APPEND           "append"

/**
 * @brief 同名的文件的处理策略 reject: 拒绝模式，服务端返回文件已经存在不能再次上传的错误码 UPLOAD_FILE_FAILED_SAME_FILE
 */
#define AIOT_MQTT_UPLOAD_FILE_MODE_FAIL             "reject"

/**
 * @brief MQTT上传文件名的最大长度
 */
#define MQTT_UPLOAD_DEFAULT_FILENAME_LEN             (128)

/**
 * @brief CRC64数据校验的长度
 */
#define MQTT_UPLOAD_DEFAULT_CRC64_LEN                (32)

/**
 * @brief UploadID的数据长度
 */
#define MQTT_UPLOAD_DEFAULT_UPLOAD_SIZE             (64)

/**
 * @brief 请求文件上传和文件传输过程中的服务端返回的文件信息
 *
 */
typedef enum {
    /**
     * @brief 文件上传正常
     *
     */
    UPLOAD_FILE_OK = 200,

    /**
     * @brief 请求上传的payload格式非法，无法解析其中的json格式数据
     *
     */
    UPLOAD_FILE_FAILED_PAYLOAD_ERROR = 400,

    /**
     * @brief 文件上传被限流
     *
     */
    UPLOAD_FILE_FAILED_QPS_LIMIT = 429,

    /**
     * @brief 文件上传请求参数错误
     *
     */
    UPLOAD_FILE_FAILED_PARAMS = 460,

    /**
     * @brief 服务端其他异常
     *
     */
    UPLOAD_FILE_FAILED_SERVER_OTHER = 500,

    /**
     * @brief 申请使用mqtt协议上传文件时，文件大小超过阈值16M
     *
     */
    UPLOAD_FILE_FAILED_MAX_FILESIZE = 78117,

    /**
     * @brief 设备上传文件分片时，分片大小超过阈值128KB
     *
     */
    UPLOAD_FILE_FALED_BLOCK_SIZE_MORE = 78118,

    /**
     * @brief 设备上传文件分片时，当前分片的offset与已上传至的服务端文件大小不一致
     *
     */
    UPLOAD_FILE_FAILED_OFFSET_UNEQUAL = 78119,

    /**
     * @brief 设备上传文件分片时，当前分片的大小小于最小阈值，除最后一片文件外，传输过程中分片大小不得小于256Byte
     *
     */
    UPLOAD_FILE_FAILED_BLOCK_SIZE_LESS = 78120,

    /**
     * @brief 设备上传文件分片时，当前分片的CRC校验失败，SDK内会自动对当前分片进行重传
     *
     */
    UPLOAD_FILE_FAILED_BLOCK_CRC = 78121,

    /**
     * @brief 上传文件的任务不存在，用户需要发起新的上传请求调用 aiot_mqtt_upload_open_stream
     *
     */
    UPLOAD_FILE_FAILED_NOT_FILE_TASK = 78122,

    /**
     * @brief 同名文件已经存在，在追加模式 AIOT_MQTT_UPLOAD_FILE_MODE_APPEND 和拒绝模式 AIOT_MQTT_UPLOAD_FILE_MODE_FAIL 下会返回该错误码
     *
     */
    UPLOAD_FILE_FAILED_SAME_FILE = 78123,

    /**
     * @brief 文件上传任务已经完成，通知用户对应的文件上传已经完成，销毁对应的upload task释放内存资源
     *
     */
    UPLOAD_FILE_HAVE_FINISHED = 78124,

    /**
     * @brief 文件完整性校验失败，在调用 aiot_mqtt_upload_open_stream 时，如果传入 degist参数服务端会对文件完整性进行校验，传入NULL时云端不校验
     *
     */
    UPLOAD_FILE_FAILED_WHOLE_CHECK = 78125,

    /**
     * @brief 重传或网络断开造成文件写入重复，忽略
     *
     */
    UPLOAD_FILE_FILE_BLOCK_DUPLICATION = 78126,

    /**
     * @brief 文件上传过程中，消息限流
     *
     */
    UPLOAD_FILE_MESSAGE_LIMIT = 78127

} aiot_mqtt_upload_result_code_t;

/**
 * @brief MQTT UPLOAD模块收到从服务端应答的报文类型
 */
typedef enum {
    /**
     * @brief 收到的服务端应答请求初始化时的报文信息，通过回调函数反馈给用户
     *
     */
    AIOT_MQTT_UPLOADRECV_INIT_REPLY,
    /**
     * @brief 收到的服务端应答上传分片文件时的应答报文消息，通过回调反馈给用户，同时也会调用回调获取下一段分片文件
     *
     */
    AIOT_MQTT_UPLOADRECV_UPLOAD_REPLY,
    /**
     * @brief 收到的服务端应答取消上传的报文应答
     *
     */
    AIOT_MQTT_UPLOADRECV_CANCEL_REPLY
} aiot_mqtt_upload_recv_type_t;

/**
 * @brief 接收到的服务端应答消息的报文信息
 *
 */
typedef struct {
    /**
     * @brief 服务端应答的文件名信息，用户调用 aiot_mqtt_upload_open_stream 传入的文件名，可用于回调函数中读取文件信息
     *
     */
    char        *file_name;

    /**
     * @brief 对应上传文件的总长度，用户调用 aiot_mqtt_upload_open_stream 传入的文件大小
     */
    uint32_t    file_size;

    /**
     * @brief 服务端应答文件上传请求唯一识别信息，在文件上传的声明周期内uploadId唯一，有效时间24H
     *
     */
    char        *upload_id;

    /**
     * @brief 已经上传文件的偏移量，偏移量的是相对于文件开始的起始点算起，可以用于回调函数中读取新分片文件的偏移量
     */
    uint32_t    file_offset;

    /**
     * @brief 本次进行分片上传的分片长度
     */
    uint32_t    block_size;

    /**
     * @brief 服务端应答的错误编码，对应本次上传的状态
     */
    aiot_mqtt_upload_result_code_t    code;

    /**
     * @brief 服务端应答，错误码对应的错误信息
     */
    char        *message;

    /**
     * @brief 服务端应答文件上传完成后才有的应答，complete true: 文件上传成功，false: 文件上传失败
     */
    uint8_t     complete;

} aiot_mqtt_upload_desc_t;

/**
 * @brief MQTT UPLOAD模块收到服务端应答的报文信息, 通知用户的报文内容
 */
typedef struct {
    /**
     * @brief 收到服务端应答的报文类型
     *
     */
    aiot_mqtt_upload_recv_type_t type;

    /**
     * @brief 收到服务端应答的报文描述信息
     *
     */
    aiot_mqtt_upload_desc_t      desc;
} aiot_mqtt_upload_recv_t;

/**
 * @brief @ref aiot_mqtt_upload_setopt 接口的option参数可选值.
 *
 * @details 下文每个选项中的数据类型, 指的是@ref aiot_mqtt_upload_setopt 中, data参数的数据类型
 *
 */
typedef enum {

    /**
     * @brief 关联MQTT Handle会话，UPLOAD过程中使用MQTT的通道能力, 用以向服务端请求上传、发送分片文件、请求关闭通道
     *
     */
    AIOT_MQTT_UPLOADOPT_MQTT_HANDLE,

    /**
     * @brief MQTT UPLOAD会话发送消息时，超时重发的最长时间间隔, 数据类型为(uint32_t *)
     */
    AIOT_MQTT_UPLOADOPT_RSP_TIMEOUT_MS,

    /**
     * @brief 配置MQTT文件上传单包发送超时时候的重试次数, 数据类型为(uint32_t *)
     */
    AIOT_MQTT_UPLOADOPT_RETRY_COUNT,

    /**
     * @brief 配置文件上传的分片长度，默认是2048byte, 数据类型为(uint32_t *)
     */
    AIOT_MQTT_UPLOADOPT_DEFAULLT_BLOCK_SIZE,

    /**
     * @brief 配置需要上传的文件，数据类型为aiot_mqtt_upload_file_opt_t
     */
    AIOT_MQTT_UPLOADOPT_FILE_OPTION,
    /**
     * @brief 设置MQTT的最大值，设置失败
     */
    AIOT_MQTT_UPLOADOPT_MAX

} aiot_mqtt_upload_option_t;

/**
 * @brief aiot_mqtt_upload_process 循环处理，返回对应文件的上传状态给到用户
 *
 */
typedef struct
{
    /**
     * @brief 通过MQTT通过上传的情况
     *
     */
    int32_t status;

    /**
     * @brief 云端应答的错误码,参考@ref aiot_mqtt_upload_result_code_t
     *
     */
    int32_t err_code;

    /**
     * @brief 结果对应的文件名，文件上传成功后，返回为空 NULL
     *
     */
    char    file_name[MQTT_UPLOAD_DEFAULT_FILENAME_LEN];

    /**
     * @brief 返回文件名对应的uploadid
     *
     */
    char    uploadid[MQTT_UPLOAD_DEFAULT_UPLOAD_SIZE];

} aiot_mqtt_upload_result_t;

/**
 * @brief 回调函数读取用户的本地文件或内存数据不能在回调函数中执行
 * @ref aiot_mqtt_upload_open_stream 和 @ref aiot_mqtt_upload_cancel_stream
 *  多个文件同时请求上传时，可以使用不同的回调函数，也可以使用同一个回调函数用户根据file_name区分对应的文件或内存区
 *
 * @param[in] packet 服务端的应答报文,包含应答的状态和错误信息，更多信息参考 @ref aiot_mqtt_upload_recv_t
 * @param[in] data 指向用于接收用户数据的缓冲块的指针
 * @param[in] size 数据缓冲区大小, 此大小在发送过程中会变化
 * @param[in] userdata 指向用户上下文的指针
 *
 * @return int32_t
 * @retval 用户实际拷贝到buf中的数据长度
 */
typedef int32_t (*aiot_mqtt_upload_read_handler_t)(const aiot_mqtt_upload_recv_t *packet, uint8_t *data, uint32_t size, void *userdata);

/**
 * @brief @ref aiot_mqtt_upload_setopt 对应参数AIOT_MQTT_UPLOADOPT_FILE_OPTION的结构体
 *
 * @details 首先配置对应的文件信息，然后调用aiot_mqtt_upload_open_stream接口开始文件上传
 *
 */
typedef struct
{
    char *file_name;
    uint32_t file_size;
    char *mode;
    void *digest;
    aiot_mqtt_upload_read_handler_t read_data_handler;
    void *userdata;
}aiot_mqtt_upload_file_opt_t;

/**
 * @brief 创建MQTT UPLOAD会话实例, 并以默认值配置会话参数
 *
 * @return void *
 * @retval 非NULL, 返回mqtt_upload_handle_t实例的句柄
 * @retval NULL   初始化失败, 一般是内存分配失败导致
 *
 */
void *aiot_mqtt_upload_init(void);

/**
 * @brief 配置MQTT UPLOAD会话
 *
 * @param[in] handle MQTT UPLOAD会话句柄
 * @param[in] option 配置选项, 更多信息请参考 @ref aiot_mqtt_upload_option_t
 * @param[in] data   配置选项数据, 更多信息请参考 @ref aiot_mqtt_upload_option_t
 *
 * @return int32_t
 * @retval STATE_SUCCESS  参数配置成功
 * @retval STATE_USER_INPUT_NULL_POINTER 入参handle或data为NULL
 * @retval STATE_USER_INPUT_OUT_RANGE 入参optioin的枚举值>=AIOT_DMOPT_MAX
 * @retval others 参考 @ref aiot_state_api.h
 *
 */
int32_t aiot_mqtt_upload_setopt(void *handle, aiot_mqtt_upload_option_t option, void *data);

/**
 * @brief 结束MQTT UPLOAD会话, 销毁实例并回收资源
 *
 * @param[in] handle 指向MQTT UPLOAD会话句柄的指针
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  执行失败
 * @retval >=STATE_SUCCESS 执行成功
 *
 */
int32_t aiot_mqtt_upload_deinit(void **handle);

/**
 * @brief 向MQTT UPLOAD 服务请求发送文件
 *
 * @param[in] handle MQTT UPLOAD会话句柄
 * @param[in] file_name 需要在服务端创建的文件名
 * @param[in] file_size 需要上传的文件大小
 * @param[in] mode 同名的文件的处理策略, 更多信息请参考 @ref AIOT_MQTT_UPLOAD_FILE_MODE_OVERWRITE
 *              @ref AIOT_MQTT_UPLOAD_FILE_MODE_APPEND @ref AIOT_MQTT_UPLOAD_FILE_MODE_FAIL
 * @param[in] digest 文件完整的CRC64，不需要完整性校验则为NULL
 * @param[in] read_data_handler 读取用户文件数据的回调函数, 更多信息请参考 @ref aiot_mqtt_upload_read_handler_t
 * @param[in] userdata 用户上下文
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  请求发送失败
 * @retval >=STATE_SUCCESS 请求发送成功
 * @retval others 参考@ref aiot_state_api.h 或@ref STATE_MQTT_UPLOAD_BASE 中对应的错误码说明
 *
 */
int32_t aiot_mqtt_upload_open_stream(void *handle, char *file_name, aiot_mqtt_upload_recv_t *packet);

/**
 * @brief 向mqtt_upload服务器请求关闭MQTT通道数据上传命令
 *
 * @param[in] handle MQTT UPLOAD会话句柄
 * @param[in] file_name 需要取消继续上传的文件名
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  请求发送失败
 * @retval >=STATE_SUCCESS 请求发送成功
 * @retval others 参考@ref aiot_state_api.h 或@ref STATE_MQTT_UPLOAD_BASE 中对应的错误码说明
 *
 */
int32_t aiot_mqtt_upload_cancel_stream(void *handle, char *file_name);

/**
 * @brief MQTT Upload 处理函数,处理超时和重发逻辑，返回对应文件的执行的状态
 *
 * @param[in] handle MQTT UPLOAD 会话句柄
 *
 * @return aiot_mqtt_upload_result_t 返回消息包括两个<i>status</i>, <i>file_name</i>; 更多信息请参考 @ref aiot_mqtt_upload_result_t
 * @retval STATE_MQTT_UPLOAD_NONE 无任何状态，未开始进行文件上传的处理
 * @retval STATE_MQTT_UPLOAD_REQUEST_INIT 请求服务端进行文件上传，等待服务端返回结果
 * @retval STATE_MQTT_UPLOAD_IS_UPLOADING 正在执行文件上传任务
 * @retval STATE_MQTT_UPLOAD_REQUEST_CANCEL 通知服务端取消文件上传
 * @retval STATE_MQTT_UPLOAD_CANCEL_SUCCESS 文件上传，取消上传任务成功，取消成功后会销毁对应的upload task释放内存资源
 * @retval STATE_MQTT_UPLOAD_CANCEL_FAILED 文件上传，取消上传任务失败
 * @retval STATE_MQTT_UPLOAD_FAILED 文件上传失败
 * @retval STATE_MQTT_UPLOAD_FAILED_TIMEOUT 文件上传超时失败
 * @retval STATE_MQTT_UPLOAD_FAILED_WHOLE_CHECK 文件完整性校验失败
 * @retval STATE_MQTT_UPLOAD_FINISHED 已经完成文件上传的任务，完成文件上传后会销毁对应的upload task释放内存资源
 */
aiot_mqtt_upload_result_t aiot_mqtt_upload_process(void *handle);

/**
 * @brief MQTT Upload 数据发送接口，如果没有配置aiot_mqtt_upload_read_handler_t时，使用该接口发送数据
 *
 * @param[in] handle MQTT UPLOAD会话句柄
 * @param[in] file_name 需要上传的文件名
 * @param[in] data 需要上传的数据缓冲区
 * @param[in] datalen 需要上传数据的长度
 * @param[in] is_complete 数据是否上传完成,上传最后一包文件时需要设置为1,否则云端不会存储上传文件
 *
 * @return int32_t
 * @retval <STATE_SUCCESS  请求发送失败
 * @retval >=STATE_SUCCESS 请求发送成功
 * @retval others 参考@ref aiot_state_api.h 或@ref STATE_MQTT_UPLOAD_BASE 中对应的错误码说明
 *
 */
int32_t aiot_mqtt_upload_send_data(void *handle, char *file_name, uint8_t *data, uint32_t datalen, uint8_t is_complete);


#if defined(__cplusplus)
}
#endif

#endif  /* __AIOT_MQTT_UPLOAD_API_H__ */

