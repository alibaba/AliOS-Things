#include "include.h"
#include "arm_arch.h"

#include "i2c2.h"
#include "i2c_pub.h"

#include "intc_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"

#include "drv_model_pub.h"
#include "mem_pub.h"

typedef struct i2c2_msg {
	UINT8 TxMode;		//0: Read;  1: Write
	UINT8 RegAddr;
    UINT16 RemainNum;
	UINT8 *pData;
	volatile UINT8 TransDone;
} I2C2_MSG_ST, *I2C2_MSG_PTR;

static DD_OPERATIONS i2c2_op =
{
    i2c2_open,
    i2c2_close,
    i2c2_read,
    i2c2_write,
    i2c2_ctrl
};

I2C2_MSG_ST gi2c2;

static void i2c2_set_idle_cr(UINT32 idle_cr)
{
    UINT32 reg_addr = REG_I2C2_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    reg_val = (reg_val & ~(I2C2_IDLE_CR_MASK<< I2C2_IDLE_CR_POSI)) 
        | ((idle_cr & I2C2_IDLE_CR_MASK) << I2C2_IDLE_CR_POSI);
    
    REG_WRITE(reg_addr, reg_val);
}

static void i2c2_set_scl_cr(UINT32 scl_cr)
{
    UINT32 reg_addr = REG_I2C2_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    reg_val = (reg_val & ~(I2C2_SCL_CR_MASK<< I2C2_SCL_CR_POSI)) 
        | ((scl_cr & I2C2_SCL_CR_MASK) << I2C2_SCL_CR_POSI);
    
    REG_WRITE(reg_addr, reg_val);
}

static void i2c2_set_freq_div(UINT32 div)
{
    UINT32 reg_addr = REG_I2C2_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    reg_val = (reg_val & ~(I2C2_FREQ_DIV_MASK<< I2C2_FREQ_DIV_POSI)) 
        | ((div & I2C2_FREQ_DIV_MASK) << I2C2_FREQ_DIV_POSI);
    
    REG_WRITE(reg_addr, reg_val);
}

static void i2c2_set_slave_addr(UINT32 addr)
{
    UINT32 reg_addr = REG_I2C2_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);
    
    reg_val = (reg_val & ~(I2C2_SLV_ADDR_MASK<< I2C2_SLV_ADDR_POSI)) 
        | ((addr & I2C2_SLV_ADDR_MASK) << I2C2_SLV_ADDR_POSI);
    REG_WRITE(reg_addr, reg_val);
}

static void i2c2_set_smbus_cs(UINT32 cs)
{
    UINT32 reg_addr = REG_I2C2_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);
    
    reg_val = (reg_val & ~(I2C2_SMB_CS_MASK<< I2C2_SMB_CS_POSI)) 
        | ((cs & I2C2_SMB_CS_MASK) << I2C2_SMB_CS_POSI);
    REG_WRITE(reg_addr, reg_val);
}

static void i2c2_set_timeout_en(UINT32 timeout)
{
    UINT32 reg_addr = REG_I2C2_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);
    
    if(timeout)
        reg_val |= I2C2_SMB_TOE;
    else
        reg_val &= ~I2C2_SMB_TOE;
    REG_WRITE(reg_addr, reg_val);
}

static void i2c2_set_free_detect(UINT32 free)
{
    UINT32 reg_addr = REG_I2C2_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);
    
    if(free)
        reg_val |= I2C2_SMB_FTE;
    else
        reg_val &= ~I2C2_SMB_FTE;
    REG_WRITE(reg_addr, reg_val);
}

static void i2c2_set_salve_en(UINT32 en)
{
    UINT32 reg_addr = REG_I2C2_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);
    
    if(en)
        reg_val |= I2C2_INH;
    else
        reg_val &= ~I2C2_INH;
    REG_WRITE(reg_addr, reg_val);
}

static void i2c2_set_smbus_en(UINT32 en)
{
    UINT32 reg_addr = REG_I2C2_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);
    
    if(en)
        reg_val |= I2C2_ENSMB;
    else
        reg_val &= ~I2C2_ENSMB;
    REG_WRITE(reg_addr, reg_val);
}

////////////////////////////////////////////////////////////////////////////////
static void i2c2_gpio_config(void)
{
    UINT32 param;

    param = GFUNC_MODE_I2C2;
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
}

static void i2c2_power_up(void)
{
    UINT32 param;
    param = PWD_I2C2_CLK_BIT;
	sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param);
}

static void i2c2_power_down(void)
{
    UINT32 param;
    param = PWD_I2C2_CLK_BIT;
	sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, &param);
}

static void i2c2_enable_interrupt(void)
{
    UINT32 param;
    param = (IRQ_I2C2_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);
}

static void i2c2_disable_interrupt(void)
{
    UINT32 param;
    param = (IRQ_I2C2_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
}

static void i2c2_isr(void)
{
    UINT32 I2cStat;

    I2cStat = REG_READ(REG_I2C2_STA);
    if(I2cStat & I2C2_SMBUS_SI)
    {
        if(I2cStat & I2C2_SMBUS_STA) {
            I2cStat &= ~I2C2_SMBUS_STA; //Clear STA
            REG_WRITE(REG_I2C2_STA, I2cStat);
            if(!(I2cStat & I2C2_SMBUS_ACK)) {
                I2cStat |= I2C2_SMBUS_STOP;
                REG_WRITE(REG_I2C2_STA, I2cStat); //Set STO
                gi2c2.TransDone = 1;
            } else {
                if(gi2c2.TxMode)
                    REG_WRITE(REG_I2C2_DAT, gi2c2.RegAddr); //Tx Reg Address	
                else {
                    //I2cStat &= ~i2c2_TX_MODE;
                    //REG_WRITE(REG_i2c2_CONFIG, I2cStat); //Clear TXMODE
                }
            }
        } else {
            if(gi2c2.TxMode) { //Reg Write
                if(!(I2cStat & I2C2_SMBUS_ACK) || !gi2c2.RemainNum) {	//Rx NACK or All Data TX Done
                    I2cStat |= I2C2_SMBUS_STOP;
                    REG_WRITE(REG_I2C2_STA, I2cStat); //Set STO	
                    gi2c2.TransDone = 1;
                } else {
                    REG_WRITE(REG_I2C2_DAT, *(gi2c2.pData++));
                    gi2c2.RemainNum --;
                }
            } else  { //Reg Read
                *(gi2c2.pData++) = REG_READ(REG_I2C2_DAT);
                gi2c2.RemainNum --;
                if(gi2c2.RemainNum) {
                    I2cStat |= I2C2_SMBUS_ACK;
                    REG_WRITE(REG_I2C2_STA, I2cStat); //Set STO	
                } else {
                    I2cStat &= ~I2C2_SMBUS_ACK;
                    REG_WRITE(REG_I2C2_STA, I2cStat);

                    I2cStat |= I2C2_SMBUS_STOP;
                    REG_WRITE(REG_I2C2_STA, I2cStat);
                    gi2c2.TransDone = 1;
                }
            }
        }
        I2cStat &= ~I2C2_SMBUS_SI;
        REG_WRITE(REG_I2C2_STA, I2cStat);  //Clear SI
    }  
}

static void i2c2_software_init(void)
{
    ddev_register_dev(I2C2_DEV_NAME, &i2c2_op);
}

static void i2c2_hardware_init(void)
{
    //UINT32 reg;
    /* register interrupt */
    intc_service_register(IRQ_I2C2, PRI_IRQ_I2C2, i2c2_isr);

    /* clear all setting */
    REG_WRITE(REG_I2C2_CONFIG, 0);
    REG_WRITE(REG_I2C2_STA, 0);    
}

void i2c2_init(void)
{
    os_memset(&gi2c2, 0, sizeof(I2C2_MSG_ST));
    i2c2_software_init();
    i2c2_hardware_init();
}

void i2c2_exit(void)
{
    /* clear all setting */
    REG_WRITE(REG_I2C2_CONFIG, 0);
    REG_WRITE(REG_I2C2_STA, 0);  

    ddev_unregister_dev(I2C2_DEV_NAME);
}

static UINT32 i2c2_open(UINT32 op_flag)
{
    //UINT32 reg;
    REG_WRITE(REG_I2C2_CONFIG, 0);
    REG_WRITE(REG_I2C2_STA, 0);
    
    if(op_flag) {
        i2c2_set_freq_div(op_flag);
    } else {
        i2c2_set_freq_div(I2C_CLK_DIVID(I2C_DEFAULT_BAUD));
    }

    i2c2_set_idle_cr(0x3);
    i2c2_set_scl_cr(0x4);
    i2c2_set_smbus_cs(0x3);
    i2c2_set_timeout_en(1);
    i2c2_set_free_detect(1); 
    i2c2_set_slave_addr(0xf);
    i2c2_set_salve_en(1);  // diable i2c slave

    i2c2_enable_interrupt();
    i2c2_power_up();
    i2c2_gpio_config();

    i2c2_set_smbus_en(1);

    return I2C2_SUCCESS;
}

static UINT32 i2c2_close(void)
{
    i2c2_set_smbus_en(0);
    
    i2c2_disable_interrupt();
    i2c2_power_down();

    return I2C2_SUCCESS;
}

static UINT32 i2c2_read(char *user_buf, UINT32 count, UINT32 op_flag)
{
    UINT32 reg;
    I2C_OP_PTR i2c_op;
    GLOBAL_INT_DECLARATION();

    i2c_op = (I2C_OP_PTR)op_flag;

    // write cycle, write the subaddr to device
    GLOBAL_INT_DISABLE();
    gi2c2.TxMode = 1;
    gi2c2.RemainNum = 0;
    gi2c2.RegAddr = i2c_op->op_addr;
    gi2c2.pData = NULL;
    gi2c2.TransDone = 0;
    GLOBAL_INT_RESTORE();

    reg = ((i2c_op->salve_id & 0x7f)<<1) + 0;  //SET LSB 0
    REG_WRITE(REG_I2C2_DAT, reg);

    reg = REG_READ(REG_I2C2_STA);
    reg |= I2C2_SMBUS_STA;  //Set STA & TXMODE
    REG_WRITE(REG_I2C2_STA, reg);

    while(!gi2c2.TransDone);

    // read cycle, read the data. 
    GLOBAL_INT_DISABLE();
    gi2c2.TxMode = 0;
    gi2c2.RemainNum = count;
    gi2c2.RegAddr = 0;
    gi2c2.pData = (UINT8*)user_buf;
    gi2c2.TransDone = 0;
    GLOBAL_INT_RESTORE();

    reg = ((i2c_op->salve_id & 0x7f)<<1) + 1;  //SET LSB 1
    REG_WRITE(REG_I2C2_DAT, reg);

    reg = REG_READ(REG_I2C2_STA);
    reg |= I2C2_SMBUS_STA;  //Set STA & TXMODE
    REG_WRITE(REG_I2C2_STA, reg);

    while(!gi2c2.TransDone);

    return count;
}

static UINT32 i2c2_write(char *user_buf, UINT32 count, UINT32 op_flag)
{
    UINT32 reg;
    I2C_OP_PTR i2c_op;
    GLOBAL_INT_DECLARATION();

    i2c_op = (I2C_OP_PTR)op_flag;

    GLOBAL_INT_DISABLE();
    gi2c2.TxMode = 1;
    gi2c2.RemainNum = count;
    gi2c2.RegAddr = i2c_op->op_addr;
    gi2c2.pData = (UINT8 *)user_buf;
    gi2c2.TransDone = 0;
    GLOBAL_INT_RESTORE();

    reg = ((i2c_op->salve_id & 0x7f)<<1) + 0;  //SET LSB 0
    REG_WRITE(REG_I2C2_DAT, reg);

    reg = REG_READ(REG_I2C2_STA);
    reg |= I2C2_SMBUS_STA;  //Set STA & TXMODE
    REG_WRITE(REG_I2C2_STA, reg);

    while(!gi2c2.TransDone);

    return count;
}

static UINT32 i2c2_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret = I2C2_SUCCESS;

    switch(cmd)
    {

        
    default:
        break;
    }
    
    return ret;
}


