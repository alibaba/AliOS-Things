/**
 * @file aiot_task_api.h
 * @brief task模块头文件, 提供任务管理的能力
 * @date 2020-11-25
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 * @details
 *
 *
 */

#ifndef __AIOT_TASK_API_H__
#define __AIOT_TASK_API_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief -0x0B00~-0x0BFF表达SDK在task模块内的状态码
 *
 */
#define STATE_TASK_BASE                                             (-0x0B00)

/**
 * @brief 销毁task会话实例时, 发现会话句柄为空, 中止销毁动作
 *
 */
#define STATE_TASK_DEINIT_HANDLE_IS_NULL                            (-0x0B01)

/**
 * @brief 配置task会话实例时, 发现会话句柄为空, 中止配置动作
 *
 */
#define STATE_TASK_SETOPT_HANDLE_IS_NULL                            (-0x0B02)

/**
 * @brief 接收到服务器notify下行消息时的日志状态码
 */
#define STATE_TASK_RECV_NOTIFY                                      (-0x0B03)

/**
 * @brief 解析服务器下推的MQTT下行JSON报文时出错
 */
#define STATE_TASK_PARSE_NOTIFY_FAILED                              (-0x0B04)

/**
 * @brief 为解析JSON报文而申请内存时, 未获取到所需内存而解析失败
 */
#define STATE_TASK_PARSE_JSON_MALLOC_FAILED                         (-0x0B05)

/**
 * @brief 接收到服务器notify下行消息时的日志状态码
 */
#define STATE_TASK_PARSE_JSON_ERROR                                 (-0x0B06)

/**
 * @brief 接收到查询task id是空
 */
#define STATE_TASK_QUERY_TASK_ID_IS_NULL                            (-0x0B07)

/**
 * @brief 接收到服务器get list reply下行消息时的日志状态码
 */
#define STATE_TASK_RECV_GET_LIST_REPLY                              (-0x0B08)

/**
 * @brief 配置task会话实例时, 发现会话句柄为空, 中止配置动作
 *
 */
#define STATE_TASK_SETOPT_DATA_IS_NULL                              (-0x0B09)

/**
 * @brief 配置task 描述时状态设置不对
 *
 */
#define STATE_TASK_UPDATE_STATUS_INVALID                            (-0x0B0A)

/**
 * @brief aiot_task_setopt 接口的option参数可选值.
 */

/**
 * @brief update task的时候task status_details只能为NULL或者json字符串对象
 *
 */
#define STATE_TASK_UPDATE_STATUS_DETAILS_INVALID                     (-0x0B0B)

typedef enum {
    /**
     * @brief 设置MQTT的handle
     *
     * @details
     *
     * OTA过程中使用MQTT的通道能力, 用以向云端上报版本号, 进度, 以及错误码
     *
     * 数据类型: (void *)
     */
    AIOT_TASKOPT_MQTT_HANDLE,

    /**
     * @brief 设置处理task消息的用户回调函数
     *
     * @details
     *
     * 从云端下发或者返回的数据的处理函数
     *
     * 数据类型: (void *)
     */
    AIOT_TASKOPT_RECV_HANDLER,

    /**
     * @brief 用户需要SDK暂存的上下文
     *
     * @details
     *
     * 这个上下文指针会在 AIOT_TASKOPT_RECV_HANDLER设置的回调被调用时, 由SDK传给用户
     *
     * 数据类型: (void *)
     */
    AIOT_TASKOPT_USERDATA,
    AIOT_TASKOPT_MAX
} aiot_task_option_t;

/**
 * @brief 任务的状态.
 */
typedef enum {
    AIOT_TASK_STATUS_QUEUED,             /* 服务端设置的状态: 任务处于队列中, 还没有推送 */
    AIOT_TASK_STATUS_SENT,               /* 服务端设置的状态: 任务已推送 */
    AIOT_TASK_STATUS_IN_PROGRESS,        /* 设备端设置的状态: 任务进行中. 设备端开始执行一个任务后, 将 */
    AIOT_TASK_STATUS_SUCCEEDED,          /* 设备端设置的状态: 任务完成 */
    AIOT_TASK_STATUS_FAILED,             /* 设备端设置的状态: 任务执行失败 */
    AIOT_TASK_STATUS_REJECTED,           /* 设备端设置的状态: 设备端拒绝执行任务 */
    AIOT_TASK_STATUS_CANCELLED,          /* 服务端设置的状态: 任务被服务端取消 */
    AIOT_TASK_STATUS_REMOVED,            /* 服务端设置的状态: 任务从服务端删除 */
    AIOT_TASK_STATUS_TIMED_OUT,          /* 服务端设置的状态: 任务执行超时 */
    AIOT_TASK_STATUS_NOT_FOUND           /* 服务端设置的状态: 没有找到此任务相关信息 */
} aiot_task_status_t;

/**
 * @brief 下行有关的数据结构
 */
typedef enum {
    AIOT_TASKRECV_NOTIFY,               /* 对应/sys/{productKey}/{deviceName}/thing/job/notify 这个下行topic, 云端主动下推, 带任务详情 */
    AIOT_TASKRECV_GET_DETAIL_REPLY,     /* 对应/sys/{productKey}/{deviceName}/thing/job/get_reply 这个下行topic, 可以是单个任务的详情, 也可以是任务列表的简单描述 */
    AIOT_TASKRECV_GET_LIST_REPLY,       /* 对应/sys/{productKey}/{deviceName}/thing/job/get_reply 这个下行topic, 可以是单个任务的详情, 也可以是任务列表的简单描述 */
    AIOT_TASKRECV_UPDATE_REPLY          /* 对应/sys/{productKey}/{deviceName}/thing/job/update_reply 这个下行topic, 里面包含某个任务的update的结果, 即是否成功 */
} aiot_task_recv_type_t;

/**
 * @brief 任务描述的数据结构
 */
typedef struct {
    char *task_id;                      /* 任务ID */
    aiot_task_status_t status;          /* 任务的状态 */
    char *job_document;                 /* 任务执行规则 */
    char *sign_method;                  /* 文件签名的方法 */
    char *sign;                         /* 文件的签名 */
    char *document_file_url;            /* 任务文件下载的url */
    char *status_details;               /* 客户自定义状态，透传到云端, 注意格式为json对象，例如 "{\"key\": \"value\"", strlen("\"key\": \"value\"}"*/
    uint8_t progress;                   /* 任务处理的进度，数字从0-100 */
    void *handle;                       /* 任务处理的句柄 */
} task_desc_t;

/**
 * @brief 从云端拉取list时每个任务的简要描述
 */
typedef struct {
    char *task_id;                      /* 任务ID */
    aiot_task_status_t status;          /* 任务的状态 */
} task_summary_t;

/**
 * @brief 从云端拉取list返回的数据
 */
typedef struct {
    uint32_t number;                    /* 从云端拉取的任务list的大小 */
    task_summary_t *tasks;              /* 拉取的任务数组指针 */
} task_get_list_reply_t;

/**
 * @brief 从云端拉取任务详细信息时返回的数据
 */
typedef struct {
    uint32_t code;                      /* 云端返回的code */
    task_desc_t task;                   /* 任务描述的详细信息 */
} task_get_detail_reply_t;

/**
 * @brief 更新任务状态到云端后，云端返回的数据
 */
typedef struct {
    uint32_t code;                      /* 云端返回的code */
    char *task_id;                      /* 更新任务后返回的任务id */
    aiot_task_status_t status;          /* 更新任务后返回的状态 */
} task_update_reply_t;

/**
 * @brief 云端主动下发或更新任务云端返回的数据
 */
typedef struct {
    aiot_task_recv_type_t type;                     /* 返回的数据类型 */
    union {
        task_desc_t notify;                         /* 云端主动推送任务的数据 */
        task_get_list_reply_t get_list_reply;       /* 请求任务list返回的数据 */
        task_get_detail_reply_t get_detail_reply;   /* 请求任务详细状态返回的数据 */
        task_update_reply_t update_reply;           /* 更新任务状态返回的数据 */
    } data;
} aiot_task_recv_t;

/**
 * @brief 设备收到task的mqtt下行报文时的接收回调函数
 *
 * @param[in] handle task实例句柄
 * @param[in] recv 云端下行的消息
 * @param[in] userdata 用户上下文
 *
 * @return void
 */
typedef void (* aiot_task_recv_handler_t)(void *handle, const aiot_task_recv_t *recv, void *userdata);

/**
 * @brief 创建一个task实例
 *
 * @return void*
 * @retval 非NULL task实例句柄
 * @retval NULL 初始化失败, 或者是因为没有设置portfile, 或者是内存分配失败导致
 *
 */
void   *aiot_task_init(void);

/**
 * @brief 销毁task实例句柄
 *
 * @param[in] handle 指向task实例句柄的指针
 *
 * @return int32_t
 * @retval STATE_USER_INPUT_NULL_POINTER handle或者handle所指向的地址为空
 * @retval STATE_SUCCESS 执行成功
 *
 */
int32_t aiot_task_deinit(void **handle);

/**
 * @brief 设置task句柄的参数
 *
 * @details
 *
 * 对task会话进行配置, 常见的配置选项包括
 *
 * @param[in] handle task句柄
 * @param[in] option 配置选项, 更多信息请参考@ref aiot_task_option_t
 * @param[in] data   配置选项数据, 更多信息请参考@ref aiot_task_option_t
 *
 * @return int32_t
 * @retval STATE_TASK_SETOPT_HANDLE_IS_NULL task句柄为空
 * @retval STATE_TASK_SETOPT_DATA_IS_NULL 参数data字段为空
 * @retval STATE_USER_INPUT_UNKNOWN_OPTION option不支持
 * @retval STATE_SUCCESS 参数设置成功
 *
 */
int32_t aiot_task_setopt(void *handle, aiot_task_option_t option, void *data);

/**
 * @brief 从云端获取task列表
 *
 * @details
 *
 * 从云端获取task列表
 *
 * @param[in] handle task句柄
 *
 * @return int32_t
 * @retval STATE_TASK_SETOPT_DATA_IS_NULL 参数的handle字段为空
 * @retval STATE_SUCCESS                  发送成功
 */
int32_t aiot_task_get_task_list(void *handle);

/* 发送报文到/sys/{productKey}/{deviceName}/thing/job/get. 若函数入参user_task_id不为空, 则上行报文的payload为 "taskId": user_task_id, 返回该任务的详情; */
/* 若user_task_id为空, 则上行报文的payload为 "taskId": "$next", 云端返回未处于终态的任务队列中时间排在最前面一个任务, 该任务状态为QUEUED、SENT、IN_PROGRESS三者之一 */

/**
 * @brief 从云端获取task详细内容
 *
 * @details
 *
 * 从云端获取task详细内容
 *
 * @param[in] handle task句柄
 * @param[in] user_task_id task的id或者$next
 *
 * @return int32_t
 * @retval STATE_TASK_SETOPT_DATA_IS_NULL或user_task_id 参数的handle字段为空
 * @retval STATE_SUCCESS 发送成功
 *
 */
int32_t aiot_task_get_task_detail(void *handle, char *user_task_id);

/**
 * @brief 更新任务状态到云端
 *
 * @details
 *
 * 更新任务状态到云端
 *
 * @param[in] handle task句柄
 * @param[in] task task信息
 *
 * @return int32_t
 * @retval STATE_TASK_SETOPT_DATA_IS_NULL或task 参数的handle字段为空
 * @retval STATE_SUCCESS 更新成功
 *
 */
int32_t aiot_task_update(void *handle, task_desc_t *task);
#if defined(__cplusplus)
}
#endif

#endif  /* #ifndef __AIOT_TASK_API_H__ */


