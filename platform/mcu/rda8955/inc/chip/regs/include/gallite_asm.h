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


#ifndef _GALLITE_ASM_H_
#define _GALLITE_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'gallite'."
#endif

#include "globals_asm.h"
#include "global_macros_asm.h"
#include "gallite_generic_config_asm.h"
#include "bb_cp2_asm.h"
#include "bb_ifc_asm.h"
#include "sys_ifc2_asm.h"
#include "bb_irq_asm.h"
#include "bb_sram_asm.h"
#include "comregs_asm.h"
#include "bcpu_cache_asm.h"
#include "bcpu_tag_asm.h"
#include "cipher_asm.h"
#include "cipher_a53_asm.h"
#include "debug_host_asm.h"
#include "debug_host_internals_asm.h"
#include "pmu_asm.h"
#include "abb_asm.h"
#include "xcv_asm.h"
#include "debug_uart_asm.h"
#include "dma_asm.h"
#include "spi_flash_asm.h"
#include "mem_bridge_asm.h"
#include "gpio_asm.h"
#include "i2c_master_asm.h"
#include "itlv_asm.h"
#include "page_spy_asm.h"
#include "rf_if_asm.h"
#include "rf_spi_asm.h"
#include "sci_asm.h"
#include "spi_asm.h"
#include "sys_ctrl_asm.h"
#include "sys_ifc_asm.h"
#include "sys_irq_asm.h"
#include "tcu_asm.h"
#include "timer_asm.h"
#include "uart_asm.h"
#include "iomux_asm.h"
#include "vitac_asm.h"
#include "xcor_asm.h"
#include "cp0_asm.h"
#include "regdef_asm.h"
#include "xcpu_cache_asm.h"
#include "xcpu_tag_asm.h"
#include "xcpu_asm.h"
#include "pwm_asm.h"
#include "calendar_asm.h"
#include "usbc_asm.h"
#include "sdmmc_asm.h"
#include "cfg_regs_asm.h"
#include "cordic_asm.h"
#include "keypad_asm.h"
#include "excor_asm.h"
#include "gouda_asm.h"
#include "aif_asm.h"
#include "camera_asm.h"
#include "voc_ram_asm.h"
#include "voc_ahb_asm.h"
#include "voc_cfg_asm.h"
#include "seg_scan_asm.h"
#include "psram8_ctrl_asm.h"
#include "cholk_asm.h"
#include "fmd_asm.h"

/// VOC AHB base
#define REG_VOC_BASE                            (0X01940000)
/// System APB base
#define REG_SYS_APB_BASE                        (0X01A00000)
/// System APB2 base
#define REG_SYS_APB2_BASE                       (0X01920000)
/// Baseband APB base
#define REG_BB_APB_BASE                         (0X01900000)


#endif
