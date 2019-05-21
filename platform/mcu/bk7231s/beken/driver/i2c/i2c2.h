#ifndef _I2C2_H_
#define _I2C2_H_

#include "uart_pub.h"
#define I2C2_DEBUG

#ifdef I2C2_DEBUG
#define I2C2_PRT                 os_printf
#define I2C2_WPRT                warning_prf
#else
#define I2C2_PRT                 os_null_printf
#define I2C2_WPRT                os_null_printf
#endif

#define I2C2_BASE_ADDR                       (0x0802600)

#define REG_I2C2_CONFIG                      (I2C2_BASE_ADDR + 4 * 0)
#define I2C2_IDLE_CR_POSI                      (0)
#define I2C2_IDLE_CR_MASK                      (0x7)
#define I2C2_SCL_CR_POSI                       (3)
#define I2C2_SCL_CR_MASK                       (0x7)
#define I2C2_FREQ_DIV_POSI                     (6)
#define I2C2_FREQ_DIV_MASK                     (0x3FF)
#define I2C2_SLV_ADDR_POSI                     (16)
#define I2C2_SLV_ADDR_MASK                     (0x3FF)
#define I2C2_SMB_CS_POSI                       (26)
#define I2C2_SMB_CS_MASK                       (0x3)
#define I2C2_SMB_TOE                           (1 << 28)
#define I2C2_SMB_FTE                           (1 << 29)
#define I2C2_INH                               (1 << 30)
#define I2C2_ENSMB                             (1U << 31)

#define REG_I2C2_STA                         (I2C2_BASE_ADDR + 4 * 1)
#define I2C2_SMBUS_SI                          (1 << 0)
#define I2C2_SCL_TIMEOUT                       (1 << 1)
#define I2C2_ARB_LOST                          (1 << 3)
#define I2C2_RXFIFO_EMPTY                      (1 << 4)
#define I2C2_TXFIFO_FULL                       (1 << 5)
#define I2C2_INT_MODE_POSI                     (6)
#define I2C2_INT_MODE_MASK                     (0x3)
#define I2C2_SMBUS_ACK                         (1 << 8)
#define I2C2_SMBUS_STOP                        (1 << 9)
#define I2C2_SMBUS_STA                         (1 << 10)
#define I2C2_ADDR_MATCH                        (1 << 11)
#define I2C2_ACK_REQ                           (1 << 12)
#define I2C2_TX_MODE                           (1 << 13)
#define I2C2_MASTER                            (1 << 14)
#define I2C2_BUSSY                             (1 << 15)

#define REG_I2C2_DAT                         (I2C2_BASE_ADDR + 4 * 2)
#define I2C2_DAT_MASK                          (0xFF)

static UINT32 i2c2_open(UINT32 op_flag);
static UINT32 i2c2_close(void);
static UINT32 i2c2_read(char *user_buf, UINT32 count, UINT32 op_flag);
static UINT32 i2c2_write(char *user_buf, UINT32 count, UINT32 op_flag);
static UINT32 i2c2_ctrl(UINT32 cmd, void *param);

#endif  // _I2C2_H_
