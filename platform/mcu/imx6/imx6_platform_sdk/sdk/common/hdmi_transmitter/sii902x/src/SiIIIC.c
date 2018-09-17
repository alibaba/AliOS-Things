/*
 * Copyright (c) 2010-2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "sdk.h"
#include "SiIIIC.h"
#include "siHdmiTx_902x_TPI.h"

//------------------------------------------------------------------------------
// Local macros to simplify bit operations
//------------------------------------------------------------------------------

#define I2C_IADR                    0x0
#define I2C_IFDR                    0x4
#define I2C_I2CR                    0x8
#define I2C_I2SR                    0xC
#define I2C_I2DR                    0x10

#define I2C_I2CR_IEN                (1 << 7)
#define I2C_I2CR_IIEN               (1 << 6)
#define I2C_I2CR_MSTA               (1 << 5)
#define I2C_I2CR_MTX                (1 << 4)
#define I2C_I2CR_TXAK               (1 << 3)
#define I2C_I2CR_RSTA               (1 << 2)

#define I2C_I2SR_ICF                (1 << 7)
#define I2C_I2SR_IAAS               (1 << 6)
#define I2C_I2SR_IBB                (1 << 5)
#define I2C_I2SR_IAL                (1 << 4)
#define I2C_I2SR_SRW                (1 << 2)
#define I2C_I2SR_IIF                (1 << 1)
#define I2C_I2SR_RXAK               (1 << 0)

#define LAST_BYTE      1
#define NOT_LAST_BYTE  0

extern unsigned int hdmi_i2c_base;
unsigned char regVal = 0;
#define WAIT_RXAK_LOOPS 0x10000

static inline void imx_send_stop(unsigned int base)
{
    writew((readw(base + I2C_I2CR) & ~(I2C_I2CR_MSTA)), base + I2C_I2CR);
}

static inline int wait_till_busy(unsigned int base)
{
    int i = 10000;

    while (((readw(base + I2C_I2SR) & I2C_I2SR_IBB) == 0) && (--i > 0)) {
        if (readw(base + I2C_I2SR) & I2C_I2SR_IAL) {
            printf("Error: arbitration lost!\n");
            return ERR_ARB_LOST;
        }
    }

    if (i <= 0) {
        return -1;
    }

    return 0;
}

static inline int is_bus_free(unsigned int base)
{
    int i = WAIT_RXAK_LOOPS;

    while ((readw(base + I2C_I2SR) & I2C_I2SR_IBB) && (--i > 0)) ;

    if (i <= 0) {
        printf("Error: I2C Bus not free!\n");
        return -1;
    }

    return 0;
}

static inline int wait_op_done(unsigned int base, int is_tx)
{
    volatile unsigned short v;
    int i = WAIT_RXAK_LOOPS;

    while ((((v = readw(base + I2C_I2SR)) & I2C_I2SR_IIF) == 0) && (--i > 0)) ;

    if (i <= 0) {
        printf("I2C Error: timeout unexpected\n");
        return -1;
    }

    writew(0x0, base + I2C_I2SR);

    if (v & I2C_I2SR_IAL) {
        printf("Error %d: Arbitration lost\n", __LINE__);
        return ERR_ARB_LOST;
    }

    return 0;
}

static int I2CTxByte(unsigned char *data, unsigned int base)
{
    int ret = 0;

    // clear both IAL and IIF bits
    writew(0, base + I2C_I2SR);

    writew(*data, base + I2C_I2DR);

    if ((ret = wait_op_done(base, 1)) != 0)
        return ret;

    return ret;
}

// For master RX
static int I2CRxByte(unsigned char *data, unsigned int base, int sz)
{
    unsigned short i2cr;
    int i, ret = 0;

    for (i = 0; sz > 0; sz--, i++) {
        if ((ret = wait_op_done(base, 0)) != 0)
            return ret;

        // the next two if-statements setup for the next read control register value
        if (sz == 1) {
            // last byte --> generate STOP
            imx_send_stop(base);
        }

        if (sz == 2) {
            // 2nd last byte --> set TXAK bit to NOT generate ACK
            i2cr = readw(base + I2C_I2CR);
            writew(i2cr | I2C_I2CR_TXAK, base + I2C_I2CR);
        }
        // read the true data
        data[i] = readw(base + I2C_I2DR);
    }

    return ret;
}

static int i2c_start(unsigned int base)
{
    unsigned short i2cr = 0;

    /* Check if bus is free, if not return error */
    if (is_bus_free(base) != 0) {
        return -1;
    }
    // reset and enable I2C
    writew(0, base + I2C_I2CR);
    writew(I2C_I2CR_IEN, base + I2C_I2CR);
    /* Need wait at least 2 cycles of per_clk */
    hal_delay_us(5000);

    // 1.2 clear both IAL and IIF bits
    writew(0, base + I2C_I2SR);

    // 1.3 assert START signal and also indicate TX mode
    i2cr = I2C_I2CR_IEN | I2C_I2CR_MSTA | I2C_I2CR_MTX;
    writew(i2cr, base + I2C_I2CR);

    // 1.4 make sure bus is busy after the START signal
    if (wait_till_busy(base) != 0) {
        return ERR_TX;
    }

    return 0;
}

int I2CReadWrite(unsigned int base, struct imx_i2c_request *rq, int dir)
{
    unsigned int reg;
    unsigned char i, data;
    unsigned short i2cr;
    int ret = 0;

    //step 1: generate start
    if ((ret = i2c_start(base)) != 0) {
        printf("GEN START ERR %d\n", ret);
        return ret;
    }
    // Step 2: send slave address + read/write at the LSB
    data = (rq->dev_addr << 1) | I2C_WRITE;

    if ((ret = I2CTxByte(&data, base)) != 0) {
        printf("START TX ERR %d\n", ret);

        if (ret == ERR_NO_ACK) {
            return ERR_NO_ACK_ON_START;
        } else {
            return ret;
        }
    }
    // Step 3: send I2C device register address
    reg = rq->reg_addr;

    for (i = 0; i < rq->reg_addr_sz; i++, reg >>= 8) {
        data = reg & 0xFF;

        if (I2CTxByte(&data, base) != 0) {
            return -1;
        }
    }

    // Step 4: read/write data
    if (dir == I2C_READ) {
        if (rq->reg_addr_sz != 0) {
            // do repeat-start
            i2cr = readw(base + I2C_I2CR);
            writew(i2cr | I2C_I2CR_RSTA, base + I2C_I2CR);

            // make sure bus is busy after the repeat-start signal
            if (wait_till_busy(base) != 0) {
                return ERR_TX;
            }
            // send slave address again, but indicate read operation
            data = (rq->dev_addr << 1) | I2C_READ;

            if (I2CTxByte(&data, base) != 0) {
                return -1;
            }
        }
        // change to receive mode
        i2cr = readw(base + I2C_I2CR);

        if (rq->buffer_sz == 1) {
            // if only one byte to read, make sure don't send ack
            i2cr |= I2C_I2CR_TXAK;
        }

        writew(i2cr & ~I2C_I2CR_MTX, base + I2C_I2CR);
        // dummy read
        readw(base + I2C_I2DR);

        // now reading ...
        if (I2CRxByte(rq->buffer, base, rq->buffer_sz) != 0) {
            printf("RX ERR %d\n", ret);
            return -1;
        }
    } else {
        // I2C_WRITE
        for (i = 0; i < rq->buffer_sz; i++) {
            // send device register value
            data = rq->buffer[i];

            if ((ret = I2CTxByte(&data, base)) != 0) {
                printf("TX ERR %d\n", ret);
                break;
            }
        }

        // generate STOP by clearing MSTA bit
        imx_send_stop(base);
    }

    return ret;
}

int I2CSegmentRead(unsigned int base, struct imx_i2c_request *rq, unsigned char segPointer)
{
    unsigned int reg;
    unsigned char i, data;
    unsigned short i2cr;
    int ret = 0;

    //step 1: generate start
    if ((ret = i2c_start(base)) != 0) {
        printf("GEN START ERR %d\n", ret);
        return ret;
    }
    // Step 2: send slave address + read/write at the LSB
    data = EDID_SEG_ADDR;

    if ((ret = I2CTxByte(&data, base)) != 0) {
        printf("No ACK in tx segment pointer\n");
        return ret;
    }

    data = segPointer;

    if ((ret = I2CTxByte(&data, base)) != 0) {
        printf("No ACK in tx segment pointer\n");
        return ret;
    }
    // do repeat-start
    i2cr = readw(base + I2C_I2CR);
    writew(i2cr | I2C_I2CR_RSTA, base + I2C_I2CR);

    // make sure bus is busy after the repeat-start signal
    if (wait_till_busy(base) != 0) {
        return ERR_TX;
    }
    // Step 2: send slave address + read/write at the LSB
    data = (rq->dev_addr << 1) | I2C_WRITE;

    if ((ret = I2CTxByte(&data, base)) != 0) {
        printf("START TX ERR %d\n", ret);

        if (ret == ERR_NO_ACK) {
            return ERR_NO_ACK_ON_START;
        } else {
            return ret;
        }
    }
    // Step 3: send I2C device register address
    if (rq->reg_addr_sz > 4) {
        printf("Warning register address size %d should less than 4\n", rq->reg_addr_sz);
        rq->reg_addr_sz = 4;
    }

    reg = rq->reg_addr;

    for (i = 0; i < rq->reg_addr_sz; i++, reg >>= 8) {
        data = reg & 0xFF;
        printf("sending I2C=0x%x device register: data=0x%x, byte %d\n", base, data, i);

        if (I2CTxByte(&data, base) != 0) {
            return -1;
        }
    }

    // Step 4: read/write data
    if (rq->reg_addr_sz != 0) {
        // do repeat-start
        i2cr = readw(base + I2C_I2CR);
        writew(i2cr | I2C_I2CR_RSTA, base + I2C_I2CR);

        // make sure bus is busy after the repeat-start signal
        if (wait_till_busy(base) != 0) {
            return ERR_TX;
        }
        // send slave address again, but indicate read operation
        data = (rq->dev_addr << 1) | I2C_READ;

        if (I2CTxByte(&data, base) != 0) {
            return -1;
        }
    }
    // change to receive mode
    i2cr = readw(base + I2C_I2CR);

    if (rq->buffer_sz == 1) {
        // if only one byte to read, make sure don't send ack
        i2cr |= I2C_I2CR_TXAK;
    }

    writew(i2cr & ~I2C_I2CR_MTX, base + I2C_I2CR);
    // dummy read
    readw(base + I2C_I2DR);

    // now reading ...
    if (I2CRxByte(rq->buffer, base, rq->buffer_sz) != 0) {
        printf("RX ERR %d\n", ret);
        return -1;
    }

    return ret;
}

//-------------------------------------------------------------------
byte I2CReadByte(byte SlaveAddr, byte RegAddr)
{
    struct imx_i2c_request rq;
    rq.dev_addr = SlaveAddr >> 1;
    rq.reg_addr = RegAddr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = 1;
    rq.buffer = &regVal;

    I2CReadWrite(hdmi_i2c_base, &rq, 1);

    return regVal;
}

//-------------------------------------------------------------------
void I2CWriteByte(byte SlaveAddr, byte RegAddr, byte Data)
{
    struct imx_i2c_request rq;
    rq.dev_addr = SlaveAddr >> 1;
    rq.reg_addr = RegAddr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = 1;
    rq.buffer = &Data;
    I2CReadWrite(hdmi_i2c_base, &rq, 0);
}

//------------------------------------------------------------------------------
// Function Name: I2CReadBlock
// Function Description: Reads block of data from HDMI Device
//------------------------------------------------------------------------------
byte I2CReadBlock(byte SlaveAddr, byte RegAddr, byte NBytes, byte * Data)
{
    struct imx_i2c_request rq;
    rq.dev_addr = SlaveAddr >> 1;
    rq.reg_addr = RegAddr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = NBytes;
    rq.buffer = Data;
    I2CReadWrite(hdmi_i2c_base, &rq, 1);

    return regVal;
}

//------------------------------------------------------------------------------
// Function Name:  I2CWriteBlock
// Function Description: Writes block of data from HDMI Device
//------------------------------------------------------------------------------
byte I2CWriteBlock(byte SlaveAddr, byte RegAddr, byte NBytes, byte * Data)
{
    int retVal = 0;
    struct imx_i2c_request rq;
    rq.dev_addr = SlaveAddr >> 1;
    rq.reg_addr = RegAddr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = NBytes;
    rq.buffer = Data;

    I2CReadWrite(hdmi_i2c_base, &rq, 0);

    return retVal;
}

//------------------------------------------------------------------------------
// Function Name:  siiReadSegmentBlockEDID
// Function Description: Reads segment block of EDID from HDMI Downstream Device
//------------------------------------------------------------------------------
byte siiReadSegmentBlockEDID(byte SlaveAddr, byte Segment, byte Offset, byte * Buffer, byte Length)
{
    struct imx_i2c_request rq;

    rq.dev_addr = SlaveAddr >> 1;
    rq.reg_addr = Offset;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = Length;
    rq.buffer = Buffer;

    I2CSegmentRead(hdmi_i2c_base, &rq, Segment);

    TPI_DEBUG_PRINT(("SII read segment block EDID"));

    return IIC_OK;
}
