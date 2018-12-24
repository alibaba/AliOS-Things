/*!
    \file  gd32f30x_exmc.h
    \brief definitions for the EXMC
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.2, firmware for GD32F30x
*/

#ifndef GD32F30X_EXMC_H
#define GD32F30X_EXMC_H

#include "gd32f30x.h"

/* EXMC definitions */
#define EXMC                              (EXMC_BASE)                   /*!< EXMC register base address */

/* registers definitions */
/* NOR/PSRAM */
#define EXMC_SNCTL0                       REG32(EXMC + 0x00U)           /*!< EXMC SRAM/NOR flash control register 0 */
#define EXMC_SNTCFG0                      REG32(EXMC + 0x04U)           /*!< EXMC SRAM/NOR flash timing configuration register 0 */
#define EXMC_SNWTCFG0                     REG32(EXMC + 0x104U)          /*!< EXMC SRAM/NOR flash write timing configuration register 0 */

#define EXMC_SNCTL1                       REG32(EXMC + 0x08U)           /*!< EXMC SRAM/NOR flash control register 1 */
#define EXMC_SNTCFG1                      REG32(EXMC + 0x0CU)           /*!< EXMC SRAM/NOR flash timing configuration register 1 */
#define EXMC_SNWTCFG1                     REG32(EXMC + 0x10CU)          /*!< EXMC SRAM/NOR flash write timing configuration register 1 */

#define EXMC_SNCTL2                       REG32(EXMC + 0x10U)           /*!< EXMC SRAM/NOR flash control register 2 */
#define EXMC_SNTCFG2                      REG32(EXMC + 0x14U)           /*!< EXMC SRAM/NOR flash timing configuration register 2 */
#define EXMC_SNWTCFG2                     REG32(EXMC + 0x114U)          /*!< EXMC SRAM/NOR flash write timing configuration register 2 */

#define EXMC_SNCTL3                       REG32(EXMC + 0x18U)           /*!< EXMC SRAM/NOR flash control register 3 */
#define EXMC_SNTCFG3                      REG32(EXMC + 0x1CU)           /*!< EXMC SRAM/NOR flash timing configuration register 3 */
#define EXMC_SNWTCFG3                     REG32(EXMC + 0x11CU)          /*!< EXMC SRAM/NOR flash write timing configuration register 3 */

/* NAND/PC card */
#define EXMC_NPCTL1                       REG32(EXMC + 0x60U)           /*!< EXMC NAND/PC card control register 1 */
#define EXMC_NPINTEN1                     REG32(EXMC + 0x64U)           /*!< EXMC NAND/PC card interrupt enable register 1 */
#define EXMC_NPCTCFG1                     REG32(EXMC + 0x68U)           /*!< EXMC NAND/PC card common space timing configuration register 1 */
#define EXMC_NPATCFG1                     REG32(EXMC + 0x6CU)           /*!< EXMC NAND/PC card attribute space timing configuration register 1 */
#define EXMC_NECC1                        REG32(EXMC + 0x74U)           /*!< EXMC NAND ECC register 1 */

#define EXMC_NPCTL2                       REG32(EXMC + 0x80U)           /*!< EXMC NAND/PC card control register 2 */
#define EXMC_NPINTEN2                     REG32(EXMC + 0x84U)           /*!< EXMC NAND/PC card interrupt enable register 2 */
#define EXMC_NPCTCFG2                     REG32(EXMC + 0x88U)           /*!< EXMC NAND/PC card common space timing configuration register 2 */
#define EXMC_NPATCFG2                     REG32(EXMC + 0x8CU)           /*!< EXMC NAND/PC card attribute space timing configuration register 2 */
#define EXMC_NECC2                        REG32(EXMC + 0x94U)           /*!< EXMC NAND ECC register 2 */

#define EXMC_NPCTL3                       REG32(EXMC + 0xA0U)           /*!< EXMC NAND/PC card control register 3 */
#define EXMC_NPINTEN3                     REG32(EXMC + 0xA4U)           /*!< EXMC NAND/PC card interrupt enable register 3 */
#define EXMC_NPCTCFG3                     REG32(EXMC + 0xA8U)           /*!< EXMC NAND/PC card common space timing configuration register 3 */
#define EXMC_NPATCFG3                     REG32(EXMC + 0xACU)           /*!< EXMC NAND/PC card attribute space timing configuration register 3 */
#define EXMC_PIOTCFG3                     REG32(EXMC + 0xB0U)           /*!< EXMC PC card I/O space timing configuration register */

/* bits definitions */
/* EXMC_SNCTLx,x=0..3 */
#define EXMC_SNCTL_NRBKEN                 BIT(0)                        /*!< NOR bank enable */
#define EXMC_SNCTL_NRMUX                  BIT(1)                        /*!< NOR bank memory address/data multiplexing */
#define EXMC_SNCTL_NRTP                   BITS(2,3)                     /*!< NOR bank memory type */
#define EXMC_SNCTL_NRW                    BITS(4,5)                     /*!< NOR bank memory data bus width */
#define EXMC_SNCTL_NREN                   BIT(6)                        /*!< NOR flash access enable */
#define EXMC_SNCTL_SBRSTEN                BIT(8)                        /*!< synchronous burst enable */
#define EXMC_SNCTL_NRWTPOL                BIT(9)                        /*!< NWAIT signal polarity */
#define EXMC_SNCTL_WRAPEN                 BIT(10)                       /*!< wrapped burst mode enable */
#define EXMC_SNCTL_NRWTCFG                BIT(11)                       /*!< NWAIT signal configuration, only work in synchronous mode  */
#define EXMC_SNCTL_WREN                   BIT(12)                       /*!< write enable */
#define EXMC_SNCTL_NRWTEN                 BIT(13)                       /*!< NWAIT signal enable */
#define EXMC_SNCTL_EXMODEN                BIT(14)                       /*!< extended mode enable */
#define EXMC_SNCTL_ASYNCWAIT              BIT(15)                       /*!< asynchronous wait */
#define EXMC_SNCTL_CPS                    BITS(16,18)                   /*!< CRAM page size */
#define EXMC_SNCTL_SYNCWR                 BIT(19)                       /*!< synchronous write */

/* EXMC_SNTCFGx,x=0..3 */
#define EXMC_SNTCFG_ASET                  BITS(0,3)                     /*!< address setup time */
#define EXMC_SNTCFG_AHLD                  BITS(4,7)                     /*!< address hold time */
#define EXMC_SNTCFG_DSET                  BITS(8,15)                    /*!< data setup time */
#define EXMC_SNTCFG_BUSLAT                BITS(16,19)                   /*!< bus latency */
#define EXMC_SNTCFG_CKDIV                 BITS(20,23)                   /*!< synchronous clock divide ratio */
#define EXMC_SNTCFG_DLAT                  BITS(24,27)                   /*!< data latency for NOR flash */
#define EXMC_SNTCFG_ASYNCMOD              BITS(28,29)                   /*!< asynchronous access mode */

/* EXMC_SNWTCFGx,x=0..3 */
#define EXMC_SNWTCFG_WASET                BITS(0,3)                     /*!< address setup time */
#define EXMC_SNWTCFG_WAHLD                BITS(4,7)                     /*!< address hold time */
#define EXMC_SNWTCFG_WDSET                BITS(8,15)                    /*!< data setup time */
#define EXMC_SNWTCFG_WBUSLAT              BITS(16,19)                   /*!< bus latency */
#define EXMC_SNWTCFG_WASYNCMOD            BITS(28,29)                   /*!< asynchronous access mode */

/* EXMC_NPCTLx,x=1..3 */
#define EXMC_NPCTL_NDWTEN                 BIT(1)                        /*!< wait feature enable */
#define EXMC_NPCTL_NDBKEN                 BIT(2)                        /*!< NAND bank enable */
#define EXMC_NPCTL_NDTP                   BIT(3)                        /*!< NAND bank memory type */
#define EXMC_NPCTL_NDW                    BITS(4,5)                     /*!< NAND bank memory data bus width */
#define EXMC_NPCTL_ECCEN                  BIT(6)                        /*!< ECC enable */
#define EXMC_NPCTL_CTR                    BITS(9,12)                    /*!< CLE to RE delay */
#define EXMC_NPCTL_ATR                    BITS(13,16)                   /*!< ALE to RE delay */
#define EXMC_NPCTL_ECCSZ                  BITS(17,19)                   /*!< ECC size */

/* EXMC_NPINTENx,x=1..3 */
#define EXMC_NPINTEN_INTRS                BIT(0)                        /*!< interrupt rising edge status */
#define EXMC_NPINTEN_INTHS                BIT(1)                        /*!< interrupt high-level status */
#define EXMC_NPINTEN_INTFS                BIT(2)                        /*!< interrupt falling edge status */
#define EXMC_NPINTEN_INTREN               BIT(3)                        /*!< interrupt rising edge detection enable */
#define EXMC_NPINTEN_INTHEN               BIT(4)                        /*!< interrupt high-level detection enable */
#define EXMC_NPINTEN_INTFEN               BIT(5)                        /*!< interrupt falling edge detection enable */
#define EXMC_NPINTEN_FFEPT                BIT(6)                        /*!< FIFO empty flag */

/* EXMC_NPCTCFGx,x=1..3 */
#define EXMC_NPCTCFG_COMSET               BITS(0,7)                     /*!< common memory setup time */
#define EXMC_NPCTCFG_COMWAIT              BITS(8,15)                    /*!< common memory wait time */
#define EXMC_NPCTCFG_COMHLD               BITS(16,23)                   /*!< common memory hold time */
#define EXMC_NPCTCFG_COMHIZ               BITS(24,31)                   /*!< common memory data bus HiZ time */

/* EXMC_NPATCFGx,x=1..3 */
#define EXMC_NPATCFG_ATTSET               BITS(0,7)                     /*!< attribute memory setup time */
#define EXMC_NPATCFG_ATTWAIT              BITS(8,15)                    /*!< attribute memory wait time */
#define EXMC_NPATCFG_ATTHLD               BITS(16,23)                   /*!< attribute memory hold time */
#define EXMC_NPATCFG_ATTHIZ               BITS(24,31)                   /*!< attribute memory data bus HiZ time */

/* EXMC_PIOTCFG3 */
#define EXMC_PIOTCFG3_IOSET               BITS(0,7)                     /*!< IO space setup time */
#define EXMC_PIOTCFG3_IOWAIT              BITS(8,15)                    /*!< IO space wait time */
#define EXMC_PIOTCFG3_IOHLD               BITS(16,23)                   /*!< IO space hold time */
#define EXMC_PIOTCFG3_IOHIZ               BITS(24,31)                   /*!< IO space data bus HiZ time */

/* EXMC_NECCx,x=1,2 */
#define EXMC_NECC_ECC                     BITS(0,31)                    /*!< ECC result */

/* constants definitions */
/* EXMC NOR/SRAM timing initialize struct */
typedef struct
{
    uint32_t asyn_access_mode;                                          /*!< asynchronous access mode */
    uint32_t syn_data_latency;                                          /*!< configure the data latency */
    uint32_t syn_clk_division;                                          /*!< configure the clock divide ratio */
    uint32_t bus_latency;                                               /*!< configure the bus latency */
    uint32_t asyn_data_setuptime;                                       /*!< configure the data setup time,asynchronous access mode valid */
    uint32_t asyn_address_holdtime;                                     /*!< configure the address hold time,asynchronous access mode valid */
    uint32_t asyn_address_setuptime;                                    /*!< configure the data setup time,asynchronous access mode valid */
}exmc_norsram_timing_parameter_struct;

/* EXMC NOR/SRAM initialize struct */
typedef struct
{
    uint32_t norsram_region;                                            /*!< select the region of EXMC NOR/SRAM bank */
    uint32_t write_mode;                                                /*!< the write mode, synchronous mode or asynchronous mode */
    uint32_t extended_mode;                                             /*!< enable or disable the extended mode */
    uint32_t asyn_wait;                                                 /*!< enable or disable the asynchronous wait function */
    uint32_t nwait_signal;                                              /*!< enable or disable the NWAIT signal while in synchronous bust mode */
    uint32_t memory_write;                                              /*!< enable or disable the write operation */
    uint32_t nwait_config;                                              /*!< NWAIT signal configuration */
    uint32_t wrap_burst_mode;                                           /*!< enable or disable the wrap burst mode */
    uint32_t nwait_polarity;                                            /*!< specifies the polarity of NWAIT signal from memory */
    uint32_t burst_mode;                                                /*!< enable or disable the burst mode */
    uint32_t databus_width;                                             /*!< specifies the databus width of external memory */
    uint32_t memory_type;                                               /*!< specifies the type of external memory */
    uint32_t address_data_mux;                                          /*!< specifies whether the data bus and address bus are multiplexed */
    exmc_norsram_timing_parameter_struct* read_write_timing;            /*!< timing parameters for read and write if the extended mode is not used or the timing 
                                                                             parameters for read if the extended mode is used */
    exmc_norsram_timing_parameter_struct* write_timing;                 /*!< timing parameters for write when the extended mode is used */
}exmc_norsram_parameter_struct;

/* EXMC NAND/PC card timing initialize struct */
typedef struct
{
    uint32_t databus_hiztime;                                           /*!< configure the dadtabus HiZ time for write operation */
    uint32_t holdtime;                                                  /*!< configure the address hold time(or the data hold time for write operation) */
    uint32_t waittime;                                                  /*!< configure the minimum wait time */
    uint32_t setuptime;                                                 /*!< configure the address setup time */
}exmc_nand_pccard_timing_parameter_struct;

/* EXMC NAND initialize struct */
typedef struct
{
    uint32_t nand_bank;                                                 /*!< select the bank of NAND */ 
    uint32_t ecc_size;                                                  /*!< the page size for the ECC calculation */
    uint32_t atr_latency;                                               /*!< configure the latency of ALE low to RB low */
    uint32_t ctr_latency;                                               /*!< configure the latency of CLE low to RB low */
    uint32_t ecc_logic;                                                 /*!< enable or disable the ECC calculation logic */
    uint32_t databus_width;                                             /*!< the NAND flash databus width */
    uint32_t wait_feature;                                              /*!< enables or disables the wait feature */
    exmc_nand_pccard_timing_parameter_struct* common_space_timing;      /*!< the timing parameters for NAND flash common space */
    exmc_nand_pccard_timing_parameter_struct* attribute_space_timing;   /*!< the timing parameters for NAND flash attribute space */
}exmc_nand_parameter_struct;

/* EXMC PC card initialize struct */
typedef struct
{
    uint32_t atr_latency;                                               /*!< configure the latency of ALE low to RB low */
    uint32_t ctr_latency;                                               /*!< configure the latency of CLE low to RB low */
    uint32_t wait_feature;                                              /*!< enables or disables the Wait feature */
    exmc_nand_pccard_timing_parameter_struct*  common_space_timing;     /*!< the timing parameters for NAND flash common space */
    exmc_nand_pccard_timing_parameter_struct*  attribute_space_timing;  /*!< the timing parameters for NAND flash attribute space */  
    exmc_nand_pccard_timing_parameter_struct*  io_space_timing;         /*!< the timing parameters for NAND flash IO space */
}exmc_pccard_parameter_struct;;


/* EXMC_register address */
#define EXMC_SNCTL(region)                REG32(EXMC + 0x08U * (region))                  /*!< EXMC SRAM/NOR flash control register */
#define EXMC_SNTCFG(region)               REG32(EXMC + 0x04U + 0x08U * (region))          /*!< EXMC SRAM/NOR flash timing configuration register */
#define EXMC_SNWTCFG(region)              REG32(EXMC + 0x104U + 0x08U * (region))         /*!< EXMC SRAM/NOR flash write timing configuration register */

#define EXMC_NPCTL(bank)                  REG32(EXMC + 0x40U + 0x20U * (bank))            /*!< EXMC NAND/PC card control register */
#define EXMC_NPINTEN(bank)                REG32(EXMC + 0x44U + 0x20U * (bank))            /*!< EXMC NAND/PC card interrupt enable register */
#define EXMC_NPCTCFG(bank)                REG32(EXMC + 0x48U + 0x20U * (bank))            /*!< EXMC NAND/PC card common space timing configuration register */
#define EXMC_NPATCFG(bank)                REG32(EXMC + 0x4CU + 0x20U * (bank))            /*!< EXMC NAND/PC card attribute space timing configuration register */
#define EXMC_NECC(bank)                   REG32(EXMC + 0x54U + 0x20U * (bank))            /*!< EXMC NAND ECC register */

/* CRAM page size */
#define SNCTL_CPS(regval)                 (BITS(16,18) & ((uint32_t)(regval) << 16))
#define EXMC_CRAM_AUTO_SPLIT              SNCTL_CPS(0)                  /*!< automatic burst split on page boundary crossing */
#define EXMC_CRAM_PAGE_SIZE_128_BYTES     SNCTL_CPS(1)                  /*!< page size is 128 bytes */
#define EXMC_CRAM_PAGE_SIZE_256_BYTES     SNCTL_CPS(2)                  /*!< page size is 256 bytes */
#define EXMC_CRAM_PAGE_SIZE_512_BYTES     SNCTL_CPS(3)                  /*!< page size is 512 bytes */
#define EXMC_CRAM_PAGE_SIZE_1024_BYTES    SNCTL_CPS(4)                  /*!< page size is 1024 bytes */

/* NOR bank memory data bus width */
#define SNCTL_NRW(regval)                 (BITS(4,5) & ((uint32_t)(regval) << 4))
#define EXMC_NOR_DATABUS_WIDTH_8B         SNCTL_NRW(0)                  /*!< NOR data width 8 bits */
#define EXMC_NOR_DATABUS_WIDTH_16B        SNCTL_NRW(1)                  /*!< NOR data width 16 bits */

/* NOR bank memory type */
#define SNCTL_NRTP(regval)                (BITS(2,3) & ((uint32_t)(regval) << 2))
#define EXMC_MEMORY_TYPE_SRAM             SNCTL_NRTP(0)                 /*!< SRAM,ROM */
#define EXMC_MEMORY_TYPE_PSRAM            SNCTL_NRTP(1)                 /*!< PSRAM,CRAM */
#define EXMC_MEMORY_TYPE_NOR              SNCTL_NRTP(2)                 /*!< NOR flash */

/* asynchronous access mode */
#define SNTCFG_ASYNCMOD(regval)           (BITS(28,29) & ((uint32_t)(regval) << 28))
#define EXMC_ACCESS_MODE_A                SNTCFG_ASYNCMOD(0)            /*!< mode A access */
#define EXMC_ACCESS_MODE_B                SNTCFG_ASYNCMOD(1)            /*!< mode B access */
#define EXMC_ACCESS_MODE_C                SNTCFG_ASYNCMOD(2)            /*!< mode C access */
#define EXMC_ACCESS_MODE_D                SNTCFG_ASYNCMOD(3)            /*!< mode D access */

/* data latency for NOR flash */
#define SNTCFG_DLAT(regval)               (BITS(24,27) & ((uint32_t)(regval) << 24))
#define EXMC_DATALAT_2_CLK                SNTCFG_DLAT(0)                /*!< data latency 2 EXMC_CLK */
#define EXMC_DATALAT_3_CLK                SNTCFG_DLAT(1)                /*!< data latency 3 EXMC_CLK */
#define EXMC_DATALAT_4_CLK                SNTCFG_DLAT(2)                /*!< data latency 4 EXMC_CLK */
#define EXMC_DATALAT_5_CLK                SNTCFG_DLAT(3)                /*!< data latency 5 EXMC_CLK */
#define EXMC_DATALAT_6_CLK                SNTCFG_DLAT(4)                /*!< data latency 6 EXMC_CLK */
#define EXMC_DATALAT_7_CLK                SNTCFG_DLAT(5)                /*!< data latency 7 EXMC_CLK */
#define EXMC_DATALAT_8_CLK                SNTCFG_DLAT(6)                /*!< data latency 8 EXMC_CLK */
#define EXMC_DATALAT_9_CLK                SNTCFG_DLAT(7)                /*!< data latency 9 EXMC_CLK */
#define EXMC_DATALAT_10_CLK               SNTCFG_DLAT(8)                /*!< data latency 10 EXMC_CLK */
#define EXMC_DATALAT_11_CLK               SNTCFG_DLAT(9)                /*!< data latency 11 EXMC_CLK */
#define EXMC_DATALAT_12_CLK               SNTCFG_DLAT(10)               /*!< data latency 12 EXMC_CLK */
#define EXMC_DATALAT_13_CLK               SNTCFG_DLAT(11)               /*!< data latency 13 EXMC_CLK */
#define EXMC_DATALAT_14_CLK               SNTCFG_DLAT(12)               /*!< data latency 14 EXMC_CLK */
#define EXMC_DATALAT_15_CLK               SNTCFG_DLAT(13)               /*!< data latency 15 EXMC_CLK */
#define EXMC_DATALAT_16_CLK               SNTCFG_DLAT(14)               /*!< data latency 16 EXMC_CLK */
#define EXMC_DATALAT_17_CLK               SNTCFG_DLAT(15)               /*!< data latency 17 EXMC_CLK */

/* synchronous clock divide ratio */
#define SNTCFG_CKDIV(regval)              (BITS(20,23) & ((uint32_t)(regval) << 20))
#define EXMC_SYN_CLOCK_RATIO_DISABLE      SNTCFG_CKDIV(0)               /*!< EXMC_CLK disable */
#define EXMC_SYN_CLOCK_RATIO_2_CLK        SNTCFG_CKDIV(1)               /*!< EXMC_CLK = 2*HCLK */
#define EXMC_SYN_CLOCK_RATIO_3_CLK        SNTCFG_CKDIV(2)               /*!< EXMC_CLK = 3*HCLK */
#define EXMC_SYN_CLOCK_RATIO_4_CLK        SNTCFG_CKDIV(3)               /*!< EXMC_CLK = 4*HCLK */
#define EXMC_SYN_CLOCK_RATIO_5_CLK        SNTCFG_CKDIV(4)               /*!< EXMC_CLK = 5*HCLK */
#define EXMC_SYN_CLOCK_RATIO_6_CLK        SNTCFG_CKDIV(5)               /*!< EXMC_CLK = 6*HCLK */
#define EXMC_SYN_CLOCK_RATIO_7_CLK        SNTCFG_CKDIV(6)               /*!< EXMC_CLK = 7*HCLK */
#define EXMC_SYN_CLOCK_RATIO_8_CLK        SNTCFG_CKDIV(7)               /*!< EXMC_CLK = 8*HCLK */
#define EXMC_SYN_CLOCK_RATIO_9_CLK        SNTCFG_CKDIV(8)               /*!< EXMC_CLK = 9*HCLK */
#define EXMC_SYN_CLOCK_RATIO_10_CLK       SNTCFG_CKDIV(9)               /*!< EXMC_CLK = 10*HCLK */
#define EXMC_SYN_CLOCK_RATIO_11_CLK       SNTCFG_CKDIV(10)              /*!< EXMC_CLK = 11*HCLK */
#define EXMC_SYN_CLOCK_RATIO_12_CLK       SNTCFG_CKDIV(11)              /*!< EXMC_CLK = 12*HCLK */
#define EXMC_SYN_CLOCK_RATIO_13_CLK       SNTCFG_CKDIV(12)              /*!< EXMC_CLK = 13*HCLK */
#define EXMC_SYN_CLOCK_RATIO_14_CLK       SNTCFG_CKDIV(13)              /*!< EXMC_CLK = 14*HCLK */
#define EXMC_SYN_CLOCK_RATIO_15_CLK       SNTCFG_CKDIV(14)              /*!< EXMC_CLK = 15*HCLK */
#define EXMC_SYN_CLOCK_RATIO_16_CLK       SNTCFG_CKDIV(15)              /*!< EXMC_CLK = 16*HCLK */

/* ECC size */
#define NPCTL_ECCSZ(regval)               (BITS(17,19) & ((uint32_t)(regval) << 17))
#define EXMC_ECC_SIZE_256BYTES            NPCTL_ECCSZ(0)                /* 256 bytes */
#define EXMC_ECC_SIZE_512BYTES            NPCTL_ECCSZ(1)                /* 512 bytes */
#define EXMC_ECC_SIZE_1024BYTES           NPCTL_ECCSZ(2)                /* 1024 bytes */
#define EXMC_ECC_SIZE_2048BYTES           NPCTL_ECCSZ(3)                /* 2048 bytes */
#define EXMC_ECC_SIZE_4096BYTES           NPCTL_ECCSZ(4)                /* 4096 bytes */
#define EXMC_ECC_SIZE_8192BYTES           NPCTL_ECCSZ(5)                /* 8192 bytes */

/* ALE to RE delay */
#define NPCTL_ATR(regval)                 (BITS(13,16) & ((uint32_t)(regval) << 13))
#define EXMC_ALE_RE_DELAY_1_HCLK          NPCTL_ATR(0)                  /* ALE to RE delay = 1*HCLK */
#define EXMC_ALE_RE_DELAY_2_HCLK          NPCTL_ATR(1)                  /* ALE to RE delay = 2*HCLK */
#define EXMC_ALE_RE_DELAY_3_HCLK          NPCTL_ATR(2)                  /* ALE to RE delay = 3*HCLK */
#define EXMC_ALE_RE_DELAY_4_HCLK          NPCTL_ATR(3)                  /* ALE to RE delay = 4*HCLK */
#define EXMC_ALE_RE_DELAY_5_HCLK          NPCTL_ATR(4)                  /* ALE to RE delay = 5*HCLK */
#define EXMC_ALE_RE_DELAY_6_HCLK          NPCTL_ATR(5)                  /* ALE to RE delay = 6*HCLK */
#define EXMC_ALE_RE_DELAY_7_HCLK          NPCTL_ATR(6)                  /* ALE to RE delay = 7*HCLK */
#define EXMC_ALE_RE_DELAY_8_HCLK          NPCTL_ATR(7)                  /* ALE to RE delay = 8*HCLK */
#define EXMC_ALE_RE_DELAY_9_HCLK          NPCTL_ATR(8)                  /* ALE to RE delay = 9*HCLK */
#define EXMC_ALE_RE_DELAY_10_HCLK         NPCTL_ATR(9)                  /* ALE to RE delay = 10*HCLK */
#define EXMC_ALE_RE_DELAY_11_HCLK         NPCTL_ATR(10)                 /* ALE to RE delay = 11*HCLK */
#define EXMC_ALE_RE_DELAY_12_HCLK         NPCTL_ATR(11)                 /* ALE to RE delay = 12*HCLK */
#define EXMC_ALE_RE_DELAY_13_HCLK         NPCTL_ATR(12)                 /* ALE to RE delay = 13*HCLK */
#define EXMC_ALE_RE_DELAY_14_HCLK         NPCTL_ATR(13)                 /* ALE to RE delay = 14*HCLK */
#define EXMC_ALE_RE_DELAY_15_HCLK         NPCTL_ATR(14)                 /* ALE to RE delay = 15*HCLK */
#define EXMC_ALE_RE_DELAY_16_HCLK         NPCTL_ATR(15)                 /* ALE to RE delay = 16*HCLK */

/* CLE to RE delay */
#define NPCTL_CTR(regval)                 (BITS(9,12) & ((uint32_t)(regval) << 9))
#define EXMC_CLE_RE_DELAY_1_HCLK          NPCTL_CTR(0)                  /* CLE to RE delay = 1*HCLK */
#define EXMC_CLE_RE_DELAY_2_HCLK          NPCTL_CTR(1)                  /* CLE to RE delay = 2*HCLK */
#define EXMC_CLE_RE_DELAY_3_HCLK          NPCTL_CTR(2)                  /* CLE to RE delay = 3*HCLK */
#define EXMC_CLE_RE_DELAY_4_HCLK          NPCTL_CTR(3)                  /* CLE to RE delay = 4*HCLK */
#define EXMC_CLE_RE_DELAY_5_HCLK          NPCTL_CTR(4)                  /* CLE to RE delay = 5*HCLK */
#define EXMC_CLE_RE_DELAY_6_HCLK          NPCTL_CTR(5)                  /* CLE to RE delay = 6*HCLK */
#define EXMC_CLE_RE_DELAY_7_HCLK          NPCTL_CTR(6)                  /* CLE to RE delay = 7*HCLK */
#define EXMC_CLE_RE_DELAY_8_HCLK          NPCTL_CTR(7)                  /* CLE to RE delay = 8*HCLK */
#define EXMC_CLE_RE_DELAY_9_HCLK          NPCTL_CTR(8)                  /* CLE to RE delay = 9*HCLK */
#define EXMC_CLE_RE_DELAY_10_HCLK         NPCTL_CTR(9)                  /* CLE to RE delay = 10*HCLK */
#define EXMC_CLE_RE_DELAY_11_HCLK         NPCTL_CTR(10)                 /* CLE to RE delay = 11*HCLK */
#define EXMC_CLE_RE_DELAY_12_HCLK         NPCTL_CTR(11)                 /* CLE to RE delay = 12*HCLK */
#define EXMC_CLE_RE_DELAY_13_HCLK         NPCTL_CTR(12)                 /* CLE to RE delay = 13*HCLK */
#define EXMC_CLE_RE_DELAY_14_HCLK         NPCTL_CTR(13)                 /* CLE to RE delay = 14*HCLK */
#define EXMC_CLE_RE_DELAY_15_HCLK         NPCTL_CTR(14)                 /* CLE to RE delay = 15*HCLK */
#define EXMC_CLE_RE_DELAY_16_HCLK         NPCTL_CTR(15)                 /* CLE to RE delay = 16*HCLK */

/* NAND bank memory data bus width */
#define NPCTL_NDW(regval)                 (BITS(4,5) & ((uint32_t)(regval) << 4))
#define EXMC_NAND_DATABUS_WIDTH_8B        NPCTL_NDW(0)                  /*!< NAND data width 8 bits */
#define EXMC_NAND_DATABUS_WIDTH_16B       NPCTL_NDW(1)                  /*!< NAND data width 16 bits */

/* EXMC NOR/SRAM bank region definition */
#define EXMC_BANK0_NORSRAM_REGION0        ((uint32_t)0x00000000U)       /*!< bank0 NOR/SRAM region0 */
#define EXMC_BANK0_NORSRAM_REGION1        ((uint32_t)0x00000001U)       /*!< bank0 NOR/SRAM region1 */
#define EXMC_BANK0_NORSRAM_REGION2        ((uint32_t)0x00000002U)       /*!< bank0 NOR/SRAM region2 */
#define EXMC_BANK0_NORSRAM_REGION3        ((uint32_t)0x00000003U)       /*!< bank0 NOR/SRAM region3 */

/* EXMC NOR/SRAM write mode */
#define EXMC_ASYN_WRITE                   ((uint32_t)0x00000000U)       /*!< asynchronous write mode */
#define EXMC_SYN_WRITE                    ((uint32_t)0x00080000U)       /*!< synchronous write mode */

/* EXMC NWAIT signal configuration */
#define EXMC_NWAIT_CONFIG_BEFORE          ((uint32_t)0x00000000U)       /*!< NWAIT signal is active one data cycle before wait state */
#define EXMC_NWAIT_CONFIG_DURING          ((uint32_t)0x00000800U)       /*!< NWAIT signal is active during wait state */

/* EXMC NWAIT signal polarity configuration */
#define EXMC_NWAIT_POLARITY_LOW           ((uint32_t)0x00000000U)       /*!< low level is active of NWAIT */
#define EXMC_NWAIT_POLARITY_HIGH          ((uint32_t)0x00000200U)       /*!< high level is active of NWAIT */

/* EXMC NAND/PC card bank definition */
#define EXMC_BANK1_NAND                   ((uint32_t)0x00000001U)       /*!< bank1 NAND flash */
#define EXMC_BANK2_NAND                   ((uint32_t)0x00000002U)       /*!< bank2 NAND flash */
#define EXMC_BANK3_PCCARD                 ((uint32_t)0x00000003U)       /*!< bank3 PC card */

/* EXMC flag bits */
#define EXMC_NAND_PCCARD_FLAG_RISE        EXMC_NPINTEN_INTRS            /*!< interrupt rising edge status */
#define EXMC_NAND_PCCARD_FLAG_LEVEL       EXMC_NPINTEN_INTHS            /*!< interrupt high-level status */
#define EXMC_NAND_PCCARD_FLAG_FALL        EXMC_NPINTEN_INTFS            /*!< interrupt falling edge status */
#define EXMC_NAND_PCCARD_FLAG_FIFOE       EXMC_NPINTEN_FFEPT            /*!< FIFO empty flag */

/* EXMC interrupt enable bits */
#define EXMC_NAND_PCCARD_INT_RISE         EXMC_NPINTEN_INTREN           /*!< interrupt rising edge detection enable */
#define EXMC_NAND_PCCARD_INT_LEVEL        EXMC_NPINTEN_INTHEN           /*!< interrupt high-level detection enable */
#define EXMC_NAND_PCCARD_INT_FALL         EXMC_NPINTEN_INTFEN           /*!< interrupt falling edge detection enable */

/* EXMC interrupt flag bits */
#define EXMC_NAND_PCCARD_INT_FLAG_RISE    EXMC_NPINTEN_INTREN           /*!< interrupt rising edge detection enable */
#define EXMC_NAND_PCCARD_INT_FLAG_LEVEL   EXMC_NPINTEN_INTHEN           /*!< interrupt high-level detection enable */
#define EXMC_NAND_PCCARD_INT_FLAG_FALL    EXMC_NPINTEN_INTFEN           /*!< interrupt falling edge detection enable */

/* function declarations */
/* deinitialize EXMC NOR/SRAM region */
void exmc_norsram_deinit(uint32_t exmc_norsram_region);
/* initialize EXMC NOR/SRAM region */
void exmc_norsram_init(exmc_norsram_parameter_struct* exmc_norsram_init_struct);
/* exmc_norsram_parameter_struct parameter initialize */
void exmc_norsram_parameter_init(exmc_norsram_parameter_struct* exmc_norsram_init_struct);
/* CRAM page size configure */
void exmc_norsram_page_size_config(uint32_t exmc_norsram_region, uint32_t page_size);
/* EXMC NOR/SRAM bank enable */
void exmc_norsram_enable(uint32_t exmc_norsram_region);
/* EXMC NOR/SRAM bank disable */
void exmc_norsram_disable(uint32_t exmc_norsram_region);


/* deinitialize EXMC NAND bank */
void exmc_nand_deinit(uint32_t exmc_nand_bank);
/* initialize EXMC NAND bank */
void exmc_nand_init(exmc_nand_parameter_struct* exmc_nand_init_struct);
/* exmc_norsram_parameter_struct parameter initialize */
void exmc_nand_parameter_init(exmc_nand_parameter_struct* exmc_nand_init_struct);
/* EXMC NAND bank enable */
void exmc_nand_enable(uint32_t exmc_nand_bank);
/* EXMC NAND bank disable */
void exmc_nand_disable(uint32_t exmc_nand_bank);
/* enable or disable the EXMC NAND ECC function */
void exmc_nand_ecc_config(uint32_t exmc_nand_bank, ControlStatus newvalue);
/* get the EXMC ECC value */
uint32_t exmc_ecc_get(uint32_t exmc_nand_bank);

/* deinitialize EXMC PC card bank */
void exmc_pccard_deinit(void);
/* initialize EXMC PC card bank */
void exmc_pccard_init(exmc_pccard_parameter_struct* exmc_pccard_init_struct);
/* exmc_pccard_parameter_struct parameter initialize */
void exmc_pccard_parameter_init(exmc_pccard_parameter_struct* exmc_pccard_init_struct);
/* EXMC PC card bank enable */
void exmc_pccard_enable(void);
/* EXMC PC card bank disable */
void exmc_pccard_disable(void);

/* check EXMC flag is set or not */
FlagStatus exmc_flag_get(uint32_t exmc_bank,uint32_t flag);
/* clear EXMC flag */
void exmc_flag_clear(uint32_t exmc_bank,uint32_t flag);
/* check EXMC flag is set or not */
FlagStatus exmc_interrupt_flag_get(uint32_t exmc_bank,uint32_t interrupt_source);
/* clear EXMC flag */
void exmc_interrupt_flag_clear(uint32_t exmc_bank,uint32_t interrupt_source);
/* enable EXMC interrupt */
void exmc_interrupt_enable(uint32_t exmc_bank,uint32_t interrupt_source);
/* disable EXMC interrupt */
void exmc_interrupt_disable(uint32_t exmc_bank,uint32_t interrupt_source);

#endif /* GD32F30X_EXMC_H */
