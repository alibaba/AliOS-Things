/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "aos/kernel.h"
#include "aos/errno.h"
#include "aos/vfs.h"
#include "aos/hal/uart.h"

#include <stdarg.h>
#include <stdio.h>

#ifdef AOS_BT
#include <zephyr/types.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include "hrs.h"
#include "bas.h"
#include "dis.h"
#include "ali_core.h"
#include <api_export.h>
#include "ais_ota.h"
#endif



extern int hci_driver_init();
extern uint32_t *fetch_ali_context();



void *sys_aos_malloc(unsigned int size, size_t allocator)
{
    void *tmp = aos_malloc(size);

    if (tmp == NULL) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    if ((size & AOS_UNSIGNED_INT_MSB) == 0) {
        krhino_owner_attach(tmp, allocator);
    }
#endif

    return tmp;
}

void *sys_aos_realloc(void *mem, unsigned int size, size_t allocator)
{
    void *tmp = aos_realloc(mem, size);

    if (tmp == NULL) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    if ((size & AOS_UNSIGNED_INT_MSB) == 0) {
        krhino_owner_attach(tmp, allocator);
    }
#endif

    return tmp;
}

void *sys_aos_zalloc(unsigned int size, size_t allocator)
{
    void *tmp = aos_zalloc(size);

    if (tmp == NULL) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    if ((size & AOS_UNSIGNED_INT_MSB) == 0) {
        krhino_owner_attach(tmp, allocator);
    }
#endif

    return tmp;
}

int get_errno(void)
{
    return errno;
}

void set_errno(int err)
{
    errno = err;
}

int aos_vprintf(char *format, va_list param)
{
    return vprintf(format, param);
}

int aos_fflush(FILE *stream)
{
    return fflush(stream);
}

#define SYSCALL_MAX 150
#define SYSCALL_NUM 150

#define SYSCALL(nr, func) [nr] = func,

const void *kmbins_tbl[] __attribute__ ((section(".kmbins_tbl"))) = {
    [0 ... SYSCALL_MAX - 1] = (void *)0XABCDABCD,
#include <kmbins_tbl.h>
    };

