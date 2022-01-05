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
