#include <stdio.h>
#include "lega_cm4.h"
#include "lega_common.h"
#include "lega_peripheral_reg.h"
#include "lega_i2c.h"
#include "lega_timer.h"

lega_i2c_slv_callback_t g_lega_i2c_slv_callback_handler[LEGA_I2C_NUM];
lega_timer_dev_t g_lega_timer1;
volatile int g_lega_i2c_timeout = 0;

void I2C0_IRQHandler(void)
{
    lega_intrpt_enter();
    uint8_t rx_data;
    if(I2C0->IC_INTR_STAT & RD_REQ)
    {
        if(I2C0->IC_RAW_INTR_STAT & TX_ABRT)
        {
            I2C0->IC_CLR_TX_ABRT; //clear interrupt
        }
        I2C0->IC_CLR_RD_REG; //clear interrupt
        if(g_lega_i2c_slv_callback_handler[I2C_DEVICE0].tx_func)
        {
            g_lega_i2c_slv_callback_handler[I2C_DEVICE0].tx_func();
        }
    }
    if(I2C0->IC_INTR_STAT & RX_FULL)
    {
        rx_data = I2C0->IC_DATA_CMD & 0xFF;
        if(g_lega_i2c_slv_callback_handler[I2C_DEVICE0].rx_func)
        {
            g_lega_i2c_slv_callback_handler[I2C_DEVICE0].rx_func(rx_data);
        }
    }
    lega_intrpt_exit();
}

void I2C1_IRQHandler(void)
{
    lega_intrpt_enter();
    uint8_t rx_data;
    if(I2C1->IC_INTR_STAT & RD_REQ)
    {
        if(I2C1->IC_RAW_INTR_STAT & TX_ABRT)
        {
            I2C1->IC_CLR_TX_ABRT; //clear interrupt
        }
        I2C1->IC_CLR_RD_REG; //clear interrupt
        if(g_lega_i2c_slv_callback_handler[I2C_DEVICE1].tx_func)
        {
            g_lega_i2c_slv_callback_handler[I2C_DEVICE1].tx_func();
        }
    }
    if(I2C1->IC_INTR_STAT & RX_FULL)
    {
        rx_data = I2C1->IC_DATA_CMD & 0xFF;
        if(g_lega_i2c_slv_callback_handler[I2C_DEVICE1].rx_func)
        {
            g_lega_i2c_slv_callback_handler[I2C_DEVICE1].rx_func(rx_data);
        }
    }
    lega_intrpt_exit();
}

int32_t lega_i2c_reset(lega_i2c_dev_t *i2c)
{
    if(NULL == i2c)
    {
        return -1;
    }
    if(I2C_DEVICE0 == i2c->port) // I2C_DEVICE0
    {
        REG_WR(I2C0_SW_RESET_REG, 1);
        delay(10);
        REG_WR(I2C0_SW_RESET_REG, 0);
    }
    else if(I2C_DEVICE1 == i2c->port) // I2C_DEVICE1
    {
        REG_WR(I2C1_SW_RESET_REG, 1);
        delay(10);
        REG_WR(I2C1_SW_RESET_REG, 0);
    }
    else
    {
        return -1;
    }
    return 0;
}

/**
 * Initialises an I2C interface
 * Prepares an I2C hardware interface for communication as a master or slave
 *
 * @param[in]  i2c  the device for which the i2c port should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t lega_i2c_init(lega_i2c_dev_t *i2c)
{
    uint32_t reg_value;
    I2C_TypeDef *I2Cx;
    if(NULL == i2c)
    {
        return -1;
    }
    if(I2C_DEVICE0 == i2c->port) // I2C_DEVICE0
    {
        I2Cx = I2C0;
    }
    else if(I2C_DEVICE1 == i2c->port) // I2C_DEVICE1
    {
        I2Cx = I2C1;
    }
    else
    {
        return -1;
    }
    //pinmux cfg
    if(I2C_DEVICE0 == i2c->port) // I2C_DEVICE0
    {
    #if (0 == I2C_PAD_GROUP) // set pad2:3 for i2c0 func 4
        reg_value = REG_RD(PINMUX_CTRL_REG0) & (~I2C0_PINMUX_MASK0);
        REG_WR(PINMUX_CTRL_REG0, (reg_value|I2C0_PINMUX_VALUE0));
    #else // set pad20:21 for i2c0 scl/sda func 1
        reg_value = REG_RD(PINMUX_CTRL_REG2) & (~I2C0_PINMUX_MASK1);
        REG_WR(PINMUX_CTRL_REG2, (reg_value|I2C0_PINMUX_VALUE1));
    #endif
    }
    else // I2C_DEVICE1
    {
    #if (0 == I2C_PAD_GROUP) // set pad 8:9 for i2c1 func 3
        reg_value = REG_RD(PINMUX_CTRL_REG1) & (~I2C1_PINMUX_MASK0);
        REG_WR(PINMUX_CTRL_REG1, (reg_value|I2C1_PINMUX_VALUE0));
    #else // set pad22:23 for i2c1 scl/sda func 1
        reg_value = REG_RD(PINMUX_CTRL_REG2) & (~I2C1_PINMUX_MASK1);
        REG_WR(PINMUX_CTRL_REG2, (reg_value|I2C1_PINMUX_VALUE1));
    #endif
    }

    //i2c clk enable
    if(I2C_DEVICE0 == i2c->port) // I2C_DEVICE0
    {
        //I2C0 clock enable
        reg_value = REG_RD(I2C0_CLK_CFG);
        REG_WR(I2C0_CLK_CFG, (reg_value|I2C0_CLK_EN));
    }
    else // I2C_DEVICE1
    {
        //I2C1 clock enable
        reg_value = REG_RD(I2C1_CLK_CFG);
        REG_WR(I2C1_CLK_CFG, (reg_value|I2C1_CLK_EN));
    }

    I2Cx->IC_ENABLE = 0;
    while(I2Cx->IC_ENABLE_STATUS & 0x01);

    if(I2C_MODE_MASTER == i2c->config.mode) //master mode
    {
        if(i2c->config.freq <= I2C_SS_MAX_SCL_FREQ) // standard mode 100k
        {
            I2Cx->IC_SS_SCL_HCNT = (SYSTEM_CLOCK/2/i2c->config.freq - 12); //SS_SCL_HCNT;  //set i2c clock timing
            I2Cx->IC_SS_SCL_LCNT = (SYSTEM_CLOCK/2/i2c->config.freq - 1); //SS_SCL_LCNT;  //set i2c clock timing
            I2Cx->IC_CON = (IC_CON_SS_MODE);
        }
        else if(i2c->config.freq <= I2C_FS_PLUS_MAX_SCL_FREQ) // fast mode 400k, fast mode plus 1m
        {
            I2Cx->IC_FS_SCL_HCNT = (SYSTEM_CLOCK/2/i2c->config.freq - 12); //FS_SCL_HCNT;  //set i2c clock timing
            I2Cx->IC_FS_SCL_LCNT = (SYSTEM_CLOCK/2/i2c->config.freq - 1); //FS_SCL_LCNT;  //set i2c clock timing
            I2Cx->IC_CON = (IC_CON_FS_MODE);
        }
        else if(i2c->config.freq <= I2C_HS_MAX_SCL_FREQ) // high speed 3.4m
        {
            I2Cx->IC_HS_SCL_HCNT = (SYSTEM_CLOCK/2/i2c->config.freq - 8); //HS_SCL_HCNT;  //set i2c clock timing
            I2Cx->IC_HS_SCL_LCNT = (SYSTEM_CLOCK/2/i2c->config.freq - 1); //HS_SCL_LCNT;  //set i2c clock timing
            I2Cx->IC_HS_MADDR = MASTER_CODE; //set master code: 0x0F(8 + IC_HS_MADDR)
            I2Cx->IC_CON = (IC_CON_HS_MODE);
        }
        else
        {
        }
        if(I2C_ADDR_WIDTH_10BIT == i2c->config.address_width)
        {
            I2Cx->IC_CON |= (IC_CON_IC_10BITADDR_MASTER);
        }
        I2Cx->IC_CON |= (IC_CON_MASTER_MODE | IC_CON_IC_RESTART_EN | IC_SLAVE_DISABLE | IC_CON_RX_FIFO_FULL_HLD_CTRL);
    #if 1
        I2Cx->IC_INTR_MASK = 0; //M_RX_FULL; //0x04; //enable RX_FULL -- 1: unmask interrupt 0: mask interrupt
    #endif
    }
    else //slave mode
    {
        if(i2c->config.freq <= I2C_SS_MAX_SCL_FREQ) // standard mode 100k
        {
            I2Cx->IC_CON = (IC_CON_SS_MODE);
        }
        else if(i2c->config.freq <= I2C_FS_PLUS_MAX_SCL_FREQ) // fast mode 400k, fast mode plus 1m
        {
            I2Cx->IC_CON = (IC_CON_FS_MODE);
        }
        else if(i2c->config.freq <= I2C_HS_MAX_SCL_FREQ) // high speed 3.4m
        {
            I2Cx->IC_CON = (IC_CON_HS_MODE);
        }
        else
        {
        }
        if(I2C_ADDR_WIDTH_7BIT == i2c->config.address_width)
        {
            I2Cx->IC_SAR = i2c->config.dev_addr;
        }
        else
        {
            I2Cx->IC_SAR = i2c->config.dev_addr;
            I2Cx->IC_CON |= (IC_CON_IC_10BITADDR_SLAVE);
        }
        I2Cx->IC_CON |= (IC_CON_IC_RESTART_EN | IC_CON_RX_FIFO_FULL_HLD_CTRL);
    #if 1
        I2Cx->IC_INTR_MASK = (M_RX_FULL | M_RD_REQ); //0x24; //enable RX_FULL and RD_REQ
    #endif
        if(i2c->priv)
        {
            g_lega_i2c_slv_callback_handler[i2c->port].tx_func = ((lega_i2c_slv_callback_t *)(i2c->priv))->tx_func;
            g_lega_i2c_slv_callback_handler[i2c->port].rx_func = ((lega_i2c_slv_callback_t *)(i2c->priv))->rx_func;
        }
    }

    I2Cx->IC_RX_TL = 0;
    I2Cx->IC_TX_TL = 0;
    I2Cx->IC_ENABLE = 1;
    while(!(I2Cx->IC_ENABLE_STATUS & 0x01));
#if 1
    //TODO: I2C interrupt enable should be put before I2C enable
    if(I2C_DEVICE0 == i2c->port) // I2C_DEVICE0
    {
        NVIC_EnableIRQ(I2C0_IRQn); //enable I2C0 interrupt
    }
    else
    {
        NVIC_EnableIRQ(I2C1_IRQn); //enable I2C1 interrupt
    }
#endif
    return 0;
}

void lega_timer1_irq_handler(void *arg)
{
    g_lega_i2c_timeout = 1;
}

/**
 * I2c master send
 *
 * @param[in]  i2c       the i2c device
 * @param[in]  dev_addr  device address
 * @param[in]  data      i2c send data
 * @param[in]  size      i2c send data size
 * @param[in]  timeout   timeout in ms
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t lega_i2c_master_send(lega_i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    //master write data
    I2C_TypeDef *I2Cx;
    uint16_t i = 0;
    int ret = 0;
    if(NULL == i2c)
    {
        return -1;
    }

    if(I2C_DEVICE0 == i2c->port) // I2C_DEVICE0
    {
        I2Cx = I2C0;
    }
    else if(I2C_DEVICE1 == i2c->port) // I2C_DEVICE1
    {
        I2Cx = I2C1;
    }
    else
    {
        return -1;
    }

    //set target addr
    if(I2C_ADDR_WIDTH_7BIT == i2c->config.address_width)
    {
        I2Cx->IC_TAR = dev_addr & 0x00FF;
    }
    else
    {
        I2Cx->IC_TAR = dev_addr;
    }

    if(0xFFFFFFFF != timeout)
    {
        g_lega_timer1.port = LEGA_TIMER1_INDEX;
        g_lega_timer1.config.reload_mode = TIMER_RELOAD_MANU;
        g_lega_timer1.config.cb = lega_timer1_irq_handler;
        g_lega_timer1.config.arg = NULL;

        g_lega_timer1.config.period = timeout*1000; //us

        g_lega_i2c_timeout = 0;
        lega_timer_init(&g_lega_timer1);
        lega_timer_start(&g_lega_timer1);
    }

    while(i < size)
    {
        if(I2Cx->IC_RAW_INTR_STAT & TX_ABRT)
        {
            ret = -1;
            goto EXIT;
        }
        if(g_lega_i2c_timeout)
        {
            g_lega_i2c_timeout = 0;
            ret = -1;
            goto EXIT;
        }
        if(I2Cx->IC_STATUS & TFNF)
        {
            if((size-1) == i)
            {
                I2Cx->IC_DATA_CMD = (IC_DATA_CMD_STOP | IC_DATA_CMD_WRITE | data[i++]);
            }
            else
            {
                I2Cx->IC_DATA_CMD = (IC_DATA_CMD_WRITE | data[i++]);
            }
        }
    }
    while(!(I2Cx->IC_RAW_INTR_STAT & M_STOP_DET))
    {
        if(g_lega_i2c_timeout)
        {
            g_lega_i2c_timeout = 0;
            ret = -1;
            goto EXIT;
        }
    }

EXIT:
    if(0xFFFFFFFF != timeout)
    {
        lega_timer_finalize(&g_lega_timer1);
    }
    return ret;
}

/**
 * I2c master recv
 *
 * @param[in]   i2c       the i2c device
 * @param[in]   dev_addr  device address
 * @param[out]  data      i2c receive data
 * @param[in]   size      i2c receive data size
 * @param[in]   timeout   timeout in ms
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t lega_i2c_master_recv(lega_i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    //master read data
    I2C_TypeDef *I2Cx;
    uint16_t i = 0;
    int ret = 0;
    if(NULL == i2c)
    {
        return -1;
    }

    if(I2C_DEVICE0 == i2c->port) // I2C_DEVICE0
    {
        I2Cx = I2C0;
    }
    else if(I2C_DEVICE1 == i2c->port) // I2C_DEVICE1
    {
        I2Cx = I2C1;
    }
    else
    {
        return -1;
    }

    //set target addr
    if(I2C_ADDR_WIDTH_7BIT == i2c->config.address_width)
    {
        I2Cx->IC_TAR = dev_addr & 0x00FF;
    }
    else
    {
        I2Cx->IC_TAR = dev_addr;
    }

    if(0xFFFFFFFF != timeout)
    {
        g_lega_timer1.port = LEGA_TIMER1_INDEX;
        g_lega_timer1.config.reload_mode = TIMER_RELOAD_MANU;
        g_lega_timer1.config.cb = lega_timer1_irq_handler;
        g_lega_timer1.config.arg = NULL;

        g_lega_timer1.config.period = timeout*1000; //us

        g_lega_i2c_timeout = 0;
        lega_timer_init(&g_lega_timer1);
        lega_timer_start(&g_lega_timer1);
    }

    while(i < size)
    {
        if(g_lega_i2c_timeout)
        {
            g_lega_i2c_timeout = 0;
            ret = -1;
            goto EXIT;
        }
        if(I2Cx->IC_STATUS & TFNF)
        {
            if((size-1) == i)
            {
                I2Cx->IC_DATA_CMD = (IC_DATA_CMD_STOP | IC_DATA_CMD_READ);
            }
            else
            {
                I2Cx->IC_DATA_CMD = IC_DATA_CMD_READ;
            }

            while(!(I2Cx->IC_RAW_INTR_STAT & RX_FULL))
            {
                if(I2Cx->IC_RAW_INTR_STAT & TX_ABRT)
                {
                    ret = -1;
                    goto EXIT;
                }
                if(g_lega_i2c_timeout)
                {
                    g_lega_i2c_timeout = 0;
                    ret = -1;
                    goto EXIT;
                }
            }
            *(data+i) = I2Cx->IC_DATA_CMD & 0xFF;
            i++;
        }
    }
    while(!(I2Cx->IC_RAW_INTR_STAT & M_STOP_DET))
    {
        if(g_lega_i2c_timeout)
        {
            g_lega_i2c_timeout = 0;
            ret = -1;
            goto EXIT;
        }
    }
EXIT:
    if(0xFFFFFFFF != timeout)
    {
        lega_timer_finalize(&g_lega_timer1);
    }
    return ret;
}

/**
 * I2c mem write
 *
 * @param[in]  i2c            the i2c device
 * @param[in]  dev_addr       device address
 * @param[in]  mem_addr       mem address
 * @param[in]  mem_addr_size  mem address
 * @param[in]  data           i2c master send data
 * @param[in]  size           i2c master send data size
 * @param[in]  timeout        timeout in ms
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t lega_i2c_mem_write(lega_i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    //master write data
    I2C_TypeDef *I2Cx;
    uint16_t i = 0;
    int ret = 0;
    if(NULL == i2c)
    {
        return -1;
    }

    if(I2C_DEVICE0 == i2c->port) // I2C_DEVICE0
    {
        I2Cx = I2C0;
    }
    else if(I2C_DEVICE1 == i2c->port) // I2C_DEVICE1
    {
        I2Cx = I2C1;
    }
    else
    {
        return -1;
    }

    if(I2C_ADDR_WIDTH_7BIT == i2c->config.address_width)
    {
        I2Cx->IC_TAR = dev_addr & 0x00FF;
    }
    else
    {
        I2Cx->IC_TAR = dev_addr;
    }

    if(0xFFFFFFFF != timeout)
    {
        g_lega_timer1.port = LEGA_TIMER1_INDEX;
        g_lega_timer1.config.reload_mode = TIMER_RELOAD_MANU;
        g_lega_timer1.config.cb = lega_timer1_irq_handler;
        g_lega_timer1.config.arg = NULL;

        g_lega_timer1.config.period = timeout*1000; //us

        g_lega_i2c_timeout = 0;
        lega_timer_init(&g_lega_timer1);
        lega_timer_start(&g_lega_timer1);
    }

    while(i < mem_addr_size) //I2C_MEM_ADDR_SIZE_8BIT or I2C_MEM_ADDR_SIZE_16BIT
    {
        if(I2Cx->IC_RAW_INTR_STAT & TX_ABRT)
        {
            ret = -1;
            goto EXIT;
        }
        if(g_lega_i2c_timeout)
        {
            g_lega_i2c_timeout = 0;
            ret = -1;
            goto EXIT;
        }
        if(I2Cx->IC_STATUS & TFNF)
        {
            I2Cx->IC_DATA_CMD = (IC_DATA_CMD_WRITE | ((mem_addr >> (8*i++)) & 0x00FF));
        }
    }

    i = 0;
    while(i < size)
    {
        if(I2Cx->IC_RAW_INTR_STAT & TX_ABRT)
        {
            ret = -1;
            goto EXIT;
        }
        if(g_lega_i2c_timeout)
        {
            g_lega_i2c_timeout = 0;
            ret = -1;
            goto EXIT;
        }
        if(I2Cx->IC_STATUS & TFNF)
        {
            if((size-1) == i)
            {
                I2Cx->IC_DATA_CMD = (IC_DATA_CMD_STOP | IC_DATA_CMD_WRITE | data[i++]);
            }
            else
            {
                I2Cx->IC_DATA_CMD = (IC_DATA_CMD_WRITE | data[i++]);
            }
        }
    }
    while(!(I2Cx->IC_RAW_INTR_STAT & M_STOP_DET))
    {
        if(g_lega_i2c_timeout)
        {
            g_lega_i2c_timeout = 0;
            ret = -1;
            goto EXIT;
        }
    }
EXIT:
    if(0xFFFFFFFF != timeout)
    {
        lega_timer_finalize(&g_lega_timer1);
    }
    return ret;
}

/**
 * I2c master mem read
 *
 * @param[in]   i2c            the i2c device
 * @param[in]   dev_addr       device address
 * @param[in]   mem_addr       mem address
 * @param[in]   mem_addr_size  mem address
 * @param[out]  data           i2c master send data
 * @param[in]   size           i2c master send data size
 * @param[in]   timeout        timeout in ms
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t lega_i2c_mem_read(lega_i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    //combined format
    I2C_TypeDef *I2Cx;
    uint16_t i = 0;
    int ret = 0;
    if(NULL == i2c)
    {
        return -1;
    }

    if(I2C_DEVICE0 == i2c->port) // I2C_DEVICE0
    {
        I2Cx = I2C0;
    }
    else if(I2C_DEVICE1 == i2c->port) // I2C_DEVICE1
    {
        I2Cx = I2C1;
    }
    else
    {
        return -1;
    }

    if(I2C_ADDR_WIDTH_7BIT == i2c->config.address_width)
    {
        I2Cx->IC_TAR = dev_addr & 0x00FF;
    }
    else
    {
        I2Cx->IC_TAR = dev_addr;
    }

    if(0xFFFFFFFF != timeout)
    {
        g_lega_timer1.port = LEGA_TIMER1_INDEX;
        g_lega_timer1.config.reload_mode = TIMER_RELOAD_MANU;
        g_lega_timer1.config.cb = lega_timer1_irq_handler;
        g_lega_timer1.config.arg = NULL;

        g_lega_timer1.config.period = timeout*1000; //us

        g_lega_i2c_timeout = 0;
        lega_timer_init(&g_lega_timer1);
        lega_timer_start(&g_lega_timer1);
    }

    while(i < mem_addr_size) //I2C_MEM_ADDR_SIZE_8BIT or I2C_MEM_ADDR_SIZE_16BIT
    {
        if(I2Cx->IC_RAW_INTR_STAT & TX_ABRT)
        {
            ret = -1;
            goto EXIT;
        }
        if(g_lega_i2c_timeout)
        {
            g_lega_i2c_timeout = 0;
            ret = -1;
            goto EXIT;
        }
        if(I2Cx->IC_STATUS & TFNF)
        {
            I2Cx->IC_DATA_CMD = (IC_DATA_CMD_WRITE | ((mem_addr >> (8*i++)) & 0x00FF));
        }
    }

    i = 0;
    while(i < size)
    {
        if(g_lega_i2c_timeout)
        {
            g_lega_i2c_timeout = 0;
            ret = -1;
            goto EXIT;
        }
        if(I2Cx->IC_STATUS & TFNF)
        {
            //master send read cmd
            if(0 == i)
            {
                I2Cx->IC_DATA_CMD = IC_DATA_CMD_RESTART | IC_DATA_CMD_READ; //IC_DATA_CMD_RESTART can be omit
            }
            else if((size - 1) == i)
            {
                I2Cx->IC_DATA_CMD = IC_DATA_CMD_STOP | IC_DATA_CMD_READ;
            }
            else
            {
                I2Cx->IC_DATA_CMD = IC_DATA_CMD_READ;
            }
            //master read data
            while(!(I2Cx->IC_RAW_INTR_STAT & RX_FULL))
            {
                if(I2Cx->IC_RAW_INTR_STAT & TX_ABRT)
                {
                    ret = -1;
                    goto EXIT;
                }
                if(g_lega_i2c_timeout)
                {
                    g_lega_i2c_timeout = 0;
                    ret = -1;
                    goto EXIT;
                }
            }
            *(data+i) = I2Cx->IC_DATA_CMD & 0xFF;
            i++;
        }
    }
    while(!(I2Cx->IC_RAW_INTR_STAT & M_STOP_DET))
    {
        if(g_lega_i2c_timeout)
        {
            g_lega_i2c_timeout = 0;
            ret = -1;
            goto EXIT;
        }
    }
EXIT:
    if(0xFFFFFFFF != timeout)
    {
        lega_timer_finalize(&g_lega_timer1);
    }
    return ret;
}

int32_t lega_i2c_tx_data(lega_i2c_dev_t *i2c, uint8_t data)
{
    I2C_TypeDef *I2Cx;
    if(NULL == i2c)
    {
        return -1;
    }
    if(I2C_DEVICE0 == i2c->port) // I2C_DEVICE0
    {
        I2Cx = I2C0;
    }
    else if(I2C_DEVICE1 == i2c->port) // I2C_DEVICE1
    {
        I2Cx = I2C1;
    }
    else
    {
        return -1;
    }

    I2Cx->IC_DATA_CMD = (IC_DATA_CMD_WRITE | data);
    return 0;
}

/**
 * Deinitialises an I2C device
 *
 * @param[in]  i2c  the i2c device
 *
 * @return  0 : on success, EIO : if an error occurred during deinitialisation
 */
int32_t lega_i2c_finalize(lega_i2c_dev_t *i2c)
{
    I2C_TypeDef *I2Cx;
    uint32_t reg_value;
    if(NULL == i2c)
    {
        return -1;
    }
    if(I2C_DEVICE0 == i2c->port) // I2C_DEVICE0
    {
        I2Cx = I2C0;
    }
    else if(I2C_DEVICE1 == i2c->port) // I2C_DEVICE1
    {
        I2Cx = I2C1;
    }
    else
    {
        return -1;
    }
    //i2c clk disable
    if(I2C_DEVICE0 == i2c->port) // I2C_DEVICE0
    {
        //I2C0 clock disable
        reg_value = REG_RD(I2C0_CLK_CFG);
        REG_WR(I2C0_CLK_CFG, (reg_value&(~I2C0_CLK_EN)));
    }
    else // I2C_DEVICE1
    {
        //I2C1 clock disable
        reg_value = REG_RD(I2C1_CLK_CFG);
        REG_WR(I2C1_CLK_CFG, (reg_value&(~I2C1_CLK_EN)));
    }

    // disable i2c cm4 irq
    if(I2C_DEVICE0 == i2c->port) // I2C_DEVICE0
    {
        NVIC_DisableIRQ(I2C0_IRQn); //disable I2C0 interrupt
    }
    else
    {
        NVIC_DisableIRQ(I2C1_IRQn); //disable I2C1 interrupt
    }

    //callback function pointer clear
    g_lega_i2c_slv_callback_handler[i2c->port].tx_func = NULL;
    g_lega_i2c_slv_callback_handler[i2c->port].rx_func = NULL;

    I2Cx->IC_ENABLE = 0;
    while(I2Cx->IC_ENABLE_STATUS & 0x01);
    return 0;
}

