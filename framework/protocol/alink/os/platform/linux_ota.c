/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <assert.h>

#include "platform.h"

static FILE *fp;

#define otafilename "/tmp/alinkota.bin"
void platform_flash_program_start(void)
{
    fp = fopen(otafilename, "w");
    assert(fp);
    return;
}

int platform_flash_program_write_block(_IN_ char *buffer, _IN_ uint32_t length)
{
    unsigned int written_len = 0;
    written_len = fwrite(buffer, 1, length, fp);

    if (written_len != length) {
        return -1;
    }
    return 0;
}

int platform_flash_program_stop(void)
{
    if (fp != NULL) {
        fclose(fp);
    }

    /* check file md5, and burning it to flash ... finally reboot system */

    return 0;
}
