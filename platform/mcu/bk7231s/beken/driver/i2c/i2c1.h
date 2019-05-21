#ifndef _I2C1_H_
#define _I2C1_H_

#include "uart_pub.h"
#define I2C1_DEBUG

#ifdef I2C1_DEBUG
#define I2C1_PRT                 os_printf
#define I2C1_WPRT                warning_prf
#else
#define I2C1_PRT                 os_null_printf
#define I2C1_WPRT                os_null_printf
#endif

#define I2C1_BASE_ADDR                       (0x0802300)

#define REG_I2C1_CONFIG                      (I2C1_BASE_ADDR + 4 * 0)
#define I2C1_ENSMB                             (1 << 0)
#define I2C1_STA                               (1 << 1)
#define I2C1_STO                               (1 << 2)
#define I2C1_ACK_TX                            (1 << 3)
#define I2C1_TX_MODE                           (1 << 4)
#define I2C1_FREQ_DIV_POSI                     (6)
#define I2C1_FREQ_DIV_MASK                     (0x3FF)
#define I2C1_SI                                (1 << 16)
#define I2C1_ACK_RX                            (1 << 17)
#define I2C1_ACK_REQ                           (1 << 18)
#define I2C1_BUSY                              (1 << 19)

#define REG_I2C1_DAT                         (I2C1_BASE_ADDR + 4 * 1)
#define I2C1_DAT_MASK                          (0xFF)

static UINT32 i2c1_open(UINT32 op_flag);
static UINT32 i2c1_close(void);
static UINT32 i2c1_read(char *user_buf, UINT32 count, UINT32 op_flag);
static UINT32 i2c1_write(char *user_buf, UINT32 count, UINT32 op_flag);
static UINT32 i2c1_ctrl(UINT32 cmd, void *param);

#endif  // _I2C1_H_

