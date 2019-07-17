/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_UPGRADER_H
#define BE_UPGRADER_H

#if defined(__cplusplus)
extern "C" {
#endif

#define MD5_LEN 32

typedef enum {
    BE_UPGD_INVALID_TYPE = -1,
    BE_UPGD_KERNEL_TYPE  = 0,
    BE_UPGD_SPIFFS_TYPE  = 1,
    BE_UPGD_MAX_TYPE
} be_upgd_type_t;

typedef struct {
    be_upgd_type_t type;
    char md5[MD5_LEN + 1];
    char *url;
    int total_size;
} be_upgd_param_t;

typedef int (*be_write_flash_cb_t)(int32_t writed_size, uint8_t *buf,
                                   int32_t buf_len, int type);

typedef int (*be_upgd_finish_cb_t)(int finish_result, void *updated_version);

void be_upgrader_handler(be_upgd_param_t *param);

#if defined(__cplusplus)
}
#endif

#endif /* BE_UPGRADER_H */
