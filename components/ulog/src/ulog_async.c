/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "ulog/ulog.h"
#include "ulog_api.h"
#include "ulog_ring_fifo.h"
#include "aos/kernel.h"
#include "k_config.h"
#if ULOG_POP_CLOUD_ENABLE
#include "uagent.h"
#endif

#define SUB_FUNC_MAX_NUM  2

static aos_task_t ulog_routine;

typedef void(*on_ulog_man_service)(const uint32_t, const uint32_t);

typedef struct {
    char                 *func_mark;
    char                 *sub_func_mark[SUB_FUNC_MAX_NUM];
    on_ulog_man_service  handler;
} ulog_man_handler_service_t;

static void ulog_man_handler(const char* raw_str);

const ulog_man_handler_service_t ulog_man_handler_service[] = {
#if ULOG_POP_UDP_ENABLE
    { "listen", {"ip","port"}, on_update_syslog_watcher_addr },
    { "tcpip",  {"on","off" }, on_tcpip_service_on           },
#endif
#if ULOG_POP_FS_ENABLE
    { "fspause", {"on","off"}, on_fs_record_pause },
#if ULOG_UPLOAD_LOG_FILE
    { "up", {"idx","none"}, on_fs_upload },
#endif
#endif
};

static void ulog_handler(void* para, void* log_text, const uint16_t log_len)
{
    if ((log_text != NULL) && log_len > 0) {
        char *str = (char*)log_text;
        char *text_info = NULL;
        if ((str[0] == '<') && ((text_info = strchr(str, '>')) != NULL)) {
            /* pri = facility*8+level */
            const uint32_t pri = strtoul(&str[1], NULL, 10);
            const uint8_t severity = (pri & 0x7);
#if LOG_DIR_ASYNC
            if (check_pass_pop_out(ulog_session_std, severity)) {
                if (log_get_mutex()) {
                    puts(&((char*)log_text)[LOG_PREFIX_LEN]);
                    log_release_mutex();
                }
            }
#endif

#if ULOG_POP_UDP_ENABLE
            if (check_pass_pop_out(ulog_session_udp, severity)) {
                (void)pop_out_on_udp(&((char *)log_text)[LOG_PREFIX_LEN], log_len);
            }
#endif

#if ULOG_POP_FS_ENABLE
            if (check_pass_pop_out(ulog_session_file, severity)) {
                (void)pop_out_on_fs(&((char *)log_text)[LOG_PREFIX_LEN], log_len);
            }
#endif

#if ULOG_POP_CLOUD_ENABLE
            if ((FACILITY_NORMAL_LOG_NO_POP_CLOUD != (pri & 0xF8))
                && check_pass_pop_out(ulog_session_cloud, severity)) {
                (void)uagent_send(UAGENT_MOD_ULOG, ULOG_SHOW, strlen(text_info + 1), text_info + 1, 0);
            }
#endif

        } else if (0 == strncmp(str, ULOG_CMD_PREFIX, strlen(ULOG_CMD_PREFIX))) {
            /* syslog management */
            ulog_man_handler(&str[strlen(ULOG_CMD_PREFIX)]);
        } else { /* unkown format */
            puts(log_text);
        }
    }
}

static void log_routine(void* para)
{
    while (1) {
        /* PRI      HEADER            MSG */
        /* <130>Oct 9 22:33:20.111 soc kernel.c[111]: The audit daemon is exiting. */
        uring_fifo_pop_cb(ulog_handler, NULL);
    }
}

void ulog_async_init()
{
    /* start log on fs feature if both vfs && spiffs support */
#if ULOG_POP_FS_ENABLE
    ulog_fs_init();
#endif
    uring_fifo_init();

    if (0 == aos_task_new_ext(&ulog_routine,
                              "ulog",
                              log_routine,
                              NULL,
                              LOG_ROUTINE_TASK_STACK_DEPTH,
                              RHINO_CONFIG_USER_PRI_MAX)) {

    }
}

void ulog_man_handler(const char* raw_str)
{
    const int8_t man_handler_service_size = sizeof(ulog_man_handler_service) / sizeof(ulog_man_handler_service_t);
    int8_t i = 0;
    uint8_t j = 0;
    int func_len = 0;
    int subfunc_len = 0;
    unsigned short data_len = 0;
    uint32_t param[SUB_FUNC_MAX_NUM] = { -1, -1 };
    char *p = NULL;
    char *val_str = NULL;

    if (raw_str == NULL) {
        return ;
    }

    data_len = strlen(raw_str);

    for (; i < man_handler_service_size; i++) {
        func_len =  strlen(ulog_man_handler_service[i].func_mark);
        if (0 == strncmp(ulog_man_handler_service[i].func_mark, raw_str, func_len)) {
            /*
            printf("%s %d find ulog func mark %s \r\n", __FILE__, __LINE__, ulog_man_handler_service[i].func_mark);
            */
            p = &raw_str[func_len];
            if (p[0] == ' ') {
                for (; j < SUB_FUNC_MAX_NUM; j++) {
                    subfunc_len = strlen(ulog_man_handler_service[i].sub_func_mark[j]);
                    if (strncmp(&p[1], ulog_man_handler_service[i].sub_func_mark[j], subfunc_len) == 0) {
                        /*
                        printf("%s %d find ulog sub func mark %s \r\n", __FILE__, __LINE__, ulog_man_handler_service[i].sub_func_mark[j]);
                        */
                        if (data_len > strlen(ulog_man_handler_service[i].func_mark) + 1 + subfunc_len + 1) {
                            /*jump off the space and the '='*/
                            val_str = &p[subfunc_len + 1 + 1];
                            param[j] = strtoul(val_str, NULL, 10);
                            /*
                            printf("%s %d find ulog sub func mark %s param value %s param %d \r\n", __FILE__, __LINE__,
                                        ulog_man_handler_service[i].sub_func_mark[j], val_str, param[j]);
                            */
                        } else {
                            /*there is no param for this cmd ,set true */
                            param[j] = 1;
                            /*
                            printf("%s %d find ulog sub func mark %s param %d \r\n", __FILE__, __LINE__,
                                        ulog_man_handler_service[i].sub_func_mark[j], param[j]);
                            */
                        }
                    }
                }
                if ((param[0] != -1 || param[1] != -1) && (ulog_man_handler_service[i].handler != NULL)) {
                    /*
                    printf("%s %d find ulog man handler %d param0 %d param1 %d\r\n", __FILE__, __LINE__, i, param[0], param[1]);
                    */
                    ulog_man_handler_service[i].handler(param[0], param[1]);
                }
            }
            break;
        }
    }

}

