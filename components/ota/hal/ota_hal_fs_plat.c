#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <aos/vfs.h>
#include "ota_log.h"
#include "ota_import.h"
#include "ota_hal_os.h"
#include "ota_hal.h"
#include "upack_data_file.h"

OTA_WEAK int ota_fopen(const char *filename, int mode)
{
    int fd = -1;
    if (filename != NULL) {
        fd = aos_open(filename, mode);
    }
    return fd;
}

OTA_WEAK int ota_fwrite(int fd, const void *buf, unsigned int size)
{
    int ret = -1;
    if ((fd >= 0) && (buf != NULL)) {
        ret = aos_write(fd, buf, size);
    }
    return ret;
}

OTA_WEAK int ota_fread(int fd, void *buf, unsigned int size)
{
    int ret = -1;
    if ((fd >= 0) && (buf != NULL)) {
        ret = aos_read(fd, buf, size);
    }
    return ret;
}

OTA_WEAK int ota_fclose(int fd)
{
    int ret = -1;
    if (fd >= 0) {
        ret = aos_close(fd);
    }
    return ret;
}

OTA_WEAK int ota_jsapp_version_get(char *version, char *file_path)
{
    int ret = -1;
    int i = 0;
    int fd = -1;
    char read_buf[65];
    int read_len = 0;
    char *pos = 0;
    if ((version != NULL) && (file_path != NULL)) {
        ret = 0;
        fd = ota_fopen(file_path, O_RDONLY);
        if (fd < 0) {
            OTA_LOG_I("can't find app.json file\r\n");
            strcpy(version, "0.0.0");
        } else {
            memset(read_buf, 0x00, sizeof(read_buf));
            read_len = ota_fread(fd, read_buf, sizeof(read_buf) - 1);
            if (read_len > 13) {
                pos = strstr(read_buf, "\"version\":");
                if (pos != NULL) {
                    pos += 10;
                    while (pos[0] != '"') {
                        pos++;
                    }
                    pos++;
                    while (pos[0] != '"') {
                        version[i++] = pos[0];
                        pos++;
                    }
                    version[i] = 0;
                }
            }
        }
        if (fd >= 0) {
            ota_fclose(fd);
        }
    }
    return ret;
}

int ota_install_jsapp(void *ota_ctx, char *store_file, int store_file_len, char *install_path)
{
    int ret = -1;
    ota_service_t *ctx = ota_ctx;
    OTA_LOG_I("upgrade sub module\n");
    if ((store_file != NULL) && (install_path != NULL)) {
        OTA_LOG_I("store_file:%s, install_file:%s\n", store_file, install_path);
        ret = data_file_unpack(store_file, store_file_len, install_path);
    }
    if (ret < 0) {
        OTA_LOG_E("js app install failed\n");
        if ((ctx != NULL) && (ctx->report_func.report_status_cb !=  NULL)) {
            ctx->report_func.report_status_cb(ctx->report_func.param, ret);
        }
    }
    if ((ctx != NULL) && (ctx->feedback_func.on_user_event_cb != NULL)) {
        ctx->feedback_func.on_user_event_cb(OTA_EVENT_INSTALL, ret, ctx->feedback_func.param);
    }
    return ret;
}