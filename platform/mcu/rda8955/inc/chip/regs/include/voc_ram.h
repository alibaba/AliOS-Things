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


#ifndef _VOC_RAM_H_
#define _VOC_RAM_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'voc_ram'."
#endif



// =============================================================================
//  MACROS
// =============================================================================
#define VOC_RAM_X_SIZE                          (0X5000)
#define VOC_RAM_Y_SIZE                          (0X5000)
#define VOC_ROM_Z_SIZE                          (0X0000)
#define VOC_RAM_I_SIZE                          (0X5000)
#define VOC_RAM_X_SPACE                         (0X8000)
#define VOC_RAM_Y_SPACE                         (0X8000)
#define VOC_ROM_Z_SPACE                         (0X10000)
#define VOC_RAM_I_SPACE                         (0X20000)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// VOC_RAM_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_VOC_RAM_BASE           0x01940000

typedef volatile struct
{
    UINT8 voc_ram_x_base[20480];                //0x00000000
    UINT8 voc_ram_x_hole[12288];                //0x00005000
    UINT8 voc_ram_y_base[20480];                //0x00008000
    UINT8 voc_ram_y_hole[12288];                //0x0000D000
    UINT8 voc_rom_z_hole[65536];                //0x00010000
    UINT8 voc_ram_i_base[20480];                //0x00020000
    UINT8 voc_ram_i_hole[110592];               //0x00025000
} HWP_VOC_RAM_T;

#define hwp_vocRam                 ((HWP_VOC_RAM_T*) KSEG1(REG_VOC_RAM_BASE))






#endif

