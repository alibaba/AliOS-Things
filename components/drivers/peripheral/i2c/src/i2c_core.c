
#include <aos/list.h>
#include <drv/iic.h>

#include <devicevfs/devicevfs.h>
#include <drivers/ddkc_log.h>
#include "i2c_core.h"

static dlist_t g_i2c_controller_list;
static aos_mutex_t g_i2c_controller_lock;

static i2c_master_dev_t *find_i2c_controller(uint32_t id) {
    dlist_t *head = &g_i2c_controller_list;
    dlist_t *pos = NULL, *n = NULL;
    i2c_master_dev_t *master = NULL;

    dlist_for_each_safe(pos, n, head) {
        master = aos_container_of (pos, i2c_master_dev_t, host);
        if (master->id == id) {
            return master;
        }
    }

    return NULL;
}

aos_status_t _i2c_master_set_clk (i2c_master_dev_t *master, uint32_t clk) {
    int32_t ret = 0;
    csi_iic_speed_t speed = IIC_BUS_SPEED_STANDARD;

    if (master->clk == clk)
        return 0;

    if (clk <= I2C_BUS_CLK_100K) {
        speed = IIC_BUS_SPEED_STANDARD;
    } else if (clk <= I2C_BUS_CLK_400K) {
        speed = IIC_BUS_SPEED_FAST;
    } else if (clk <= I2C_BUS_CLK_1000K) {
        speed = IIC_BUS_SPEED_FAST_PLUS;
    } else if (clk <= I2C_BUS_CLK_3400K) {
        speed = IIC_BUS_SPEED_HIGH;
    } else {
        speed = IIC_BUS_SPEED_STANDARD;
        ddkc_warn("invalid clk:%d, set to standard speed by default\r\n", clk);
    }

    ret = csi_iic_speed(&master->csi_dev, speed);
    if (ret == CSI_OK) {
        ddkc_dbg("set addr speed on i2c%d success\r\n", master->id);
        ret = 0;
    } else {
        ddkc_err("set addr speed on i2c%d success fail, ret:%d\r\n", master->id, ret);
        return ret;
    }
    master->clk = clk;

    return 0;
}

aos_status_t _i2c_master_set_addr_mode (i2c_master_dev_t *master, uint32_t addr_width) {
    int32_t ret = 0;
    csi_iic_addr_mode_t addr_mode = IIC_ADDRESS_7BIT;

    if (master->addr_width == addr_width)
        return 0;

    if (addr_width == I2C_SLAVE_ADDR_WIDTH_7BIT) {
        addr_mode = IIC_ADDRESS_7BIT;
    } else if (addr_width <= I2C_SLAVE_ADDR_WIDTH_10BIT) {
        addr_mode = IIC_ADDRESS_10BIT;
    } else {
        addr_mode = IIC_ADDRESS_7BIT;
        ddkc_warn("invalid addr mode:%d, set to 7 bit mode by default\r\n", addr_width);
    }

    ret = csi_iic_addr_mode(&master->csi_dev, addr_mode);
    if (ret == CSI_OK) {
        ddkc_dbg("set addr mode on i2c%d success\r\n", master->id);
    } else {
        ddkc_err("set addr mode on i2c%d success fail, ret:%d\r\n", master->id, ret);
        return ret;
    }

    master->addr_width = addr_width;

    return 0;
}

/**
 * setup new I2C channel with specified slave device's config to communicate with slave device
 *
 * @param[in]  id  host I2C controller's channel ID
 * @param[in]  config slave device's address, address width and clock settings, \ref i2c_slave_config_t
 *
 * @return  handle of the i2c device, used for later i2c operations on I2C slave device, when open operation success;
            NULL when open operation fails
 */
i2c_dev_handle_t aos_i2c_open (uint32_t id, i2c_slave_config_t *config) {
    i2c_master_dev_t *master = NULL;
    i2c_slave_dev_t *slave = NULL;
    int32_t ret = 0;
    csi_iic_t *i2c_dev = NULL;
    aos_status_t status = -1;

    if (id >= aos_i2c_get_num()) {
        ddkc_err("invalid id:%d, max id:%d\r\n", id, aos_i2c_get_num());
        return NULL;
    }

    if (!config) {
        ddkc_err("config is NULL, invalid\r\n", config);
        return NULL;
    }

    status = aos_mutex_lock(&g_i2c_controller_lock, AOS_WAIT_FOREVER);
    if (status) {
        ddkc_warn("lock g_i2c_controller_lock failed, status:%d\r\n", id, status);
        return NULL;
    }

    master = find_i2c_controller(id);

    if (!master) {
        // create master deivce struct it it does not exist
        master = (i2c_master_dev_t *)malloc(sizeof(i2c_master_dev_t));
        if (!master) {
            ddkc_err("malloc for i2c master failed\r\n");
            goto err_master;
        }
        master->id = id;
        master->init = 0;
        master->addr = 0x0;
        master->addr_width = 0x0;
        master->clk = 0x0;
        ret = aos_mutex_new(&master->lock);
        if (ret) {
            ddkc_err("aos_mutex_new failed, ret:%d\r\n", ret);
            goto err_master;
        }
        dlist_init(&master->slave);
        dlist_init(&master->host);

        i2c_dev = &master->csi_dev;

        master->init = 0;
        ret = csi_iic_init(i2c_dev, id);
        if (ret == CSI_OK) {
            ddkc_dbg("i2c%d init success\r\n", id);
        } else {
            ddkc_err("i2c%d init fail, ret:%d\r\n", id, ret);
            goto err_master;
        }
        master->init = 1;
        dlist_add(&master->host, &g_i2c_controller_list);
    } else {
        i2c_dev = &master->csi_dev;
    }

    status = aos_mutex_unlock(&g_i2c_controller_lock);
    if (status) {
        // This should never happen
        ddkc_warn("THIS SHOULD NEVER HAPPEN - unlock g_i2c_controller_lock failed, status:%d\r\n", id, status);
    }
    /*
     * Note: check whether the slave with the same address is already opened or not?
     * whether there's any node in master->slave list has the same slave address with target slave
     * DON'T cover this case for the moment
     * component/application layer must make sure only one user open the same i2c slave device
     * */

    slave = (i2c_slave_dev_t *)malloc(sizeof(i2c_slave_dev_t));
    if (!slave) {
        ddkc_err("malloc for i2c slave failed\r\n");
        return NULL;
    }

    slave->i2c_dev_handle = AOS_I2C_MAGIC;

    memcpy(&slave->config, config, sizeof(i2c_slave_config_t));

    // Try set I2C config to controller and chech whether target config is supported or not
    status = aos_mutex_lock(&master->lock, AOS_WAIT_FOREVER);
    if (status) {
        ddkc_warn("lock i2c%d controller failed, status:%d\r\n", id, status);
        free(slave);
        slave = NULL;
        return NULL;
    }
    ret = aos_mutex_new(&slave->lock);
    if (ret) {
        ddkc_assert(1, "aos_mutex_new for slave->lock failed, ret:%d\r\n", ret);
        goto err_slave;
    }
    // address mode should not be a problem, ignore this operation
    /*
    if (_i2c_master_set_addr_mode(master, config->addr_width)) {
        goto err_slave;
    }
    */

    if (_i2c_master_set_clk(master, config->clk)) {
        ddkc_assert(1, "_i2c_master_set_clk failed, ret:%d\r\n", ret);
        goto err_slave;
    }

    // init slave device's node and add it into controller's slave list
    dlist_init(&slave->node);
    dlist_add(&slave->node, &master->slave);
    slave->master = master;
    ddkc_dbg("i2c slave config done\r\n");

    status = aos_mutex_unlock(&master->lock);
    if (status) {
        // This should never happen
        ddkc_warn("THIS SHOULD NEVER HAPPEN - unlock i2c%d controller failed, status:%d\r\n", id, status);
    }

    return &slave->i2c_dev_handle;

err_slave:
    status = aos_mutex_unlock(&master->lock);
    if (status) {
        ddkc_warn("THIS SHOULD NEVER HAPPEN - unlock i2c%d controller failed, status:%d\r\n", id, status);
    }

    if (slave) {
        free(slave);
        slave = NULL;
    }
    return NULL;

err_master:
    status = aos_mutex_unlock(&g_i2c_controller_lock);
    if (status) {
        // This should never happen
        ddkc_warn("THIS SHOULD NEVER HAPPEN - unlock g_i2c_controller_lock failed, status:%d\r\n", id, status);
    }
    if (master) {
        free(master);
        master = NULL;
    }

    return NULL;
}

/**
 * close target I2C communication channel
 *
 * @param[in]  dev_h handle of the i2c device to be operated, must be the return value of aos_i2c_open
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
aos_status_t aos_i2c_close (i2c_dev_handle_t dev_h) {
    aos_status_t status = -1;
    i2c_slave_dev_t *slave = NULL;
    i2c_master_dev_t *master = NULL;

    if (dev_h == NULL || *dev_h != AOS_I2C_MAGIC) {
        ddkc_err("invalid dev:%p, 0x%x\r\n", dev_h, !dev_h ? 0 : *dev_h);
        return -EINVAL;
    }

    slave = aos_container_of(dev_h, i2c_slave_dev_t, i2c_dev_handle);
    master = (i2c_master_dev_t *)(slave->master);

    status = aos_mutex_lock(&master->lock, AOS_WAIT_FOREVER);
    if (status) {
        ddkc_warn("lock i2c%d failed, status:%d\r\n", master->id, status);
        return -EIO;
    }

    // remove slave node from master's slave list
    dlist_del(&slave->node);
    slave->master = NULL;

    memset(slave, 0, sizeof(i2c_slave_dev_t));
    free (slave);

    if (dlist_empty(&master->slave)) {
        ddkc_info("i2c%d's slave list is empty\r\n");
    }

    status = aos_mutex_unlock(&master->lock);
    if (status) {
        ddkc_warn("THIS SHOULD NEVER HAPPEN - unlock i2c%d failed, status:%d\r\n", master->id, status);
    }

    return 0;
}

/**
 * set I2C bus's clock when communicate with target I2C slave
 *
 * @param[in]  dev_h handle of the i2c device to be operated, must be the return value of aos_i2c_open
 * @param[in]  clk  target clock to be used
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
aos_status_t aos_i2c_clk_set (i2c_dev_handle_t dev_h,  uint32_t clk) {
    aos_status_t status = -1;
    i2c_slave_dev_t *slave = NULL;
    i2c_master_dev_t *master = NULL;

    if (dev_h == NULL || *dev_h != AOS_I2C_MAGIC) {
        ddkc_err("invalid dev:%p, 0x%x\r\n", dev_h, !dev_h ? 0 : *dev_h);
        return -EINVAL;
    }

    // TODO lock i2c slave's lock
    slave = aos_container_of(dev_h, i2c_slave_dev_t, i2c_dev_handle);
    status = aos_mutex_lock(&slave->lock, AOS_WAIT_FOREVER);
    if (status) {
        ddkc_warn("lock i2c slave 0x%x failed, status:%d\r\n", slave->config.addr, status);
        return -EIO;
    }

    // just log target clk setting into slave's config
    slave->config.clk = clk;

    status = aos_mutex_unlock(&slave->lock);
    if (status) {
        ddkc_warn("THIS SHOULD NEVER HAPPEN - unlock i2c slave 0x%x failed, status:%d\r\n", slave->config.addr, status);
    }

    return status;
}

/**
 * set slave device's address witdh on target I2C channel
 *
 * @param[in]  dev_h handle of the i2c device to be operated, must be the return value of aos_i2c_open
 * @param[in]  addr_width  slave device's address width, 7-bit or 10-bit only
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
aos_status_t aos_i2c_addr_width_set (i2c_dev_handle_t dev_h,  uint32_t addr_width) {
    aos_status_t status = -1;
    i2c_slave_dev_t *slave = NULL;
    i2c_master_dev_t *master = NULL;

    if (dev_h == NULL || *dev_h != AOS_I2C_MAGIC) {
        ddkc_err("invalid dev:%p, 0x%x\r\n", dev_h, !dev_h ? 0 : *dev_h);
        return -EINVAL;
    }

    slave = aos_container_of(dev_h, i2c_slave_dev_t, i2c_dev_handle);
    status = aos_mutex_lock(&slave->lock, AOS_WAIT_FOREVER);
    if (status) {
        ddkc_warn("lock i2c slave 0x%x failed, status:%d\r\n", slave->config.addr, status);
        return -EIO;
    }

    // just log target address width setting into slave's config
    slave->config.addr_width = addr_width;

    status = aos_mutex_unlock(&slave->lock);
    if (status) {
        ddkc_warn("THIS SHOULD NEVER HAPPEN - unlock i2c slave 0x%x failed, status:%d\r\n", slave->config.addr, status);
    }
    return status;
}

/**
 * set slave device's address on target I2C channel
 *
 * @param[in]  dev_h handle of the i2c device to be operated, must be the return value of aos_i2c_open
 * @param[in]  addr  slave device's address width
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
aos_status_t aos_i2c_slave_addr_set (i2c_dev_handle_t dev_h,  uint16_t  addr) {
    aos_status_t status = -1;
    i2c_slave_dev_t *slave = NULL;
    i2c_master_dev_t *master = NULL;

    if (dev_h == NULL || *dev_h != AOS_I2C_MAGIC) {
        ddkc_err("invalid dev:%p, 0x%x\r\n", dev_h, !dev_h ? 0 : *dev_h);
        return -EINVAL;
    }

    slave = aos_container_of(dev_h, i2c_slave_dev_t, i2c_dev_handle);
    status = aos_mutex_lock(&slave->lock, AOS_WAIT_FOREVER);
    if (status) {
        ddkc_warn("lock i2c slave 0x%x failed, status:%d\r\n", slave->config.addr, status);
        return -EIO;
    }

    // log slave address into its config
    slave->config.addr = addr;

    status = aos_mutex_unlock(&slave->lock);
    if (status) {
        ddkc_warn("THIS SHOULD NEVER HAPPEN - unlock i2c slave 0x%x failed, status:%d\r\n", slave->config.addr, status);
    }

    return status;
}

i2c_slave_dev_t* _i2c_tx_rx_prepare(i2c_dev_handle_t dev_h) {
    int ret = 0;
    aos_status_t status = -1;
    i2c_slave_dev_t *slave = NULL;
    i2c_master_dev_t *master = NULL;

    if (dev_h == NULL || *dev_h != AOS_I2C_MAGIC) {
        ddkc_err("invalid dev:%p, 0x%x\r\n", dev_h, !dev_h ? 0 : *dev_h);
        return NULL;
    }

    // the whole operation should be locked by i2c clave's lock
    slave = aos_container_of(dev_h, i2c_slave_dev_t, i2c_dev_handle);
    master = (i2c_master_dev_t *)(slave->master);

    // lock master's lock
    status = aos_mutex_lock(&master->lock, AOS_WAIT_FOREVER);
    if (status) {
        ddkc_warn("lock i2c%d controller failed, status:%d\r\n", master->id, status);
        return NULL;
    }

    /* set to target clk and address width */
    ret = _i2c_master_set_clk(master, slave->config.clk);
    if (ret) {
        ddkc_warn("set clk to i2c%d failed, ret:%d\r\n", master->id, ret);
    }
    ret = _i2c_master_set_addr_mode(master, slave->config.addr_width);

    if (ret) {
        ddkc_warn("set address mode to i2c%d failed, ret:%d\r\n", master->id, ret);
    }

    return slave;
}

aos_status_t _i2c_tx_rx_unprepare(i2c_slave_dev_t *slave) {
    aos_status_t status = 0;
    i2c_master_dev_t *master = (i2c_master_dev_t *)(slave->master);

    status = aos_mutex_unlock(&master->lock);
    if (status) {
        ddkc_warn("THIS SHOULD NEVER HAPPEN - unlock i2c%d controller failed, status:%d\r\n", master->id, status);
    }

    return status;
}

/**
 * issue I2C write operation on target I2C channel
 *
 * @param[in]  dev_h handle of the i2c device to be operated, must be the return value of aos_i2c_open
 * @param[in]  buf  pointer to the buffer to be sent
 * @param[in]  len  number of data to be sent
 * @param[in]  timeout  in unit of ms, current i2c transaction's timeout value
 *             use AOS_WAIT_FOREVER for endless wait until operation success or fails
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
int32_t aos_i2c_master_send (i2c_dev_handle_t dev_h, char *buf, uint32_t len, uint32_t timeout) {
    int ret = 0;
    i2c_slave_dev_t *slave = NULL;
    i2c_master_dev_t *master = NULL;

    slave = _i2c_tx_rx_prepare(dev_h);
    if (!slave) {
        ddkc_err("i2c prepare failed\r\n");
        return -1;
    }

    master = (i2c_master_dev_t *)(slave->master);

    /* do i2c master send operation */
    ret = csi_iic_master_send(&master->csi_dev, slave->config.addr, buf, len, timeout);
    if (ret != len) {
        ddkc_err("i2c%d master send failed, ret:%d\r\n", master->id, ret);
    }

    _i2c_tx_rx_unprepare(slave);

    return ret;
}

/**
 * issue I2C read operation on target I2C channel
 *
 * @param[in]  dev_h handle of the i2c device to be operated, must be the return value of aos_i2c_open
 * @param[in]  buf  pointer to the buffer where to save the received data
 * @param[in]  len  number of data to be read
 * @param[in]  timeout  in unit of ms, current i2c transaction's timeout value
 *             use AOS_WAIT_FOREVER for endless wait until operation success or fails
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
int32_t aos_i2c_master_recv (i2c_dev_handle_t dev_h, char *buf, uint32_t len, uint32_t timeout) {
    int ret = 0;
    i2c_slave_dev_t *slave = NULL;
    i2c_master_dev_t *master = NULL;

    slave = _i2c_tx_rx_prepare(dev_h);
    if (!slave) {
        ddkc_err("i2c prepare failed\r\n");
        return -1;
    }

    master = (i2c_master_dev_t *)(slave->master);
    /* do i2c master send operation */
    ret = csi_iic_master_receive(&master->csi_dev, slave->config.addr, buf, len, timeout);
    if (ret <= 0) {
        ddkc_err("i2c%d master send failed, ret:%d", master->id, ret);
    }

    _i2c_tx_rx_unprepare(slave);

    return ret;
}

/**
 * issue I2C memory/register write operation on target I2C channel
 *
 * @param[in]  dev_h handle of the i2c device to be operated, must be the return value of aos_i2c_open
 * @param[in]  addr address of target memory/register to be write
 * @param[in]  addr_len  address's length, either I2C_MEM_ADDR_SIZE_8BIT or I2C_MEM_ADDR_SIZE_16BIT
 * @param[in]  buf  pointer to the buffer to be written
 * @param[in]  len  number of data to be written
 * @param[in]  timeout  in unit of ms, current i2c transaction's timeout value
 *             use AOS_WAIT_FOREVER for endless wait until operation success or fails
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
int32_t aos_i2c_mem_write (i2c_dev_handle_t dev_h, uint16_t addr, uint8_t addr_len, uint8_t *buf, uint32_t len, uint32_t timeout) {
    int ret = 0;
    i2c_slave_dev_t *slave = NULL;
    i2c_master_dev_t *master = NULL;
    csi_iic_mem_addr_size_t addr_size;

    slave = _i2c_tx_rx_prepare(dev_h);
    if (!slave) {
        ddkc_err("i2c prepare failed\r\n");
        return -1;
    }

    master = (i2c_master_dev_t *)(slave->master);
    addr_size = (addr_len == 1) ? IIC_MEM_ADDR_SIZE_8BIT : IIC_MEM_ADDR_SIZE_16BIT;

    /* do i2c master send operation */
    ret = csi_iic_mem_send(&master->csi_dev, slave->config.addr, addr, addr_size, buf, len, timeout);
    if (ret != len) {
        ddkc_err("i2c%d memory send failed, ret:%d\r\n", master->id, ret);
    }

    _i2c_tx_rx_unprepare(slave);

    return ret;
}

/**
 * issue I2C memory/register read operation on target I2C channel
 *
 * @param[in]  dev_h handle of the i2c device to be operated, must be the return value of aos_i2c_open
 * @param[in]  addr address of target memory/register to be read
 * @param[in]  addr_len  address's length, , either I2C_MEM_ADDR_SIZE_8BIT or I2C_MEM_ADDR_SIZE_16BIT
 * @param[in]  buf  pointer to the buffer for read operation
 * @param[in]  len  number of data to be read
 * @param[in]  timeout  in unit of ms, current i2c transaction's timeout value
 *             use AOS_WAIT_FOREVER for endless wait until operation success or fails
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
int32_t aos_i2c_mem_read (i2c_dev_handle_t dev_h, uint16_t addr, uint8_t addr_len, uint8_t *buf, uint32_t len, uint32_t timeout) {
    int ret = 0;
    i2c_slave_dev_t *slave = NULL;
    i2c_master_dev_t *master = NULL;
    csi_iic_mem_addr_size_t addr_size;

    slave = _i2c_tx_rx_prepare(dev_h);
    if (!slave) {
        ddkc_err("i2c prepare failed\r\n");
        return -1;
    }

    master = (i2c_master_dev_t *)(slave->master);
    addr_size = (addr_len == 1) ? IIC_MEM_ADDR_SIZE_8BIT : IIC_MEM_ADDR_SIZE_16BIT;

    /* do i2c master send operation */
    ret = csi_iic_mem_receive(&master->csi_dev, slave->config.addr, addr, addr_size, buf, len, timeout);
    if (ret != len) {
        ddkc_err("i2c%d master send failed, ret:%d", master->id, ret);
    }

    _i2c_tx_rx_unprepare(slave);

    return ret;
}

/**
 * issue I2C memory/register read operation on target I2C channel
 *
 * @param[in]  dev_h handle of the i2c device to be operated, must be the return value of aos_i2c_open
 * @param[in]  level  level of current power management level
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
aos_status_t aos_i2c_pm_ctrl (i2c_dev_handle_t dev_h, int level) {
    aos_status_t status = -1;
    i2c_slave_dev_t *slave = NULL;
    i2c_master_dev_t *master = NULL;

    if (dev_h == NULL || *dev_h != AOS_I2C_MAGIC) {
        ddkc_err("invalid dev:%p, 0x%x\r\n", dev_h, !dev_h ? 0 : *dev_h);
        return -EINVAL;
    }

    slave = aos_container_of(dev_h, i2c_slave_dev_t, i2c_dev_handle);
    master = (i2c_master_dev_t *)(slave->master);

    return -ENOTSUP;
}

/**
 * get the number of I2C hardware module
 *
 * @param[in]  dev_h handle of the i2c device to be operated, must be the return value of aos_i2c_open
 * @param[in]  level  level of current power management level
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
int32_t aos_i2c_get_num (void) {
    return 2;
}


int32_t aos_i2c_init (void) {
    aos_status_t ret = 0;

    // init global i2c controller's lock
    ret = aos_mutex_new(&g_i2c_controller_lock);
    if (ret) {
        ddkc_assert(1, "aos_mutex_new for g_i2c_controller_lock failed, ret:%d\r\n", ret);
    }

    // init global i2c controller list
    INIT_AOS_DLIST_HEAD(&g_i2c_controller_list);
    ddkc_warn("enter %s\r\n", __func__);


    return 0;
}

BUS_DRIVER_ENTRY(aos_i2c_init)

#if CONFIG_U_I2C_WEAK_CSI
/**
  \brief       Init iic ctrl block
               Initializes the resources needed for the iic instance
  \param[in]   iic    Handle of iic instance
  \param[in]   idx    Index of instance
  \return      error code \ref csi_error_t
*/
__weak csi_error_t csi_iic_init(csi_iic_t *iic, uint32_t idx) {
    ddkc_warn("enter weak %s\r\n", __func__);
    return CSI_OK;
}

/**
  \brief       Uninit iic ctrl block
               Stops operation and releases the software resources used by the instance
  \param[in]   iic    Handle of iic instance
  \return      None
*/
__weak void csi_iic_uninit(csi_iic_t *iic) {
    ddkc_warn("enter weak %s\r\n", __func__);
    return;
}

/**
  \brief       Config iic master or slave mode
  \param[in]   iic     Handle of iic instance
  \param[in]   mode    iic mode \ref csi_iic_mode_t
  \return      error code \ref csi_error_t
*/
__weak csi_error_t csi_iic_mode(csi_iic_t *iic, csi_iic_mode_t mode) {
    ddkc_warn("enter weak %s\r\n", __func__);
    return CSI_OK;
}

/**
  \brief       Config iic addr mode
  \param[in]   iic          Handle of iic instance
  \param[in]   addr_mode    iic addr mode \ref csi_iic_addr_mode_t
  \return      error code \ref csi_error_t
*/
__weak csi_error_t csi_iic_addr_mode(csi_iic_t *iic, csi_iic_addr_mode_t addr_mode) {
    ddkc_warn("enter weak %s\r\n", __func__);
    return CSI_OK;
}

/**
  \brief       Config iic speed
  \param[in]   iic      Handle of iic instance
  \param[in]   speed    iic speed mode \ref csi_iic_speed_t
  \return      error code \ref csi_error_t
*/
__weak csi_error_t csi_iic_speed(csi_iic_t *iic, csi_iic_speed_t speed) {
    ddkc_warn("enter weak %s\r\n", __func__);
    return CSI_OK;
}

/**
  \brief       Start sending data as iic master
               This function is blocking
  \param[in]   iic       Handle of iic instance
  \param[in]   devaddr   Addrress of slave device
  \param[in]   data      Pointer to send data buffer
  \param[in]   size      Size of data items to send
  \param[in]   timout    Unit of time delay(ms)
  \return      The amount of real data sent or error code
*/
__weak int32_t csi_iic_master_send(csi_iic_t *iic, uint32_t devaddr, const void *data, uint32_t size, uint32_t timeout) {
    ddkc_warn("enter weak %s\r\n", __func__);
    return size;
}

/**
  \brief       Start receiving data as iic master
               This function is blocking
  \param[in]   iic        Handle to operate
  \param[in]   devaddr    iic addrress of slave device
  \param[out]  data       Pointer to buffer for data to receive from iic receiver
  \param[in]   size       Size of data items to receive
  \param[in]   timeout    Unit of time delay(ms)
  \return      The amount of real data received or error code
*/
__weak int32_t csi_iic_master_receive(csi_iic_t *iic, uint32_t devaddr, void *data, uint32_t size, uint32_t timeout) {
    ddkc_warn("enter weak %s\r\n", __func__);
    return size;
}

/**
  \brief       Start sending data as iic master
               This function is blocking
  \param[in]   iic             Handle of iic instance
  \param[in]   devaddr         Addrress of slave device
  \param[in]   memaddr         Internal addr of device
  \param[in]   memaddr_size    Internal addr mode of device
  \param[in]   data            Pointer to send data buffer
  \param[in]   size            Size of data items to send
  \param[in]   timout          Unit of time delay(ms)
  \return      The amount of real data sent or error code
*/
__weak int32_t csi_iic_mem_send(csi_iic_t *iic, uint32_t devaddr, uint16_t memaddr, csi_iic_mem_addr_size_t memaddr_size, const void *data, uint32_t size, uint32_t timeout) {
    ddkc_warn("enter weak %s\r\n", __func__);
    return size;
}

/**
  \brief       Start receiving data as iic master
               This function is blocking
  \param[in]   iic             Handle to operate
  \param[in]   devaddr         iic addrress of slave device
  \param[in]   memaddr         Internal addr of device
  \param[in]   memaddr_mode    Internal addr mode of device
  \param[out]  data            Pointer to buffer for data to receive from eeprom device
  \param[in]   size            Size of data items to receive
  \param[in]   timeout         Unit of time delay(ms)
  \return      The amount of real data received or error code
*/
__weak int32_t csi_iic_mem_receive(csi_iic_t *iic, uint32_t devaddr, uint16_t memaddr, csi_iic_mem_addr_size_t memaddr_size, void *data, uint32_t size, uint32_t timeout) {
    ddkc_warn("enter weak %s\r\n", __func__);
    return size;
}

#endif