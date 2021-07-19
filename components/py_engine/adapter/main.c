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

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include "miniunz.h"
#include "haas_main.h"

#define LOG_TAG "haas_main"

static char *stack_top = NULL;
static bool is_engine_run = false ;

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

    if (argc == 0) {
        temp_args->is_bootup = true;
        temp_args->argc = 2;
    } else {
        temp_args->is_bootup = false;
        temp_args->argc = argc;
    }

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

static void py_engine_entry(void *p)
{
    mpy_thread_args *args = (mpy_thread_args *)p;
    if (args == NULL) {
        LOGE(LOG_TAG, "%s:args is illegal\n", __func__);
        return;
    }

    int ret = mpy_init(args);
    if(ret) {
        LOGE(LOG_TAG, "%s:init python engine failed\n", __func__);
    } else {
        mpy_run(args->argc, args->argv);
        mpy_deinit();
    }

    free_mpy_thread_args(args);
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
    // netmgr_service_init(NULL);
    // netmgr_set_auto_reconnect(NULL, true);
    // netmgr_wifi_set_auto_save_ap(true);

    // netmgr_add_dev("/dev/wifi0");
    // aos_network_status_registercb(NULL, NULL);

    // char * dev_name = aos_get_device_name();
    // if (!dev_name) {
    //     LOGE(LOG_TAG, "internal device name is null");
    //     aos_task_exit(0);
    //     return;
    // }
    // LOGD(LOG_TAG, "internal device name is %s",dev_name);
    while(!aos_get_network_status()) {
        aos_msleep(1000);
    }
    py_app_management_center_init();
    aos_task_exit(0);
    return;
}
#endif

void haas_main(int argc, char **argv)
{
    printf(" Welcome to MicroPython \n");

#if PY_BUILD_BOOT
    amp_boot_main();
#endif

    int ret = -1 ;
    aos_task_t engine_entry;
    aos_task_t engine_task;

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

    mpy_thread_args *args = alloc_mpy_thread_args(argc, argv);

    ret = aos_task_new_ext(&engine_task, "py_engine_task", py_engine_task, args, 1024 * 8, AOS_DEFAULT_APP_PRI);
    if (ret != 0) {
        LOGE(LOG_TAG, "pyengine task creat failed!");
        return;
    }

    ret = aos_task_new_ext(&engine_entry, "py_engine", py_engine_entry, args, 1024 * 20, AOS_DEFAULT_APP_PRI);
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
    if (is_engine_run){
        amp_warn(LOG_TAG,"engine already run! ");
        return  -1 ;
    }
    is_engine_run = true;
	
    uint32_t stack_size = 1024;

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
        printf("mpy_main:heap alloc fail!\r\n");
        return -1;
    }
    gc_init(heap, heap + MICROPY_GC_HEAP_SIZE);
#endif

    mp_init();

    /*set default mp_sys_path*/
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)

    char *path_root = MP_FS_ROOT_DIR"/lib/micropython";
    mp_obj_list_append(mp_sys_path, mp_obj_new_str_via_qstr(path_root, strlen(path_root)));

    char *path_sdcard = "/sdcard/lib/micropython";
    mp_obj_list_append(mp_sys_path, mp_obj_new_str_via_qstr(path_sdcard, strlen(path_sdcard)));

    mp_obj_list_init(mp_sys_argv, 0);

    if(args->is_bootup == true) {
        FILE *fd = fopen(AMP_PY_ENTRY_DEFAULE, "r");
        if( fd != NULL) {
            args->argc = 2;
            args->argv[0] = strdup("python");
            args->argv[1] = strdup(AMP_PY_ENTRY_DEFAULE);
            printf(" ==== python execute main.py from sdcard ====\n");
            fclose(fd);
        } else {
            fd = fopen(AMP_PY_ENTRY_BAK, "r");
            if( fd != NULL) {
                args->argc = 2;
                args->argv[0] = strdup("python");
                args->argv[1] = strdup(AMP_PY_ENTRY_BAK);
                printf(" ==== python execute main.py from data ====\n");
                fclose(fd);
            }
        }
    }
    return  0 ;
}

int mpy_deinit(void)
{
    // need relese python run mem
#if MICROPY_ENABLE_GC
    if(NULL != heap){
        // printf("free python heap mm\r\n");
        free(heap);
        heap == NULL;
    }
#endif
    is_engine_run = false ;
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

	//give_timeout_signle(wait_time_ms);
	
	do{
		int c = mp_hal_stdin_rx_chr();
		LOGD(LOG_TAG,"0x%02x",c);
		if(c == '\x03')
		{
			ret = 1;
			//give_timeout_signle_delete();
			break;
		}
		now_time = aos_now_ms();
        uint32_t time = now_time - begin_time ;
        if(time >= wait_time_ms){
            ret = 0;
			LOGD(LOG_TAG,"tmout:0x%02x",'\x03');
			break;
        }else{
            LOGD(LOG_TAG," time is  %d",time);

        }
        aos_boot_delay(1);


	} while(1);


	return ret;
}


int mpy_run(int argc, char *argv[]) {


    #if 1
    int mode = 0 ;
	mode = is_download_mode(300);
	// if(mode == 0)
	// {
	// 	int mp_stat = pyexec_file_if_exists("main.py");
	// 	ZYF_LOG("mp_stat %d.\n",mp_stat);
	// 	if(1 == mp_stat)
	// 	{
	// 		ZYF_LOG("gpio.py is not found.\n");
	// 	}
	// }
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

mp_import_stat_t mp_import_stat(const char *path) {
    int len;
    char * sys_path;
    struct aos_stat pst;
    int32_t ret;

    if(NULL == path){
        return MP_IMPORT_STAT_NO_EXIST;
    }

    ret = aos_stat(path, &pst);
    if(0 == ret){
        if(pst.st_mode & S_IFDIR){
            return MP_IMPORT_STAT_DIR;
        }else{
            return MP_IMPORT_STAT_FILE;
        }
    }

    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs
) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void nlr_jump_fail(void *val) {
    while (1);
}

void NORETURN __fatal_error(const char *msg) {
    while (1);
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func,
const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif




int DEBUG_printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    int ret = vprintf(fmt, args);
    va_end(args);

    fflush(stdout);

    return ret;
}

#ifdef AOS_COMP_CLI
#if BOARD_HAAS700
#include <console.h>
#endif
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(haas_main, python, start micropython)
ALIOS_CLI_CMD_REGISTER(handle_unzip_cmd, unzip, start unzip)
#endif
