/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <aos/kernel.h>
#include <aos/cli.h>
#include <aos/mtd.h>

static void mtd_unit_test(int argc, char *argv[])
{
    ssize_t count;
    aos_mtd_t *mtd;
    aos_status_t ret;
    char buf[64] = "mtd test.";
    int buf_test_len = strlen(buf);

    mtd = aos_mtd_open(MTD_PART_NAME_KERNEL2);
    if (mtd == NULL) {
        printf("%s:%d: aos_mtd_open failed.\n", __func__, __LINE__);
        return;
    }

    ret = aos_mtd_erase(mtd, 0, strlen("mtd test."));
    if (ret < 0) {
        printf("%s:%d: aos_mtd_erase failed ret: %d\n", __func__, __LINE__, ret);
        aos_mtd_close(mtd);
        return;
    }

    count = aos_mtd_write(mtd, 0, buf, strlen(buf));
    if (count < 0) {
        printf("%s:%d: aos_mtd_write failed ret: %d\n", __func__, __LINE__, count);
        aos_mtd_close(mtd);
        return;
    }
    printf("%s:%d:buf\"%s\" was successfully written.\n", __func__, __LINE__, buf);

    memset(buf, 0, sizeof(buf));
    count = aos_mtd_read(mtd, 0, buf, buf_test_len);
    if (count < 0) {
        printf("%s:%d: aos_mtd_read failed ret: %d\n", __func__, __LINE__, count);
        aos_mtd_close(mtd);
        return;
    }
    printf("%s:%d:buf\"%s\" was successfully read.\n", __func__, __LINE__, buf);

    if (strcmp(buf, "mtd test.") != 0) {
        printf("%s:%d:buf:%s aos_mtd_test failed!\n", __func__, __LINE__, buf);
    } else {
        printf("%s:%d: aos_mtd_test success!\n", __func__, __LINE__);
    }

    aos_mtd_close(mtd);
    return;
}

#ifdef AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(mtd_unit_test, mtd_example, aos mtd example)
#endif