#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <aos/errno.h>
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include <drivers/u_ld.h>
#include <vfsdev/i2c_dev.h>
#include <aos/driver/i2c.h>

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

static int32_t aos_i2c_test(int32_t port, int32_t slave_addr, char *tx_buffer, uint32_t tx_cnt, char *rx_buffer, uint32_t rx_cnt, uint32_t loop, uint32_t period)
{
    int ret = -1;
    i2c_dev_handle_t slave = NULL;
    i2c_slave_config_t config;

    config.addr = slave_addr;
    config.addr_width = I2C_SLAVE_ADDR_WIDTH_7BIT;
    config.clk = I2C_BUS_CLK_400K;

    slave = aos_i2c_open(port, &config);
    if (!slave) {
        printf("i2c%d open failed\r\n", port);
        return -1;
    }

    if (tx_buffer && tx_cnt) {
        int i = loop;

        while (i--) {
            ret = aos_i2c_master_send(slave, tx_buffer, tx_cnt, 1000);
            if (ret != tx_cnt) {
                printf("i2c%d tx failed, addr:0x%x, tx_buffer:%p, tx_cnt:%d, ret:%d\n",
                    port, slave_addr, tx_buffer, tx_cnt, ret);
            } else {
                printf("i2c%d tx success, addr:0x%x, tx_buffer:%p, tx_cnt:%d\n",
                    port, slave_addr, tx_buffer, tx_cnt);
            }
            aos_msleep(period <= 50 ? 50 : period);
        }
    }

    if (rx_buffer && rx_cnt) {
        int i = loop;

        while (i--) {
            ret = aos_i2c_master_recv(slave, rx_buffer, rx_cnt, 1000);
            if (ret != rx_cnt) {
                printf("i2c%d rx failed, addr:0x%x, rx_buffer:%p, rx_cnt:%d, ret:%d\n",
                    port, slave_addr, rx_buffer, rx_cnt, ret);
            } else {
                printf("i2c%d rx success, addr:0x%x, rx_buffer:%p, rx_cnt:%d\n",
                    port, slave_addr, rx_buffer, rx_cnt);
            }
            aos_msleep(period <= 50 ? 50 : period);
        }
    }

    ret = aos_i2c_close(slave);
    if (ret) {
        printf("i2c%d close failed, ret:%d\r\n", port, ret);
    }

    return ret;
}

// command format: i2c_write <port> <slave_address> <tx_data> <tx data length> <loop> <period>
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

    //ret = vfs_i2c_test(port, slave_addr, tx_buffer, number, NULL, 0, loop, wait);
    ret = aos_i2c_test(port, slave_addr, tx_buffer, number, NULL, 0, loop, wait);

    if (!ret) {
        printf ("i2c comp write test success!\r\n");
    } else {
        printf ("i2c comp write test failed, ret:%d!\r\n", ret);
    }
    free(tx_buffer);

    return;
}

// command format: i2c_read <port> <slave_address> <rx data length> <loop> <period>
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
    //ret = vfs_i2c_test(port, slave_addr, NULL, 0, rx_buffer, number, loop, wait);
    ret = aos_i2c_test(port, slave_addr, NULL, 0, rx_buffer, number, loop, wait);
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



static int32_t aos_i2c_mem_test(int32_t port, int32_t slave_addr, uint16_t addr, uint8_t addr_len, char *tx_buffer, uint32_t tx_cnt, char *rx_buffer, uint32_t rx_cnt, uint32_t loop, uint32_t period)
{
    int ret = -1;
    i2c_dev_handle_t slave = NULL;
    i2c_slave_config_t config;

    config.addr = slave_addr;
    config.addr_width = I2C_SLAVE_ADDR_WIDTH_7BIT;
    config.clk = I2C_BUS_CLK_400K;

    slave = aos_i2c_open(port, &config);
    if (!slave) {
        printf("i2c%d open failed\r\n", port);
        return -1;
    }

    if (tx_buffer && tx_cnt) {
        int i = loop;

        while (i--) {
            ret = aos_i2c_mem_write(slave, addr, addr_len, tx_buffer, tx_cnt, 1000);
            if (ret != tx_cnt) {
                printf("i2c%d tx failed, slave addr:0x%x, memory address:0x%x, memory address length:%d, tx_buffer:%p, tx_cnt:%d, ret:%d\n",
                    port, slave_addr, addr, addr_len, tx_buffer, tx_cnt, ret);
            } else {
                printf("i2c%d tx success, slave addr:0x%x, memory address:0x%x, memory address length:%d, tx_buffer:%p, tx_cnt:%d\n",
                    port, slave_addr, addr, addr_len, tx_buffer, tx_cnt);
            }
            aos_msleep(period <= 50 ? 50 : period);
        }
    }

    if (rx_buffer && rx_cnt) {
        int i = loop;

        while (i--) {
            ret = aos_i2c_mem_read(slave, addr, addr_len, rx_buffer, rx_cnt, 1000);
            if (ret != rx_cnt) {
                printf("i2c%d rx failed, slave addr:0x%x, memory address:0x%x, memory address length:%d, rx_buffer:%p, tx_cnt:%d, ret:%d\n",
                    port, slave_addr, addr, addr_len, rx_buffer, tx_cnt, ret);
            } else {
                printf("i2c%d rx success, slave addr:0x%x, memory address:0x%x, memory address length:%d, rx_buffer:%p, tx_cnt:%d\n",
                    port, slave_addr, addr, addr_len, rx_buffer, tx_cnt);
            }
            aos_msleep(period <= 50 ? 50 : period);
        }
    }

    ret = aos_i2c_close(slave);
    if (ret) {
        printf("i2c%d close failed, ret:%d\r\n", port, ret);
    }

    return ret;
}

//command format: i2c_mem_read <port> <slave_address> <memory address> <rx data length> <loop> <period>
static void i2c_mem_read_test(int32_t argc, char **argv)
{
    int i = 0;
    int32_t ret = 0;
    int32_t port = argc > 1 ? atoi(argv[1]) : 1;
    int32_t slave_addr = argc > 2 ? atoi(argv[2]) : 0x1e;  // 0x1e is ap3216c's address
    uint16_t addr = argc > 3 ? atoi(argv[3]) : 0xc;        // memory address
    uint32_t number = argc > 4 ? atoi(argv[4]) : 1;        // number of bytes to read in single i2c rx procedure
    uint32_t loop = argc > 5 ? atoi(argv[5]) : 1;          // whether do i2c tx multiple times
    uint32_t wait = argc > 6 ? atoi(argv[6]) : 50;         // wait time during successive i2c rx
    char *rx_buffer = (char *)malloc(number);

    printf("i2c comp read test start\r\n");
    if (!rx_buffer) {
        printf("i2c comp read test failed, malloc memory failed\r\n");
        return;
    }
    memset (rx_buffer, 0, number);
    ret = aos_i2c_mem_test(port, slave_addr, addr, 1, NULL, 0, rx_buffer, number, loop, wait);
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


//command format: i2c_mem_write <port> <slave_address> <memory address> <tx data> <tx data length> <loop> <period>
static void i2c_mem_write_test(int32_t argc, char **argv)
{

    int32_t ret = 0;
    int32_t port = argc > 1 ? atoi(argv[1]) : 1;
    int32_t slave_addr = argc > 2 ? atoi(argv[2]): 0x1e; // 0x1e is ap3216c's address
    uint16_t addr = argc > 3 ? atoi(argv[3]) : 0x0;      // 0x0 is ap3216c's system configuration register
    char tx = argc > 4 ? atoi(argv[4]) : 0x3;              // 0x3 is the command to active ap3216c's ALS and PS+IR functions
    uint32_t number = argc > 5 ? atoi(argv[5]) : 1;      // number of bytes to send in single i2c tx procedure
    uint32_t loop = argc > 7 ? atoi(argv[6]) : 1;        // whether do i2c tx multiple times
    uint32_t wait = argc > 7 ? atoi(argv[7]) : 50;       // wait time during successive i2c tx
    char *tx_buffer = (char *)malloc(number);

    printf("i2c comp write test start\r\n");
    if (!tx_buffer) {
        printf("i2c comp write test failed, malloc memory failed\r\n");
        return;
    }
    memset (tx_buffer, tx, number);
    ret = aos_i2c_mem_test(port, slave_addr, addr, 1, tx_buffer, number, NULL, 0, loop, wait);
    if (!ret) {
        printf ("i2c comp write test success!\r\n");
    } else {
        printf ("i2c comp write test failed, ret:%d!\r\n", ret);
    }
    free(tx_buffer);

    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(i2c_write_test, i2c_write, i2c write operation example)
ALIOS_CLI_CMD_REGISTER(i2c_read_test, i2c_read, i2c read operation example)
ALIOS_CLI_CMD_REGISTER(i2c_mem_read_test, i2c_mem_read, i2c memory read operation example)
ALIOS_CLI_CMD_REGISTER(i2c_mem_write_test, i2c_mem_write, i2c memory write operation example)

#endif
