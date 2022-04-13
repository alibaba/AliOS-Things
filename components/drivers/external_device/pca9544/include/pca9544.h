#ifndef PCA9544_H
#define PCA9544_H

#ifdef __cplusplus
extern "C" {
#endif

/* ===== System include ===== */
#include "aos/hal/i2c.h"

/* ===== Default Configuration Based on BSP ===== */
#ifdef HAAS_I2C
#define I2C_PORT 1
#define I2C_ADDR_W_8BIT 8
#define I2C_BR_100K 100000
#endif

/* ===== MACRO Definition ===== */
#define PCA9544_CHAN_CLEAN 0
#define PCA9544_CONFIG_REG_LEN 1U
#define PCA9544_IRQ_MASK 0xF0
#define PCA9544_BASE_ADDR 0x70

/* ===== Dev Type Definition ===== */
/* pca9544 channel list */
typedef enum {
    PCA9544_CH_NULL = 0,
    PCA9544_CHO     = 4,
    PCA9544_CH1     = 5,
    PCA9544_CH2     = 6,
    PCA9544_CH3     = 7,
} PCA9544_CH_E;

/* pca9544 parameters configuration */
typedef struct {
    /* pca9544 i2c address */
    uint8_t dev_addr;
    /* pca9544 interrupt status */
    uint8_t irq_status;
    /* the current pca9544 channel */
    PCA9544_CH_E pca9544_ch;
    /* the sub dev addr for the selected pca9544 channel */
    uint8_t subdev_addr;
    // uint8_t control_reg;
    /* the sub dev register address */
    uint8_t reg_addr;
} PCA9544_DEV_CFG_T;

/* ===== API Lists ===== */
/**********************************************************
 * @fun    pca9544_init
 * @breif  pca9544 initialization
 * @param  i2c:the pointer for i2c configuration
 * @param  dev_cfg: the pointer for dev configuration
 * @rtn
 *********************************************************/
int pca9544_init(i2c_dev_t *i2c, PCA9544_DEV_CFG_T *dev_cfg);

/**********************************************************
 * @fun    pca9544_deinit
 * @breif  pca9544 deinitialization
 * @param  none
 * @rtn    none
 *********************************************************/
void pca9544_deinit();

/**********************************************************
 * @fun    pca9544_set_chan
 * @breif  pca9544 channel set
 * @param  chan: the object channel
 * @rtn    ret
 *********************************************************/
int pca9544_set_chan(PCA9544_CH_E chan);

/**********************************************************
 * @fun    pca9544_clean_chan
 * @breif  pca9544 channel clean
 * @param  none
 * @rtn    ret
 *********************************************************/
int pca9544_clean_chan();

/**********************************************************
 * @fun    pca9544_read_chan
 * @breif  pca9544 channel reading
 * @param  none
 * @rtn    chan: the current selected channel of pca9544
 *********************************************************/
PCA9544_CH_E pca9544_read_chan();

/**********************************************************
 * @fun    pca9544_write_reg
 * @breif  pca9544 register writing operation
 * @param  dev_addr: i2c device address
 * @param  regval: register value
 * @rtn    ret
 *********************************************************/
int pca9544_write_reg(uint16_t dev_addr, uint8_t regval);

/**********************************************************
 * @fun    pca9544_read_reg
 * @breif  pca9544 register reading operation
 * @param  dev_addr: i2c device address
 * @param  regval: register value
 * @rtn    ret
 *********************************************************/
int pca9544_read_reg(uint16_t dev_addr, uint8_t *data);

int pca954x_irq_handler(uint16_t dev_addr, uint8_t *data);
int pca9544_get_irq_state(uint16_t dev_addr, uint8_t *regval);

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
int pca9544_raw_data_write(uint16_t subdev_addr, uint16_t reg_addr, uint16_t size, uint8_t *data, uint16_t len);

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
int pca9544_raw_data_read(uint16_t subdev_addr, uint16_t reg_addr, uint16_t size, uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PCA9544_H */
