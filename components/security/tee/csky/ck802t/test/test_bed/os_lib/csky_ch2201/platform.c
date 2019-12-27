/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <uart.h>
#include <log.h>

extern void CK_INTC_Init(void);
extern void CK_Exception_Init(void);
extern void ck_uart_putchar(uint8_t ch);

int putch(char c)
{
    ck_uart_putchar(c);
    return 0;
}

/*
 * initialize the device registered
 */
void CK_Platform_Init(void)
{

    CK_INTC_Init();
    CK_Exception_Init();
}

typedef enum _tst_stub_open_flag_t
{
    TSOF_CREAT = 0x00000001,
    TSOF_EXCL  = 0x00000002,
    TSOF_READ  = 0x00000004,
    TSOF_WRITE = 0x00000008,
} tst_stub_open_flag_t;

int32_t tst_stub_open(const int8_t *path, tst_stub_open_flag_t flags)
{
    return 0;
}

int32_t tst_stub_close(int32_t fd)
{
    return -1;
}

int32_t tst_stub_read(int32_t fd, void *buf, size_t count)
{
    return -1;
}

int32_t tst_stub_write(int32_t fd, const void *buf, size_t count)
{
    return -1;
}

uint32_t tst_stub_lseek(int32_t fd, int32_t off, int32_t whence)
{
    return -1;
}

int32_t tst_stub_unlink(const int8_t *path)
{
    return -1;
}

// vincent add
extern void wsc_entry(void);

__attribute__((__section__(".wsc.data")))
const void (*ebr_except_table[1])(void) = { wsc_entry };
//
