/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <signal.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <aos/aos.h>

#include <arg_options.h>


#define TAG "main"

#ifdef TFS_EMULATE
extern int tfs_emulate_id2_index;
#endif

extern void krhino_lwip_init(int enable_tapif);
/* check in gcc sources gcc/gcov-io.h for the prototype */
extern void __gcov_flush(void);
extern void rl_free_line_state(void);
extern void rl_cleanup_after_signal(void);
extern void hw_start_hal(options_t *poptions);
extern void trace_start();
extern void netmgr_init(void);
extern int aos_framework_init(void);
extern int aos_cli_init(void);

static options_t options = { 0 };

static void aos_features_init(void);
static void signal_handler(int signo);
static int  setrlimit_for_vfs(void);
extern int application_start(int argc, char **argv);

static void exit_clean(void)
{
    char fn[64] = {0};
    snprintf(fn, sizeof(fn), "rm -f ./aos_partition_%d_*", getpid());
    system(fn);
}

static void app_entry(void *arg)
{
    int i = 0;

    aos_features_init();

#ifdef CONFIG_AOS_MESHYTS
    options.flash.per_pid = true;
#else
    options.flash.per_pid = false;
#endif
    hw_start_hal(&options);

#ifdef AOS_VFS
    vfs_init();
    vfs_device_init();
#endif

#ifdef CONFIG_AOS_CLI
    if (options.cli.enable)
        aos_cli_init();
#endif

#ifdef AOS_KV
    aos_kv_init();
#endif

#ifdef AOS_LOOP
    aos_loop_init();
#endif

#ifdef AOS_FRAMEWORK_COMMON
    aos_framework_init();
#endif

#ifdef VCALL_RHINO
    trace_start();    
#endif

    application_start(options.argc, options.argv);
}

static void start_app()
{
    aos_task_new("app", app_entry, NULL, 8192);
}

int csp_get_args(const char ***pargv)
{
    *pargv = (const char **)options.argv;
    return options.argc;
}

void aos_features_init(void)
{
#ifdef CONFIG_NET_LWIP
    if (options.lwip.enable) {
        krhino_lwip_init(options.lwip.tapif);
    }
#endif
}

void signal_handler(int signo)
{
    LOGD(TAG, "Received signal %d\n", signo);

#ifdef ENABLE_GCOV
    __gcov_flush();
#endif

#ifdef CONFIG_AOS_DDA
    rl_free_line_state ();
    rl_cleanup_after_signal ();
#endif

    exit(0);
}

#define ARCH_MAX_NOFILE 64
int setrlimit_for_vfs(void)
{
    int           ret;
    struct rlimit rlmt;

    getrlimit(RLIMIT_NOFILE, &rlmt);
    if (rlmt.rlim_cur > ARCH_MAX_NOFILE) {
        rlmt.rlim_cur = ARCH_MAX_NOFILE;
        ret = setrlimit(RLIMIT_NOFILE, &rlmt);
        if (ret != 0) {
            LOGE(TAG, "setrlimit error: %s", strerror(errno));
            return ret;
        }
    }
    LOGD(TAG, "Limit max open files to %d", (int)rlmt.rlim_cur);

    return 0;
}

int main(int argc, char **argv)
{
    int ret;

    setvbuf(stdout, NULL, _IONBF, 0);

    options.argc        = argc;
    options.argv        = argv;
    options.lwip.enable = true;
#if defined(TAPIF_DEFAULT_OFF) || !defined(WITH_LWIP)
    options.lwip.tapif  = false;
#else
    options.lwip.tapif  = true;
#endif
    options.log_level   = AOS_LL_WARN;
    options.cli.enable  = true;

#if defined(CONFIG_AOS_DDA) || defined(ENABLE_GCOV)
    signal(SIGINT, signal_handler);
#endif
#ifdef CONFIG_AOS_UT
    signal(SIGPIPE, SIG_IGN);
#endif

    atexit(exit_clean);

    krhino_init();

    ret = setrlimit_for_vfs();
    if (ret != 0) {
        return ret;
    }

    parse_options(&options);

    aos_set_log_level(options.log_level);

#ifdef TFS_EMULATE
    tfs_emulate_id2_index = options.id2_index;
#endif

    start_app(argc, argv);

    krhino_start();

    return ret;
}

int board_cli_init(void)
{
    return 0;
}

