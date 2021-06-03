
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <k_api.h>
#include <aos/kernel.h>

#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "lib/utils/pyexec.h"
#include "py/mphal.h"
#include "py/stackctrl.h"

#include "sys/stat.h"
#include "vfs_types.h"

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

static char *stack_top;
#if MICROPY_ENABLE_GC
#define MICRO_GC_HEAP_SIZE (2048 * 50)
static char *heap;
// static char heap[MICRO_GC_HEAP_SIZE];
#endif

void mpy_add_path(char* path,int8_t length){
    mp_obj_list_append(mp_sys_path, mp_obj_new_str_via_qstr("/data/lib/micropython",length));
    mp_obj_list_append(mp_sys_path, mp_obj_new_str_via_qstr("/sdcard/lib/micropython",length));

}

int mpy_init(void)
{
    int stack_dummy;
    ktask_t * curtask = krhino_cur_task_get();
    stack_top = (char*)&stack_dummy;

    #if MICROPY_PY_THREAD
    mp_thread_init(curtask->task_stack_base, curtask->stack_size);
    #endif
    mp_stack_set_top(stack_top);
    // adjust the stack_size to provide room to recover from hitting the limit
    #if MICROPY_STACK_CHECK
    mp_stack_set_limit(curtask->stack_size * sizeof(cpu_stack_t) - 1024);
    #endif

    #if MICROPY_ENABLE_GC
    heap = (char *)malloc(MICRO_GC_HEAP_SIZE);
    if(NULL == heap){
        printf("mpy_main:heap alloc fail!\r\n");
        return -1;
    }
    gc_init(heap, heap + MICRO_GC_HEAP_SIZE);
    // gc_init(heap, heap + sizeof(heap));
    #endif
    mp_init();
    /*set default mp_sys_path*/
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)
    mp_obj_list_append(mp_sys_path, mp_obj_new_str_via_qstr("/data/lib/micropython",21));
    mp_obj_list_append(mp_sys_path, mp_obj_new_str_via_qstr("/sdcard/lib/micropython",23));

    mp_obj_list_init(mp_sys_argv, 0);

    return  0 ;
}

int mpy_deinit(void)
{
    // need relese python run mem
    #if MICROPY_ENABLE_GC
    if(NULL != heap){
        printf("free python heap mm\r\n");
        free(heap);
        heap == NULL;
    }
    #endif
}

STATIC void set_sys_argv(char *argv[], int argc, int start_arg) {
    for (int i = start_arg; i < argc; i++) {
        mp_obj_list_append(mp_sys_argv, MP_OBJ_NEW_QSTR(qstr_from_str(argv[i])));
    }
}

int mpy_run(int argc, char *argv[]) {

    if (argc >= 2) {
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
    } else {
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
    vfs_stat_t pst;
    int32_t ret;

    if(NULL == path){
        return MP_IMPORT_STAT_NO_EXIST;
    }

    extern int32_t vfs_stat(const char *path, vfs_stat_t *st);
    ret = vfs_stat(path, &pst);
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


#include <stdarg.h>

int DEBUG_printf(const char *fmt, ...)
{
    va_list args;
    int ret;

    va_start(args, fmt);
    ret = vprintf(fmt, args);
    va_end(args);

    fflush(stdout);

    return ret;
}

