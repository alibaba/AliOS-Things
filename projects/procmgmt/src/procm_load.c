/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "aos/cli.h"
#include "u_api.h"
#include <procmgmt.h>
#include "aos/kernel.h"

#include <procmgmt.h>
#include "process_service.h"

#ifdef AOS_PROC_LOAD_FROM_USAPCE
extern int32_t aos_uproc_load(const char *file);
extern int krhino_process_ramload(const char *ram_addr);
static void loadm_app_cmd(char *pbuffer, int outlen, int argc, char **argv)
{
    int ret;

    if (argc != 2) {
        aos_cli_printf("Usage: load app_name\r\n");
        return;
    }

    ret = aos_uproc_load(argv[1]);

    if (ret > 0) {
        aos_cli_printf("load %s success\r\n", argv[1]);
    } else {
        aos_cli_printf("load %s failed\r\n", argv[1]);
    }
}

static const struct cli_command loadm_app_builtin_cmds[] = {
    { "loadm", "load app from ram and vfs in procm pro", loadm_app_cmd},
};

void uproc_loadm_cli_cmds_register(void)
{
    aos_cli_register_commands(loadm_app_builtin_cmds,
                          sizeof(loadm_app_builtin_cmds) / sizeof(loadm_app_builtin_cmds[0]));
}

static void *uproc_file2ram(const char *path)
{
    int   file_len;
    int   read_count;
    int   fd;
    void *pbuff;
    off_t off = 0;

    fd = aos_open(path, O_RDONLY);
    if (fd < 0) {
        printf("aos_open failed\r\n");
        return NULL;
    }

    file_len = aos_lseek(fd, off, SEEK_END);
    aos_lseek(fd, off, SEEK_SET);

    if (file_len <= 0) {
        aos_close(fd);
        return NULL;
    }

    pbuff = malloc(file_len);
    if (pbuff == NULL) {
        aos_close(fd);
        return NULL;
    }

    read_count = aos_read(fd, pbuff, file_len);
    aos_close(fd);
    if (read_count != file_len) {
        free(pbuff);
        return NULL;
    }

    fd = -1;
    return pbuff;
}

int32_t aos_uproc_load(const char *file)
{
    int pid;
    char *ram_addr;
    ram_addr = uproc_file2ram(file);
    if(NULL == ram_addr){
        printf("Error:aos_uproc_load call uproc_file2ram failed\r\n");
        return -1;
    }
    pid = krhino_process_ramload(ram_addr);
    free(ram_addr);

    if (pid > 0) {
        proc_map_add(file, pid);
    }

    return pid;
}

#endif