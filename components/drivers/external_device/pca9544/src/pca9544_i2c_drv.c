#include "pca9544_i2c_drv.h"
#include <aos/errno.h>
#include <stddef.h>
#include <stdint.h>
#include <vfsdev/i2c_dev.h>

#define I2C_PORT_MAX 4

static int g_i2c_fd[I2C_PORT_MAX] = { -1, -1, -1, -1 };

/**********************************************************
 * @fun    pca9544_i2c_init
 * @breif  pca9544 i2c initialization
 * @param  i2c:the pointer for i2c configuration
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t pca9544_i2c_init(i2c_dev_t *i2c)
{
    int32_t          ret      = 0;
    int32_t          *p_fd    = NULL;
    char             name[16] = { 0 };
    io_i2c_control_u i2c_ctrl;

    if (!i2c) {
        printf("i2c is null\r\n");
        return -EINVAL;
    }

    if (i2c->port >= I2C_PORT_MAX) {
        return -EINVAL;
    }

    p_fd = &g_i2c_fd[i2c->port];
    if (*p_fd >= 0) {
        printf("i2c port[%d] isn't ready\r\n", i2c->port);
        return -EALREADY;
    }

    memset(&i2c_ctrl, 0, sizeof(io_i2c_control_u));

    snprintf(name, sizeof(name), "/dev/i2c%d", i2c->port);

    *p_fd = open(name, 0);
    if (*p_fd < 0) {
        printf("open %s failed, fd:%d\r\n", name, *p_fd);
        return -EIO;
    }

    i2c_ctrl.c.addr       = i2c->config.dev_addr;      /* sensor's address */
    i2c_ctrl.c.addr_width = i2c->config.address_width; /* 7-bit address */
    i2c_ctrl.c.role       = 1;                         /* master mode */
    ret                   = ioctl(*p_fd, IOC_I2C_SET_CONFIG, (unsigned long)&i2c_ctrl);
    if (ret) {
        printf("set i2c config on %s failed\r\n", name);
        return -EINVAL;
    }

    i2c_ctrl.freq = i2c->config.freq;
    ret           = ioctl(*p_fd, IOC_I2C_SET_FREQ, (unsigned long)&i2c_ctrl);
    if (ret) {
        printf("IOC_I2C_SET_CONFIG or IOC_I2C_SET_FREQ on %s failed, ret:%d\r\n", name, ret);
        close(*p_fd);
        *p_fd = -1;
    }

    return ret;
}

/********************************************************
 * @fun    pca9544_i2c_master_send
 *
 * @param[in]  i2c            the i2c device
 * @param[in]  dev_addr       device address
 * @param[in]  data           i2c master send data
 * @param[in]  size           i2c master send data size
 * @param[in]  timeout        timeout in milisecond, set this value to
 *HAL_WAIT_FOREVER if you want to wait forever
 *
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t pca9544_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t* data, uint16_t size, uint32_t timeout)
{
    int32_t *p_fd = NULL;
    int32_t ret  = -1;
    io_i2c_data_t i2c_data;

    if (!i2c) {
        return -EINVAL;
    }

    if (i2c->port >= I2C_PORT_MAX) {
        return -EINVAL;
    }

    p_fd = &g_i2c_fd[i2c->port];
    if (!p_fd || (*p_fd < 0)) {
        printf("i2c port[%d] has not been initialized\r\n", i2c->port);
        return -EINVAL;
    }

    i2c_data.addr    = dev_addr;
    i2c_data.data    = data;
    i2c_data.length  = size;
    i2c_data.maddr   = 0;
    i2c_data.mlength = 0;
    i2c_data.timeout = timeout;

    ret = ioctl(*p_fd, IOC_I2C_MASTER_TX, (unsigned long)&i2c_data);

    return ret;
}

/********************************************************
 * @fun    pca9544_i2c_master_recv
 *
 * @param[in]  i2c            the i2c device
 * @param[in]  dev_addr       device address
 * @param[in]  data           i2c master recv data
 * @param[in]  size           i2c master recv data size
 * @param[in]  timeout        timeout in milisecond, set this value to
 *HAL_WAIT_FOREVER if you want to wait forever
 *
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t pca9544_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t *p_fd = NULL;
    int32_t  ret  = -1;
    io_i2c_data_t i2c_data;

    if (!i2c) {
        printf("i2c is null\r\n");
        return -EINVAL;
    }

    if (i2c->port >= I2C_PORT_MAX) {
        return -EINVAL;
    }

    p_fd = &g_i2c_fd[i2c->port];
    if (!p_fd || (*p_fd < 0)) {
        printf("i2c port[%d] has not been initialized\r\n", i2c->port);
        return -EINVAL;
    }

    i2c_data.addr    = dev_addr;
    i2c_data.data    = data;
    i2c_data.length  = size;
    i2c_data.maddr   = 0;
    i2c_data.mlength = 0;
    i2c_data.timeout = timeout;

    ret = ioctl(*p_fd, IOC_I2C_MASTER_RX, (unsigned long)&i2c_data);

    return 0;
}

/*********************************************************
 * @fun    pca9544_i2c_mem_write
 * @param[in]  i2c            the i2c device
 * @param[in]  dev_addr       device address
 * @param[in]  mem_addr       register address
 * @param[in]  mem_addr_size  register address size
 * @param[in]  data           i2c master send data
 * @param[in]  size           i2c master send data size
 * @param[in]  timeout        timeout in milisecond, set this value to
 *HAL_WAIT_FOREVER if you want to wait forever
 *
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t pca9544_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size, const uint8_t* data, uint16_t size, uint32_t timeout)
{
    int32_t *p_fd = NULL;
    int32_t  ret  = -1;
    io_i2c_data_t i2c_data;

    if (!i2c) {
        return -EINVAL;
    }

    if (i2c->port >= I2C_PORT_MAX) {
        return -EINVAL;
    }

    p_fd = &g_i2c_fd[i2c->port];
    if (!p_fd || (*p_fd < 0)) {
        printf("i2c port[%d] has not been initialized\r\n", i2c->port);
        return -EINVAL;
    }

    i2c_data.addr    = dev_addr;
    i2c_data.data    = data;
    i2c_data.length  = size;
    i2c_data.maddr   = mem_addr;
    i2c_data.mlength = mem_addr_size;
    i2c_data.timeout = timeout;

    ret = ioctl(*p_fd, IOC_I2C_MEM_TX, (unsigned long)&i2c_data);
    return ret;
}

/*********************************************************
 * @fun    pca9544_i2c_mem_read
 * @param[in]  i2c            the i2c device
 * @param[in]  dev_addr       device address
 * @param[in]  mem_addr       register address
 * @param[in]  mem_addr_size  register address size
 * @param[in]  data           i2c master send data
 * @param[in]  size           i2c master send data size
 * @param[in]  timeout        timeout in milisecond, set this value to
 *HAL_WAIT_FOREVER if you want to wait forever
 *
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t pca9544_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size, uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t *p_fd = NULL;
    int32_t ret  = -1;
    io_i2c_data_t i2c_data;

    if (!i2c) {
        return -EINVAL;
    }

    if (i2c->port >= I2C_PORT_MAX) {
        return -EINVAL;
    }

    p_fd = &g_i2c_fd[i2c->port];
    if (!p_fd || (*p_fd < 0)) {
        printf("i2c port[%d] has not been initialized\r\n", i2c->port);
        return -EINVAL;
    }

    memset(&i2c_data, 0, sizeof(io_i2c_data_t));

    i2c_data.addr    = dev_addr;
    i2c_data.data    = data;
    i2c_data.length  = size;
    i2c_data.maddr   = mem_addr;
    i2c_data.mlength = mem_addr_size;
    i2c_data.timeout = timeout;

    ret = ioctl(*p_fd, IOC_I2C_MEM_RX, (unsigned long)&i2c_data);
    return ret;
}

/********************************************************
 * @fun    pca9544_i2c_finalize
 * @param[in]  i2c            the i2c device
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t pca9544_i2c_finalize(i2c_dev_t *i2c)
{
    int32_t  ret  = 0;
    int32_t *p_fd = NULL;

    if (!i2c || !i2c->priv) {
        return -EINVAL;
    }

    if (i2c->port >= I2C_PORT_MAX) {
        return -EINVAL;
    }

    p_fd = &g_i2c_fd[i2c->port];
    if (!p_fd || (*p_fd < 0)) {
        printf("i2c port[%d] has not been initialized\r\n", i2c->port);
        return -EINVAL;
    }

    close(*p_fd);

    *p_fd = -1;

    return 0;
}
