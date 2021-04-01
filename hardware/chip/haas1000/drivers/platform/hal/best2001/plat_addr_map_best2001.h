/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef __PLAT_ADDR_MAP_BEST2001_H__
#define __PLAT_ADDR_MAP_BEST2001_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ROM_BASE                                0x00000000
#define ROMD_BASE                               0x24000000

#ifndef ROM_SIZE
#define ROM_SIZE                                0x00010000
#endif

#define RAM0_BASE                               0x20000000
#define RAMX0_BASE                              0x00200000
#define RAM1_BASE                               0x20040000
#define RAMX1_BASE                              0x00240000
#define RAM2_BASE                               0x20060000
#define RAMX2_BASE                              0x00260000
#define RAM3_BASE                               0x20080000
#define RAMX3_BASE                              0x00280000
#define RAM4_BASE                               0x200A0000
#define RAMX4_BASE                              0x002A0000
#define RAM5_BASE                               0x200C0000
#define RAMX5_BASE                              0x002C0000
#define RAM6_BASE                               0x200E0000
#define RAMX6_BASE                              0x002E0000
#define RAM7_BASE                               0x20100000
#define RAMX7_BASE                              0x00300000
#define RAM8_BASE                               0x20180000
#define RAMX8_BASE                              0x00380000
#define RAM9_BASE                               0x20200000
#define RAMX9_BASE                              0x00400000
#define RAM_BASE                                RAM0_BASE
#define RAMX_BASE                               RAMX0_BASE

#define RAM9_SIZE                               0x00080000

#ifdef LARGE_RAM
#ifndef RAMCP_BASE
#define RAMCP_BASE                              RAM9_BASE
#endif
#ifndef RAMCP_SIZE
#define RAMCP_SIZE                              0x30000
#endif
#ifndef RAMCPX_BASE
#define RAMCPX_BASE                             (RAMX9_BASE + RAMCP_SIZE)
#endif
#ifndef RAMCPX_SIZE
#define RAMCPX_SIZE                             (RAM9_SIZE - RAMCP_SIZE)
#endif
#else /*LARGE_RAM*/
#ifndef RAMCP_BASE
#define RAMCP_BASE                              RAM2_BASE
#endif
#ifndef RAMCP_SIZE
#define RAMCP_SIZE                              (RAM3_BASE - RAMCP_BASE)
#endif
#ifndef RAMCPX_BASE
#define RAMCPX_BASE                             RAMX1_BASE
#endif
#ifndef RAMCPX_SIZE
#define RAMCPX_SIZE                             (RAMX2_BASE - RAMCPX_BASE)
#endif

#endif /*LARGE_RAM*/

#define MAX_RAM_SIZE                            (RAM9_BASE + RAM9_SIZE - RAM0_BASE) // 0x00280000

#ifndef RAM_SIZE
#ifdef LARGE_RAM
#ifdef NO_RAMCP
#define RAM_SIZE                                MAX_RAM_SIZE
#else
#define RAM_SIZE                                (RAM9_BASE - RAM0_BASE) // 0x00080000
#endif
#else
#ifdef NO_RAMCP
#define RAM_SIZE                                (RAM3_BASE - RAM0_BASE) // 0x00080000
#else
#define RAM_SIZE                                (RAM1_BASE - RAM0_BASE) // 0x00040000
#endif
#endif
#endif

#ifndef MCU_PSRAMUHS_SIZE
#define MCU_PSRAMUHS_SIZE			(0)
#endif

#define FLASH_BASE                              0x2C000000
#define FLASH_NC_BASE                           0x28000000
#define FLASHX_BASE                             0x0C000000
#define FLASHX_NC_BASE                          0x08000000

#define PSRAM_BASE                              0x34000000
#define PSRAM_NC_BASE                           0x30000000
#define PSRAMX_BASE                             0x14000000
#define PSRAMX_NC_BASE                          0x10000000

#define PSRAMUHS_BASE                           0x3C000000
#define PSRAMUHS_NC_BASE                        0x38000000
#define PSRAMUHSX_BASE                          0x1C000000
#define PSRAMUHSX_NC_BASE                       0x18000000

#define ICACHE_CTRL_BASE                        0x07FFE000
#define ICACHECP_CTRL_BASE                      0x07FFE000
#define DCACHE_CTRL_BASE                        0x27FFE000
#define DCACHECP_CTRL_BASE                      0x27FFE000

#define CMU_BASE                                0x40000000
#define MCU_WDT_BASE                            0x40001000
#define MCU_TIMER0_BASE                         0x40002000
#define MCU_TIMER1_BASE                         0x40003000
#define MCU_TIMER2_BASE                         0x40004000
#define I2C0_BASE                               0x40005000
#define I2C1_BASE                               0x40006000
#define SPI_BASE                                0x40007000
#define SPILCD_BASE                             0x40008000
#define ISPI_BASE                               0x40009000
#define SPIPHY_BASE                             0x4000A000
#define UART0_BASE                              0x4000B000
#define UART1_BASE                              0x4000C000
#define UART2_BASE                              0x4000D000
#define BTPCM_BASE                              0x4000E000
#define I2S0_BASE                               0x4000F000
#define SPDIF0_BASE                             0x40010000
#define TRANSQW_BASE                            0x40011000
#define TRANSQD_BASE                            0x40012000
#define TRNG_BASE                               0x40013000
#define SEC_ENG_BASE                            0x40020000

#define AON_CMU_BASE                            0x40080000
#define AON_GPIO_BASE                           0x40081000
#define AON_WDT_BASE                            0x40082000
#define AON_PWM_BASE                            0x40083000
#define AON_TIMER_BASE                          0x40084000
#define AON_GPIOAUX_BASE                        0x40085000
#define AON_IOMUX_BASE                          0x40086000
#define AON_SPIDPD_BASE                         0x40087000

#define CHECKSUM_BASE                           0x40100000
#define CRC_BASE                                0x40108000
#define SDMMC_BASE                              0x40110000
#define BES2001_AUDMA_BASE                      0x40120000
#define BES2001_GPDMA_BASE                      0x40130000
#define FLASH_CTRL_BASE                         0x40140000
#define PSRAM_CTRL_BASE                         0x40150000
#define I2C_SLAVE_BASE                          0x40160000
#define WFDUMP_BASE                             0x40170000
#define USB_BASE                                0x40180000
#define SEDMA_BASE                              0x401D0000
#define BTDUMP_BASE                             0x401E0000
#define PSRAMUHS_CTRL_BASE                      0x401F0000

#define CODEC_BASE                              0x40300000

#define PATCH_ENTRY_NUM                         8
#define PATCH_CTRL_BASE                         0x000FFE00
#define PATCH_DATA_BASE                         0x000FFF00
#define WIFI_PATCH_CTRL_BASE                    0x6007fe00
#define WIFI_PATCH_DATA_BASE                    0x6007ff00

#define BT_SUBSYS_BASE                          0xA0000000
#define BT_RAM_BASE                             0xC0000000
#define BT_UART_BASE                            0xD0300000
#define BT_CMU_BASE                             0xD0330000

#define WIFI_SUBSYS_BASE                        0x60000000
#define WIFI_RAM_BASE                           0x80000000
#define WIFI_PAS_BASE                           0x82000000
#define WIFI_TRANSQM_BASE                       0x9000A000

#define DSP_SUBSYS_BASE                         0x50000000

#define DSP_BOOT_BASE                           0x00000000
#define DSP_BOOT_SIZE                           (24 * 4)

#define DSP_RAM0_BASE                           0x50000000
#define DSP_RAM1_BASE                           0x50080000
#define DSP_RAM2_BASE                           0x50100000
#define DSP_RAM3_BASE                           0x50180000
#define DSP_RAM4_BASE                           0x50200000
#define DSP_RAM5_BASE                           0x50220000
#define DSP_RAM_BASE                            DSP_RAM0_BASE

#define DSP_RAM5_SIZE                           0x00020000

#define MAX_DSP_RAM_SIZE                        (DSP_RAM5_BASE + DSP_RAM5_SIZE - DSP_RAM0_BASE)

#ifndef DSP_RAM_SIZE
#ifdef LARGE_DSP_RAM
#define DSP_RAM_SIZE                            MAX_DSP_RAM_SIZE
#else /*LARGE_DSP_RAM*/
#ifdef FPGA
#define DSP_RAM_SIZE                            (DSP_RAM1_BASE - DSP_RAM0_BASE)
#else /*FPGA*/
#define DSP_RAM_SIZE                            (DSP_RAM4_BASE - DSP_RAM0_BASE)
#endif /*FPGA*/
#endif /*LARGE_DSP_RAM*/
#endif

#ifndef DSP_PSRAMUHS_SIZE
#define DSP_PSRAMUHS_SIZE			(0)
#endif

#define DSP_PSRAMUHS_BASE		(PSRAMUHS_BASE)
#define DSP_PSRAMUHSX_BASE		(PSRAMUHSX_BASE)

#define MCU_PSRAMUHS_BASE		(PSRAMUHS_BASE + DSP_PSRAMUHS_SIZE)
#define MCU_PSRAMUHS_NC_BASE		(PSRAMUHS_NC_BASE + DSP_PSRAMUHS_SIZE)
#define MCU_PSRAMUHSX_BASE		(PSRAMUHSX_BASE + DSP_PSRAMUHS_SIZE)

#define DSP_BOOT_REG                            0x58000000
#define DSP_WDT_BASE                            0x58001000
#define DSP_TIMER0_BASE                         0x58002000
#define DSP_TIMER1_BASE                         0x58003000
#define DSP_TRANSQM_BASE                        0x58004000
#define DSP_TIMESTAMP_GEN_BASE                  0x58020000

#define DSP_DEBUGSYS_APB_BASE                   0x58040000

#define DSP_XDMA_BASE                           0x58200000

#define GIC_DISTRIBUTOR_BASE                    0x58301000
#define GIC_INTERFACE_BASE                      0x58302000

#define IOMUX_BASE                              AON_IOMUX_BASE
#define GPIO_BASE                               AON_GPIO_BASE
#define GPIOAUX_BASE                            AON_GPIOAUX_BASE
#define PWM_BASE                                AON_PWM_BASE
#define SPIDPD_BASE                             AON_SPIDPD_BASE

#ifdef CHIP_BEST2001_DSP
#define TIMER0_BASE                             DSP_TIMER0_BASE
#define TIMER1_BASE                             DSP_TIMER1_BASE
#define TRANSQ0_BASE                            TRANSQW_BASE
#define TRANSQ0_PEER_BASE                       WIFI_TRANSQM_BASE
#define TRANSQ1_BASE                            DSP_TRANSQM_BASE
#define TRANSQ1_PEER_BASE                       TRANSQD_BASE
#define WDT_BASE                                DSP_WDT_BASE
#else
#define TIMER0_BASE                             MCU_TIMER0_BASE
#define TIMER1_BASE                             MCU_TIMER1_BASE
#define TRANSQ0_BASE                            TRANSQW_BASE
#define TRANSQ0_PEER_BASE                       WIFI_TRANSQM_BASE
#define TRANSQ1_BASE                            TRANSQD_BASE
#define TRANSQ1_PEER_BASE                       DSP_TRANSQM_BASE
#define WDT_BASE                                AON_WDT_BASE
#endif

#ifndef DSP_USE_AUDMA
#define AUDMA_BASE                              BES2001_AUDMA_BASE
#define GPDMA_BASE                              BES2001_GPDMA_BASE
#else /*DSP_USE_AUDMA*/
#ifdef CHIP_BEST2001_DSP
#define AUDMA_BASE                              BES2001_AUDMA_BASE //A7 use AUDMA
#else
#define AUDMA_BASE                              BES2001_GPDMA_BASE //MCU use GPDMA
#endif
#endif /*DSP_USE_AUDMA*/

/* For module features */
#define SEC_ENG_HAS_HASH

/* For linker scripts */

#define VECTOR_SECTION_SIZE                     360
#define REBOOT_PARAM_SECTION_SIZE               64
#define ROM_BUILD_INFO_SECTION_SIZE             40
#define ROM_EXPORT_FN_SECTION_SIZE              128

#define PSRAMUHSX_TO_PSRAMUHS(d)                      ((d) - PSRAMUHSX_BASE + PSRAMUHS_BASE)
#define PSRAMUHS_TO_PSRAMUHSX(d)                      ((d) - PSRAMUHS_BASE + PSRAMUHSX_BASE)
#define PSRAMUHS_TO_PSRAMUHSNC(d)                     ((d) - PSRAMUHS_BASE + PSRAMUHS_NC_BASE)

#define BT_INTESYS_MEM_OFFSET                   0x00000000

#ifdef __cplusplus
}
#endif

#endif
