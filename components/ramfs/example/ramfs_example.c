/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include "ramfs.h"

static void ramfs_example_fn(int argc, char **argv)
{
    (void)argc;
    (void)argv;
	int fd;
    int ret;
    char teststring = "1234";
    char readbuf[10];

    ramfs_register("/test");
    fd = open("/test/file1", O_RDWR);
    if(fd < 0){
        printf("ramfs: fd open fail!\r\n");
        return;
    }
    ret = write(fd, teststring, 5);
    if(ret < 0){
        printf("ramfs: fd write fail!\r\n");
        close(fd);
        return;
    }
    lseek(fd, 0, SEEK_SET);
    ret = read(fd, readbuf, 5);
    if(ret < 0){
        printf("ramfs: fd read fail!\r\n");
        close(fd);
        return;
    }
    if(strncmp(readbuf, teststring, 5)){
        printf("ramfs: fd test fail! readbuf:%s\r\n",readbuf);
    }else{
        printf("ramfs: fd test success!\r\n");
    }
    close(fd);

    printf("ramfs comp test success!\r\n");
    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(ramfs_example_fn, ramfs_example, ramfs component base example)
#endif
