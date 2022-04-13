/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef __SYS_API_PROGRAMMER_H__
#define __SYS_API_PROGRAMMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tool_msg.h"

#ifdef _WIN32
#include "stdio.h"

#ifndef TRACE
#define TRACE(str, ...)                 do { printf("[%u] %s/" str "\n", get_current_time(), __FUNCTION__, __VA_ARGS__); } while (0)
#endif
#ifndef ASSERT
#define ASSERT(cond, str, ...)          \
    do { if (!(cond)) { printf("[%u] [ASSERT] %s/" str "\n", get_current_time(), __FUNCTION__, __VA_ARGS__); while (1); } } while (0)
#endif
#ifndef TRACE_TIME
#define TRACE_TIME(str, ...)            TRACE(str, __VA_ARGS__)
#endif
#define CUST_CMD_INIT_TBL_LOC
#define CUST_CMD_HDLR_TBL_LOC

int write_flash_data(const unsigned char *data, unsigned int len);
#else
#include "hal_trace.h"
#include "hal_timer.h"

#define TRACE_TIME(str, ...)            TRACE("[%05u] " str, TICKS_TO_MS(hal_sys_timer_get()), ##__VA_ARGS__)
#define CUST_CMD_INIT_TBL_LOC           __attribute__((section(".cust_cmd_init_tbl"), used))
#define CUST_CMD_HDLR_TBL_LOC           __attribute__((section(".cust_cmd_hldr_tbl"), used))
#endif

enum PGM_DOWNLOAD_TIMEOUT {
    PGM_TIMEOUT_RECV_SHORT,
    PGM_TIMEOUT_RECV_IDLE,
    PGM_TIMEOUT_RECV_4K_DATA,
    PGM_TIMEOUT_SEND,
    PGM_TIMEOUT_SYNC,
};

enum CUST_CMD_STAGE {
    CUST_CMD_STAGE_HEADER,
    CUST_CMD_STAGE_DATA,
    CUST_CMD_STAGE_EXTRA,

    CUST_CMD_STAGE_QTY
};

struct CUST_CMD_PARAM {
    enum CUST_CMD_STAGE stage;
    const struct message_t *msg;
    int extra;
    unsigned char *buf;
    size_t expect;
    size_t size;
    unsigned int timeout;
};

typedef void (*CUST_CMD_INIT_T)(void);
typedef enum ERR_CODE (*CUST_CMD_HANDLER_T)(struct CUST_CMD_PARAM *param);

void init_download_context(void);

void init_cust_cmd(void);
enum ERR_CODE handle_cust_cmd(struct CUST_CMD_PARAM *param);

void reset_transport(void);

void set_recv_timeout(unsigned int timeout);
void set_send_timeout(unsigned int timeout);

unsigned int get_pgm_timeout(enum PGM_DOWNLOAD_TIMEOUT timeout);
unsigned int get_current_time(void);

int send_data(const unsigned char *buf, size_t len);
int recv_data_ex(unsigned char *buf, size_t len, size_t expect, size_t *rlen);
int handle_error(void);
int cancel_input(void);

int debug_read_enabled(void);
int debug_write_enabled(void);

void system_reboot(void);
void system_shutdown(void);
void system_flash_boot(void);
void system_set_bootmode(unsigned int bootmode);
void system_clear_bootmode(unsigned int bootmode);
unsigned int system_get_bootmode(void);
int get_sector_info(unsigned int addr, unsigned int *sector_addr, unsigned int *sector_len);
int get_flash_id(unsigned char *buf, unsigned int len);
int get_flash_unique_id(unsigned char *buf, unsigned int len);
int erase_sector(unsigned int sector_addr, unsigned int sector_len);
int erase_chip(void);
int burn_data(unsigned int addr, const unsigned char *data, unsigned int len);
int verify_flash_data(unsigned int addr, const unsigned char *data, unsigned int len);

int enable_flash_remap(unsigned int addr, unsigned int len, unsigned int offset, int burn_both);
int disable_flash_remap(void);

int wait_data_buf_finished(void);
int wait_all_data_buf_finished(void);

int read_security_register(unsigned int addr, unsigned char *data, unsigned int len);
int erase_security_register(unsigned int addr, unsigned int len);
int lock_security_register(unsigned int addr, unsigned int reg_len);
int burn_security_register(unsigned int addr, const unsigned char *data, unsigned int len);
int verify_security_register_data(unsigned int addr, unsigned char *load_buf, const unsigned char *data, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif

