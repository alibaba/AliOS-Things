/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _TEE_ADDR_MAP_H_
#define _TEE_ADDR_MAP_H_

//HOBBIT_NBIOT_256 --> TRM FAKE KEY 1

#define _SIZE_256K          (0x00040000)
#define _SIZE_224K          (0x00038000)
#define _SIZE_192K          (0x00030000)
#define _SIZE_184K          (0x0002E000)
#define _SIZE_96K           (0x00018000)
#define _SIZE_64K           (0x00010000)
#define _SIZE_48K           (0x0000c000)
#define _SIZE_34K           (0x00008800)
#define _SIZE_32K           (0x00008000)
#define _SIZE_16K           (0x00004000)
#define _SIZE_8K            (0x00002000)
#define _SIZE_4K            (0x00001000)
#define _SIZE_1K            (0x00000400)

#if CONFIG_FPGA
/*****************************************/
/* Memory Map for FPGA                   */
/*****************************************/
#if HOBBIT_NBIOT_256
#define FPGA_FLASH_START    (0x00080000)
#define FPGA_FLASH_SIZE     (_SIZE_256K)
#define FPGA_ISRAM_START    (0x60000000)
#define FPGA_ISRAM_SIZE     (_SIZE_256K)
#elif PHOBOS_GENERAL
/* PHOBOS for FPGA, alias PHOBOS_RUNINRAM_256 */
#define FPGA_FLASH_START    (0x20000000)
#define FPGA_FLASH_SIZE     (_SIZE_192K)
#define FPGA_ISRAM_START    (FPGA_FLASH_START + FPGA_FLASH_SIZE)
#define FPGA_ISRAM_SIZE     (_SIZE_64K)
#else /* !HOBBIT_NBIOT_256 */
#define FPGA_FLASH_START    (0x60000000)
#define FPGA_FLASH_SIZE     (_SIZE_96K)
#define FPGA_ISRAM_START    (FPGA_FLASH_START + FPGA_FLASH_SIZE)
#define FPGA_ISRAM_SIZE     (_SIZE_32K)
#endif

/* SRAM */
#define SRAM_START            FPGA_ISRAM_START
#define SRAM_SIZE             FPGA_ISRAM_SIZE
#define SRAM_END              (SRAM_START + SRAM_SIZE)
/* FLASH */
#define FLASH_START           FPGA_FLASH_START
#define FLASH_SIZE            FPGA_FLASH_SIZE
#define FLASH_END             (FLASH_START + FLASH_SIZE)

#else
/*****************************************/
/* Memory Map for Sillicon               */
/*****************************************/
#if PHOBOS_GENERAL
/* PHOBOS for Silicon */
#define FLASH_START           (0x10000000)
#define FLASH_SIZE            (_SIZE_256K)
#define FLASH_END             (FLASH_START + FLASH_SIZE)
#define SRAM_START            (0x20000000)
#define SRAM_SIZE             (_SIZE_64K)
#define SRAM_END              (SRAM_START + SRAM_SIZE)
#else /* !PHOBOS_GENERAL */
/* other silicon */
/* SRAM */
#define SRAM_START            0x60000000
#define SRAM_SIZE             _SIZE_16K    /* 16k */
#define SRAM_END              (SRAM_START + SRAM_SIZE)
/* FLASH */
#define FLASH_START           0x10000000
#define FLASH_SIZE            (_SIZE_256K - SRAM_SIZE)
#define FLASH_END             (FLASH_START + FLASH_SIZE)
#endif /* PHOBOS_GENERAL */

#endif

/*******************************************************************/
/*  TEE TW address map                                             */
/*******************************************************************/
#if CONFIG_TEST
/* only for test */
#define TW_RO_ADDR          (FLASH_START)
#define TW_RO_SIZE          (_SIZE_64K) /* 64K Flash */
#define NTW_ENTRY_ADDR      (TW_RO_ADDR + _SIZE_64K)
#define TW_RW_ADDR          (SRAM_START)
#define TW_RW_SIZE          (_SIZE_16K) /* 16K ISRAM */
#define TEE_HEAP_SIZE       (_SIZE_8K) /* 8K Stack */
#else /* CONFIG_TEST */
#define TW_RO_ADDR          (FLASH_START)
#define TW_RO_SIZE          (_SIZE_32K) /* 32K Flash */
#define NTW_ENTRY_ADDR      (TW_RO_ADDR + _SIZE_32K)
#define TW_RW_ADDR          (SRAM_START)
#define TW_RW_SIZE          (_SIZE_8K) /* 8K ISRAM */
#define TEE_HEAP_SIZE       (_SIZE_4K) /* 4K Stack */
#endif /* CONFIG_TEST */

/*******************************************************************/
/*  TEE NTW(test bed) address map                                  */
/*******************************************************************/
#if CONFIG_TEST
/* for test */
#define NTW_RO_ADDR         (FLASH_START  + _SIZE_64K)
#define NTW_RO_SIZE         (_SIZE_32K)
#define NTW_RW_ADDR         (SRAM_START + _SIZE_16K)
#define NTW_RW_SIZE         (_SIZE_16K)
#define NTW_HEAP_SIZE       (_SIZE_4K)
#else /* CONFIG_TEST */
#define NTW_RO_ADDR         (FLASH_START  + _SIZE_32K)
#define NTW_RO_SIZE         (_SIZE_64K)
#define NTW_RW_ADDR         (SRAM_START + _SIZE_16K)
#define NTW_RW_SIZE         (_SIZE_16K)
#define NTW_HEAP_SIZE       (_SIZE_8K)
#endif /* CONFIG_TEST */

#endif /* _TEE_ADDR_MAP_H_ */
