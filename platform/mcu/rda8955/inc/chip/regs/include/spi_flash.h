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


#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'spi_flash'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SPI_FLASH_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_SPI_FLASH_BASE         0x07FFF000

typedef volatile struct
{
    REG32                          spi_cmd_addr;                 //0x00000000
    REG32                          spi_block_size;               //0x00000004
    REG32                          spi_data_fifo_wo;             //0x00000008
    REG32                          spi_data_fifo_ro;             //0x0000000C
    REG32                          spi_read_back;                //0x00000010
    REG32                          spi_config;                   //0x00000014
    REG32                          spi_fifo_control;             //0x00000018
    REG32                          spi_cs_size;                  //0x0000001C
    REG32                          spi_read_cmd;                 //0x00000020
    REG32                          spi_flash_24;                 //0x00000024
    REG32                          spi_flash_28;                 //0x00000028
    REG32                          spi_flash_2c;                 //0x0000002C
    REG32                          spi_flash_30;                 //0x00000030
    REG32                          spi_flash_34;                 //0x00000034
} HWP_SPI_FLASH_T;

#define hwp_spiFlash               ((HWP_SPI_FLASH_T*) KSEG1(REG_SPI_FLASH_BASE))


//spi_cmd_addr
#define SPI_FLASH_SPI_TX_CMD(n)    (((n)&0xFF)<<0)
#define SPI_FLASH_SPI_ADDRESS(n)   (((n)&0xFFFFFF)<<8)

//spi_block_size
#define SPI_FLASH_SPI_MODEBIT(n)   (((n)&0xFF)<<0)
#define SPI_FLASH_SPI_RW_BLK_SIZE(n) (((n)&0x1FF)<<8)
#define SPI_FLASH_CONTINUOUS_ENABLE (1<<24)

//spi_data_fifo_wo
#define SPI_FLASH_SPI_TX_DATA(n)   (((n)&0xFF)<<0)
#define SPI_FLASH_SPI_SEND_TYPE    (1<<8)

//spi_data_fifo_ro
#define SPI_FLASH_SPI_FLASH_BUSY   (1<<0)
#define SPI_FLASH_TX_FIFO_EMPTY    (1<<1)
#define SPI_FLASH_TX_FIFO_FULL     (1<<2)
#define SPI_FLASH_RX_FIFO_EMPTY    (1<<3)
#define SPI_FLASH_RX_FIFO_COUNT(n) (((n)&31)<<4)
#define SPI_FLASH_READ_STATE_BUSY  (1<<9)
#define SPI_FLASH_MAIN_FSM_IDLE    (1<<10)

//spi_read_back
#define SPI_FLASH_SPI_READ_BACK_REG(n) (((n)&0xFFFFFFFF)<<0)

//spi_config
#define SPI_FLASH_QUAD_MODE_SPI_READ (0<<0)
#define SPI_FLASH_QUAD_MODE_QUAD_READ (1<<0)
#define SPI_FLASH_SPI_WPROTECT_PIN (1<<1)
#define SPI_FLASH_SPI_HOLD_PIN     (1<<2)
#define SPI_FLASH_SAMPLE_DELAY(n)  (((n)&7)<<4)
#define SPI_FLASH_BYPASS_START_CMD (1<<7)
#define SPI_FLASH_CLK_DIVIDER(n)   (((n)&0xFF)<<8)
#define SPI_FLASH_CMD_QUAD         (1<<16)
#define SPI_FLASH_TX_RX_SIZE(n)    (((n)&3)<<17)

//spi_fifo_control
#define SPI_FLASH_RX_FIFO_CLR      (1<<0)
#define SPI_FLASH_TX_FIFO_CLR      (1<<1)

//spi_cs_size
#define SPI_FLASH_SPI_CS_NUM_1_SPIFLASH (0<<0)
#define SPI_FLASH_SPI_CS_NUM_2_SPIFLASH (1<<0)
#define SPI_FLASH_SPI_SIZE_32M     (0<<1)
#define SPI_FLASH_SPI_SIZE_64M     (1<<1)
#define SPI_FLASH_SPI_SIZE_16M     (2<<1)
#define SPI_FLASH_SPI_SIZE_8M      (3<<1)
#define SPI_FLASH_SPI_128M_OTHER_SPIFLASH (0<<3)
#define SPI_FLASH_SPI_128M_128M__SPIFLASH (1<<3)
#define SPI_FLASH_AHB_READ_DISABLE_ENABLE_AHB_READ (0<<4)
#define SPI_FLASH_AHB_READ_DISABLE_DISABLE_AHB_READ (1<<4)
#define SPI_FLASH_SPI_1_SEL_FLASH_0 (0<<5)
#define SPI_FLASH_SPI_1_SEL_FLASH_1 (1<<5)
#define SPI_FLASH_SPI_CS1_SEL1     (1<<6)
#define SPI_FLASH_DIFF_128M_DIFF_CMD_EN (1<<7)

//spi_read_cmd
#define SPI_FLASH_QREAD_CMD(n)     (((n)&0xFF)<<0)
#define SPI_FLASH_FREAD_CMD(n)     (((n)&0xFF)<<8)
#define SPI_FLASH_READ_CMD(n)      (((n)&0xFF)<<16)
#define SPI_FLASH_PROTECT_BYTE(n)  (((n)&0xFF)<<24)

//spi_flash_24
#define SPI_FLASH_NAND_SEL         (1<<0)
#define SPI_FLASH_NAND_ADDR(n)     (((n)&3)<<1)
#define SPI_FLASH_PAGE_READ_CMD(n) (((n)&0xFF)<<8)
#define SPI_FLASH_GET_STS_CMD(n)   (((n)&0xFF)<<16)
#define SPI_FLASH_RAM_READ_CMD(n)  (((n)&0xFF)<<24)

//spi_flash_28
#define SPI_FLASH_GET_STS_ADDR(n)  (((n)&0xFF)<<0)
#define SPI_FLASH_PROGRAM_EXE_CMD(n) (((n)&0xFF)<<8)
#define SPI_FLASH_STS_QIP(n)       (((n)&0xFF)<<16)

//spi_flash_2c
#define SPI_FLASH_FOUR_BYTE_ADDR   (1<<0)
#define SPI_FLASH_DUMMY_CYCLE_EN   (1<<1)
#define SPI_FLASH_DUMMY_CYCLE(n)   (((n)&15)<<8)

//spi_flash_30
#define SPI_FLASH_FIRST_128M_CMD(n) (((n)&0xFF)<<0)
#define SPI_FLASH_SECOND_128M_CMD(n) (((n)&0xFF)<<8)
#define SPI_FLASH_THIRD_128M_CMD(n) (((n)&0xFF)<<16)
#define SPI_FLASH_FOURTH_128M_CMD(n) (((n)&0xFF)<<24)

//spi_flash_34
#define SPI_FLASH_TIMEOUT_VALUE(n) (((n)&0xFFFFFFFF)<<0)





#endif

