/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <fcntl.h>
#include <sys/ioctl.h>
#include <epoll/epoll.h>

#include "aos/vfs.h"
#include "aos/cli.h"

#include <drivers/u_ld.h>
#include <drivers/ddkc_log.h>
#include <hal2vfs/io_uart.h>
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

static void uart_write_test(char *buf, int len, int argc, char **argv) {
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

    ddkc_info("writing %s\r\n", string);
    ret = write(fd, string, strlen(string));
    ddkc_info("write return %d\r\n", ret);
    do {
        ret = read(fd, buffer, sizeof(buffer) - 1);
        if (ret > 0) {
            for (i = 0; i < ret; i++)
                printf("%c", buffer[i]);
            printf("\r\n");

            ret = write(fd, buffer, sizeof(buffer));
            if (ret != sizeof(buffer)) {
                ddkc_warn("write error, ret:%d\r\n", ret);
            }
        }

        memset(buffer, 0, sizeof(buffer));
        usleep(100000);
        j++;
    } while(j < 100);

    ddkc_info("closing %s\r\n", dev_name);
    close(fd);
    ddkc_info("%s closed\r\n", dev_name);
#if 0
    main_rpc_name = u_driver_get_main_service_name();

	ddkc_warn("*port_id:%d, string:%s, main_rpc_name:%s\n", port_id, string, main_rpc_name);
    ret = aos_rpc_open_sync(main_rpc_name, 256, &main_rpc_handle);
    ddkc_warn("open rpc service:%s %s, ret:%d, main_rpc_handle:%x\n",
                main_rpc_name, ret ? "failed" : "success", ret, main_rpc_handle);

    if (ret) {
        return;
    }

    int rmsg_len = 0;
    u_cdev_msg_t *rmsg = NULL;
    u_cdev_msg_ie_t *ie = NULL;
    u_fops_arg_u *arg = NULL;
    u_fops_result_u res;
    void *farg = NULL;
    struct aos_parcel_t reply;
#if 1
    memset(&res, 0, sizeof(res));
    ddkc_info("sizeof(u_cdev_msg_t):%d\n", sizeof(u_cdev_msg_t));
    ddkc_info("sizeof(u_cdev_msg_ie_t):%d\n", sizeof(u_cdev_msg_ie_t));
    ddkc_info("sizeof(u_fops_arg_u):%d\n", sizeof(u_fops_arg_u));
    rmsg_len = sizeof(u_cdev_msg_t) + sizeof(u_cdev_msg_ie_t) + sizeof(u_fops_arg_u) + strlen("/dev/ttyUART1") + 1;
    rmsg = malloc(rmsg_len);
    if (!rmsg)
        return -ENOMEM;
    rmsg->t = CDEV_MSG_FOPS;
    rmsg->l = rmsg_len - sizeof(u_cdev_msg_t);
    ie = (u_cdev_msg_ie_t *)rmsg->v;
    ie->t = FOPS_OPEN;
    ie->l = rmsg->l - sizeof(u_cdev_msg_ie_t);
    arg = (u_fops_arg_u *)ie->v;
    arg->open.flags = 0;
    strcpy(arg->open.path, "/dev/ttyUART1");
    ddkc_info("main_rpc_handle:%d, rmsg_len:%d, rmsg:%p\n", main_rpc_handle, rmsg_len, rmsg);
    ret = aos_rpc_call_sync(main_rpc_handle, &reply, 1, rmsg_len, rmsg);
    if (ret) {
        ddkc_warn("aos_rpc_call_sync returns %d\n", ret);
        return;
    }

    /* parse response */
    aos_parcel_get(AP_BUF, &res, &len, &reply);
    ddkc_info("open status:%d, farg:%p\n", res.open.status, res.open.farg);
    farg = res.open.farg;
    free(rmsg);
    rmsg = NULL;
#else
    memset(&res, 0, sizeof(res));
    ddkc_info("sizeof(u_cdev_msg_t):%d\n", sizeof(u_cdev_msg_t));
    ddkc_info("sizeof(u_cdev_msg_ie_t):%d\n", sizeof(u_cdev_msg_ie_t));
    ddkc_info("sizeof(u_fops_arg_u):%d\n", sizeof(u_fops_arg_u));
    rmsg_len = sizeof(u_cdev_msg_t) + sizeof(u_cdev_msg_ie_t) + sizeof(arg->open);
    int rmsg_len1 = sizeof(u_cdev_msg_t) + sizeof(u_cdev_msg_ie_t) + sizeof(u_fops_arg_u);
    rmsg = malloc(rmsg_len);
    if (!rmsg)
        return -ENOMEM;
    rmsg->t = CDEV_MSG_FOPS;
    rmsg->l = rmsg_len - sizeof(u_cdev_msg_t);
    ie = (u_cdev_msg_ie_t *)rmsg->v;
    ie->t = FOPS_OPEN;
    ie->l = rmsg->l - sizeof(u_cdev_msg_ie_t);
    arg = (u_fops_arg_u *)ie->v;
    arg->open.flags = 0;
    ddkc_info("main_rpc_handle:%d, rmsg_len:%d, rmsg_len1:%d, rmsg:%p\n", main_rpc_handle, rmsg_len, rmsg_len1, rmsg);
    ret = aos_rpc_call_sync(main_rpc_handle, &reply, 2, rmsg_len, rmsg, strlen("/dev/ttyUART1") + 1, "/dev/ttyUART1");
    if (ret) {
        ddkc_warn("aos_rpc_call_sync returns %d\n", ret);
        //return;
    }

    /* parse response */
    aos_parcel_get(AP_BUF, &res, &len, &reply);
    ddkc_info("open status:%d, farg:%p\n", res.open.status, res.open.farg);
    farg = res.open.farg;
    free(rmsg);
    rmsg = NULL;
#endif

    uart_rpc_name = "rpc-0-ttyUART1";
    ret = aos_rpc_open_sync(uart_rpc_name, 256, &uart_rpc_handle);
    ddkc_warn("open rpc service:%s %s, ret:%d, uart_rpc_handle:%x\n",
            uart_rpc_name, ret ? "failed" : "success", ret, uart_rpc_handle);
    if (ret) {
        goto err;
    }
    // TODO: write test
    rmsg_len = sizeof(u_cdev_msg_t) + sizeof(u_cdev_msg_ie_t) + sizeof(u_fops_arg_u) + strlen("uart_rpc_test\r\n");
    rmsg = malloc(rmsg_len);
    if (!rmsg)
        return -ENOMEM;
    rmsg->t = CDEV_MSG_FOPS;
    rmsg->l = rmsg_len - sizeof(u_cdev_msg_t);
    ie = (u_cdev_msg_ie_t *)rmsg->v;
    ie->t = FOPS_WRITE;
    ie->l = rmsg->l - sizeof(u_cdev_msg_ie_t);
    arg = (u_fops_arg_u *)ie->v;
    arg->write.farg = farg;
    arg->write.len = strlen("uart_rpc_test\r\n");
    strcpy(arg->write.buf, "uart_rpc_test\r\n");
    ddkc_info("uart_rpc_handle:%d, rmsg_len:%d, rmsg:%p\n", uart_rpc_handle, rmsg_len, rmsg);
    ret = aos_rpc_call_sync(uart_rpc_handle, &reply, 1, rmsg_len, rmsg);
    if (ret) {
        ddkc_warn("aos_rpc_call_sync returns %d\n", ret);
        //return;
    }

    /* parse response */
    aos_parcel_get(AP_BUF, &res, &len, &reply);
    ddkc_info("write status:%d, len:%p\n", res.write.status, res.write.len);
    free(rmsg);

    // TODO: ethan - read test
    u_fops_result_u *p_res;
    p_res = malloc(sizeof(u_fops_result_u) + sizeof(read_buf));
    rmsg_len = sizeof(u_cdev_msg_t) + sizeof(u_cdev_msg_ie_t) + sizeof(u_fops_arg_u);
    rmsg = malloc(rmsg_len);
    if (!rmsg || !p_res)
        return -ENOMEM;
    rmsg->t = CDEV_MSG_FOPS;
    rmsg->l = rmsg_len - sizeof(u_cdev_msg_t);
    ie = (u_cdev_msg_ie_t *)rmsg->v;
    ie->t = FOPS_READ;
    ie->l = rmsg->l - sizeof(u_cdev_msg_ie_t);
    arg = (u_fops_arg_u *)ie->v;
    arg->read.farg = farg;
    arg->read.len = sizeof(read_buf);
    ddkc_info("uart_rpc_handle:%d, rmsg_len:%d, rmsg:%p\n", uart_rpc_handle, rmsg_len, rmsg);
    ret = aos_rpc_call_sync(uart_rpc_handle, &reply, 1, rmsg_len, rmsg);
    if (ret) {
        ddkc_warn("aos_rpc_call_sync returns %d\n", ret);
        //return;
    }

    /* parse response */
    aos_parcel_get(AP_BUF, p_res, &len, &reply);
    ddkc_info("read status:%d, len:%p\n", p_res->write.status, p_res->write.len);
    free(rmsg);
    rmsg = NULL;
    free(p_res);
    p_res = NULL;

    // TODO: ioctl test
    p_res = malloc(sizeof(u_fops_result_u) + sizeof(read_buf));
    rmsg_len = sizeof(u_cdev_msg_t) + sizeof(u_cdev_msg_ie_t) + sizeof(u_fops_arg_u);
    rmsg = malloc(rmsg_len);
    if (!rmsg || !p_res)
        return -ENOMEM;
    rmsg->t = CDEV_MSG_FOPS;
    rmsg->l = rmsg_len - sizeof(u_cdev_msg_t);
    ie = (u_cdev_msg_ie_t *)rmsg->v;
    ie->t = FOPS_IOCTL;
    ie->l = rmsg->l - sizeof(u_cdev_msg_ie_t);
    arg = (u_fops_arg_u *)ie->v;
    arg->ioctl.farg = farg;
    arg->ioctl.cmd = IOC_UART_SET_CFLAG;
    arg->ioctl.arg = B1500000 | CS8;
    ddkc_info("uart_rpc_handle:%d, rmsg_len:%d, rmsg:%p\n", uart_rpc_handle, rmsg_len, rmsg);
    ret = aos_rpc_call_sync(uart_rpc_handle, &reply, 1, rmsg_len, rmsg);
    if (ret) {
        ddkc_warn("aos_rpc_call_sync returns %d\n", ret);
        //return;
    }

    /* parse response */
    aos_parcel_get(AP_BUF, p_res, &len, &reply);
    ddkc_info("ioctl status:%d, len:%p\n", p_res->write.status, p_res->write.len);
    free(rmsg);
    rmsg = NULL;
    free(p_res);
    p_res = NULL;

    p_res = malloc(sizeof(u_fops_result_u) + sizeof(read_buf));
    rmsg_len = sizeof(u_cdev_msg_t) + sizeof(u_cdev_msg_ie_t) + sizeof(u_fops_arg_u);
    rmsg = malloc(rmsg_len);
    if (!rmsg || !p_res)
        return -ENOMEM;
    rmsg->t = CDEV_MSG_FOPS;
    rmsg->l = rmsg_len - sizeof(u_cdev_msg_t);
    ie = (u_cdev_msg_ie_t *)rmsg->v;
    ie->t = FOPS_CLOSE;
    ie->l = rmsg->l - sizeof(u_cdev_msg_ie_t);
    arg = (u_fops_arg_u *)ie->v;
    arg->close.farg = farg;
    ddkc_info("main_rpc_handle:%d, rmsg_len:%d, rmsg:%p\n", main_rpc_handle, rmsg_len, rmsg);
    ret = aos_rpc_call_sync(main_rpc_handle, &reply, 1, rmsg_len, rmsg);
    if (ret) {
        ddkc_warn("aos_rpc_call_sync returns %d\n", ret);
        //return;
    }

    /* parse response */
    aos_parcel_get(AP_BUF, p_res, &len, &reply);
    ddkc_info("ioctl status:%d, len:%p\n", p_res->write.status, p_res->write.len);
    free(rmsg);
    rmsg = NULL;
    free(p_res);
    p_res = NULL;

err:
    if (rmsg)
        free(rmsg);

    if (p_res)
        free(p_res);

    aos_rpc_close(main_rpc_handle);
    main_rpc_handle = 0;

    aos_rpc_close(uart_rpc_handle);
    uart_rpc_handle = 0;
#endif
    return;
}

static void uart_read_test(char *buf, int len, int argc, char **argv) {
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

    ddkc_info("set baudrate to 115200\r\n");
    ret = ioctl(fd, IOC_UART_SET_CFLAG, B115200 | CS8);
    if (ret) {
        ddkc_err("ioctl on %s failed, ret:%d\r\n", dev_name, ret);
        close(fd);
        return -1;
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

static void uart_echo_test(char *buf, int len, int argc, char **argv) {
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

struct cli_command vfs_uart_cli_cmds[] = {
    { "uartw","uart tx test", uart_write_test },
    { "uartr","uart rx test", uart_read_test },
    { "uarte","uart echo test", uart_echo_test },
};

int vfs_uart_test_cmd_init(void) {
    return aos_cli_register_commands(&vfs_uart_cli_cmds[0],
                                     sizeof(vfs_uart_cli_cmds)/sizeof(vfs_uart_cli_cmds[0]));
}

POST_DRIVER_ENTRY(vfs_uart_test_cmd_init)
