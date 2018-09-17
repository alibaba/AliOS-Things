/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef _REC_PUB_H_
#define _REC_PUB_H_

#include <stddef.h>
#include <stdint.h>
#include "typedef.h"
#include "hal/soc/flash.h"
#include "rec_hal.h"

#ifndef FALSE
#define	FALSE			    (0)
#endif
#ifndef TRUE
#define	TRUE			    (1)
#endif

typedef unsigned char       u_char;
typedef signed long         off_t;

typedef struct REC_FLAG_INFO {
    unsigned int flag;    // recovery标志
    unsigned int num;     // recovery序号
}REC_FLAG_INFO_STRU;

#define LOG(fmt, ...) printf("%d"fmt"\r\n", __LINE__, ##__VA_ARGS__)

/* !!!attention!!! do not use any libc func in fota update process */
#define memset              rec_memset
#define memcpy              rec_memcpy
#define memcmp              rec_memcmp
#define memmove             rec_memmove
#define strlen              rec_strlen
#define strcat              rec_strcat
#define puts                rec_puts
#define printf              rec_printf
#define malloc              rec_malloc
#define free                rec_free

void *rec_memset(void *s, int c, size_t n);
void *rec_memcpy(void *dest, const void *src, size_t n);
int rec_memcmp(const void * buf1, const void * buf2, size_t count);
void *rec_memmove(void *dest, const void *src, size_t count);
size_t rec_strlen(char * str);
char *rec_strcat (char * dst, const char * src);
int rec_puts(const char *str);
int rec_printf(const char *format, ...);
void *rec_malloc(size_t alloc_size);
void rec_free(void *pfree);
size_t rec_freesize(void);
size_t rec_freesize_min(void);

/* CRC lib */
#define CRC16_Init          rec_CRC16_Init
#define CRC16_Update        rec_CRC16_Update
#define CRC16_Final         rec_CRC16_Final
typedef struct {
    uint16_t crc;
} CRC16_Context;
void rec_CRC16_Init( CRC16_Context *inContext );
void rec_CRC16_Update( CRC16_Context *inContext, const void *inSrc, size_t inLen );
void rec_CRC16_Final( CRC16_Context *inContext, uint16_t *outResult );


/* hal api for fota update process, defined by soc */
void rec_start();
void rec_hal_init();
void rec_uart_init();
void rec_reboot();
void rec_err_print(void *errinfo);
void rec_delayms(volatile int ms);

void rec_uart_send(unsigned char *buff, int len);
void rec_uart_send_string(char *buff);

void rec_flash_init(void);
unsigned long rec_flash_addr2ofst(unsigned long addr);
void rec_flash_erase(unsigned long offset);
void rec_flash_read_data(unsigned char *buffer, unsigned long offset, unsigned long len);
void rec_flash_write_data(unsigned char *buffer, unsigned long offset, unsigned long len);
hal_logic_partition_t *rec_flash_get_info(hal_partition_t pno);
uint32_t rec_get_recflag_addr();

uint32_t get_partion_length(int par);

#endif //_REC_PUB_H_
