#include "include.h"
#include "arm_arch.h"

#include "i2c1.h"
#include "i2c_pub.h"

#include "intc_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"

#include "drv_model_pub.h"
#include "mem_pub.h"

typedef struct i2c1_msg {
	UINT8 TxMode;		//0: Read;  1: Write
	UINT8 RegAddr;
    UINT16 RemainNum;
	UINT8 *pData;
	volatile UINT8 TransDone;
} I2C1_MSG_ST, *I2C1_MSG_PTR;

static DD_OPERATIONS i2c1_op =
{
    i2c1_open,
    i2c1_close,
    i2c1_read,
    i2c1_write,
    i2c1_ctrl
};

I2C1_MSG_ST gi2c1;

static void i2c1_set_ensmb(UINT32 enable)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    if(enable)
        reg_val |= I2C1_ENSMB;
    else
        reg_val &= ~I2C1_ENSMB;
    REG_WRITE(reg_addr, reg_val);
}

static void i2c1_set_smbus_sta(UINT32 enable)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    if(enable)
        reg_val |= I2C1_STA;
    else
        reg_val &= ~I2C1_STA;
    REG_WRITE(reg_addr, reg_val);
}

static void i2c1_set_smbus_stop(UINT32 enable)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    if(enable)
        reg_val |= I2C1_STO;
    else
        reg_val &= ~I2C1_STO;
    REG_WRITE(reg_addr, reg_val);
}

static void i2c1_set_smbus_ack_tx(UINT32 enable)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    if(enable)
        reg_val |= I2C1_ACK_TX;
    else
        reg_val &= ~I2C1_ACK_TX;
    REG_WRITE(reg_addr, reg_val);
}

static void i2c1_set_smbus_tx_mode(UINT32 enable)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    if(enable)
        reg_val |= I2C1_TX_MODE;
    else
        reg_val &= ~I2C1_TX_MODE;
    REG_WRITE(reg_addr, reg_val);
}

static void i2c1_set_freq_div(UINT32 div)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);
    
    reg_val = (reg_val & ~(I2C1_FREQ_DIV_MASK << I2C1_FREQ_DIV_POSI)) 
        | ((div & I2C1_FREQ_DIV_MASK) << I2C1_FREQ_DIV_POSI);
    REG_WRITE(reg_addr, reg_val);
}

static UINT32 i2c1_get_smbus_interrupt(void)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    return (reg_val & I2C1_SI)? 1: 0;
}

static void i2c1_clear_smbus_interrupt(void)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    reg_val |= I2C1_SI;

    REG_WRITE(reg_addr, reg_val);
}

static UINT32 i2c1_get_ack_rx(void)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    return (reg_val & I2C1_ACK_RX)? 1: 0;
}

static UINT32 i2c1_get_ack_req(void)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    return (reg_val & I2C1_ACK_REQ)? 1: 0;
}

static UINT32 i2c1_get_smbus_busy(void)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    return (reg_val & I2C1_BUSY)? 1: 0;
}

////////////////////////////////////////////////////////////////////////////////
static void i2c1_gpio_config(void)
{
    UINT32 param;

    param = GFUNC_MODE_I2C1;
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
}

static void i2c1_power_up(void)
{
    UINT32 param;
    param = PWD_I2C1_CLK_BIT;
	sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param);
}

static void i2c1_power_down(void)
{
    UINT32 param;
    param = PWD_I2C1_CLK_BIT;
	sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, &param);
}

static void i2c1_enable_interrupt(void)
{
    UINT32 param;
    param = (IRQ_I2C1_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);
}

static void i2c1_disable_interrupt(void)
{
    UINT32 param;
    param = (IRQ_I2C1_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
}

static void i2c1_isr(void)
{
    UINT32 I2cStat;

    I2cStat = REG_READ(REG_I2C1_CONFIG);
    if(I2cStat & I2C1_SI)
    {
        if(I2cStat & I2C1_STA) {
            I2cStat &= ~I2C1_STA; //Clear STA
            REG_WRITE(REG_I2C1_CONFIG, I2cStat);
            if(!(I2cStat & I2C1_ACK_RX)) {
                I2cStat |= I2C1_STO;
                REG_WRITE(REG_I2C1_CONFIG, I2cStat); //Set STO
                gi2c1.TransDone = 1;
            } else {
                if(gi2c1.TxMode)
                    REG_WRITE(REG_I2C1_DAT, gi2c1.RegAddr); //Tx Reg Address	
                else {
                    I2cStat &= ~I2C1_TX_MODE;
                    REG_WRITE(REG_I2C1_CONFIG, I2cStat); //Clear TXMODE
                }
            }
        } else {
            if(gi2c1.TxMode) { //Reg Write
                if(!(I2cStat & I2C1_ACK_RX) || !gi2c1.RemainNum) {	//Rx NACK or All Data TX Done
                    I2cStat |= I2C1_STO;
                    REG_WRITE(REG_I2C1_CONFIG, I2cStat); //Set STO	
                    gi2c1.TransDone = 1;
                } else {
                    REG_WRITE(REG_I2C1_DAT, *(gi2c1.pData++));
                    gi2c1.RemainNum --;
                }
            } else  { //Reg Read
                *(gi2c1.pData++) = REG_READ(REG_I2C1_DAT);
                gi2c1.RemainNum --;
                if(gi2c1.RemainNum) {
                    I2cStat |= I2C1_ACK_TX;
                    REG_WRITE(REG_I2C1_CONFIG, I2cStat); //Set STO	
                } else {
                    I2cStat &= ~I2C1_ACK_TX;
                    REG_WRITE(REG_I2C1_CONFIG, I2cStat);

                    REG_WRITE(REG_I2C1_CONFIG, I2C1_STO);
                    gi2c1.TransDone = 1;
                }
            }
        }
        I2cStat &= ~I2C1_SI;
        REG_WRITE(REG_I2C1_CONFIG, I2cStat);  //Clear SI
    }  
}

static void i2c1_software_init(void)
{
    ddev_register_dev(I2C1_DEV_NAME, &i2c1_op);
}

static void i2c1_hardware_init(void)
{
    /* register interrupt */
    intc_service_register(IRQ_I2C1, PRI_IRQ_I2C1, i2c1_isr);

    /* clear all setting */
    REG_WRITE(REG_I2C1_CONFIG, 0);
}

void i2c1_init(void)
{
    os_memset(&gi2c1, 0, sizeof(I2C1_MSG_ST));
    i2c1_software_init();
    i2c1_hardware_init();
}

void i2c1_exit(void)
{
    REG_WRITE(REG_I2C1_CONFIG, 0);

    ddev_unregister_dev(I2C1_DEV_NAME);
}

static UINT32 i2c1_open(UINT32 op_flag)
{
   // UINT32 reg;
    if(op_flag) {
        i2c1_set_freq_div(op_flag);
    } else {
        i2c1_set_freq_div(I2C_CLK_DIVID(I2C_DEFAULT_BAUD));  // 400KHZ
    }

    i2c1_enable_interrupt();
    i2c1_power_up();
    i2c1_gpio_config();

    //i2c1_set_ensmb(1);

    return I2C1_SUCCESS;
}

static UINT32 i2c1_close(void)
{
    i2c1_set_ensmb(0);
    i2c1_disable_interrupt();
    i2c1_power_down();

    return I2C1_SUCCESS;
}

static UINT32 i2c1_read(char *user_buf, UINT32 count, UINT32 op_flag)
{
    UINT32 reg;
    I2C_OP_PTR i2c_op;
    GLOBAL_INT_DECLARATION();

    i2c_op = (I2C_OP_PTR)op_flag;

    // write cycle, write the subaddr to device
    GLOBAL_INT_DISABLE();
    gi2c1.TxMode = 1;
    gi2c1.RemainNum = 0;
    gi2c1.RegAddr = i2c_op->op_addr;
    gi2c1.pData = NULL;
    gi2c1.TransDone = 0;
    GLOBAL_INT_RESTORE();

    reg = ((i2c_op->salve_id & 0x7f)<<1) + 0;  //SET LSB 0
    REG_WRITE(REG_I2C1_DAT, reg);

    reg = REG_READ(REG_I2C1_CONFIG);
    reg |= I2C1_STA | I2C1_TX_MODE | I2C1_ENSMB;//Set STA & TXMODE
    REG_WRITE(REG_I2C1_CONFIG, reg);

    while(!gi2c1.TransDone);

    // read cycle, read the data. 
    GLOBAL_INT_DISABLE();
    gi2c1.TxMode = 0;
    gi2c1.RemainNum = count;
    gi2c1.RegAddr = 0;
    gi2c1.pData = (UINT8*)user_buf;
    gi2c1.TransDone = 0;
    GLOBAL_INT_RESTORE();

    reg = ((i2c_op->salve_id & 0x7f)<<1) + 1;  //SET LSB 1
    REG_WRITE(REG_I2C1_DAT, reg);

    reg = REG_READ(REG_I2C1_CONFIG);
    reg |= I2C1_STA | I2C1_TX_MODE | I2C1_ENSMB;//Set STA & TXMODE
    REG_WRITE(REG_I2C1_CONFIG, reg);

    while(!gi2c1.TransDone);

    return count;
}

static UINT32 i2c1_write(char *user_buf, UINT32 count, UINT32 op_flag)
{
    UINT32 reg;
    I2C_OP_PTR i2c_op;
    GLOBAL_INT_DECLARATION();

    i2c_op = (I2C_OP_PTR)op_flag;

    GLOBAL_INT_DISABLE();
    gi2c1.TxMode = 1;
    gi2c1.RemainNum = count;
    gi2c1.RegAddr = i2c_op->op_addr;
    gi2c1.pData = (UINT8 *)user_buf;
    gi2c1.TransDone = 0;
    GLOBAL_INT_RESTORE();

    reg = ((i2c_op->salve_id & 0x7f)<<1) + 0;  //SET LSB 0
    REG_WRITE(REG_I2C1_DAT, reg);

    reg = REG_READ(REG_I2C1_CONFIG);
    reg |= I2C1_STA | I2C1_TX_MODE | I2C1_ENSMB;//Set STA & TXMODE
    REG_WRITE(REG_I2C1_CONFIG, reg);

    while(!gi2c1.TransDone);

    return count;
}

static UINT32 i2c1_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret = I2C1_SUCCESS;

    switch(cmd)
    {
    case I2C1_CMD_SET_ENSMB:
        i2c1_set_ensmb(*((UINT32 *)param));
        break;
    case I2C1_CMD_SET_SMBUS_STA:
        i2c1_set_smbus_sta(*((UINT32 *)param));
        break;
    case I2C1_CMD_SET_SMBUS_STOP:
        i2c1_set_smbus_stop(*((UINT32 *)param));
        break;
    case I2C1_CMD_SET_SMBUS_ACK_TX:
        i2c1_set_smbus_ack_tx(*((UINT32 *)param));
        break;      
    case I2C1_CMD_SET_SMBUS_TX_MODE:
        i2c1_set_smbus_tx_mode(*((UINT32 *)param));
        break;        
    case I2C1_CMD_SET_FREQ_DIV:
        i2c1_set_freq_div(*((UINT32 *)param));
        break;
    case I2C1_CMD_GET_SMBUS_INTERRUPT:
        ret = i2c1_get_smbus_interrupt();
        break;
    case I2C1_CMD_CLEAR_SMBUS_INTERRUPT:
        i2c1_clear_smbus_interrupt();
        break;
    case I2C1_CMD_GET_ACK_RX:
        ret = i2c1_get_ack_rx();
        break;      
    case I2C1_CMD_GET_ACK_REQ:
        ret = i2c1_get_ack_req();
        break; 
    case I2C1_CMD_GET_SMBUS_BUSY:
        ret = i2c1_get_smbus_busy();
        break;
        
    default:
        break;
    }
    
    return ret;
}

