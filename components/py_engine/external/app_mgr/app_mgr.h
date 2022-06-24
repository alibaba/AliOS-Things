/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef APP_MGR_H
#define APP_MGR_H

#include <stdio.h>
#include <string.h>
// #include <unistd.h>
typedef enum {
    PYAMP_SAVE_PATH_DEFULT_DATA = 0,
    PYAMP_SAVE_PATH_DEFULT_SDCARD = 1,
} PYAMP_SAVE_PATH_DEFULT;

void pyamp_save_path_defult_set(PYAMP_SAVE_PATH_DEFULT mode);
int pyamp_save_path_defult_get();

int pyamp_check_file_path(unsigned char *path, int32_t path_len);
void pyamp_save_file_path(unsigned char *path, int32_t path_len);
int pyamp_get_file_path(unsigned char *path, int32_t path_len);

void pyamp_apppack_init();

int pyapp_download_and_upgrade(char *url);

int pyamp_apppack_upgrade(char *url);

#endif /* APP_MGR_H */
