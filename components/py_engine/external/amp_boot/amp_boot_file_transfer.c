/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "amp_boot_file_transfer.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "amp_platform.h"
#include "amp_task.h"
#include "amp_utils.h"
#include "aos/kernel.h"
#include "aos/vfs.h"
#include "aos_fs.h"
#include "aos_system.h"
#include "app_upgrade.h"
#include "py_defines.h"
#include "app_mgr.h"

#define MOD_STR "AMP_FILE_TRANSFER"

/* status led mode */
static AMP_FILE_TRANSFER_STATUS pyamp_status = AMP_FILE_TRANSFER_STATUS_NORMAL;

// this function use to recevie ymodem data
extern int pyamp_ymodem_upgrade(void (*func)(unsigned char *, int));

static unsigned char *recevie_filename = NULL;
static uint32_t recevie_file_size = 0;
static int file_fp = -1;
static uint32_t receive_file_offset = 0;
static int receive_file_end_flag = 0;
// static char save_path[256] = { 0 };
// static int32_t save_path_len = 0;

// static PYAMP_SAVE_PATH_DEFULT pyamp_defult_save_path = PYAMP_SAVE_PATH_DEFULT_DATA;

// void pyamp_save_path_defult_set(PYAMP_SAVE_PATH_DEFULT mode)
// {
//     pyamp_defult_save_path = mode;
// }

// int pyamp_save_path_defult_get()
// {
//     return pyamp_defult_save_path;
// }

// int pyamp_ymodem_check_file_path(unsigned char *path, int32_t path_len)
// {
//     int ret = 0;
//     // set path to test
//     char test_path[256] = { 0 };
//     char transferpath[256] = { 0 };

//     memset(transferpath, 0, path_len);
//     memcpy(transferpath, path, path_len);

//     strncat(test_path, transferpath, sizeof(test_path) - 1);
//     strncat(test_path, "_pathTest", sizeof(test_path) - 1);

//     ret = aos_mkdir(test_path);
//     if (ret == 0) {
//         ret = aos_rmdir_r(test_path);
//     }

//     return ret;
// }

// void pyamp_ymodem_save_file_path(unsigned char *path, int32_t path_len)
// {
//     if (path_len != 0) {
//         save_path_len = path_len;
//         memset(save_path, 0, path_len);
//         memcpy(save_path, path, path_len);
//     } else {
//         save_path_len = path_len;
//     }
// }

// int pyamp_ymodem_get_file_path(unsigned char *path, int32_t path_len)
// {
//     if (save_path_len != 0) {
//         memcpy(path, save_path, path_len);
//         return save_path_len;
//     }
//     return 0;
// }

// init file transfer,  register write file callback function
void pyamp_ymodem_recovery_file_transfer_init()
{
    // aos_rmdir_r(AMP_FS_ROOT_DIR);
    char save_path[256] = { 0 };
    int save_path_len = 0;
    save_path_len = pyamp_get_file_path(save_path, 256);

    if (save_path_len != 0) {
        aos_mkdir(save_path);
    } else {
        if (pyamp_save_path_defult_get() == PYAMP_SAVE_PATH_DEFULT_SDCARD) {
            aos_mkdir(AMP_FS_EXT_ROOT_DIR);
        } else {
            aos_mkdir(AMP_FS_ROOT_DIR);
        }
    }
}

// finish this package transfer
// only transfer this package
void pyamp_ymodem_recovery_file_transfer_finish(void)
{
    pyamp_file_transfer_status_set(AMP_FILE_TRANSFER_STATUS_NORMAL);
    // LOGD(MOD_STR, "pyamp_ymodem_file_transfer_finish");
}

void pyamp_ymodem_installapp()
{
    const char *pFile;
    int ret = 0;
    char save_path[256] = { 0 };
    int save_path_len = 0;
    save_path_len = pyamp_get_file_path(save_path, 256);

    // step 1: judge download status
    if (pyamp_status != AMP_FILE_TRANSFER_STATUS_INSTALL) {
        // need free recevie file name
        if (recevie_filename != NULL) {
            aos_free(recevie_filename);
            recevie_filename = NULL;
        }
        return;
    }

    // step 2: judge download file name
    pFile = strrchr(recevie_filename, '.');
    if (pFile != NULL) {
        // judge zip file need install
        if (!(strcmp(pFile, ".zip"))) {
            // make pkg path
            char path[256] = { 0 };
            if (save_path_len != 0) {
                strncat(path, save_path, sizeof(path) - 1);
                strncat(path, "/", sizeof(path) - 1);
            } else {
                if (pyamp_save_path_defult_get() == PYAMP_SAVE_PATH_DEFULT_SDCARD) {
                    strncat(path, AMP_FS_EXT_ROOT_DIR "/", sizeof(path) - 1);
                } else {
                    strncat(path, AMP_FS_ROOT_DIR "/", sizeof(path) - 1);
                }
            }
            strncat(path, recevie_filename, sizeof(path) - 1);
            // install app from path to default path
            // ret = install_pyapp(path, AMP_PY_PKG_DIR);
            if ((path != NULL) && (AMP_PY_PKG_DIR != NULL)) {
                // amp_debug(MOD_STR, "store_file:%s, install_file:%s\n", path, AMP_PY_PKG_DIR);
                ret = miniUnzip(path, AMP_PY_PKG_DIR);
            }
            if (ret < 0) {
                amp_error(MOD_STR, "py app install failed\n");
            }
        }
    }

    // do not need install
    // need free recevie file name
    if (recevie_filename != NULL) {
        aos_free(recevie_filename);
        recevie_filename = NULL;
    }
}

int pyamp_ymodem_receive_open_file(const char *targetname)
{
    int fp;
    char path[256] = { 0 };
    char save_path[256] = { 0 };
    int save_path_len = 0;
    save_path_len = pyamp_get_file_path(save_path, 256);

    if (targetname == NULL) {
        return -1;
    }
    if (save_path_len != 0) {
        strncat(path, save_path, sizeof(path) - 1);
        strncat(path, "/", sizeof(path) - 1);
    } else {
        if (pyamp_save_path_defult_get() == PYAMP_SAVE_PATH_DEFULT_SDCARD) {
            strncat(path, AMP_FS_EXT_ROOT_DIR "/", sizeof(path) - 1);
        } else {
            strncat(path, AMP_FS_ROOT_DIR "/", sizeof(path) - 1);
        }
    }
    strncat(path, targetname, sizeof(path) - 1);

    int i = 1;
    int len = strlen(path);
    for (; i < len; i++) {
        if (path[i] == '/') {
            path[i] = 0;
            aos_mkdir(path);
            path[i] = '/';
        }
    }
    // aos_printf("path=%s", path);
    fp = aos_open(path, O_WRONLY | O_CREAT | O_TRUNC);

    return fp;
}

int pyamp_ymodem_save_transfer_file(uint8_t *buf, int32_t buf_len, AMP_FILE_RECEVIE_FLAG flag)
{
    int32_t write_buf_len = 0;
    // judge flag
    if (flag == YMODEM_RECEVIE_HEADER) {
        // save file name
        // check receive file name if free
        if (recevie_filename != NULL) {
            aos_free(recevie_filename);
            recevie_filename = NULL;
        }
        recevie_filename = aos_malloc(buf_len);
        memset(recevie_filename, 0, buf_len);
        memcpy(recevie_filename, buf, buf_len);

        // aos_printf("name=%s", recevie_filename);
    } else if (flag == YMODEM_RECEVIE_FILE_SIZE) {
        // save file size
        recevie_file_size = buf_len;
        // aos_printf("size=%d", recevie_file_size);
    } else if (flag == YMODEM_RECEVIE_BODY) {
        int ret;

        if (file_fp == -1) {
            file_fp = pyamp_ymodem_receive_open_file(recevie_filename);
        }

        if (file_fp >= 0) {
            int32_t i = 0;
            // judge receive file size
            receive_file_offset += buf_len;
            if (receive_file_offset > recevie_file_size) {
                // need cal real size, delete 0x1A
                for (i = buf_len - 1; i > 0; i--) {
                    if ((0x1a == buf[i])) {
                        // aos_printf("len=%d", write_buf_len);
                        // break;
                    } else {
                        write_buf_len = (i + 1);
                        break;
                    }
                }
                // aos_printf("len=%d", write_buf_len);
                receive_file_end_flag = 1;
            } else if (receive_file_offset == recevie_file_size) {
                write_buf_len = buf_len;
                receive_file_end_flag = 1;
            } else {
                write_buf_len = buf_len;
            }

            if (buf_len > 0) {
                ret = aos_write(file_fp, buf, write_buf_len);
            }

            if (receive_file_end_flag == 1) {
                ret = aos_sync(file_fp);
                ret = aos_close(file_fp);
                file_fp = -1;
                // reset flags
                receive_file_end_flag = 0;
                receive_file_offset = 0;
                recevie_file_size = 0;
            }
        }
    } else if (flag == YMODEM_RECEVIE_END) {
        // finish write file need close file fp
        // set status install
        pyamp_file_transfer_status_set(AMP_FILE_TRANSFER_STATUS_INSTALL);
    } else {
    }

    return 0;
}

// recive ymodem transfer data and write to file
void pyamp_ymodem_transfer_file_write(unsigned char *buf, int size, AMP_FILE_RECEVIE_FLAG flag)
{
    pyamp_file_transfer_status_set(AMP_FILE_TRANSFER_STATUS_UPDATING);
    pyamp_ymodem_save_transfer_file(buf, size, flag);
}

void pyamp_file_transfer_status_set(AMP_FILE_TRANSFER_STATUS mode)
{
    if (mode >= AMP_FILE_TRANSFER_STATUS_END) {
        return;
    }
    pyamp_status = mode;
}

int pyamp_recovery_file_transfer()
{
    pyamp_ymodem_recovery_file_transfer_init();
    pyamp_ymodem_upgrade(pyamp_ymodem_transfer_file_write);
    // instal app
    pyamp_ymodem_installapp();
    pyamp_ymodem_recovery_file_transfer_finish();
}
