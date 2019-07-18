/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef ULOG_SESSION_FILE_H__
#define ULOG_SESSION_FILE_H__
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef unsigned short ulog_idx_type;

#define ULOG_FILE_IDX_INVALID (ulog_idx_type)0xFFFF

#define ULOG_FILE_IDX_START   (ulog_idx_type)1

#define ULOG_CFG_PARA_VAL_SIZE 24

#define SESSION_FS_INFO printf
#define SESSION_FS_DEBUG(...)

#define ULOG_FILE_FORMAT     FS_PATH"ulog%03d.log"

#define ULOG_FILE_CFG_IDX    0

#define ULOG_CFG_LINE_MAX_SIZE 128

#define LOG_LINE_SEPARATOR '\n'
#define LOG_LINE_END_STR   "\n"

#define LOG_LIST_PREFIX "{\"time\":\"%s\",list:["
#define LOG_LIST_SUFFIX "]}"

#define LOG_LIST_NODE "{\"idx\":%d,\"start\":\"%s\",\"end\":\"%s\"},"

typedef enum{
    ulog_cfg_para_start = 0,
    ulog_cfg_para_end,
    ulog_cfg_para_cnt,
    ulog_cfg_para_none
}ulog_cfg_para_t;

typedef enum{
    ulog_cfg_type_working = 0,
    ulog_cfg_type_list,
    ulog_cfg_type_cnt
} ulog_cfg_type_t;

#ifndef CSP_LINUXHOST

#include "aos/vfs.h"
#else /* !CSP_LINUXHOST */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>

typedef DIR aos_dir_t;

typedef struct dirent aos_dirent_t;

#define aos_open open

#define aos_close close

#define aos_write write

#define aos_read read

#define aos_opendir opendir

#define aos_readdir readdir

#define aos_closedir closedir

#define aos_lseek lseek

#define aos_sync fsync

#define aos_unlink unlink

#endif /*CSP_LINUXHOST */

#ifdef __cplusplus
extern "C" {
#endif

void cfg_init_mutex(void);

bool cfg_get_mutex(void);

void cfg_release_mutex(void);

/*
 * get one line from specify fd.
 *
 * @param[in]  fd file description.
 * @param[out]  buf buffer to save the output character.
 * @param[in]  buf_len buffer room size.
 */
int get_log_line(const int fd, char* buf, const uint16_t buf_len);

/* O_APPEND */
int open_log_file(const ulog_idx_type file_idx, int flag, const off_t off);

ulog_idx_type get_working_from_cfg_mm(void);

int update_mm_cfg(const ulog_cfg_type_t type, const unsigned short idx, const ulog_cfg_para_t para_type, char val[ULOG_CFG_PARA_VAL_SIZE]);
int cfg_mm_2_file(const int fd);
int write_log_line(const int file_instanse, const char* buf, const bool keep_open);
int load_cfg_2_mm(void);

#ifdef __cplusplus
}
#endif

#endif /* ULOG_SESSION_FILE_H__ */

