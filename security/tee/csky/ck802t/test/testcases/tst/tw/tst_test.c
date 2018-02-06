/**
 * Copyright (C) 2017 The YunOS Project. All rights reserved.
 */

#include "tee_tos.h"
#include "tee_tst.h"
#include "tee_dbg.h"

#define CONFIG_TST_DBG 1

#if CONFIG_TST_DBG
#define TST_DBG_E(_f, _a ...)  tee_dbg_print(ERR, _f, ##_a)
#define TST_DBG_I(_f, _a ...)  tee_dbg_print(INF, _f, ##_a)
#else
#define TST_DBG_E(_f, _a ...)  TEE_ERROR(-1)
#define TST_DBG_I(_f, _a ...)
#endif

#define TST_MALLOC      malloc
#define TST_FREE        free
#define TST_MEMCPY      memcpy
#define TST_MEMSET      memset
#define TST_MEMCMP      memcmp
#define TST_STRLEN      strlen

static uint8_t _g_data[] = {
0x12, 0x34, 0x56, 0x78,
0x78, 0x56, 0x34, 0x12};

int32_t tee_tst_test(void)
{
    int32_t fd;
    int32_t flags;
    ssize_t  size;
    uint8_t data[8];

    flags = TOF_CREAT | TOF_READ | TOF_WRITE;
    fd = tee_open("tst_test", flags);
    if (-1 == fd) {
        TST_DBG_E("open fail\n");
        return -1;
    }

    size = tee_write(fd, _g_data, sizeof(_g_data));
    if (size != sizeof(_g_data)) {
        TST_DBG_E("write fail\n");
        tee_close(fd);
        return -1;
    }

    size = tee_lseek(fd, 0, TEE_SEEK_SET);
    if (0 != size) {
        TST_DBG_E("lseek fail\n");
        tee_close(fd);
        return -1;
    }

    size = tee_read(fd, data, sizeof(_g_data));
    if (size != sizeof(_g_data)) {
        TST_DBG_E("write fail\n");
        tee_close(fd);
        return -1;
    }

    if ( 0 != TST_MEMCMP(data, _g_data, 8)) {
        return -1;
    }

    return 0;
}
