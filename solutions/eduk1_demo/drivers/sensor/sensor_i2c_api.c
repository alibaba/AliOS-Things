#include <stdint.h>
#include <stddef.h>
#include <aos/errno.h>
#include <vfsdev/i2c_dev.h>
#include "ulog/ulog.h"

#define EDU_MAX_I2C_PORT 4
static int g_i2c_fd[EDU_MAX_I2C_PORT] = {-1, -1, -1, -1};

int32_t sensor_i2c_open(uint32_t port, uint16_t dev_addr, uint32_t freq, uint32_t address_width)
{
    int ret = 0;
    int *p_fd = NULL;
    char name[16] = {0};
    io_i2c_control_u c;

    if (port >= EDU_MAX_I2C_PORT)
        return -EINVAL;

    p_fd = &g_i2c_fd[port];

    if (*p_fd >= 0) {
        LOGE("SENSOR", "i2c%d is already opened\r\n", port, *p_fd);
        return -EALREADY;
    }
    memset(&c, 0, sizeof(io_i2c_control_u));

    snprintf(name, sizeof(name), "/dev/i2c%d", port);

    *p_fd = open(name, 0);

    if (*p_fd < 0) {
        LOGE ("SENSOR", "open %s failed, fd:%d\r\n", name, *p_fd);
        return -EIO;
    }
    c.c.addr = dev_addr;            /* sensor's address */
    c.c.addr_width = address_width; /* 7-bit address */
    c.c.role = 1;                   /* master mode */
    ret = ioctl(*p_fd, IOC_I2C_SET_CONFIG, (unsigned long)&c);

    c.freq = freq;
    ret += ioctl(*p_fd, IOC_I2C_SET_FREQ, (unsigned long)&c);

    if (ret) {
        LOGE("SENSOR", "IOC_I2C_SET_CONFIG or IOC_I2C_SET_FREQ on %s failed, ret:%d\r\n", name, ret);
        close(*p_fd);
        *p_fd = -1;
    }
    return ret;
}

int32_t sensor_i2c_master_send(uint32_t port, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    int *p_fd = NULL;
    int ret = -1;
    io_i2c_data_t d;

    p_fd = &g_i2c_fd[port];
    if ((port >= EDU_MAX_I2C_PORT) || (*p_fd < 0))
        return -EINVAL;

    memset(&d, 0, sizeof(io_i2c_data_t));

    d.addr = dev_addr;
    d.data = data;
    d.length = size;
    d.maddr = 0;
    d.mlength = 0;
    d.timeout = timeout;

    ret = ioctl(*p_fd, IOC_I2C_MASTER_TX, (unsigned long)&d);

    return ret;
}

int32_t sensor_i2c_master_recv(uint32_t port, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    int *p_fd = NULL;
    int ret = -1;
    io_i2c_data_t d;

    p_fd = &g_i2c_fd[port];
    if ((port >= EDU_MAX_I2C_PORT) || (*p_fd < 0))
        return -EINVAL;

    memset(&d, 0, sizeof(io_i2c_data_t));
    d.addr = dev_addr;
    d.data = data;
    d.length = size;
    d.maddr = 0;
    d.mlength = 0;
    d.timeout = timeout;

    ret = ioctl(*p_fd, IOC_I2C_MASTER_RX, (unsigned long)&d);

    return ret;
}

int32_t sensor_i2c_mem_write(uint32_t port, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    int *p_fd = NULL;
    int ret = -1;
    io_i2c_data_t d;

    p_fd = &g_i2c_fd[port];
    if ((port >= EDU_MAX_I2C_PORT) || (*p_fd < 0))
        return -EIO;

    memset(&d, 0, sizeof(io_i2c_data_t));
    d.addr = dev_addr;
    d.data = data;
    d.length = size;
    d.maddr = mem_addr;
    d.mlength = mem_addr_size;
    d.timeout = timeout;

    ret = ioctl(*p_fd, IOC_I2C_MEM_TX, (unsigned long)&d);
    return ret;
}

int32_t sensor_i2c_mem_read(uint32_t port, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    int *p_fd = NULL;
    int ret = -1;
    io_i2c_data_t d;

    p_fd = &g_i2c_fd[port];
    if ((port >= EDU_MAX_I2C_PORT) || (*p_fd < 0))
        return -EIO;

    memset(&d, 0, sizeof(io_i2c_data_t));

    d.addr = dev_addr;
    d.data = data;
    d.length = size;
    d.maddr = mem_addr;
    d.mlength = mem_addr_size;
    d.timeout = timeout;

    ret = ioctl(*p_fd, IOC_I2C_MEM_RX, (unsigned long)&d);
    return ret;
}

int32_t sensor_i2c_close(uint32_t port)
{
    int *p_fd = NULL;

    p_fd = &g_i2c_fd[port];
    if ((port >= EDU_MAX_I2C_PORT) || (*p_fd < 0)) {
        LOGE ("SENSOR", "invalid port:%d or fd:%d\r\n", port, *p_fd);
        return -EIO;
    }

    close(*p_fd);

    *p_fd = -1;

    return 0;
}
