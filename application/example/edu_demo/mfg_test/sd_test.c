/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>
#include "aos/cli.h"
#define SDTEST_FILE_NAME "/sdcard/sdtest.test"
#define SDTEST_FILE_LEN (1024*1024)
#define SDTEST_WORDS "sdtest\r\n"

static FILE *testfd = NULL;

static int sdtest_write(void)
{
    int i = 0;
    int j = 0;
    int ret = 0;
    char *data = NULL;
    int readbytes  = 0;
    int writebytes = 0;
    int words_len = strlen(SDTEST_WORDS);

    printf("\n====sd write test start ... === \n");

    testfd = fopen(SDTEST_FILE_NAME, "w");
    if(testfd == NULL) {
        printf("open /sdcard/test fail !!!\n");
        printf("====Result: sd write test FAIL !!!=== \n");
        return -1;
    }

    printf("begin write zero test\n");
    data = (char *) malloc(words_len);
    for(i = 0; i < SDTEST_FILE_LEN / words_len; i ++) {
        memset(data, 0, words_len);
        writebytes = fwrite(data, 1, words_len, testfd);
        if(writebytes != words_len) {
            printf("sdtest write %d bytes to %s failed, return %d\n", words_len, SDTEST_FILE_NAME, writebytes);
            printf("====Result: sd write test FAIL !!!=== \n");
            free(data);
            fclose(testfd);
            return -1;
        }
    }
    fclose(testfd);
    free(data);

    testfd = fopen(SDTEST_FILE_NAME, "r");
    if(testfd == NULL) {
        printf("open /sdcard/test fail !!!\n");
        printf("====Result: sd write test FAIL !!!=== \n");
        return -1;
    }

    data = (char *) malloc(SDTEST_FILE_LEN);
    memset(data, 0, SDTEST_FILE_LEN);
    readbytes = fread(data, 1, SDTEST_FILE_LEN, testfd);
    if(readbytes != SDTEST_FILE_LEN) {
        printf("sdtest read %d bytes form %s failed, return %d\n", SDTEST_FILE_LEN, SDTEST_FILE_NAME, readbytes);
        printf("====Result: sd write test FAIL !!!=== \n");
        free(data);
        fclose(testfd);
        return -1;
    }

    for(i = 0; i < SDTEST_FILE_LEN; i++) {
        if(data[i] != 0) {
            printf("sdtest read %d bytes form %s failed, data[%d] %d\n", SDTEST_FILE_LEN, SDTEST_FILE_NAME, i, (int)data[i]);
            printf("====Result: sd write test FAIL !!!=== \n");
            free(data);
            fclose(testfd);
            return -1;
        }
    }
    fclose(testfd);

    testfd = fopen(SDTEST_FILE_NAME, "w");
    if(testfd == NULL) {
        printf("open /sdcard/test fail !!!\n");
        printf("====Result: sd write test FAIL !!!=== \n");
        free(data);
        return -1;
    }

    for (i = 0; i < SDTEST_FILE_LEN / strlen(SDTEST_WORDS); i++) {
        memcpy(data + i * strlen(SDTEST_WORDS), SDTEST_WORDS, strlen(SDTEST_WORDS));
    }

    printf("begin write data test\n");
    writebytes = fwrite(data, 1, SDTEST_FILE_LEN, testfd);
    if(writebytes != SDTEST_FILE_LEN) {
        printf("write data SDTEST_FILE_LEN bytes to file /sdcard/test fail !!!\n");
        printf("====Result: sd write test FAIL !!!=== \n");
        free(data);
        fclose(testfd);
        return -1;
    }

    printf("====Result: sd write test PASS !!!=== \n");
    free(data);
    fclose(testfd);

    return 0;
}

static int sdtest_read(void)
{
    int i = 0;
    int j = 0;
    int ret = 0;
    char *data = NULL;
    int readbytes = 0;
    int words_len = strlen(SDTEST_WORDS);

    printf("\r\n====sd read test start ... === \n");

    testfd = fopen(SDTEST_FILE_NAME, "r");
    if(testfd == NULL) {
        printf("open /sdcard/test fail !!!\n");
        printf("====Result: sd read test FAIL !!!=== \n");
        return -1;
    }

    data = (char *) malloc(words_len);
    for(i = 0; i < SDTEST_FILE_LEN / words_len; i ++) {
        memset(data, 0, words_len);
        readbytes = fread(data, 1, words_len, testfd);
        if(readbytes != words_len) {
            printf("sdtest read %d bytes form %s failed, return %d\n", words_len, SDTEST_FILE_NAME, readbytes);
            ret = -1;
            goto test_err;
        }

        if(0 != strncmp(data, SDTEST_WORDS, words_len)) {
            printf("sdtest read %d bytes form %s, data error\n", words_len, SDTEST_FILE_NAME);
            printf("read ");
            for(j = 0; j < words_len; j++) {
                printf("0x%x", (uint32_t)data[j]);
            }
            printf(", ");
            printf("actual %s\n", SDTEST_WORDS);
            ret = -1;
            goto test_err;
        }
    }

    printf("====Result: sd read test PASS !!!=== \n");
test_err:
    free(data);
    fclose(testfd);
    if (ret != 0) {
        printf("====Result: sd read test FAIL !!!=== \n\n");
    }
    return ret;
}

static void handle_sdtest_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: sdtest read/write \n");
        return;
    }

    if (strcmp(argv[1], "read") == 0) {
        sdtest_read();
    } else if (strcmp(argv[1], "write") == 0) {
        sdtest_write();
    } else {
        printf("Usage: sdtest read/write \n");
        return;
    }
}

static struct cli_command sdtest_cmd = {
    .name     = "sdtest",
    .help     = "sdtest [read/write]",
    .function = handle_sdtest_cmd
};

int sdcard_test(void)
{
    int      i = 0;
    int    ret = 0;
    char *data = NULL;

    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("*********************** sdcard Test ***************************\r\n");
    printf("** How to test: pls format sdcard and insert it to socket *****\r\n");
    printf("***************************************************************\r\n");
    printf("=====sdcard test : Start=====\r\n");

    aos_cli_register_command(&sdtest_cmd);

    aos_unlink(SDTEST_FILE_NAME);
    aos_rmdir(SDTEST_FILE_NAME);

    testfd = fopen(SDTEST_FILE_NAME, "w");
    if(testfd == NULL) {
        printf("create /sdcard/sdtest.test fail !!!\n");
        return;
    }

    printf("create /sdcard/sdtest.test success !!!\n");
    data = (char *)malloc(SDTEST_FILE_LEN);
    for (i = 0; i < SDTEST_FILE_LEN / strlen(SDTEST_WORDS); i++) {
        memcpy(data + i * strlen(SDTEST_WORDS), SDTEST_WORDS, strlen(SDTEST_WORDS));
    }

    printf("begin init /sdcard/sdtest.test ... \n");
    ret = fwrite(data, 1, SDTEST_FILE_LEN, testfd);
    if(ret != SDTEST_FILE_LEN) {
        printf("write data SDTEST_FILE_LEN bytes to file /sdcard/sdtest.test fail !!!\n");
    }
    fsync(testfd);

    printf("write init data to /sdcard/sdtest.test success !!!\n");
    fclose(testfd);
    free(data);

    aos_msleep(1000);

    ret = sdtest_read();
    ret += sdtest_write();

    aos_unlink(SDTEST_FILE_NAME);

    return ret;
}