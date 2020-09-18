#ifndef HAL_PCA9544_H
#define HAL_PCA9544_H

#ifdef __cplusplus
extern "C" {
#endif

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


int PCA9544_init(pca9544_dev_cfg_t* dev_cfg);
int pca954x_irq_handler(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t* data);
int pca9544_get_irq_state(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t* regval);
int pca9544_set_chan(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t chan);
int pca9544_clean_chan(i2c_dev_t *i2c, uint16_t dev_addr);
int pca9544_write_reg(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t regval);
int pca9544_read_reg(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t* data);

ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HAL_PCA9544_H */
