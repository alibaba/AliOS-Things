/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __LOG_SECTION_H__
#define __LOG_SECTION_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <string.h>

#if 0
void init_dump_log(void);
void dump_whole_logs(void);
//void log_update_time_stamp(void);
//void set_dump_log_flush(bool isEnable);
//bool is_dump_log_flush_pending(void);
//void flush_dump_log_handler(void);
void clear_dump_log(void);
uint32_t test_dump_log_from_flash(uint32_t addr,uint32_t size);
#else

#define LOG_DUMP_SECTOR_SIZE             0x1000
#define LOG_DUMP_SECTOR_BUFFER_COUNT     4
#define LOG_DUMP_BUFFER_LEN              (LOG_DUMP_SECTOR_SIZE*LOG_DUMP_SECTOR_BUFFER_COUNT)
#define LOG_DUMP_NORFALSH_BUFFER_LEN     (LOG_DUMP_BUFFER_LEN*2)

#define DATA_BUFFER_STATE_FREE           0
#define DATA_BUFFER_STATE_WRITTING       0x1
#define DATA_BUFFER_STATE_WRITTEN        0x2

#define DUMP_LOG_MAGIC                   0xd5151001
#define DUMP_LOG_VERSION                 0x00010001
// #define DUMP_LOG_NEWLINE  '\n'

enum LOG_DUMP_FLUSH_STATE
{
    LOG_DUMP_FLASH_STATE_IDLE,
    LOG_DUMP_FLASH_STATE_ERASED,
    LOG_DUMP_FLASH_STATE_WRITTING,
    LOG_DUMP_FLASH_STATE_WRITTEN,
};

typedef struct
{
    uint32_t magic;
    uint32_t version;
    uint32_t seqnum;
    uint8_t is_bootup;
    uint8_t reseved[3];
}LOG_DUMP_HEADER;


typedef struct{
    uint32_t state;
    uint32_t offset;
    uint8_t buffer[LOG_DUMP_BUFFER_LEN];
}DATA_BUFFER;

void log_dump_init(void);
int log_dump_flush(void);
void log_dump_notify_handler(enum HAL_TRACE_STATE_T state);
void log_dump_output_handler(const unsigned char *buf, unsigned int buf_len);
void log_dump_callback(void* param);
void log_dump_clear(void);

//uint32_t test_log_dump_from_flash(uint32_t addr,uint32_t size);
#endif
#ifdef __cplusplus
}
#endif
#endif


