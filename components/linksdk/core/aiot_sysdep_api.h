/**
 * @file aiot_sysdep_api.h
 * @brief SDK Core系统依赖头文件, 所有Core中的系统依赖均在此文件中列出
 * @date 2019-12-27
 *
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 *
 */

#ifndef _AIOT_SYSDEP_API_H_
#define _AIOT_SYSDEP_API_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

typedef enum {
    AIOT_SYSDEP_NETWORK_CRED_NONE,
    AIOT_SYSDEP_NETWORK_CRED_SVRCERT_CA,
    AIOT_SYSDEP_NETWORK_CRED_SVRCERT_PSK,
    AIOT_SYSDEP_NETWORK_CRED_MAX
} aiot_sysdep_network_cred_option_t;

typedef struct {
    aiot_sysdep_network_cred_option_t option;  /* 安全策略 */
    uint32_t      max_tls_fragment;
    uint8_t       sni_enabled;
    const char   *x509_server_cert;     /* 必须位于静态存储区, SDK内部不做拷贝 */
    uint32_t      x509_server_cert_len;
    const char   *x509_client_cert;     /* 必须位于静态存储区, SDK内部不做拷贝 */
    uint32_t      x509_client_cert_len;
    const char   *x509_client_privkey;  /* 必须位于静态存储区, SDK内部不做拷贝 */
    uint32_t      x509_client_privkey_len;
    char         *tls_extend_info;
} aiot_sysdep_network_cred_t;

typedef enum {
    CORE_SYSDEP_SOCKET_TCP_CLIENT,
    CORE_SYSDEP_SOCKET_TCP_SERVER,
    CORE_SYSDEP_SOCKET_UDP_CLIENT,
    CORE_SYSDEP_SOCKET_UDP_SERVER
} core_sysdep_socket_type_t;

typedef struct {
    char *psk_id;
    char *psk;
} core_sysdep_psk_t;

typedef enum {
    CORE_SYSDEP_NETWORK_SOCKET_TYPE,             /* 需要建立的socket类型  数据类型: (core_sysdep_socket_type_t *) */
    CORE_SYSDEP_NETWORK_HOST,                    /* 用于建立网络连接的域名地址或ip地址, 内存与上层模块共用  数据类型: (char *) */
    CORE_SYSDEP_NETWORK_BACKUP_IP,               /* 当建联DNS解析失败时, 使用此备用ip重试 */
    CORE_SYSDEP_NETWORK_PORT,                    /* 用于建立网络连接的端口号  数据类型: (uint16_t *) */
    CORE_SYSDEP_NETWORK_CONNECT_TIMEOUT_MS,      /* 建立网络连接的超时时间  数据类型: (uint32_t *) */
    CORE_SYSDEP_NETWORK_CRED,                    /* 用于设置网络层安全参数  数据类型: (aiot_sysdep_network_cred_t *) */
    CORE_SYSDEP_NETWORK_PSK,                     /* 用于配合PSK模式下的psk-id和psk  数据类型: (core_sysdep_psk_t *) */
    CORE_SYSDEP_NETWORK_MAX
} core_sysdep_network_option_t;

typedef struct {
    char addr[16]; /* ipv4地址点分十进制字符串, 最大长度15字节.  */
    uint16_t port; /* 端口号 */
} core_sysdep_addr_t;

/* 这不是一个面向用户的编译配置开关, 多数情况下, 不必用户关心 */

/**
 * @brief 用以向SDK描述其运行硬件平台的资源如何使用的方法结构体
 */
typedef struct {
    /**
     * @brief 申请内存
     */
    void    *(*core_sysdep_malloc)(uint32_t size, char *name);
    /**
     * @brief 释放内存
     */
    void (*core_sysdep_free)(void *ptr);
    /**
     * @brief 获取当前的时间戳，SDK用于差值计算
     */
    uint64_t (*core_sysdep_time)(void);
    /**
     * @brief 睡眠指定的毫秒数
     */
    void (*core_sysdep_sleep)(uint64_t time_ms);
    /**
     * @brief 创建1个网络会话(L3层)
     */
    void    *(*core_sysdep_network_init)(void);

    /**
     * @brief 配置1个网络会话的连接参数
     */
    int32_t (*core_sysdep_network_setopt)(void *handle, core_sysdep_network_option_t option, void *data);
    /**
     * @brief 建立1个网络会话, 作为MQTT/HTTP等协议的底层承载
     */
    int32_t (*core_sysdep_network_establish)(void *handle);
    /**
     * @brief 从指定的网络会话上读取
     */
    int32_t (*core_sysdep_network_recv)(void *handle, uint8_t *buffer, uint32_t len, uint32_t timeout_ms,
                                        core_sysdep_addr_t *addr);
    /**
     * @brief 在指定的网络会话上发送
     */
    int32_t (*core_sysdep_network_send)(void *handle, uint8_t *buffer, uint32_t len, uint32_t timeout_ms,
                                        core_sysdep_addr_t *addr);
    /**
     * @brief 销毁1个网络会话
     */
    int32_t (*core_sysdep_network_deinit)(void **handle);
    /**
     * @brief 随机数生成方法
     */
    void (*core_sysdep_rand)(uint8_t *output, uint32_t output_len);
    /**
     * @brief 创建互斥锁
     */
    void    *(*core_sysdep_mutex_init)(void);
    /**
     * @brief 申请互斥锁
     */
    void (*core_sysdep_mutex_lock)(void *mutex);
    /**
     * @brief 释放互斥锁
     */
    void (*core_sysdep_mutex_unlock)(void *mutex);
    /**
     * @brief 销毁互斥锁
     */
    void (*core_sysdep_mutex_deinit)(void **mutex);
} aiot_sysdep_portfile_t;

void aiot_sysdep_set_portfile(aiot_sysdep_portfile_t *portfile);
aiot_sysdep_portfile_t *aiot_sysdep_get_portfile(void);

#if defined(__cplusplus)
}
#endif

#endif

