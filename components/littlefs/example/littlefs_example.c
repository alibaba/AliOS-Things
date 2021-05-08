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


#define LFS_MNT_PATH "/data"
#define TEST_FILE LFS_MNT_PATH"/demo"
#define TEST_CONTENT "haas100 littlefs demo\n"
void read_write()
{
    int buff[128] = {0};
    int ret=0;
    int fd1, fd2;
    printf("littlefs component example start !\r\n");

    fd1 = aos_open(TEST_FILE,  O_CREAT | O_RDWR | O_APPEND);
    if(fd1 > 0){
        printf("aos_open fd1=%d\n", fd1);
    }else{
        printf("littlefs comp test fail! aos_open ret:%d\r\n", fd1);
        goto Exit;
    }
   
    ret = aos_write(fd1, "haas100 littlefs demo\n", 22);
    if(ret > 0){
        printf("aos_write num=%d\n", ret);
    }else{
        printf("littlefs comp test fail! aos_write ret:%d\r\n", fd1);
        goto Exit;
    }
    
    ret = aos_sync(fd1);
    if(ret){
        printf("littlefs comp test fail! aos_sync ret:%d\r\n", ret);
        goto Exit;
    }

    fd2 = aos_open(TEST_FILE, O_RDWR);
    if(fd2 > 0){
        printf("aos_open fd2=%d\n", fd2);
    }else{
        printf("littlefs comp test fail! aos_open ret:%d\r\n", fd2);
        goto Exit;
    }
    ret =  aos_lseek(fd2, 21, 0);
    if(ret>0){
        printf("aos_lseek position=%d\n", ret);
    }else{
        printf("littlefs comp test fail! aos_lseek ret:%d\r\n", ret);
        goto Exit;
    }

    ret = aos_read(fd2, buff, 128);
    if (ret > 0) {
        printf("aos_read: %s\n", buff);
    }else{
        printf("littlefs comp test fail! aos_read ret:%d\r\n", ret);
        goto Exit;
    }

Exit:
    if(fd1>0)
    {
        aos_close(fd1);
    }
    if(fd2>0)
    {
        aos_close(fd2);
    }
   
}

void read_dir()
{
    aos_dir_t *dir = aos_opendir(LFS_MNT_PATH);

    aos_dirent_t *dp = NULL;
    do {
        dp = aos_readdir(dir);
        if (dp)
            printf("readdir: %s\n", dp->d_name);
    } while (dp != NULL);

    aos_closedir(dir);
}


static void littlefs_comp_example(int argc, char **argv)
{
    printf("littlefs component example start !\r\n");
    read_write();
    read_dir();
    printf("littlefs component example end !\r\n");
}


#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(littlefs_comp_example, littlefs_example, littlefs component base example)
#endif