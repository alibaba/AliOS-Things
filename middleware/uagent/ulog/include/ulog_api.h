/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef U_LOG_API__H
#define U_LOG_API__H

#include "ulog_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
    SESSION_DIRECTION,  /* default out direction, usually uart for rtos, termial for Linux */
#if ULOG_POP_UDP_ENABLE
    SESSION_UDP,        /* syslog via udp, port 514 is default for syslog watcher */
#endif

#if ULOG_POP_FS_ENABLE
    SESSION_FILE,       /* log on local file system */
#endif

    SESSION_CNT
}SESSION_TYPE;


extern bool log_init;

int log_get_mutex(void);

void log_release_mutex(void);

void log_init_mutex(void);

void on_sync_filter_level_change(const char level);
char get_sync_stop_level();


int32_t ulog_fs_init(void);

void on_filter_level_change(const SESSION_TYPE session, const uint8_t level);

void ulog_async_init(const char host_name[8]);

int32_t pop_out_on_udp(const char* data, const uint16_t len);

int32_t pop_out_on_fs(const char* data, const uint16_t len);

void on_show_ulog_file(void);


void update_net_cli(const char cmd, const char* param);
void fs_control_cli(const char cmd, const char* param);



void on_update_syslog_watcher_addr(const uint32_t ip_nl, const uint32_t port);
void on_tcpip_service_on(const uint32_t on, const uint32_t off);
void on_fs_record_pause(const uint32_t on, const uint32_t off);

char* trim_file_path(const char* path);
char *syslog_format_time(char *buffer, const int len);

#ifdef __cplusplus
}
#endif

#endif /*U_LOG_API__H*/
