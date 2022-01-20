#include <errno.h>
#include <driver/i2c.h>
#include <aos_hal_i2c_internal.h>

int32_t aos_hal_i2c_init(aos_hal_i2c_dev_t *i2c)
{
    i2c_config_t config = {
        .sda_pullup_en = true,
        .scl_pullup_en = true,
        /* remove as not supported by IDF-V4.2 */
        /* .clk_flags = 0, */
    };

    if (!i2c)
        return -EINVAL;

    switch (i2c->port) {
    case 0:
        config.sda_io_num = 21;
        config.scl_io_num = 22;
        break;
    case 1:
        config.sda_io_num = 32;
        config.scl_io_num = 33;
        break;
    default:
        return -EINVAL;
    }

    switch (i2c->config.address_width) {
    case I2C_HAL_ADDRESS_WIDTH_7BIT:
        break;
    default:
        return -EINVAL;
    }

    if (i2c->config.freq > 0 && i2c->config.freq <= 1000000)
        config.master.clk_speed = i2c->config.freq;
    else
        return -EINVAL;

    switch (i2c->config.mode) {
    case AOS_HAL_I2C_MODE_MASTER:
        config.mode = I2C_MODE_MASTER;
        break;
    default:
        return -EINVAL;
    }

    (void)i2c_driver_delete(i2c->port);

    if (i2c_param_config(i2c->port, &config) != ESP_OK)
        return -EINVAL;

    if (i2c_set_timeout(i2c->port, I2C_APB_CLK_FREQ / 1000000 * 10000) != ESP_OK)
        return -EINVAL;

    if (i2c_driver_install(i2c->port, config.mode, 0, 0, 0) != ESP_OK)
        return -EINVAL;

    return 0;
}

int32_t aos_hal_i2c_finalize(aos_hal_i2c_dev_t *i2c)
{
    if (!i2c)
        return -EINVAL;

    switch (i2c->port) {
    case 0:
        break;
    case 1:
        break;
    default:
        return -EINVAL;
    }

    if (i2c_driver_delete(i2c->port) != ESP_OK)
        return -EINVAL;

    return 0;
}

int32_t aos_hal_i2c_master_send(aos_hal_i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                                uint16_t size, uint32_t timeout)
{
    i2c_cmd_handle_t cmd;

    if (!i2c || !data || size == 0)
        return -EINVAL;

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, (uint8_t *)data, size, true);
    i2c_master_stop(cmd);

    if (i2c_master_cmd_begin(i2c->port, cmd, portMAX_DELAY)) {
        i2c_cmd_link_delete(cmd);
        return -EIO;
    }

    i2c_cmd_link_delete(cmd);

    return 0;
}

int32_t aos_hal_i2c_master_recv(aos_hal_i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                                uint16_t size, uint32_t timeout)
{
    i2c_cmd_handle_t cmd;

    if (!i2c || !data || size == 0)
        return -EINVAL;

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, size, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);

    if (i2c_master_cmd_begin(i2c->port, cmd, pdMS_TO_TICKS(1000))) {
        i2c_cmd_link_delete(cmd);
        return -EIO;
    }

    i2c_cmd_link_delete(cmd);

    return 0;
}

typedef struct _hw_i2c_buf_t {
    size_t len;
    uint8_t *buf;
} hw_i2c_buf_t;

static size_t fill_memaddr_buf(uint8_t *memaddr_buf, uint32_t memaddr, uint8_t addrsize)
{
    size_t memaddr_len = 0;
    if ((addrsize & 7) != 0 || addrsize > 32) {
        printf("invalid addrsize");
        return -EINVAL;
    }
    for (int16_t i = addrsize - 8; i >= 0; i -= 8) {
        memaddr_buf[memaddr_len++] = memaddr >> i;
    }
    return memaddr_len;
}

static int i2c_hw_transfer(aos_hal_i2c_dev_t *i2c, uint16_t addr, size_t n, hw_i2c_buf_t *bufs, unsigned int flags, bool stop)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr << 1 | (flags & I2C_MASTER_READ), true);

    int data_len = 0;
    for (; n--; ++bufs) {
        if (flags & I2C_MASTER_READ) {
            i2c_master_read(cmd, bufs->buf, bufs->len, n == 0 ? I2C_MASTER_LAST_NACK : I2C_MASTER_ACK);
        } else {
            if (bufs->len != 0) {
                i2c_master_write(cmd, bufs->buf, bufs->len, true);
            }
        }
        data_len += bufs->len;
    }

    if (stop) {
        i2c_master_stop(cmd);
    }

    // TODO proper timeout
    esp_err_t err = i2c_master_cmd_begin(i2c->port, cmd, 100 * (1 + data_len) / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    if (err == ESP_FAIL) {
        return -ENODEV;
    } else if (err == ESP_ERR_TIMEOUT) {
        return -ETIMEDOUT;
    } else if (err != ESP_OK) {
        return -abs(err);
    }

    return data_len;
}

static int mp_machine_i2c_readfrom(aos_hal_i2c_dev_t *i2c, uint16_t addr, uint8_t *dest, size_t len, bool stop)
{
    hw_i2c_buf_t buf = {.len = len, .buf = dest};
    unsigned int flags = I2C_MASTER_READ;
    return i2c_hw_transfer(i2c, addr, 1, &buf, flags, stop);
}

static int mp_machine_i2c_writeto(aos_hal_i2c_dev_t *i2c, uint16_t addr, const uint8_t *src, size_t len, bool stop)
{
    hw_i2c_buf_t buf = {.len = len, .buf = src};
    return i2c_hw_transfer(i2c, addr, 1, &buf, 0, stop);
}

int32_t aos_hal_i2c_mem_read(aos_hal_i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size,
                             uint8_t *data, uint16_t size, uint32_t timeout)
{
    uint8_t memaddr_buf[4];
    size_t memaddr_len = fill_memaddr_buf(&memaddr_buf[0], mem_addr, mem_addr_size);
    if (!i2c || !data || size == 0)
        return -EINVAL;

    int ret = mp_machine_i2c_writeto(i2c, dev_addr, memaddr_buf, memaddr_len, false);
    if (ret != memaddr_len) {
        // must generate STOP
        mp_machine_i2c_writeto(i2c, dev_addr, NULL, 0, true);
        return ret;
    }
    return mp_machine_i2c_readfrom(i2c, dev_addr, data, size, true);
}

int32_t aos_hal_i2c_mem_write(aos_hal_i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size,
                              const uint8_t *data, uint16_t size, uint32_t timeout)
{
    // Create buffer with memory address
    uint8_t memaddr_buf[4];
    size_t memaddr_len = fill_memaddr_buf(&memaddr_buf[0], mem_addr, mem_addr_size);

    if (!i2c || !data || size == 0)
        return -EINVAL;

    // Create partial write buffers
    hw_i2c_buf_t bufs[2] = {
        {.len = memaddr_len, .buf = memaddr_buf},
        {.len = size, .buf = (uint8_t *)data},
    };
    return i2c_hw_transfer(i2c, dev_addr, 2, bufs, 0, true);
}

/*
int32_t aos_hal_i2c_mem_write(aos_hal_i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size,
                              const uint8_t *data, uint16_t size, uint32_t timeout)
{
    uint8_t addr[2];
    size_t addr_len;
    i2c_cmd_handle_t cmd;

    if (!i2c || !data || size == 0)
        return -EINVAL;

    switch (mem_addr_size) {
    case I2C_MEM_ADDR_SIZE_8BIT:
        addr[0] = (uint8_t)mem_addr;
        addr_len = 1;
        break;
    case I2C_MEM_ADDR_SIZE_16BIT:
        addr[0] = (uint8_t)((mem_addr >> 8) & 0xFF);
        addr[1] = (uint8_t)((mem_addr >> 0) & 0xFF);
        addr_len = 2;
        break;
    default:
        return -EINVAL;
    }

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, addr, addr_len, true);
    i2c_master_write(cmd, (uint8_t *)data, size, true);
    i2c_master_stop(cmd);

    if (i2c_master_cmd_begin(i2c->port, cmd, portMAX_DELAY)) {
        i2c_cmd_link_delete(cmd);
        return -EIO;
    }

    i2c_cmd_link_delete(cmd);

    return 0;
}

int32_t aos_hal_i2c_mem_read(aos_hal_i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size,
                             uint8_t *data, uint16_t size, uint32_t timeout)
{
    uint8_t addr[2];
    size_t addr_len;
    i2c_cmd_handle_t cmd;

    if (!i2c || !data || size == 0)
        return -EINVAL;

    switch (mem_addr_size) {
    case I2C_MEM_ADDR_SIZE_8BIT:
        addr[0] = (uint8_t)mem_addr;
        addr_len = 1;
        break;
    case I2C_MEM_ADDR_SIZE_16BIT:
        addr[0] = (uint8_t)((mem_addr >> 8) & 0xFF);
        addr[1] = (uint8_t)((mem_addr >> 0) & 0xFF);
        addr_len = 2;
        break;
    default:
        return -EINVAL;
    }

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, addr, addr_len, true);
    i2c_master_stop(cmd);

    if (i2c_master_cmd_begin(i2c->port, cmd, portMAX_DELAY)) {
        i2c_cmd_link_delete(cmd);
        return -EIO;
    }

    i2c_cmd_link_delete(cmd);

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, size, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);

    if (i2c_master_cmd_begin(i2c->port, cmd, pdMS_TO_TICKS(1000))) {
        i2c_cmd_link_delete(cmd);
        return -EIO;
    }

    i2c_cmd_link_delete(cmd);

    return 0;
}
*/
