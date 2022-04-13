/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "k_api.h"
#include "aos/cli.h"

#ifndef CONFIG_ASH_SCRIPT_FILE_NAME
    #define ASH_SCRIPT_FILE_NAME "/system/aos.d"
#else
    #define ASH_SCRIPT_FILE_NAME CONFIG_ASH_SCRIPT_FILE_NAME
#endif

#define CMD_ARGS_MAX_CNT (16)

extern int32_t proc_onecmd(int argc, char *argv[]);

static int interpret_script(const char *file_name)
{
    char *file_buf;
    char *file_end;
    char *p_buf;
    char *argv[CMD_ARGS_MAX_CNT];
    int   argc;
    int file_size;
    int fd;
    int ret;

    file_buf  = NULL;

    ret = access(file_name, F_OK);
    if (ret) {
        ret = -2;
        goto free_buf;
    }

    fd = open(file_name, O_RDONLY);
    if (fd < 0) {
        ret = -3;
        goto free_buf;
    }

    /* get file size */
    file_size = lseek(fd, 0, SEEK_END);
    if (file_size <= 0) {
        ret = -4;
        goto close_fd;
    }

    /* anchor file pos to the head */
    lseek(fd, 0, SEEK_SET);

    /**
     * 1. read the whole file into buffer
     * 2. parse each line and send valid cmd to CLI
     *
     * Note: the line is commented with #
     */

    file_buf = malloc(file_size + 1);
    if (file_buf == NULL) {
        ret = -5;
        goto close_fd;
    }

    ret = read(fd, file_buf, file_size);
    if (ret != file_size) {
        ret = -6;
        goto close_fd;
    }

    file_end = file_buf + file_size;
    p_buf    = file_buf;
    while (p_buf < file_end) {
        argc = 0;
        memset(argv, 0, sizeof(argv));
        /* skip leading space */
        while (*p_buf == ' ' || *p_buf == '\t') {
            p_buf++;
        }

        /* commnet line */
        if (*p_buf == '#') {
            while (*p_buf != '\n' && p_buf < file_end) {
                p_buf++;
            }
            continue;
        }

        if (*p_buf == '\n') {
            p_buf++;
            continue;
        }

        while (1) {
            argv[argc++] = p_buf;
            while (*p_buf != ' ' && *p_buf != '\t' && *p_buf != '\n' && p_buf < file_end) {
                p_buf++;
            }

            if (p_buf == file_end) {
                *p_buf = '\0';
                break;
            }

            /* replace space with '\0' */
            if (*p_buf == ' ' || *p_buf == '\t') {
                do {
                    *p_buf = '\0';
                    p_buf++;
                } while (*p_buf == ' ' || *p_buf == '\t');
                continue;
            } else {
                /* meet '\n', delete '\r' before '\n' and leading '\n' */
                char *p_temp = p_buf - 1;
                while (*p_temp == '\r') {
                    *p_temp = '\0';
                    p_temp--;
                }

                do {
                    *p_buf = '\0';
                    p_buf++;
                } while (*p_buf == '\n');

                break;
            }
        }

#ifdef LOCAL_DEBUG
        printf("script cmd:");
        for (int i = 0; i < argc; i++) {
            printf(" %s", argv[i]);
        }
        printf("\r\n");
#endif

        proc_onecmd(argc, argv);
    }

    ret = 0;

close_fd:
    close(fd);

free_buf:
    if (file_buf != NULL) {
        free(file_buf);
    }

    return ret;
}

int interpret_ash_script(void)
{
    interpret_script(ASH_SCRIPT_FILE_NAME);
    return 0;
}

static void ash_usage(void)
{
    printf("Usage: ash file_script\r\n");
}

static int ash_main(int argc, char **argv)
{
    int ret;
    const char *file_name;

    if (argc == 2) {
        file_name = argv[1];
        interpret_script(file_name);
        ret = 0;
    } else {
        ash_usage();
        ret = -1;
    }

    return ret;
}

ALIOS_CLI_CMD_REGISTER(ash_main, ash, execute ash script);

