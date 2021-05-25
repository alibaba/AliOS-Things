#include <stdint.h>
#include <stdio.h>
#include <aos/errno.h>
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include <vfsdev/wdg_dev.h>
#include <aos/driver/wdg.h>

int vfs_wdg_test(int count, int period)
{
    int ret = 0;
    int fd = 0;

    fd = open("/dev/wdg0", 0);
    printf("open wdg %s, fd:%d\r\n", fd >= 0 ? "success" : "fail", fd);

    if (fd >= 0) {
        while (count--) {
            ret = ioctl(fd, IOC_WDG_RELOAD, 0);
            printf("wdg reload %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);
            usleep(period * 1000);
        }
        close(fd);
        ret = 0;
    } else {
        ret = -EIO;
    }

    printf("wdg test done\r\n");
    return ret;
}

static void wdg_reload_test(int32_t argc, char **argv)
{
    int ret = 0;
    int count = argc > 1 ? atoi(argv[1]) : 10;
    int period = argc > 2 ? atoi(argv[2]) : 200;

    printf("watchdog comp reload test start!\r\n");
    printf("watchdog comp reload test count:%d, period:%d ms\r\n", count, period);

    ret = vfs_wdg_test(count, period);
    if (!ret) {
        printf("watchdog comp reload test success!\r\n");
    } else {
         printf("watchdog comp reload test failed, ret:%d\r\n", ret);
    }

    return;
}

#if CONFIG_U_WDG_CORE
static void aos_wdg_reload_test(int32_t argc, char **argv)
{
    int ret = 0;
    aos_status_t status = 0;
    int count = argc > 1 ? atoi(argv[1]) : 10;
    int period = argc > 2 ? atoi(argv[2]) : 200;
    wdg_dev_handle_t wdg = NULL;

    printf("watchdog comp reload test start!\r\n");
    printf("watchdog comp reload test count:%d, period:%d ms\r\n", count, period);

    wdg = aos_wdg_open(0, 1000);
    if (!wdg) {
        printf("aos_wdg_open fails\r\n");
        return;
    }

    status = aos_wdg_start(wdg);
    if (status) {
        printf("aos_wdg_start fails, status:%d\r\n", status);
        goto out;
    }

    aos_msleep(500);
    status = aos_wdg_feed(wdg);
    if (status) {
        printf("aos_wdg_feed fails, status:%d\r\n", status);
        goto out;
    }

    status = aos_wdg_stop(wdg);
    if (status) {
        printf("aos_wdg_feed fails, status:%d\r\n", status);
        goto out;
    }

    status = aos_wdg_timeout_set(wdg, 2000);
    if (status) {
        printf("aos_wdg_feed fails, status:%d\r\n", status);
        goto out;
    }

    status = aos_wdg_start(wdg);
    if (status) {
        printf("aos_wdg_feed fails, status:%d\r\n", status);
        goto out;
    }

    aos_msleep(3000);
    status = aos_wdg_feed(wdg);
    if (status) {
        printf("aos_wdg_feed fails, status:%d\r\n", status);
        goto out;
    }

    status = aos_wdg_stop(wdg);
    if (status) {
        printf("aos_wdg_feed fails, status:%d\r\n", status);
        goto out;
    }

out:
    status = aos_wdg_close(wdg);
    if (status) {
        printf("aos_wdg_feed fails, status:%d\r\n", status);
        return;
    }

    return;
}
#endif

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(wdg_reload_test, wdg_reload, reset watchdog operation example)
#if CONFIG_U_WDG_CORE
ALIOS_CLI_CMD_REGISTER(aos_wdg_reload_test, aos_wdg_reload, reset watchdog operation example)
#endif
#endif