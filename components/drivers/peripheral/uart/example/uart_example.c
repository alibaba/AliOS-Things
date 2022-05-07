/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <fcntl.h>
#include <sys/ioctl.h>
#include "aos/vfs.h"
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include <drivers/u_ld.h>
#include <drivers/ddkc_log.h>
#include <vfsdev/uart_dev.h>
#include <drivers/char/u_device.h>

static void uart_read_test(char *buf, int len, int argc, char **argv);
static void uart_write_test(char *buf, int len, int argc, char **argv);
static void uart_echo_test(char *buf, int len, int argc, char **argv);

static void uart_test_usage(void) {
    ddkc_info("uart test purpose, please follow the following commands format\n");
    ddkc_info("\tuartw <port_id> <string>\n");
    ddkc_info("\t\t<port_id>: target UART's port id\n");
    ddkc_info("\t\t<string>: target string to be sent via UART\n");
    ddkc_info("\tuartr <port_id>\n");
    ddkc_info("\t\t<port_id>: target UART's port id\n");
}

static void uart_write_test(char *buf, int len, int argc, char **argv)
{
    int i = 0;
    int j = 0;
    int ret = 0;
    int port_id = 0;
    char *string = 0;
    int fd = -1;
    char buffer[50] = {0};
    char dev_name[16] = {0};

    ddkc_loud("buf:%s, len:%d\n", buf, len);
    for (i = 0; i < argc; i++) {
        ddkc_loud("argv[%d]:%s\n", i, *(argv + i));
    }

    if (argc <= 2) {
        uart_test_usage();
        return;
    }

    port_id = atoi(argv[1]);
    string = argv[2];

    snprintf(dev_name, sizeof(dev_name), "/dev/ttyUART%d", port_id );
    ddkc_info("opening device:%s\r\n", dev_name);
    fd = open(dev_name, 0);

    if (fd < 0) {
        ddkc_err("open %s failed\r\n", dev_name);
        return;
    }

    ddkc_info("set baudrate to 1500000\r\n");
    ret = ioctl(fd, IOC_UART_SET_CFLAG, B1500000 | CS8);
    if (ret) {
        ddkc_err("ioctl on %s failed, ret:%d\r\n", dev_name, ret);
        close(fd);
        return;
    }
    ddkc_info("set baudrate done\r\n");

    ddkc_info("writing %s\r\n", string);
    ret = write(fd, string, strlen(string));
    ddkc_info("write return %d\r\n", ret);

    ddkc_info("closing %s\r\n", dev_name);
    close(fd);
    ddkc_info("%s closed\r\n", dev_name);

    return;
}

static void uart_read_test(char *buf, int len, int argc, char **argv)
{
    int i = 0;
    int j = 0;
    int ret = 0;
    int port_id = 0;
    int ms = 0;
    char *string = 0;
    int fd = -1;
    char buffer[50] = {0};
    char dev_name[16] = {0};

    ddkc_loud("buf:%s, len:%d\n", buf, len);
    for (i = 0; i < argc; i++) {
        ddkc_loud("argv[%d]:%s\n", i, *(argv + i));
    }

    if (argc <= 2) {
        uart_test_usage();
        return;
    }

    port_id = atoi(argv[1]);
    ms = atoi(argv[2]);

    snprintf(dev_name, sizeof(dev_name), "/dev/ttyUART%d", port_id );
    ddkc_info("opening device:%s\r\n", dev_name);
    fd = open(dev_name, 0);

    if (fd < 0) {
        ddkc_err("open %s failed\r\n", dev_name);
        return;
    }

    ddkc_info("set baudrate to 1500000\r\n");
    ret = ioctl(fd, IOC_UART_SET_CFLAG, B1500000 | CS8);
    if (ret) {
        ddkc_err("ioctl on %s failed, ret:%d\r\n", dev_name, ret);
        close(fd);
        return;
    }
    ddkc_info("set baudrate done\r\n");

    do {
        ret = read(fd, buffer, sizeof(buffer));
        if (ret > 0) {
            for (i = 0; i < ret; i++)
                printf("%c", buffer[i]);
            printf("\r\n");
        }

        memset(buffer, 0, sizeof(buffer));
        usleep(100000);
        j++;
    } while(j < (ms/100));

    ddkc_info("closing %s\r\n", dev_name);
    close(fd);
    ddkc_info("%s closed\r\n", dev_name);

    return;
}

static void uart_echo_test(char *buf, int len, int argc, char **argv)
{
    int i = 0;
    int j = 0;
    int ret = 0;
    int port_id = 0;
    int ms = 0;
    char *string = 0;
    int fd = -1;
    char buffer[50] = {0};
    char dev_name[16] = {0};

    ddkc_loud("buf:%s, len:%d\n", buf, len);
    for (i = 0; i < argc; i++) {
        ddkc_loud("argv[%d]:%s\n", i, *(argv + i));
    }

    if (argc <= 2) {
        uart_test_usage();
        return;
    }

    port_id = atoi(argv[1]);
    ms = atoi(argv[2]);

    snprintf(dev_name, sizeof(dev_name), "/dev/ttyUART%d", port_id );
    ddkc_info("opening device:%s\r\n", dev_name);
    fd = open(dev_name, 0);

    if (fd < 0) {
        ddkc_err("open %s failed\r\n", dev_name);
        return;
    }
    ddkc_info("set baudrate to 1500000\r\n");
    ret = ioctl(fd, IOC_UART_SET_CFLAG, B1500000 | CS8);
    if (ret) {
        ddkc_err("ioctl on %s failed, ret:%d\r\n", dev_name, ret);
        close(fd);
        return;
    }
    ddkc_info("set baudrate done\r\n");

    do {
        ret = read(fd, buffer, sizeof(buffer));
        if (ret > 0) {
            for (i = 0; i < ret; i++)
                printf("%c", buffer[i]);
            printf("\r\n");
        }

        ret = write(fd, buffer, sizeof(buffer));
        if (ret != sizeof(buffer)) {
            ddkc_warn("write error, ret:%d\r\n", ret);
        }
        memset(buffer, 0, sizeof(buffer));
        usleep(100000);
        j++;
    } while(j < (ms/100));

    ddkc_info("closing %s\r\n", dev_name);
    close(fd);
    ddkc_info("%s closed\r\n", dev_name);

    return;
}

#if AOS_COMP_CLI
struct cli_command vfs_uart_cli_cmds[] = {
    { "uartw", "uart tx test", uart_write_test, },
    { "uartr", "uart rx test", uart_read_test, },
    { "uarte", "uart echo test", uart_echo_test, },
};

int vfs_uart_test_cmd_init(void)
{
    return aos_cli_register_commands(&vfs_uart_cli_cmds[0], sizeof(vfs_uart_cli_cmds) / sizeof(vfs_uart_cli_cmds[0]));
}

POST_DRIVER_ENTRY(vfs_uart_test_cmd_init)
#endif /* AOS_COMP_CLI */
