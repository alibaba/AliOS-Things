/*
 * Copyright (2017) Baidu Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * Author: Zhong Shuai (zhongshuai@baidu.com)
 *
 * Desc: Provide System Information
 */

#ifndef BAIDU_DUER_LIBDUER_DEVICE_LIGHTDUER_SYSTEM_INFO_H
#define BAIDU_DUER_LIBDUER_DEVICE_LIGHTDUER_SYSTEM_INFO_H

#include <stdint.h>
#include <stddef.h>

#define DUER_OS_VERSION "release-v3.1.1"

#define OS_VERSION_LEN          20
#define SW_VERSION_LEN          20
#define FIRMWARE_VERSION_LEN    15
#define BRAND_LEN               20
#define EQUIPMENT_TYPE_LEN      20
#define HARDWARE_VERSION_LEN    20
#define DEVELOPER_NAME_LEN      20

#define TASK_NAME_LEN           30

#define PARTITION_NAME_LEN      10
#define MOUNT_INFO_LEN          20
#define UNILIZATION_LEN         5

#define NETWORK_CARD_NAME_LEN   10
#define HW_ADDRESS_LEN          17
#define SSID_LEN                20
#define BSSID_LEN               17
#define IPV4_ADDRESS_LEN        32
#define BCAST_LEN               32
#define MASK_LEN                32
#define IPV6_ADDRESS_LEN        128
#define LINK_ENACP_LEN          10

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _system_static_info_s {
    char os_version[OS_VERSION_LEN + 1];
    char sw_version[SW_VERSION_LEN + 1];
    char brand[BRAND_LEN + 1];
    char equipment_type[EQUIPMENT_TYPE_LEN + 1];
    char hardware_version[HARDWARE_VERSION_LEN + 1];
    uint32_t ram_size;                                 // Unit (KB)
    uint32_t rom_size;                                 // Unit (KB)
} duer_system_static_info_t;

typedef struct _task_info_s {
    char task_name[TASK_NAME_LEN];
    int8_t cpu_occupancy_rate;              // If it is negative, it will not reported
    uint32_t stack_size;
    uint32_t stack_free_size;
    int32_t stack_max_used_size;            // If it is negative, it will not reported
    struct _task_info_s *next;
} duer_task_info_t;

typedef struct _system_dynamic_info_s {
    uint64_t system_up_time;                // Unit (Heartbeat of the system)
    uint32_t running_task;
    uint32_t total_task;
    duer_task_info_t *task_info;
    char *system_load;                      // If it is NULL, it will not reported
    size_t system_load_len;
    char *system_crash_info;                // If it is NULL, it will not reported
    size_t system_crash_info_len;
} duer_system_dynamic_info_t;

typedef enum _memory_type {
    RAM   = 1,
    PSRAM = 2,
} duer_memory_type_t;

typedef struct _memory_info {
    duer_memory_type_t memory_type;
    uint64_t total_memory_size;                        // Unit (KB)
    uint64_t available_memory_size;                    // Unit (KB)
    int64_t shared_memory_size;
    int64_t buffer_memory_size;
    int64_t swap_size;
    int64_t free_swap_size;
    uint64_t peak;                                     // Within 5 minutes Unit (KB)
    uint64_t trough;                                   // Within 5 minutes Unit (KB)
    uint64_t average;                                  // Within 5 minutes Unit (KB)
} duer_memory_info_t;

typedef struct _disk_info_t {
    char partition_name[PARTITION_NAME_LEN + 1];
    char mount_info[MOUNT_INFO_LEN + 1];
    uint8_t unilization;                               // percentage (50%)
    uint64_t total_size;                               // Unit (KB)
    uint64_t used_size;                                // Unit (KB)
    uint64_t available_size;                           // Unit (KB)
    struct _disk_info_t *next;
} duer_disk_info_t;

typedef struct _wireless_info_s {
    char ssid[SSID_LEN + 1];
    char bssid[BSSID_LEN + 1];

    int8_t  link;                                       // General quality of the reception
    int32_t level;                                     // Signal strength at the receiver
    int32_t noise;                                     // Silence level (no packet) at the receiver
} duer_wireless_info;

typedef enum {
    ETHERNET_TYPE = 1,
    WIFI_TYPE,
} duer_network_type;

// Only retport one network info which the Duer OS used
typedef struct _network_info_s {
    duer_network_type network_type;
    char hw_address[HW_ADDRESS_LEN + 1];
    char ipv4_address[IPV4_ADDRESS_LEN + 1];             // If it is NULL, it will not reported
    char ipv6_address[IPV6_ADDRESS_LEN + 1];             // If it is NULL, it will not reported
    duer_wireless_info wireless;
    uint32_t mtu;
    uint64_t transmitted_packets;
    uint64_t received_packets;
    uint64_t forwarded_packets;
    uint64_t dropped_packets;
    uint64_t checksum_error;
    uint64_t invalid_length_error;
    uint64_t routing_error;
    uint64_t protocol_error;
    uint64_t tx_average_speed;                            // Within 5 minutes Unit (bytes/s)
    uint64_t rx_average_speed;                            // Within 5 minutes Unit (bytes/s)
} duer_network_info_t;

typedef struct _system_info_ops_t {
    int (*init)(void);
    int (*get_system_static_info)(duer_system_static_info_t *info);
    int (*get_system_dynamic_info)(duer_system_dynamic_info_t *info);
    int (*free_system_dynamic_info)(duer_system_dynamic_info_t *info);
    int (*get_memory_info)(duer_memory_info_t *mem_info);
    int (*get_disk_info)(duer_disk_info_t **disk_info);
    int (*free_disk_info)(duer_disk_info_t *disk_info);
    int (*get_network_info)(duer_network_info_t *network_info);
    int (*free_network_info)(duer_network_info_t *network_info);
} duer_system_info_ops_t;

extern int duer_init_system_info(void);

extern int duer_system_info_register_system_info_ops(duer_system_info_ops_t *info_ops);

extern int duer_system_info_get_static_system_info(duer_system_static_info_t *info);

extern int duer_system_info_get_dynamic_system_info(duer_system_dynamic_info_t *info);

extern int duer_system_info_free_dynamic_system_info(duer_system_dynamic_info_t *info);

extern int duer_system_info_get_mem_info(duer_memory_info_t *info);

extern int duer_system_info_get_disk_info(duer_disk_info_t **info);

extern int duer_system_info_free_disk_info(duer_disk_info_t *info);

extern int duer_system_info_get_network_info(duer_network_info_t *info);

extern int duer_system_info_free_network_info(duer_network_info_t *info);

extern void duer_system_info_show_system_static_info(duer_system_static_info_t const *info);

extern void duer_system_info_show_system_dynamic_info(duer_system_dynamic_info_t const *info);

extern void duer_system_info_show_memory_info(duer_memory_info_t const *memory_info);

extern void duer_system_info_show_disk_info(duer_disk_info_t const *disk_info);

extern void duer_system_info_show_networkcard_info(duer_network_info_t const *network_info);

extern int duer_uninit_system_info(void);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIBDUER_DEVICE_LIGHTDUER_SYSTEM_INFO_H
