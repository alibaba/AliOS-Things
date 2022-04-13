#include <stdint.h>
#include <stdio.h>
#include <aos/errno.h>
#include <aos/cli.h>
#include <vfsdev/i2c_dev.h>

static int32_t vfs_i2c_test(int32_t port, int32_t slave_addr, char *tx_buffer, uint32_t tx_cnt, char *rx_buffer, uint32_t rx_cnt, uint32_t loop, uint32_t period)
{
    int32_t ret = -1;
    int32_t fd = -1;
    char name[16] = {0};
    io_i2c_control_u c;
    io_i2c_data_t d;

    snprintf(name, sizeof(name), "/dev/i2c%d", port);
    fd = open(name, 0);
    printf("open %s %s, fd:%d\r\n", name, fd > 0 ? "success" : "fail", fd);

    if (fd < 0) {
        return -EIO;
    }
    // set slave address, NOTICE that the address mode is 7-bit mode by default
    c.c.addr = slave_addr;
    c.c.addr_width = 0;
    c.c.role = 1;
    ret = ioctl(fd, IOC_I2C_SET_CONFIG, (unsigned long)&c);
    if (ret) {
        printf("set config to %s failed, ret:%d\n", name, ret);
        close(fd);
        return -EIO;
    } else {
        printf("set addr:%x, config to 0x%x success\n", c.c.addr, name);
    }

    // set to 400k by default
    c.freq = 400000;
    ret += ioctl(fd, IOC_I2C_SET_FREQ, (unsigned long)&c);

    if (ret) {
        printf("set freq/config to %s failed, ret:%d\n", name, ret);
        close(fd);
        return -EIO;
    } else {
        printf("set freq to %d, config to 0x%x success\n", c.freq, name);
    }

    if (tx_buffer && tx_cnt) {
        int i = loop;
        d.addr = slave_addr;
        d.data = tx_buffer;
        d.length = tx_cnt;
        d.maddr = 0x0;
        d.mlength = 0x0;
        d.timeout = 1000;

        while (i--) {
            ret = ioctl(fd, IOC_I2C_MASTER_TX, (unsigned long)&d);
            if (ret) {
                printf("%s tx failed, addr:0x%x, tx_buffer:%p, tx_cnt:%d, ret:%d\n",
                    name, slave_addr, tx_buffer, tx_cnt, ret);
            } else {
                printf("%s tx success, addr:0x%x, tx_buffer:%p, tx_cnt:%d\n",
                    name, slave_addr, tx_buffer, tx_cnt);
            }
            aos_msleep(period <= 50 ? 50 : period);
        }
    }
    if (rx_buffer && rx_cnt) {
        int i = loop;
        d.addr = slave_addr;
        d.data = rx_buffer;
        d.length = rx_cnt;
        d.maddr = 0x0;
        d.mlength = 0x0;
        d.timeout = 1000;
        while (i--) {
            ret = ioctl(fd, IOC_I2C_MASTER_RX, (unsigned long)&d);
            if (ret) {
                printf("%s rx failed, addr:0x%x, rx_buffer:%p, rx_cnt:%d, ret:%d\n",
                    name, slave_addr, rx_buffer, rx_cnt, ret);
            } else {
                printf("%s rx success, addr:0x%x, rx_buffer:%p, rx_cnt:%d\n",
                    name, slave_addr, rx_buffer, rx_cnt);
            }
            aos_msleep(period <= 50 ? 50 : period);
        }
    }

    close(fd);

    return ret;
}

static void i2c_write_test(int32_t argc, char **argv)
{
    int32_t ret = 0;
    int32_t port = argc > 1 ? atoi(argv[1]) : 1;
    int32_t slave_addr = argc > 2 ? atoi(argv[2]): 0x40; // 0x40 is SI7006 humiture's address
    char tx = argc > 3 ? atoi(argv[3]) : 0xF3;           // 0xF3 is the command to control SI7006 to measure temperature
    uint32_t number = argc > 4 ? atoi(argv[4]) : 1;      // number of bytes to send in single i2c tx procedure
    uint32_t loop = argc > 5 ? atoi(argv[5]) : 1;        // whether do i2c tx multiple times
    uint32_t wait = argc > 6 ? atoi(argv[6]) : 50;       // wait time during successive i2c tx
    char *tx_buffer = (char *)malloc(number);

    printf("i2c comp write test start\r\n");
    if (!tx_buffer) {
        printf("i2c comp write test failed, malloc memory failed\r\n");
        return;
    }
    memset (tx_buffer, tx, number);
    ret = vfs_i2c_test(port, slave_addr, tx_buffer, number, NULL, 0, loop, wait);
    if (!ret) {
        printf ("i2c comp write test success!\r\n");
    } else {
        printf ("i2c comp write test failed, ret:%d!\r\n", ret);
    }
    free(tx_buffer);
    return;
}

static void i2c_read_test(int32_t argc, char **argv)
{
    int i = 0;
    int32_t ret = 0;
    int32_t port = argc > 1 ? atoi(argv[1]) : 1;
    int32_t slave_addr = argc > 2 ? atoi(argv[2]) : 0x40; // 0x40 is SI7006 humiture's address
    uint32_t number = argc > 3 ? atoi(argv[3]) : 1;      // number of bytes to read in single i2c rx procedure
    uint32_t loop = argc > 4 ? atoi(argv[4]) : 1;        // whether do i2c tx multiple times
    uint32_t wait = argc > 5 ? atoi(argv[5]) : 50;       // wait time during successive i2c rx
    char *rx_buffer = (char *)malloc(number);

    printf("i2c comp read test start\r\n");
    if (!rx_buffer) {
        printf("i2c comp read test failed, malloc memory failed\r\n");
        return;
    }
    memset (rx_buffer, 0, number);
    ret = vfs_i2c_test(port, slave_addr, NULL, 0, rx_buffer, number, loop, wait);
    if (!ret) {
        printf ("i2c comp read test success!\r\n");
        for (i = 0; i < number; i++) {
            printf ("0x%02x \n", rx_buffer[i]);
        }
        printf("\r\n");
    } else {
        printf ("i2c comp read test failed, ret:%d!\r\n", ret);
    }
    free(rx_buffer);
    return;
}

#ifdef AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(i2c_write_test, i2c_write, i2c write operation example)
ALIOS_CLI_CMD_REGISTER(i2c_read_test, i2c_read, i2c read operation example)
#endif
