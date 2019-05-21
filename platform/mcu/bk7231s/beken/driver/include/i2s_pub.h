#ifndef _I2S_PUB_H_
#define _I2S_PUB_H_

#define I2S_FAILURE                (1)
#define I2S_SUCCESS                (0)

#define I2S_DEV_NAME                "i2s"

#define I2S_CMD_MAGIC              (0xe280000)
enum
{
	I2S_HW_SET = I2S_CMD_MAGIC + 1,
	I2S_CMD_SET_FREQ_DATAWIDTH,
    I2S_CMD_ACTIVE,
    I2S_CMD_RXACTIVE,
    I2S_CMD_SELECT_MODE,
    I2S_CMD_SET_LEVEL
};

enum
{
    I2S_MODE_I2S = 0,
    I2S_MODE_LEFT_JUST = 1,
    I2S_MODE_RIGHT_JUST = 2,
    I2S_MODE_SHORT_SYNC = 4,
    I2S_MODE_LONG_SYNC = 5,
    I2S_MODE_NORMAL_2B_D = 6,
    I2S_MODE_DELAY_2B_D = 7
};

typedef struct
{
    UINT8 rx_level;
    UINT8 tx_level;
} i2s_level_t;

typedef struct
{
    UINT32 freq;
    UINT16 datawidth;
} i2s_rate_t;

/*******************************************************************************
* Function Declarations
*******************************************************************************/
void i2s_init(void);
void i2s_exit(void);
void i2s_isr(void);
UINT8 is_i2s_active(void);
#endif //_I2S_PUB_H_
