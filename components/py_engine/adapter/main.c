/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include <stdarg.h>
#include "aos/init.h"
#include "board.h"
#include "ulog/ulog.h"
#include "amp_task.h"

#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "lib/utils/pyexec.h"
#include "py/mphal.h"
#include "py/stackctrl.h"

#include "sys/stat.h"
#include "aos/vfs.h"
#include "py_defines.h"
#include "aos_hal_uart.h"
#include "netmgr.h"

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include "miniunz.h"
#include "haas_main.h"

#define LOG_TAG "haas_main"

static char *stack_top = NULL;
static bool isPythonRunning = false;

#if MICROPY_ENABLE_GC
static char *heap = NULL;
#ifndef MICROPY_GC_HEAP_SIZE
#define MICROPY_GC_HEAP_SIZE (1024 * 64)
#endif
#endif

static mpy_thread_args *alloc_mpy_thread_args(int argc, char **argv)
{
    mpy_thread_args *temp_args = (mpy_thread_args *)calloc(1, sizeof(mpy_thread_args));
    if (temp_args == NULL) {
        LOGE(LOG_TAG, "%s;malloc mpy_thread_args failed\n", __func__);
        return NULL;
    }

    if (argc == 1) {
        temp_args->is_repl_mode = true;
    } else {
        temp_args->is_repl_mode = false;
    }
    temp_args->argc = argc;

    temp_args->argv = (char **)calloc(1, sizeof(char *) * temp_args->argc);
    if (temp_args->argv == NULL) {
        LOGE(LOG_TAG, "%s:temp_args->argv alloc memory failed\n", __func__);
        free(temp_args);
        return NULL;
    }

    for (int i = 0; i < argc; i++) {
        temp_args->argv[i] = strdup(argv[i]);
    }

    return temp_args;
}

static int free_mpy_thread_args(mpy_thread_args *args)
{
    if (args == NULL) {
        LOGE(LOG_TAG, "args is illegal\n");
        return -1;
    }

    for (int i = 0; i < args->argc; i++) {
        if (args->argv[i] != NULL) {
            free(args->argv[i]);
        }
    }

    free(args->argv);
    free(args);
    return 0;
}

static bool is_file_exist(const char* path) {
    if(path == NULL) {
        LODE(LOG_TAG, "File path null\n");
        return false;
    }

    FILE *fd = fopen(path, "r");
    if(fd != NULL) {
        fclose(fd);
        return true;
    }

    return false;
}

static uint8_t* is_mainpy_exist() {
    /* check whether main/pyamp/main.py */
    FILE *fd = fopen(AMP_PY_ENTRY_DEFAULE, "r");
    if( fd != NULL) {
        printf(" ==== python execute from %s ====\n", AMP_PY_ENTRY_DEFAULE);
        fclose(fd);
        return AMP_PY_ENTRY_DEFAULE;
    }

    fd = fopen(AMP_PY_ENTRY_BAK, "r");
    if( fd != NULL) {
        printf(" ==== python execute from %s ====\n", AMP_PY_ENTRY_BAK);
        fclose(fd);
        return AMP_PY_ENTRY_BAK;
    }

    return NULL;
}

static netmgr_hdl_t hdl_t = -1;

void set_netmgr_hdl(netmgr_hdl_t hdl) {
    hdl_t = hdl;
}

netmgr_hdl_t get_netmgr_hdl() {
    return hdl_t;
}

static void net_init() {
    #define WIFI_DEV_PATH "/dev/wifi0"

    char* wificonf = MP_FS_ROOT_DIR"/wifi.conf";
    if(is_file_exist(wificonf) == true) {
        event_service_init(NULL);
        netmgr_service_init(NULL);
        netmgr_set_auto_reconnect(NULL, true);
        netmgr_wifi_set_auto_save_ap(true);

        netmgr_add_dev(WIFI_DEV_PATH);
        netmgr_hdl_t hdl = netmgr_get_dev(WIFI_DEV_PATH);
        set_netmgr_hdl(hdl);
    }
}

#include "extmod/vfs.h"
#include "extmod/vfs_posix.h"
// Try to mount the data on "/data" and chdir to it for the boot-up directory.
static int mount_fs(char* mount_point_str) {
    mp_obj_t mount_point = mp_obj_new_str(mount_point_str, strlen(mount_point_str));
    mp_obj_t bdev = mp_type_vfs_posix.make_new(&mp_type_vfs_posix, 0, 0, NULL);
    int32_t ret = mp_vfs_mount_and_chdir_protected(bdev, mount_point);
    if (ret != 0) {
        printf("mount_fs failed with mount_point: %s\n", mount_point_str);
        return -MP_ENOENT;
    }
    return 0;
}

static void py_engine_mainloop(void *p)
{
    mpy_thread_args *args = (mpy_thread_args *)p;
    if (args == NULL) {
        LOGE(LOG_TAG, "%s:args is illegal\n", __func__);
        return;
    }

    int ret = mpy_init(args);
    if(ret) {
        LOGE(LOG_TAG, "%s:mpy_init failed, ret=%d\n", __func__, ret);
    } else {
        /* Suspend CLI to make sure REPL use UART exclusively */
        aos_cli_suspend();

        isPythonRunning = true;
        mpy_run(args->argc, args->argv);

        /* Resume CLI after REPL */
        aos_cli_resume();
    }

    free_mpy_thread_args(args);
    mpy_deinit();
    isPythonRunning = false;
}

static void py_engine_task(void *p)
{
    py_task_init();
    while (1) {
        /* loop for asynchronous operation */
        if(py_task_yield(200) == 1) {
            printf("pyengine task yield exit! \r\n");
            break;
        }
    }
    aos_task_exit(0);
}

#ifdef PY_BUILD_CHANNEL
static void network_func(void *argv)
{
    while(!aos_get_network_status()) {
        aos_msleep(1000);
    }
    py_app_management_center_init();
    aos_task_exit(0);
    return;
}
#endif

static void python_entry(int argc, char **argv)
{
    if(isPythonRunning == true) {
        printf(" **************************************************** \r\n");
        printf(" ** Python is running, cannot start another engine ** \r\n");
        printf(" **************************************************** \r\n");

        return;
    }

    printf(" Welcome to MicroPython \n");

    aos_task_t engine_entry = NULL;
    mpy_thread_args *args = alloc_mpy_thread_args(argc, argv);

    int32_t ret = aos_task_new_ext(&engine_entry, "py_engine", py_engine_mainloop, args, 1024 * 20, AOS_DEFAULT_APP_PRI);
    if (ret != 0) {
        LOGE(LOG_TAG, "py_engine_mainloop task creat failed!");
        return;
    }
}

void haas_main(int argc, char **argv)
{
#if PY_BUILD_BOOT
    aos_cli_suspend();
    pyamp_boot_main();
    aos_cli_resume();
#endif

    int ret = -1;

    /* ulog module init */
    ulog_init();
    aos_set_log_level(AOS_LL_ERROR);

#if PY_BUILD_KV
    /* kv module init */
    ret = kv_init();
    if (ret != 0) {
        LOGE(LOG_TAG, "kv init failed!");
        return;
    }
#endif

    /* net init */
    net_init();

    aos_task_t engine_task;
    ret = aos_task_new_ext(&engine_task, "py_engine_task", py_engine_task, NULL, 1024 * 8, AOS_DEFAULT_APP_PRI);
    if (ret != 0) {
        LOGE(LOG_TAG, "pyengine task creat failed!");
        return;
    }

#ifdef PY_BUILD_CHANNEL
    aos_task_t network_task;
    /* network start */
    ret = aos_task_new_ext(&network_task, "mpy_network", network_func, NULL, 1024 * 4, AOS_DEFAULT_APP_PRI);
    if (ret != 0) {
        LOGE(LOG_TAG,"network task creat failed!");
        return ret;
    }
#endif

    /* Check whether we have main.py to execute */
    uint8_t *path = is_mainpy_exist();
    if(path != NULL) {
        uint8_t *argv[2] = {"python", path};
        python_entry(2, &argv);
    }
}

static void handle_unzip_cmd(int argc, char **argv)
{
    char *zippath  ;
    char *destpath ;
    int ret = 0 ;
    if(argc < 2) {
        LOGE(LOG_TAG, "Error params,Usage: unzip /data/src.zip  /sdcard \r\n");
        return ;
    }

    if(argc == 2) {
        destpath ="/data";
    }else {
        destpath = argv[2] ;
    }
    zippath = argv[1] ;
    ret = miniUnzip(zippath,destpath);
    if(ret) {
        LOGE(LOG_TAG, "unzip failed ,errno is %d \r\n",ret);
    }else {
        LOGD(LOG_TAG, "unzip succeed \r\n");
    }
}

#if MICROPY_ENABLE_COMPILER
void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}
#endif

int mpy_init(mpy_thread_args *args)
{
    int32_t ret = -1;
    uint32_t stack_size = 1024;

    mp_uart_init(MP_REPL_UART_PORT, MP_REPL_UART_BAUDRATE);

#if MICROPY_PY_THREAD
    void *stack_addr = mp_sal_get_stack_addr();
    stack_size = mp_sal_get_stack_size();
    mp_thread_init(stack_addr, stack_size);
#endif

    mp_stack_set_top(stack_top);

    // adjust the stack_size to provide room to recover from hitting the limit
#if MICROPY_STACK_CHECK
    mp_stack_ctrl_init();
    mp_stack_set_limit(stack_size * sizeof(cpu_stack_t) - 1024);
#endif

#if MICROPY_ENABLE_GC
    heap = (char *)malloc(MICROPY_GC_HEAP_SIZE);
    if(NULL == heap){
        LOGE(LOG_TAG, "mpy_init: heap alloc fail!\r\n");
        return -1;
    }
    gc_init(heap, heap + MICROPY_GC_HEAP_SIZE);
#endif

    mp_init();

    #if MICROPY_VFS_POSIX
    {
        // Mount the host FS at the root of our internal VFS
        ret = mount_fs("/");
        if(ret != 0) {
            printf(" !!!!!!!! %s, %d, faild to mount fs !!!!!!!! \n", __func__, __LINE__);
        }
        MP_STATE_VM(vfs_cur) = MP_STATE_VM(vfs_mount_table);
    }
    #endif

    /*set default mp_sys_path*/
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)

    char *path_root = MP_FS_ROOT_DIR"/lib/micropython";
    mp_obj_list_append(mp_sys_path, mp_obj_new_str_via_qstr(path_root, strlen(path_root)));

    char *path_sdcard = "/sdcard/lib/micropython";
    mp_obj_list_append(mp_sys_path, mp_obj_new_str_via_qstr(path_sdcard, strlen(path_sdcard)));

    mp_obj_list_init(mp_sys_argv, 0);

    return 0;
}

int mpy_deinit(void)
{
    // need relese python run mem
#if MICROPY_ENABLE_GC
    if(NULL != heap){
        free(heap);
        heap == NULL;
    }
#endif

    mp_uart_deinit();
}

static void set_sys_argv(char *argv[], int argc, int start_arg) {
    for (int i = start_arg; i < argc; i++) {
        mp_obj_list_append(mp_sys_argv, MP_OBJ_NEW_QSTR(qstr_from_str(argv[i])));
    }
}

int is_download_mode(uint32_t wait_time_ms)
{
	int ret = 0;
	int c = 0;
	int64_t now_time = 0;
	int64_t begin_time = aos_now_ms();

    do {
        int c = ringbuf_get(&stdin_ringbuf);
        LOGD(LOG_TAG, "0x%02x", c);
        if (c == '\x03') {
            ret = 1;
            break;
        }
        now_time = aos_now_ms();
        uint32_t time = now_time - begin_time ;
        if(time >= wait_time_ms){
            ret = 0;
            LOGD(LOG_TAG, "tmout:0x%02x", '\x03');
            break;
        } else {
            LOGD(LOG_TAG, "time is %d", time);
        }
        pyamp_boot_delay(1);
    } while (1);

    return ret;
}

int mpy_run(int argc, char *argv[])
{
#if 1
    int mode = 0 ;
	mode = is_download_mode(300);
#endif

    if (argc >= 2 && mode == 0) {
        char *filename = argv[1] ;
        char filepath[256] = {0};

        if(filename[0] != '/') {
            getcwd(filepath, sizeof(filepath));
            strcat(filepath, "/");
            strcat(filepath, filename);
        } else {
            strcpy(filepath, filename);
        }

        char *p = strrchr(filepath, '/');

        size_t len = 0;
        mp_obj_t *path_items = NULL;
        mp_obj_list_get(mp_sys_path, &len, &path_items);

        if(p >= filepath) {
            path_items[0] = mp_obj_new_str_via_qstr(filepath, p - filepath);
        } else {
            path_items[0] = mp_obj_new_str_via_qstr(filepath, filepath - p);
        }

        set_sys_argv(argv, argc, 1);

        if (filepath != NULL) {
            pyexec_file(filepath);
        }
    }
    {
        for (;;) {
            if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
                if (pyexec_raw_repl() != 0) {
                    break;
                }
            } else {
                if (pyexec_friendly_repl() != 0) {
                    break;
                }
            }
        }
    }
    mp_deinit();
    return 0;
}

MP_WEAK mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(MP_ENOENT);
}

void nlr_jump_fail(void *val) {
    while (1);
}

void NORETURN __fatal_error(const char *msg) {
    while (1);
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func,const char *expr) {
    printf("assert:%s:%d:%s: %s\n", file, line, func, expr);
    mp_raise_msg(&mp_type_AssertionError, MP_ERROR_TEXT("C-level assert"));
}
#endif

#ifdef AOS_COMP_CLI
#if BOARD_HAAS700
#include <console.h>
#endif
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(python_entry, python, start micropython)
ALIOS_CLI_CMD_REGISTER(handle_unzip_cmd, unzip, start unzip)
#endif
