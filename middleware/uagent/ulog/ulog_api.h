/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef U_LOG_API__H
#define U_LOG_API__H
#include <stdint.h>
#include <stdbool.h>
#include "ulog_config.h"
#ifdef __cplusplus
extern "C" {
#endif

extern bool log_init;

bool log_get_mutex(void);

void log_release_mutex(void);

void log_init_mutex(void);

void on_sync_filter_level_change(const ulog_session_type_t session, const char level);
void on_filter_change(const ulog_session_type_t session, const char level);

char* get_sync_stop_level();

int32_t ulog_fs_init(void);

void on_filter_level_changes(const ulog_session_type_t session, const uint8_t level);

bool check_pass_pop_out(const ulog_session_type_t session, const uint8_t level);

void ulog_async_init();

int32_t pop_out_on_udp(const char* data, const uint16_t len);

int32_t pop_out_on_fs(const char* data, const uint16_t len);

void on_show_ulog_file(void);

void update_net_cli(const char cmd, const char* param);

void fs_control_cli(const char cmd, const char* param);

char* replace_char(char *str, char find, char replace);

void on_update_syslog_watcher_addr(const uint32_t ip_nl, const uint32_t port);
void on_tcpip_service_on(const uint32_t on, const uint32_t off);
void on_fs_record_pause(const uint32_t on, const uint32_t off);

char* trim_file_path(const char* path);
char *ulog_format_time(char *buffer, const int len);

int http_start(const char *url, const unsigned short idx);
void on_fs_upload(const uint32_t idx, const uint32_t start);

#ifdef __cplusplus
}
#endif

#endif /*U_LOG_API__H*/

