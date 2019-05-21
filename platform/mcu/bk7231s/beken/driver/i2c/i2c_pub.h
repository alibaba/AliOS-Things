#ifndef __I2C1_PUB_H__
#define __I2C1_PUB_H__

#define I2C1_FAILURE                (1)
#define I2C1_SUCCESS                (0)
#define I2C2_FAILURE                (1)
#define I2C2_SUCCESS                (0)

#define I2C1_DEV_NAME               "i2c1"
#define I2C2_DEV_NAME               "i2c2"

#define I2C1_CMD_MAGIC              (0x0AB00000)
#define I2C2_CMD_MAGIC              (0x0ABF0000)

#include "uart_pub.h"
#define I2C_DEBUG
#ifdef I2C_DEBUG
#define I2C_PRT                    os_printf
#else
#define I2C_PRT                    null_prf
#define I2C_WPRT                   null_prf
#endif

enum
{
    I2C1_CMD_SET_ENSMB = I2C1_CMD_MAGIC + 1,
    I2C1_CMD_SET_SMBUS_STA,
    I2C1_CMD_SET_SMBUS_STOP,
    I2C1_CMD_SET_SMBUS_ACK_TX,
    I2C1_CMD_SET_SMBUS_TX_MODE,
    I2C1_CMD_SET_FREQ_DIV,
    I2C1_CMD_GET_SMBUS_INTERRUPT,
    I2C1_CMD_CLEAR_SMBUS_INTERRUPT,
    I2C1_CMD_GET_ACK_RX,
    I2C1_CMD_GET_ACK_REQ,
    I2C1_CMD_GET_SMBUS_BUSY,  
};

enum
{
    I2C2_CMD_SET_ENSMB = I2C2_CMD_MAGIC + 1,
 
};

typedef struct i2c_op_st {
    UINT8 salve_id;
    UINT8 op_addr;
} I2C_OP_ST, *I2C_OP_PTR;

#define NUM_ROUND_UP(a,b)   ((a) / (b) + (((a) % (b)) ? 1 : 0))

#define I2C1_DEFAULT_CLK     26000000
#define I2C_BAUD_1KHZ        1000
#define I2C_BAUD_100KHZ      100000
#define I2C_BAUD_400KHZ      400000
#define I2C_BAUD_4MHZ        4000000
#define I2C_DEFAULT_BAUD     I2C_BAUD_400KHZ
#define I2C_CLK_DIVID(rate)  (NUM_ROUND_UP(NUM_ROUND_UP(I2C1_DEFAULT_CLK, rate) - 6, 3) - 1)
#define I2C_DEF_DIV          0x16


void i2c1_init(void);
void i2c1_exit(void);
void i2c2_init(void);
void i2c2_exit(void);

#endif // __I2C1_PUB_H__

