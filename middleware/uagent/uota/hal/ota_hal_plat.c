/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include "ota_log.h"
#include "ota_hal_plat.h"

static FILE *ota_fd = NULL;
#define OTA_IMAGE_FILE "ota_app_linux.elf"

static int ota_init(hal_ota_module_t *m, void *something)
{
    uint32_t offset = *(uint32_t *)something;
    if (offset != 0) { /*breakpoint resume*/
        if (ota_fd == NULL) {
            if ((ota_fd = fopen(OTA_IMAGE_FILE, "a+")) == NULL) {
                return -1;
            }
            if (ftell(ota_fd) <= 0) {
                return -1;
            }
        }
    }
    return 0;
}

int ota_write(hal_ota_module_t *m, volatile uint32_t *off_set, uint8_t *in_buf,
              uint32_t in_buf_len)
{
    int ret = 0;
    if (ota_fd == NULL) {
        ota_fd = fopen(OTA_IMAGE_FILE, "w");
    }

    ret = fwrite(in_buf, in_buf_len, 1, ota_fd);
    if (ret != 1) {
        OTA_LOG_E("write error: %d, %d ,%s\n", ret, in_buf_len,
                  strerror(errno));
        return -1;
    }
    return 0;
}

static int ota_read(hal_ota_module_t *m, volatile uint32_t *off_set,
                    uint8_t *out_buf, uint32_t out_buf_len)
{
    if (ota_fd != NULL) {
        fflush(ota_fd);
        fclose(ota_fd);
        ota_fd = NULL;
    }
    int ret = 0;
    if (ota_fd == NULL) {
        ota_fd = fopen(OTA_IMAGE_FILE, "r");
    }
    ret = fseek(ota_fd, *off_set, SEEK_SET);
    ret = fread(out_buf, out_buf_len, 1, ota_fd);
    if (ret != 1) {
        OTA_LOG_E("read: %d, %d ,%s\n", ret, out_buf_len, strerror(errno));
        return -1;
    }
    fclose(ota_fd);
    ota_fd = NULL;
    *off_set += out_buf_len;
    return 0;
}

static int ota_set_boot(hal_ota_module_t *m, void *something)
{
    int ret = 0;
    if (ota_fd != NULL) {
        fflush(ota_fd);
        fclose(ota_fd);
        ota_fd = NULL;
    }
    char cmd1[256] = { 0 };
    sprintf(cmd1, "chmod +x ./%s", OTA_IMAGE_FILE);
    ret = system(cmd1);
    if (ret < 0) {
        printf("system fail.");
    }
    printf("Rebooting and updating FLASH now....\n");
    return 0;
}

static int ota_rollback(hal_ota_module_t *m, void *something)
{
    return 0;
}

struct hal_ota_module_s ota_hal_module = {
    .init         = ota_init,
    .ota_write    = ota_write,
    .ota_read     = ota_read,
    .ota_set_boot = ota_set_boot,
    .ota_rollback = ota_rollback,
};
