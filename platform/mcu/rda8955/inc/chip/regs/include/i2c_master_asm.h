/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/


#ifndef _I2C_MASTER_ASM_H_
#define _I2C_MASTER_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'i2c_master'."
#endif



//==============================================================================
// i2c_master
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_I2C_MASTER_BASE        0x01A07000
#define REG_I2C_MASTER2_BASE       0x01A22000
#define REG_I2C_MASTER3_BASE       0x01A23000

#define REG_I2C_MASTER_BASE_HI     BASE_HI(REG_I2C_MASTER_BASE)
#define REG_I2C_MASTER_BASE_LO     BASE_LO(REG_I2C_MASTER_BASE)

#define REG_I2C_MASTER_CTRL        REG_I2C_MASTER_BASE_LO + 0x00000000
#define REG_I2C_MASTER_STATUS      REG_I2C_MASTER_BASE_LO + 0x00000004
#define REG_I2C_MASTER_TXRX_BUFFER REG_I2C_MASTER_BASE_LO + 0x00000008
#define REG_I2C_MASTER_CMD         REG_I2C_MASTER_BASE_LO + 0x0000000C
#define REG_I2C_MASTER_IRQ_CLR     REG_I2C_MASTER_BASE_LO + 0x00000010

#define REG_I2C_MASTER2_BASE_HI    BASE_HI(REG_I2C_MASTER2_BASE)
#define REG_I2C_MASTER2_BASE_LO    BASE_LO(REG_I2C_MASTER2_BASE)

#define REG_I2C_MASTER2_CTRL       REG_I2C_MASTER2_BASE_LO + 0x00000000
#define REG_I2C_MASTER2_STATUS     REG_I2C_MASTER2_BASE_LO + 0x00000004
#define REG_I2C_MASTER2_TXRX_BUFFER REG_I2C_MASTER2_BASE_LO + 0x00000008
#define REG_I2C_MASTER2_CMD        REG_I2C_MASTER2_BASE_LO + 0x0000000C
#define REG_I2C_MASTER2_IRQ_CLR    REG_I2C_MASTER2_BASE_LO + 0x00000010

#define REG_I2C_MASTER3_BASE_HI    BASE_HI(REG_I2C_MASTER3_BASE)
#define REG_I2C_MASTER3_BASE_LO    BASE_LO(REG_I2C_MASTER3_BASE)

#define REG_I2C_MASTER3_CTRL       REG_I2C_MASTER3_BASE_LO + 0x00000000
#define REG_I2C_MASTER3_STATUS     REG_I2C_MASTER3_BASE_LO + 0x00000004
#define REG_I2C_MASTER3_TXRX_BUFFER REG_I2C_MASTER3_BASE_LO + 0x00000008
#define REG_I2C_MASTER3_CMD        REG_I2C_MASTER3_BASE_LO + 0x0000000C
#define REG_I2C_MASTER3_IRQ_CLR    REG_I2C_MASTER3_BASE_LO + 0x00000010

//CTRL
#define I2C_MASTER_EN              (1<<0)
#define I2C_MASTER_IRQ_MASK        (1<<8)
#define I2C_MASTER_CLOCK_PRESCALE(n) (((n)&0xFFFF)<<16)
#define I2C_MASTER_CLOCK_PRESCALE_MASK (0xFFFF<<16)

//STATUS
#define I2C_MASTER_IRQ_CAUSE       (1<<0)
#define I2C_MASTER_IRQ_STATUS      (1<<4)
#define I2C_MASTER_TIP             (1<<8)
#define I2C_MASTER_AL              (1<<12)
#define I2C_MASTER_BUSY            (1<<16)
#define I2C_MASTER_RXACK           (1<<20)

//TXRX_BUFFER
#define I2C_MASTER_TX_DATA(n)      (((n)&0xFF)<<0)
#define I2C_MASTER_RX_DATA(n)      (((n)&0xFF)<<0)

//CMD
#define I2C_MASTER_ACK             (1<<0)
#define I2C_MASTER_RD              (1<<4)
#define I2C_MASTER_STO             (1<<8)
#define I2C_MASTER_WR              (1<<12)
#define I2C_MASTER_STA             (1<<16)

//IRQ_CLR
#define I2C_MASTER_IRQ_CLR         (1<<0)




#endif
