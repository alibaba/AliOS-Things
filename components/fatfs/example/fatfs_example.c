#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include <aos/vfs.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>


static void fatfs_comp_example(int argc, char **argv)
{
    char buff[512] = {0};
    int ret=0;
    printf("fatfs component example start !\r\n");

    int fd1 = aos_open("/sdcard/test5.txt",  O_CREAT | O_RDWR | O_APPEND);
    if(fd1 > 0){
        printf("aos_open /sdcard/test5.txt fd1=%d\n", fd1);
    }else{
        printf("fatfs comp test fail! aos_open ret:%d\r\n", fd1);
        return;
    }
   
    ret = aos_write(fd1, "hello world1\n", 13);
    if(ret > 0){
        printf("aos_write /sdcard/test5.txt num=%d\n", ret);
    }else{
        printf("fatfs comp test fail! aos_write ret:%d\r\n", fd1);
        return;
    }
    
    ret = aos_sync(fd1);
    if(ret){
        printf("fatfs comp test fail! aos_sync ret:%d\r\n", ret);
        return;
    }

    int fd2 = aos_open("/sdcard/test5.txt",  O_RDWR);
    if(fd2 > 0){
        printf("aos_open /sdcard/test5.txt fd2=%d\n", fd2);
    }else{
        printf("fatfs comp test fail! aos_open ret:%d\r\n", fd2);
        return;
    }
    ret =  aos_lseek(fd2, 12, 0);
    if(ret>0){
        printf("aos_lseek /sdcard/test5.txt position=%d\n", ret);
    }else{
        printf("fatfs comp test fail! aos_lseek ret:%d\r\n", ret);
        return;
    }

    ret = aos_read(fd2, buff, sizeof(buff) - 1);
    if (ret > 0) {
        printf("aos_read: %s\n", buff);
    }else{
        printf("fatfs comp test fail! aos_read ret:%d\r\n", ret);
        return;
    }

    aos_close(fd1);
    aos_close(fd2);

    aos_dir_t *dir = aos_opendir("/sdcard");

    aos_dirent_t *dp = NULL;
    do {
        dp = aos_readdir(dir);
        if (dp)
            printf("readdir: %s\n", dp->d_name);
    } while (dp != NULL);

    aos_closedir(dir);
   
    printf("fatfs component example end !\r\n");
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(fatfs_comp_example, fatfs_example, fatfs component base example)
#endif
