/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef ULOG_CONFIG_FILE__H_
#define ULOG_CONFIG_FILE__H_
#include "ulog_config.h"

typedef enum{
    ulog_cfg_type_working = 0,
    ulog_cfg_type_list,
    ulog_cfg_log_level_std,
    ulog_cfg_log_level_file,
    ulog_cfg_type_cnt
} ulog_cfg_type_t;

#define ULOG_FILE_FORMAT     FS_BACKUP_PATH"/syslog.log.%1d"

#define ULOG_CFG_FILE        FS_PATH"/syslog.conf"

#define OPERTING_LOG_FILE    FS_PATH"/syslog.log"

#define LOG_LIST_NODE "{\"idx\":%d,\"start\":\"%s\",\"end\":\"%s\"},"

#ifdef __cplusplus
extern "C" {
#endif

/* return 0xFF if get failed */
uint8_t get_ulog_level_from_cfg(const ulog_session_type_t session_type);

/* return 0 if set scucessfully, elese return -1 */
int32_t set_ulog_level_to_cfg(const ulog_session_type_t session_type, const uint8_t level);
#ifdef __cplusplus
}
#endif

#endif /* ULOG_CONFIG_FILE__H_ */

