#include "PCA9544.h"

#define PCA9544_CHAN_CLEAN      0
#define PCA9544_CONFIG_REG_LEN  1U
#define PCA9544_IRQ_MASK        0xF0
#define PCA9544_BASE_ADDR       0x70

typedef enum
{
    PCA9544_CHO = 4,
    PCA9544_CH1 = 5,
    PCA9544_CH2 = 6,
    PCA9544_CH3 = 7,
}pca9544_ch_e;

typedef struct
{
    uint8_t dev_addr;
    uint8_t control_reg;
    uint8_t irq_status;
    pca9544_ch_e pca9544_ch;
}pca9544_dev_cfg_t;

static pca9544_dev_cfg_t g_pca9544_dev_cfg;

int PCA9544_init(pca9544_dev_cfg_t* dev_cfg)
{
    int ret = 0;

    if(dev_cfg == NULL)
    {
        printf("PCA9544 cfg is null\n");
        return -1;
    }

    memset(&g_pca9544_dev_cfg, 0, sizeof(pca9544_dev_cfg_t));
    memcpy(&g_pca9544_dev_cfg, dev_cfg, sizeof(pca9544_dev_cfg_t));

    return ret;

}

int pca954x_irq_handler(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t* data)
{
    uint8_t irq_state = 0;
    ret = pca9544_get_irq_state(i2c, dev_addr, data);
    return ret;

}

int pca9544_get_irq_state(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t* regval)
{

    int ret = 0;

    ret = pca9544_read_reg(i2c, dev_addr, &regval);
    if(ret)
    {
        printf("get PCA9544 irq state failed\n");
    }
    else
    {
        regval[0] = regval[0]|PCA9544_IRQ_MASK;
        regval[0] = regval[0]>>4;

        printf("PCA9544 IRQ STATUS is %d\n", regval[0]);
    }

    return ret;
}

int pca9544_set_chan(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t chan)
{

    int ret = 0;

    ret = pca9544_write_reg(i2c, dev_addr, chan)
    if(ret)
    {
        printf("set PCA9544 channel failed\n");
    }

    return ret;
}

int pca9544_clean_chan(i2c_dev_t *i2c, uint16_t dev_addr)
{

    int ret = 0;

    ret = pca9544_write_reg(i2c, dev_addr, PCA9544_CHAN_CLEAN)
    if(ret)
    {
        printf("clean PCA9544 channel failed\n");
    }

    return ret;
}

int pca9544_write_reg(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t regval)
{

    int ret = 0;

    ret = hal_i2c_init(i2c);
    if(ret)
    {
        printf("Host I2C open failed\n");
        hal_i2c_finalize(i2c);
    }

    ret = hal_i2c_master_send(i2c, dev_addr, &regval, siezof(regval), 0);
    if(ret)
    {
        printf("write PCA9544 reg failed\n");
        hal_i2c_finalize(i2c);
    }

    return ret;
}

int pca9544_read_reg(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t* data)
{
    int ret = 0;

    ret = hal_i2c_init(i2c);
    if(ret)
    {
        printf("Host I2C open failed\n");
        hal_i2c_finalize(i2c);
    }

    ret = hal_i2c_master_recv(i2c, dev_addr, data, PCA9544_CONFIG_REG_LEN, 1000);
    if(ret)
    {
        printf("Read PCA9544 reg failed\n");
        hal_i2c_finalize(i2c);
    }

    return ret;
}


