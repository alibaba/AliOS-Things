/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <aos/network.h>
#include <hal/trace.h>
#include <aos/cli.h>
#include <sys/errno.h>

/* CLI Support */
#ifdef CONFIG_AOS_CLI

#define TRACE_TASK_STACK_SIZE 512

extern struct k_fifo trace_fifo;
extern int32_t set_filter_task(const char * task_name);
extern void set_event_mask(const uint32_t mask);
extern void trace_deinit(void);
static ktask_t *trace_task;
static uint32_t trace_buf[1025];
static int trace_is_started;
static char *filter_task;
static char *ip_addr;
static uint16_t ip_port = 8000;
static int sockfd;

void *trace_hal_init()
{
    int fd;
    struct sockaddr_in servaddr;

    if (trace_is_started == 0) {

        TRACE_INIT();

        if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            aos_cli_printf("create socket (%s:%u) error: %s(errno: %d)\r\n", ip_addr, ip_port, strerror(errno),errno);
            return 0;
        }

        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(ip_port);
        inet_aton(ip_addr, &servaddr.sin_addr);

        if (connect(fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
            aos_cli_printf("connect (%s:%u) error: %s(errno: %d)\r\n", ip_addr, ip_port, strerror(errno),errno);
            close(fd);
            return 0;
        }

        trace_is_started = 1;
        aos_cli_printf("connected on (%s:%u)\r\n", ip_addr, ip_port);

        return (void *)fd;
    }

    krhino_task_dyn_del(NULL);

    return 0;
}


ssize_t trace_hal_send(void *handle, void *buf, size_t len)
{
    int len_send = 0;
    int len_total_send = 0;

    while (1) {
        len_send = send((int)handle, (buf + len_total_send) , len - len_total_send, 0);
        if (len_send < 0) {
            aos_cli_printf("send (%s:%u) msg error: %s(errno: %d)\r\n", ip_addr, ip_port, strerror(errno), errno);
            return 0;
        }

        len_total_send += len_send;
        len -= len_send;
        if (len == 0) {
            break;
        }
    }

    return len_send;
}

ssize_t trace_hal_recv(void *handle, void *buf)
{
    return 0;
}

void trace_hal_deinit(void *handle)
{
    int fd;

    fd = (int)handle;
    close(fd);
}

static void trace_entry(void *arg)
{
    uint32_t len;

    sockfd = (int)trace_hal_init();
    if (sockfd > 0) {
        while (1) {
            len = fifo_out_all(&trace_fifo, &trace_buf[1]);
            if (len > 0) {
                trace_buf[0] = len;
                trace_hal_send((void *)sockfd, trace_buf, len + 4);
            }

            krhino_task_sleep(2);
        }
    }
}

static void handle_trace_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    const char *rtype = argc > 1 ? argv[1] : "";
    int ret = 0;
    char *buffer = NULL;

    if (strcmp(rtype, "start") == 0) {
        if (argc == 3 || argc == 4) {
            if (ip_addr == NULL) {
                ip_addr = (char *) aos_malloc(strlen(argv[2])+1);
                if (ip_addr == NULL) {
                    k_err_proc(RHINO_NO_MEM);
                }

                strncpy(ip_addr, argv[2], strlen(argv[2]));
                ip_addr[strlen(argv[2])] = '\0';
            }

            if (argc ==4){
                ip_port = atoi(argv[3]);
            }
        } else {
            aos_cli_printf("trace must specify the host ip (port)... \r\n");
            return ;
        }

        if (krhino_task_dyn_create(&trace_task, "trace_task", NULL, 3,
                0, TRACE_TASK_STACK_SIZE, trace_entry, 1) != RHINO_SUCCESS) {
            aos_cli_printf("trace task creat fail \r\n");
        }
    } else if (strcmp(rtype, "task") == 0) {
        if (argc != 3) {
            return;
        }
        if (filter_task) {
            aos_free(filter_task);
        }

        filter_task = (char *) aos_malloc(strlen(argv[2])+1);
        if (filter_task == NULL) {
            k_err_proc(RHINO_NO_MEM);
        }

        strncpy(filter_task, argv[2], strlen(argv[2]));
        filter_task[strlen(argv[2])] = '\0';

        set_filter_task(filter_task);
    } else if (strcmp(rtype, "event") == 0) {
        if (argc != 3) {
            return;
        }

        set_event_mask(atoi(argv[2]));
    } else if (strcmp(rtype, "stop") == 0) {
        if (trace_is_started) {
            trace_is_started = 0;
            trace_deinit();

            trace_hal_deinit((void *)sockfd);

            aos_cli_printf("trace (%s:%u) stop....\r\n", ip_addr, ip_port);

            if (ip_addr) {
                free(ip_addr);
                ip_addr = NULL;
            }

            if (filter_task){
                free(filter_task);
                ip_addr = NULL;
            }

            krhino_task_dyn_del(trace_task);
        }
    }
}

static struct cli_command ncmd = {
    .name = "trace",
    .help = "trace [start ip (port) | task task_name| event event_id| stop]",
    .function = handle_trace_cmd,
};

void trace_start(void)
{
    aos_cli_register_command(&ncmd);
}

#else

void trace_start(void)
{
    printf("trace should have cli to control!!!\r\n");
}

#endif



