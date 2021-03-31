#include <stdio.h>
#include <stdlib.h>

#include "pca9544.h"
#include "pca9544_i2c_drv.h"

static PCA9544_DEV_CFG_T g_pca9544_dev_cfg;
static i2c_dev_t         g_pca9544_i2c_cfg;

/**********************************************************
 * @fun    PCA9544_init
 * @breif  pca9544 initialization
 * @param  i2c:the pointer for i2c configuration
 * @param  dev_cfg: the pointer for dev configuration
 * @rtn
 *********************************************************/
int pca9544_init(i2c_dev_t *i2c, PCA9544_DEV_CFG_T *dev_cfg)
{
    int ret = 0;

    if (i2c == NULL) {
        printf("PCA9544 i2c is null\n");
        return -1;
    }

    memset(&g_pca9544_i2c_cfg, 0, sizeof(i2c_dev_t));
    memcpy(&g_pca9544_i2c_cfg, i2c, sizeof(i2c_dev_t));

    /*ret = hal_i2c_init(&g_pca9544_i2c_cfg);*/
    ret = pca9544_i2c_init(&g_pca9544_i2c_cfg);
    if (ret) {
        printf("Host I2C open failed\n");
        /*hal_i2c_finalize(&g_pca9544_i2c_cfg);*/
        pca9544_i2c_finalize(&g_pca9544_i2c_cfg);

        return -1;
    }

    if (dev_cfg == NULL) {
        printf("PCA9544 cfg is null\n");
        return -1;
    }

    memset(&g_pca9544_dev_cfg, 0, sizeof(PCA9544_DEV_CFG_T));
    memcpy(&g_pca9544_dev_cfg, dev_cfg, sizeof(PCA9544_DEV_CFG_T));

    printf("PCA9544 cfg is successful\n");
    return ret;
}

/**********************************************************
 * @fun    pca9544_deinit
 * @breif  pca9544 deinitialization
 * @param  none
 * @rtn    none
 *********************************************************/
void pca9544_deinit()
{
    memset(&g_pca9544_i2c_cfg, 0, sizeof(i2c_dev_t));
    memset(&g_pca9544_dev_cfg, 0, sizeof(PCA9544_DEV_CFG_T));
    /*hal_i2c_finalize(&g_pca9544_i2c_cfg);*/
    pca9544_i2c_finalize(&g_pca9544_i2c_cfg);
}

/**********************************************************
 * @fun    pca9544_set_chan
 * @breif  pca9544 channel set
 * @param  chan: the object channel
 * @rtn    ret
 *********************************************************/
int pca9544_set_chan(PCA9544_CH_E chan)
{

    int      ret      = 0;
    uint16_t dev_addr = g_pca9544_dev_cfg.dev_addr;
    uint8_t  regval   = 0;

    if ((chan < PCA9544_CHO) || (chan > PCA9544_CH3)) {
        printf("the setting channel is not valid\n");
        return -1;
    }
    g_pca9544_dev_cfg.pca9544_ch = chan;
    regval                       = g_pca9544_dev_cfg.pca9544_ch;

    ret = pca9544_write_reg(dev_addr, regval);
    if (ret) {
        printf("set PCA9544 channel failed\n");
        return -1;
    }

    return ret;
}

/**********************************************************
 * @fun    pca9544_read_chan
 * @breif  pca9544 channel reading
 * @param  none
 * @rtn    chan: the current selected channel of pca9544
 *********************************************************/
PCA9544_CH_E pca9544_read_chan()
{
    PCA9544_CH_E chan = g_pca9544_dev_cfg.pca9544_ch;

    return chan;
}

/**********************************************************
 * @fun    pca9544_clean_chan
 * @breif  pca9544 channel clean
 * @param  none
 * @rtn    ret
 *********************************************************/
int pca9544_clean_chan()
{

    int      ret                 = 0;
    uint16_t dev_addr            = g_pca9544_dev_cfg.dev_addr;
    g_pca9544_dev_cfg.pca9544_ch = PCA9544_CHAN_CLEAN;
    uint8_t regval               = g_pca9544_dev_cfg.pca9544_ch;

    ret = pca9544_write_reg(dev_addr, regval);
    if (ret) {
        printf("clean PCA9544 channel failed\n");
        return -1;
    }

    return ret;
}

/**********************************************************
 * @fun    pca9544_write_reg
 * @breif  pca9544 register writing operation
 * @param  dev_addr: i2c device address
 * @param  regval: register value
 * @rtn    ret
 *********************************************************/
int pca9544_write_reg(uint16_t dev_addr, uint8_t regval)
{

    int        ret     = 0;
    i2c_dev_t *i2c     = &g_pca9544_i2c_cfg;
    uint32_t   timeout = 1000;

    /*ret = hal_i2c_master_send(i2c, dev_addr, &regval, sizeof(regval),
     * timeout);*/
    ret = pca9544_i2c_master_send(i2c, dev_addr, &regval, sizeof(regval), timeout);
    if (ret) {
        printf("write PCA9544 reg failed\n");
        return -1;
    }

    return ret;
}

/**********************************************************
 * @fun    pca9544_read_reg
 * @breif  pca9544 register reading operation
 * @param  dev_addr: i2c device address
 * @param  regval: register value
 * @rtn    ret
 *********************************************************/
int pca9544_read_reg(uint16_t dev_addr, uint8_t *data)
{
    int        ret     = 0;
    i2c_dev_t *i2c     = &g_pca9544_i2c_cfg;
    uint16_t   len     = PCA9544_CONFIG_REG_LEN;
    uint32_t   timeout = 1000;

    /*ret = hal_i2c_master_recv(i2c, dev_addr, data, len, timeout);*/
    ret = pca9544_i2c_master_recv(i2c, dev_addr, data, len, timeout);
    if (ret) {
        printf("Read PCA9544 reg failed\n");
        return -1;
    }

    return ret;
}

/**********************************************************
 * @fun    pca9544_raw_data_write
 * @breif  write raw data to the selected channel
 * @param  sub_addr: the sub dev addr for
 *                   the selected pca9544 channel
 * @param  reg_addr: the sub dev register addr for
 *                   the selected pca9544 channel
 * @param  size: register size
 * @param  data: the pointer for the writing data
 * @param  len: the data length
 * @rtn    ret
 *********************************************************/
int pca9544_raw_data_write(uint16_t subdev_addr, uint16_t reg_addr, uint16_t size, uint8_t *data, uint16_t len)
{
    int        ret                = 0;
    i2c_dev_t *i2c                = &g_pca9544_i2c_cfg;
    g_pca9544_dev_cfg.subdev_addr = subdev_addr;
    g_pca9544_dev_cfg.reg_addr    = reg_addr;
    uint16_t dev_addr             = g_pca9544_dev_cfg.subdev_addr;
    uint16_t mem_addr             = g_pca9544_dev_cfg.reg_addr;
    uint16_t mem_addr_size        = size;
    uint32_t timeout              = 1000;

    /*ret = hal_i2c_mem_write(i2c, dev_addr, mem_addr, mem_addr_size, &data[0],
     * len, timeout);*/
    ret = pca9544_i2c_mem_write(i2c, dev_addr, mem_addr, mem_addr_size, &data[0], len, timeout);
    if (ret) {
        printf("PCA9544 sub dev raw data writing failed\n");
        return -1;
    }

    return ret;
}

/**********************************************************
 * @fun    pca9544_raw_data_read
 * @breif  read raw data to the selected channel
 * @param  sub_addr: the sub dev addr for
 *                   the selected pca9544 channel
 * @param  reg_addr: the sub dev register addr for
 *                   the selected pca9544 channel
 * @param  size: register size
 * @param  data: the pointer for the reading data
 * @param  len: the data length
 * @rtn    ret
 *********************************************************/
int pca9544_raw_data_read(uint16_t subdev_addr, uint16_t reg_addr, uint16_t size, uint8_t *data, uint16_t len)
{
    int        ret                = 0;
    i2c_dev_t *i2c                = &g_pca9544_i2c_cfg;
    g_pca9544_dev_cfg.subdev_addr = subdev_addr;
    g_pca9544_dev_cfg.reg_addr    = reg_addr;
    uint16_t dev_addr             = g_pca9544_dev_cfg.subdev_addr;
    uint16_t mem_addr             = g_pca9544_dev_cfg.reg_addr;
    uint16_t mem_addr_size        = size;
    uint32_t timeout              = 1000;

    /*ret = hal_i2c_mem_read(i2c, dev_addr, mem_addr, mem_addr_size, &data[0],
     * len, timeout);*/
    ret = pca9544_i2c_mem_read(i2c, dev_addr, mem_addr, mem_addr_size, &data[0], len, timeout);
    if (ret) {
        printf("PCA9544 sub dev raw data reading failed\n");
        return -1;
    }

    return ret;
}

int pca954x_irq_handler(uint16_t dev_addr, uint8_t *data)
{
    int     ret       = 0;
    uint8_t irq_state = 0;

    ret = pca9544_get_irq_state(dev_addr, data);
    return ret;
}

int pca9544_get_irq_state(uint16_t dev_addr, uint8_t *regval)
{

    int ret = 0;

    ret = pca9544_read_reg(dev_addr, &regval);
    if (ret) {
        printf("get PCA9544 irq state failed\n");
        return -1;
    } else {
        regval[0] = regval[0] | PCA9544_IRQ_MASK;
        regval[0] = regval[0] >> 4;

        printf("PCA9544 IRQ STATUS is %d\n", regval[0]);
    }

    return ret;
}
