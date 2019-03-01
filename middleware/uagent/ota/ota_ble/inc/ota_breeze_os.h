#ifndef __OTA_BREEZE_OS_H
#define __OTA_BREEZE_OS_H
#include <stdarg.h>
#include <stdio.h>
#include "ulog/ulog.h"

#define OTA_BREEZE_LOG_D(fmt, ...) LOGD("uota",fmt,##__VA_ARGS__)
#define OTA_BREEZE_LOG_I(fmt, ...) LOGI("uota",fmt,##__VA_ARGS__)
#define OTA_BREEZE_LOG_W(fmt, ...) LOGW("uota",fmt,##__VA_ARGS__)
#define OTA_BREEZE_LOG_E(fmt, ...) LOGE("uota",fmt,##__VA_ARGS__)

#define OTA_IMAGE_MD5_LEN          (16)
#define OTA_IMAGE_RESERVER_SIZE    (2)

typedef struct
{
    unsigned int   image_magic;
    unsigned int   image_size;
    unsigned char  image_md5_value[OTA_IMAGE_MD5_LEN];
    unsigned char  image_reserver[OTA_IMAGE_RESERVER_SIZE];
    unsigned short image_crc16;
} ota_breeze_image_t;

/*Thread*/
int ota_breeze_thread_create(
    void **thread_handle,
    void *(*work_routine)(void *),
    void *arg,
    void *param,
    int  stack_size);
void ota_breeze_msleep(int ms);
void ota_breeze_reboot(void);
void *ota_breeze_malloc(int size);
void ota_breeze_free(void *ptr);
#endif /*OTA_BREEZE_OS_H*/
