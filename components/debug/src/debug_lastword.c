/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "debug_api.h"

#define MIN(x,y) (((x)<(y))?(x):(y))

#if DEBUG_LAST_WORD_ENABLE
#ifdef CONFIG_VFS_LSOPEN
static void debug_paniclog_to_file(void);
#endif
static int  panic_header_init(debug_panic_info_head_t *panic_header);
static int  panic_header_show(debug_panic_info_head_t *panic_header);
static int  panic_header_set(debug_panic_info_head_t *panic_header);
static int  panic_header_get(debug_panic_info_head_t *panic_header);
static int  panic_log_set(uint8_t *info, uint32_t len);
static int  panic_log_get(uint8_t *info_buffer, uint32_t len);
static void panic_log_clean(void);

__attribute__((weak))  int clear_silent_reboot_flag(void)
{
    return 0;
}

__attribute__((weak))  int set_silent_reboot_flag(void)
{
    return 0;
}

__attribute__((weak))  void k_dcache_clean(uint32_t addr, uint32_t len)
{
    return;
}

/* set ram attributes for some mcu*/
__attribute__((weak)) void alios_debug_lastword_init_hook()
{
    return;
}

#define HEADER_START_ADDR (DEBUG_LASTWORD_RAM_ADDR)
#define LOG_START_ADDR    (DEBUG_LASTWORD_RAM_ADDR   + sizeof(debug_panic_info_head_t))
#define LOG_REGION_LEN    (DEBUG_LASTWORD_REGION_LEN - sizeof(debug_panic_info_head_t))
#define LOG_END_ADDR      (DEBUG_LASTWORD_RAM_ADDR   + DEBUG_LASTWORD_REGION_LEN)
#define CRC_CALC_LEN      (uint32_t)(&(((debug_panic_info_head_t *)0)->crc16))

static debug_panic_info_head_t panic_header_buffer;

#define POLY        0x1021

uint16_t crc16_calc(uint8_t *addr, uint32_t num, uint16_t crc)
{
    int i;
    for (; num > 0; num--) {
        crc = crc ^ (*addr++ << 8);
        for (i = 0; i < 8; i++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ POLY;
            } else {
                crc <<= 1;
            }
        }
        crc &= 0xFFFF;
    }
    return (crc);
}

void debug_lastword_init(void)
{
    printf("init lastword\r\n");

#ifdef CONFIG_VFS_LSOPEN
    mkdir(DEBUG_LOG_DIR_NAME, 0777);
#endif

    panic_header_get(&panic_header_buffer);

    if (crc16_calc((uint8_t *)&panic_header_buffer, CRC_CALC_LEN, 0xffff) != panic_header_buffer.crc16) {
        panic_header_init(&panic_header_buffer);
        panic_header_set(&panic_header_buffer);
        panic_log_clean();
        printf("[Error] lastword: init crc fail!\r\n");
        return;
    }

    printf("lastword: CRC check is OK\r\n");
#ifdef CONFIG_VFS_LSOPEN
    if (panic_header_buffer.log_magic == DEBUG_PANIC_LOG_MAGIC) {
        char path[64];
        int fd = -1;
        printf("lastword: try to read lastword to file!\r\n");
        /* open /data/smartbox_abort as a flag for panic */
        memset(path, 0, sizeof(path));
        snprintf(path, sizeof(path) - 1, "/data/smartbox_abort");
        fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0666);
        if (fd < 0) {
            printf("open panic flag file fail\r\n");
        } else {
            printf("open panic flag file fd %d\r\n", fd);
            close(fd);
        }
        printf("store panic log to file\r\n");
        debug_paniclog_to_file();
    }
#endif
    panic_log_clean();
    panic_header_buffer.log_magic = 0;

    if (panic_header_buffer.header_magic != DEBUG_PANIC_HEADER_MAGIC) {
        panic_header_init(&panic_header_buffer);
    } else {
        panic_header_buffer.reboot_sum_count++;
    }

    panic_header_show(&panic_header_buffer);
    panic_header_set(&panic_header_buffer);
    clear_silent_reboot_flag();

    alios_debug_lastword_init_hook();
}

void debug_reboot_reason_update(unsigned int reason)
{
    int ret = -1;
    static int reason_id_update_flag = 0;

    panic_header_get(&panic_header_buffer);

    if (reason_id_update_flag == 0) {
        panic_header_buffer.reboot_reason_id++;
        reason_id_update_flag = 1;
    }

    panic_header_buffer.reboot_reason = reason;

    if ((DEBUG_REBOOT_REASON_PANIC == reason) || (DEBUG_REBOOT_REASON_FATAL_ERR == reason)) {
        set_silent_reboot_flag();

        panic_header_buffer.runtime_before_painc[panic_header_buffer.runtime_record_id] = krhino_ticks_to_ms(
                            krhino_sys_tick_get());
        panic_header_buffer.runtime_record_id++;
        if (panic_header_buffer.runtime_record_id >= RUNTIME_COUNT) {
            panic_header_buffer.runtime_record_id = 0;
        }
        panic_header_buffer.panic_count++;
    }

    ret = panic_header_set(&panic_header_buffer);
    if (ret) {
        print_str("reboot reason set err\n");
    }
}

unsigned int debug_reboot_reason_get()
{
    panic_header_get(&panic_header_buffer);

    if (panic_header_buffer.header_magic != DEBUG_PANIC_HEADER_MAGIC) {
        panic_header_init(&panic_header_buffer);
        panic_header_set(&panic_header_buffer);
        return DEFAULT_REBOOT_REASON;
    }

    if (panic_header_buffer.reboot_sum_count > panic_header_buffer.reboot_reason_id) {
        panic_header_buffer.reboot_reason_id = panic_header_buffer.reboot_sum_count;
        panic_header_buffer.reboot_reason    = DEBUG_REBOOT_UNKNOWN_REASON;
        panic_header_set(&panic_header_buffer);
        return DEBUG_REBOOT_UNKNOWN_REASON;
    }

    return panic_header_buffer.reboot_reason;
}

int64_t debug_get_painc_runtime(int panic_count, int *real_panic_count)
{
    int32_t idx = 0, count = 0;
    int64_t duration = 0;

    panic_header_get(&panic_header_buffer);

    if (panic_header_buffer.header_magic != DEBUG_PANIC_HEADER_MAGIC) {
        panic_header_init(&panic_header_buffer);
        panic_header_set(&panic_header_buffer);
        *real_panic_count = 0;
        return -1;
    }

    if (panic_header_buffer.panic_count == 0) {
        *real_panic_count = 0;
        return -1;
    }

    if (panic_count > RUNTIME_COUNT) {
        printf("panic count is larger than %d\r\n", RUNTIME_COUNT);
    }

    idx = panic_header_buffer.runtime_record_id;

    for (count = 0; count < RUNTIME_COUNT; count++) {
        if (count >= panic_count) {
            break;
        }

        idx--;
        if (idx < 0) {
            idx = RUNTIME_COUNT - 1;
        }
        if (panic_header_buffer.runtime_before_painc[idx] != -1) {
            duration += panic_header_buffer.runtime_before_painc[idx];
        } else {
            break;
        }
    }

    *real_panic_count = count;
    return duration;
}

#ifdef CONFIG_VFS_LSOPEN
static void debug_paniclog_to_file(void)
{
    int  i  = 0, index = 0;
    int  fd = -1, indexfile_fd = -1;
    char path[64];
    char info_buffer[128], index_buffer[10];
    int  len;

    if (panic_header_buffer.log_magic != DEBUG_PANIC_LOG_MAGIC) {
        return;
    }

    for (i = 0; i < DEBUG_LOG_FILE_NUM; i++) {
        memset(path, 0, sizeof(path));
        snprintf(path, sizeof(path), "%s_%02d", DEBUG_LOG_FILE_NAME, i);
        /* if file not exit, create it, and output debug info to it */
        if (access(path, R_OK) != 0) {
            fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0666);
            if (fd >= 0) {
                break;
            }
        }
    }

    if (i == DEBUG_LOG_FILE_NUM) {
        if (access(DEBUG_LOG_FILE_INDEX_NAME, R_OK) != 0) {
            indexfile_fd  = open(DEBUG_LOG_FILE_INDEX_NAME, O_CREAT | O_WRONLY | O_TRUNC, 0666);
            index = 0;
        } else {
            indexfile_fd  = open(DEBUG_LOG_FILE_INDEX_NAME, O_CREAT | O_RDWR, 0666);
            read(indexfile_fd, index_buffer, sizeof(index_buffer));
            sscanf(index_buffer, "%02d", &index);

            if ((index < 0) || (index >= DEBUG_LOG_FILE_NUM)) {
                index = 0;
            } else {
                index++;
                if (index >= DEBUG_LOG_FILE_NUM) {
                    index = 0;
                }
            }
            lseek(indexfile_fd, SEEK_SET, 0);
        }

        if (indexfile_fd >= 0) {
            memset(index_buffer, 0, sizeof(index_buffer));
            len = snprintf(index_buffer, sizeof(index_buffer), "%02d", index);
            write(indexfile_fd, index_buffer, len);
            close(indexfile_fd);
        }

        memset(path, 0, sizeof(path));
        snprintf(path, sizeof(path), "%s_%02d", DEBUG_LOG_FILE_NAME, index);
        fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    }

    if (fd < 0) {
        printf("open file %s error, %s, %d\r\n", path, __FILE__, __LINE__);
        return;
    }

    while (1) {
        len = sizeof(info_buffer);
        len = panic_log_get((uint8_t *)info_buffer, len);
        if (len <= 0) {
            break;
        }
        len = write(fd, info_buffer, len);
        if (len < 0) {
            printf("write file %s error, %s, %d\r\n", path, __FILE__, __LINE__);
            //break;
        }
    }

    close(fd);
    fd = -1;

    return;
}
#endif

static int panic_header_init(debug_panic_info_head_t *panic_header)
{
    uint32_t i;
    panic_header->header_magic      = DEBUG_PANIC_HEADER_MAGIC;
    panic_header->log_magic         = 0;
    panic_header->reboot_reason     = DEFAULT_REBOOT_REASON;
    panic_header->reboot_sum_count  = 0;
    panic_header->reboot_reason_id  = 0;
    panic_header->panic_count       = 0;
    panic_header->runtime_record_id = 0;

    for (i = 0; i < RUNTIME_COUNT; i++) {
        panic_header->runtime_before_painc[i] = -1;
    }
    return 0;
}

static int panic_header_show(debug_panic_info_head_t *panic_header)
{
    printf("===========show panic header===========\r\n");
    printf("panic header reboot reason %d\r\n", (int)panic_header->reboot_reason);
    printf("panic header reboot sum count %d\r\n", (int)panic_header->reboot_sum_count);
    printf("panic header reboot reason reason id %d\r\n", (int)panic_header->reboot_reason_id);
    printf("panic header panic  count %d\r\n", (int)panic_header->panic_count);
    printf("===========end===========\r\n");
    return 0;
}

static int panic_header_set(debug_panic_info_head_t *panic_header)
{
    if (panic_header == NULL) {
        return -1;
    }

    debug_panic_info_head_t *log_header = (debug_panic_info_head_t *)HEADER_START_ADDR;
    /* calculate the crc */
    panic_header->crc16 = crc16_calc((uint8_t *)panic_header, CRC_CALC_LEN, 0xffff);

    memcpy(log_header, panic_header, sizeof(debug_panic_info_head_t));
    k_dcache_clean((uintptr_t)log_header, sizeof(debug_panic_info_head_t));

    return 0;
}

static int panic_header_get(debug_panic_info_head_t *panic_header)
{
    if (panic_header == NULL) {
        return -1;
    }

    debug_panic_info_head_t *log_header = (debug_panic_info_head_t *)HEADER_START_ADDR;
    memcpy(panic_header, log_header, sizeof(debug_panic_info_head_t));

    return 0;
}

static int panic_log_set(uint8_t *info, uint32_t len)
{
    if ((info == NULL) || (len == 0)) {
        return -1;
    }

    static uint8_t *log_ram_addr_cur = (uint8_t *)LOG_START_ADDR;

    if ((uintptr_t)(len + log_ram_addr_cur) <= (uintptr_t)LOG_END_ADDR) {
        memcpy(log_ram_addr_cur, info, len);
        k_dcache_clean((uintptr_t)log_ram_addr_cur, len);
        log_ram_addr_cur += len;
        return 0;
    }

    return -1;
}

static int panic_log_get(uint8_t *info_buffer, uint32_t len)
{
    if (info_buffer == NULL) {
        return -1;
    }

    static uint8_t *log_ram_addr_cur = (uint8_t *)LOG_START_ADDR;
    int32_t         len_tmp;

    if (log_ram_addr_cur >= (uint8_t *)LOG_END_ADDR) {
        return -1;
    }

    len = MIN(len, (uint8_t *)LOG_END_ADDR - log_ram_addr_cur);
    memcpy(info_buffer, log_ram_addr_cur, len);
    log_ram_addr_cur += len;

    len_tmp = len - 1;
    while (len_tmp >= 0) {
        if (info_buffer[len_tmp] != 0) {
            break;
        }
        len--;
        len_tmp--;
    }

    return len;
}

static void panic_log_clean(void)
{
    static uint8_t *log_ram_addr_cur = (uint8_t *)LOG_START_ADDR;
    memset(log_ram_addr_cur, 0, LOG_REGION_LEN);
}

int print_str(const char *fmt, ...)
{
    int ret = -1;
    char strbuf[400];

    va_list args;

    va_start(args, fmt);
    ret = vsnprintf((char *)strbuf, sizeof(strbuf) - 1, fmt, args);
    va_end(args);

    if (ret > 0) {
        panic_log_set((uint8_t *)strbuf, ret);
        if (panic_header_buffer.log_magic != DEBUG_PANIC_LOG_MAGIC) {
            panic_header_buffer.log_magic = DEBUG_PANIC_LOG_MAGIC;
            panic_header_set(&panic_header_buffer);
        }
    }

    return ret;
}

int vprint_str(const char *fmt, va_list ap)
{
    int ret = -1;
    char strbuf[400];

    ret = vsnprintf((char *)strbuf, sizeof(strbuf) - 1, fmt, ap);

    if (ret > 0) {
        panic_log_set((uint8_t *)strbuf, ret);
        if (panic_header_buffer.log_magic != DEBUG_PANIC_LOG_MAGIC) {
            panic_header_buffer.log_magic = DEBUG_PANIC_LOG_MAGIC;
            panic_header_set(&panic_header_buffer);
        }
    }

    return ret;
}

#endif

