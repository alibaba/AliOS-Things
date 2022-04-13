/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <iomap.h>

#include "aos/cli.h"
#include "ram/ramdrv_test.h"

#include <rpc.h>
#include <hal2vfs/io_uart.h>
#include <drivers/u_ld.h>
#include <drivers/ddkc_log.h>
#include <drivers/char/u_cdev_msg.h>
#include <hal2vfs/io_ramdrv.h>

#ifdef AOS_COMP_DEVICEVFS
#ifdef AOS_COMP_CLI
static void register_read_test(char *buf, int len, int argc, char **argv);
static void register_write_test(char *buf, int len, int argc, char **argv);
static void mem_read_test(char *buf, int len, int argc, char **argv);
static void mem_write_test(char *buf, int len, int argc, char **argv);
static void ramdrv_test(char *buf, int len, int argc, char **argv);

static struct cli_command drv_ut_cmds[] = {
    { "memw","memory write test", mem_write_test },
    { "memr","memory read test", mem_read_test },
    { "regr","register read test", register_read_test },
    { "regw","register write test", register_write_test },
    { "ramdrvt","ramdrv driver test", ramdrv_test },
};

static void mem_read_test(char *buf, int len, int argc, char **argv) {
    int i = 0;
    char *ptr = NULL;
    unsigned long *addr = NULL;

    ddkc_info("buf:%s, len:%d\n", buf, len);
	for (i = 0; i < argc; i++) {
		ddkc_info("argv[%d]:%s\n", i, *(argv + i));
	}

	if (argc <= 1) {
	    ddkc_err("argument too short\r\n");
	    return;
	}
    addr = (unsigned long *)strtol(argv[1], &ptr, 16);


	ddkc_err("addr:%p, *addr:0x%lx\r\n", addr, *addr);

	return;
}

static void mem_write_test(char *buf, int len, int argc, char **argv) {
    unsigned int i = 0;
    char *ptr = NULL;
    unsigned long *addr = NULL;
    unsigned long value = 0;

    ddkc_info("buf:%s, len:%d\n", buf, len);
	for (i = 0; i < argc; i++) {
		ddkc_info("argv[%d]:%s\n", i, *(argv + i));
	}

	if (argc <= 2) {
	    ddkc_err("argument too short\r\n");
	    return;
	}
    addr = (unsigned long *)strtol(argv[1], &ptr, 16);
    value = (unsigned long)strtol(argv[2], &ptr, 16);

	ddkc_err("before write addr:%p:0x%lx value:0x%lx\r\n", addr, *addr, value);
	*addr = value;
    ddkc_err("after write addr:%p:0x%lx\r\n", addr, *addr);
	return;
}


static void register_read_test(char *buf, int len, int argc, char **argv) {
    int i = 0;
    int ret = 0;
    char *ptr = NULL;
    unsigned long *pa = NULL;
    unsigned long *va = NULL;
    unsigned int length = 0;

    ddkc_info("buf:%s, len:%d\n", buf, len);
	for (i = 0; i < argc; i++) {
		ddkc_info("argv[%d]:%s\n", i, *(argv + i));
	}

	if (argc <= 2) {
	    ddkc_err("argument too short\r\n");
	    return;
	}

    pa = (unsigned long *)strtol(argv[1], &ptr, 16);
	length = atoi(argv[2]);

	ddkc_info("pa:%p, length:%d\r\n", pa, length);
    va = pa;
	ret = aos_io_mmap((uintptr_t)va, (uintptr_t)pa, length);
	ddkc_info("va:%p, ret:%d\r\n", va, ret);

	ddkc_info("va:%p, *va:0x%lx\r\n", va, *(va));

	ddkc_info("%s return\r\n", __func__);
	return;
}

static void register_write_test(char *buf, int len, int argc, char **argv) {
    int i = 0;
    int ret = 0;
    unsigned long *pa = NULL;
    unsigned long *va = NULL;
    unsigned long value = 0;
    char *ptr;

    ddkc_info("buf:%s, len:%d\n", buf, len);
	for (i = 0; i < argc; i++) {
		ddkc_info("argv[%d]:%s\n", i, *(argv + i));
	}

	if (argc <= 2) {
	    ddkc_err("argument too short\r\n");
	    return;
	}

    pa = (unsigned long *)strtol(argv[1], &ptr, 16);
	value = strtol(argv[2], &ptr, 16);;

	ddkc_info("pa:%p, value:%ld\r\n", pa, value);
    va = pa;
	ret = aos_io_mmap((uintptr_t)va, (uintptr_t)pa, sizeof(unsigned long));
	ddkc_info("after aos_io_mmap, va:%p, ret:%d\r\n", va, ret);

	ddkc_info("before write, va:%p, *va:0x%lx\r\n", va, *va);
    *(va) = value;
    ddkc_info("after write, va:%p, *va:0x%lx\r\n", va, *va);

    ret = aos_io_unmmap((uintptr_t)pa);
    ddkc_info("aos_io_unmmap ret:%d\r\n", ret);
	ddkc_info("%s return\r\n", __func__);

	return;
}


#endif

int drv_unit_test(void) {

    ddkc_dbg("%s starts\n", __func__);

#ifdef AOS_COMP_CLI
    aos_cli_register_commands(&drv_ut_cmds[0], sizeof(drv_ut_cmds) / sizeof(struct cli_command));
#endif

    return 0;
}

#if defined(AOS_COMP_U_RAM_TEST) && defined(AOS_COMP_U_RAMDRV)
int ramdrv_write_test (void) {
    int i = 0;

    for (i = 0; i < 2; i++)
        vfs_ramdrv_test(i);

    return 0;
}

static void ramdrv_test(char *buf, int len, int argc, char **argv) {
    int i = 0;
    int ret = 0;

    ddkc_info("buf:%s, len:%d\n", buf, len);
	for (i = 0; i < argc; i++) {
		ddkc_info("argv[%d]:%s\n", i, *(argv + i));
	}

	if (argc <= 1) {
	    ddkc_err("argument too short\r\n");
	    return;
	}

	if (!strcmp(argv[1], "load")) {
	    ddkc_info("loading ramdrv ...\r\n");
	    ret = vfs_ramdrv_drv_init();
	    ddkc_info("load ramdrv %s\r\n", !ret ? "success" : "failed");
	} else if (!strcmp(argv[1], "unload")) {
	    ddkc_info("unloading ramdrv ...\r\n");
	    ret = vfs_ramdrv_drv_deinit();
	    ddkc_info("unload ramdrv %s\r\n", !ret ? "success" : "failed");
	} else if (!strcmp(argv[1], "write")) {
        ramdrv_write_test();
	}

	return;
}
#else
static void ramdrv_test(char *buf, int len, int argc, char **argv) {
#ifndef AOS_COMP_U_RAM_TEST
    ddkc_err("please select AOS_COMP_U_RAM_TEST if ram test function is necessary\r\n");
#endif

#ifndef AOS_COMP_U_RAMDRV
        ddkc_err("please select AOS_COMP_U_RAMDRV if ram test function is necessary\r\n");
#endif

}
#endif

POST_DRIVER_ENTRY(drv_unit_test)

#endif
