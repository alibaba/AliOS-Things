/** @defgroup ota_hal_os_api
 * @{
 *
 * This is an include file of OTA OS adapt interface.
 *
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef OTA_HAL_OS_H
#define OTA_HAL_OS_H

#ifdef __cplusplus
extern "C" {
#endif

#define ota_snprintf snprintf

/*memory*/
void ota_free(void *ptr);
void *ota_malloc(int size);
void *ota_calloc(int n, int size);
void *ota_realloc(void *ptr, int size);

void ota_msleep(int ms);

/*Reboot*/
void ota_reboot(void);

/* String */
int ota_to_capital(char *value, int len);
int ota_str2hex(const char *src, char *dest, unsigned int dest_len);
int ota_hex2str(char *dest_buf, const unsigned char *src_buf, unsigned int dest_len, unsigned int src_len);

/*Base64*/
int ota_base64_decode(const unsigned char *input, unsigned int input_len, unsigned char *output, unsigned int *output_len);

/*CRC16*/
typedef struct {
    unsigned short crc;
} ota_crc16_ctx;
void ota_crc16_init(ota_crc16_ctx *ctx);
void ota_crc16_update(ota_crc16_ctx *ctx, const void *inSrc, unsigned int inLen);
void ota_crc16_final(ota_crc16_ctx *ctx, unsigned short *outResult);
unsigned short ota_get_data_crc16(const unsigned char *buf, unsigned int len);
void ota_thread_destroy(void *ptread);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif
#endif /*OTA_HAL_OS_H*/
