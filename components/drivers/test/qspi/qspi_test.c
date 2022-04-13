/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>

#include <epoll/epoll.h>
#include <devicevfs/devicevfs.h>
#include <hal2vfs/io_qspi.h>
#include <drivers/char/u_device.h>

int vfs_qspi_test(void)
{
    char buffer[16];
    int ret = 0;
    int fd = 0;
    ioc_qspi_cmd_t cmd;
    ioc_qspi_data_t data;
    ioc_qspi_transfer_t transfer;

    fd = open("/dev/qspi0", 0);
    ddkc_dbg("open spi %s, fd:%d\r\n", fd > 0 ? "success" : "fail", fd);

    if (fd >= 0) {

        ret = ioctl(fd, IOC_QSPI_SET_CFLAG, (unsigned long)(QSPI_NO_CS | QSPI_TRANSFER_PIO | QSPI_BUS_4_LINE | QSPI_MODE_0));
        ddkc_dbg("set cflags to qspi return %d\r\n", ret);

        memset(&cmd, 0, sizeof(cmd));
        cmd.instruction.skip = 0;
        cmd.instruction.lines = 1;
        cmd.instruction.value = 0x10;

        cmd.address.skip = 0;
        cmd.address.lines = 4;
        cmd.address.size = 1;
        cmd.address.value = 0x55;

        cmd.alt.skip = 0;
        cmd.alt.lines = 4;
        cmd.alt.size = 1;
        cmd.alt.value = 0xBB;

        cmd.dummy_count = 10;

        cmd.data.lines = 4;

        ddkc_dbg("send pure command, instruction:%x, address:%x, alt data:0x%x, dummy_cnt:%d\r\n",
               cmd.instruction.value, cmd.address.value, cmd.alt.value, cmd.dummy_count);

        ret = ioctl(fd, IOC_QSPI_SEND_CMD, (unsigned long)&cmd);
        ddkc_dbg("send command to qspi return %d\r\n", ret);

        ret = ioctl(fd, IOC_QSPI_SET_FREQ, (unsigned long)1000000);
        ddkc_dbg("set frequency to qspi return %d\r\n", ret);

        ret = ioctl(fd, IOC_QSPI_SET_FREQ, (unsigned long)2000000);
        ddkc_dbg("set frequency to qspi return %d\r\n", ret);


        memset(&data, 0, sizeof(data));
        data.tx_buf = (const char *)buffer;
        data.tx_size = sizeof(buffer);
        data.timeout = 100;

        transfer.cmd = &cmd;
        transfer.data = &data;

        ddkc_dbg("command + send data\r\n");
        ret = ioctl(fd, IOC_QSPI_SEND_DATA, (unsigned long)&transfer);
        ddkc_dbg("send command + data to qspi return %d\r\n", ret);

        memset(&data, 0, sizeof(data));
        data.rx_buf = buffer;
        data.rx_size = sizeof(buffer);
        data.timeout = 100;
        ddkc_dbg("command + recv data\r\n");
        ret = ioctl(fd, IOC_QSPI_RECV_DATA, (unsigned long)&transfer);
        ddkc_dbg("send command + recv data to qspi return %d\r\n", ret);

        memset(&data, 0, sizeof(data));
        data.tx_buf = buffer;
        data.tx_size = sizeof(buffer);
        data.rx_buf = buffer;
        data.rx_size = sizeof(buffer);
        data.timeout = 100;
        ddkc_dbg("command + send/recv data\r\n");

        ret = ioctl(fd, IOC_QSPI_SEND_RECV_DATA, (unsigned long)&transfer);
        ddkc_dbg("command + send data + recv data to/from qspi return %d\r\n", ret);

        ddkc_dbg("QSPI write operation\r\n");
        ret = write(fd, "test", sizeof("test"));
        ddkc_dbg("write to spi return %d\r\n", ret);

        ddkc_dbg("QSPI read operation\r\n");
        ret = read(fd, buffer, sizeof(buffer));
        ddkc_dbg("read from spi return %d\r\n", ret);

        close(fd);
    }
    return 0;
}


