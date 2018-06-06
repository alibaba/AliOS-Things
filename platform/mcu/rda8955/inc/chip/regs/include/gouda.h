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


#ifndef _GOUDA_H_
#define _GOUDA_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'gouda'."
#endif


#include "globals.h"

// =============================================================================
//  MACROS
// =============================================================================
#define GD_MAX_OUT_WIDTH                        (640)
#define GD_NB_BITS_LCDPOS                       (11)
#define GD_FP_FRAC_SIZE                         (8)
#define GD_FIXEDPOINT_SIZE                      (3+GD_FP_FRAC_SIZE)
#define GD_NB_BITS_STRIDE                       (13)
#define GD_MAX_SLCD_READ_LEN                    (4)
#define GD_MAX_SLCD_CLK_DIVIDER                 (255)
#define GD_NB_WORKBUF_WORDS                     (5856)
#define GD_NB_LCD_CMD_WORDS                     (64)
#define GD_SRAM_SIZE                            ((GD_NB_WORKBUF_WORDS+GD_NB_LCD_CMD_WORDS)*2)
#define GD_SRAM_ADDR_WIDTH                      (13)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// GOUDA_T
// -----------------------------------------------------------------------------
/// Number of bits for stride storage
// =============================================================================
#define REG_GOUDA_BASE             0x01A21000

typedef volatile struct
{
    REG32                          gd_command;                   //0x00000000
    REG32                          gd_status;                    //0x00000004
    REG32                          gd_eof_irq;                   //0x00000008
    REG32                          gd_eof_irq_mask;              //0x0000000C
    REG32                          gd_roi_tl_ppos;               //0x00000010
    REG32                          gd_roi_br_ppos;               //0x00000014
    REG32                          gd_roi_bg_color;              //0x00000018
    REG32                          gd_vl_input_fmt;              //0x0000001C
    REG32                          gd_vl_tl_ppos;                //0x00000020
    REG32                          gd_vl_br_ppos;                //0x00000024
    REG32                          gd_vl_extents;                //0x00000028
    REG32                          gd_vl_blend_opt;              //0x0000002C
    REG32                          gd_vl_y_src;                  //0x00000030
    REG32                          gd_vl_u_src;                  //0x00000034
    REG32                          gd_vl_v_src;                  //0x00000038
    REG32                          gd_vl_resc_ratio;             //0x0000003C
    /// The Overlay layers have a fixed depth relative to their index. Overlay
    /// layer 0 is the first to be drawn (thus the deepest), overlay layer 2 is
    /// the last to be drawn.
    struct
    {
        REG32                      gd_ol_input_fmt;              //0x00000040
        REG32                      gd_ol_tl_ppos;                //0x00000044
        REG32                      gd_ol_br_ppos;                //0x00000048
        REG32                      gd_ol_blend_opt;              //0x0000004C
        REG32                      gd_ol_rgb_src;                //0x00000050
    } Overlay_Layer[3];
    REG32                          gd_lcd_ctrl;                  //0x0000007C
    /// All value are in cycle number of system clock
    REG32                          gd_lcd_timing;                //0x00000080
    REG32                          gd_lcd_mem_address;           //0x00000084
    REG32                          gd_lcd_stride_offset;         //0x00000088
    REG32                          gd_lcd_single_access;         //0x0000008C
    REG32                          gd_spilcd_config;             //0x00000090
    REG32                          gd_spilcd_rd;                 //0x00000094
    REG32                          gd_vl_fix_ratio;              //0x00000098
    REG32                          TECON;                        //0x0000009C
    REG32                          TECON2;                       //0x000000A0
    REG32 Reserved_000000A4[174];               //0x000000A4
} HWP_GOUDA_T;

#define hwp_gouda                  ((HWP_GOUDA_T*) KSEG1(REG_GOUDA_BASE))


//gd_command
#define GOUDA_START                (1<<0)

//gd_status
#define GOUDA_IA_BUSY              (1<<0)
#define GOUDA_LCD_BUSY             (1<<4)

//gd_eof_irq
#define GOUDA_EOF_CAUSE            (1<<0)
#define GOUDA_EOF_STATUS           (1<<16)

//gd_eof_irq_mask
#define GOUDA_EOF_MASK             (1<<0)

//gd_roi_tl_ppos
#define GOUDA_X0(n)                (((n)&0x7FF)<<0)
#define GOUDA_Y0(n)                (((n)&0x7FF)<<16)

//gd_roi_br_ppos
#define GOUDA_X1(n)                (((n)&0x7FF)<<0)
#define GOUDA_Y1(n)                (((n)&0x7FF)<<16)

//gd_roi_bg_color
#define GOUDA_B(n)                 (((n)&31)<<0)
#define GOUDA_G(n)                 (((n)&0x3F)<<5)
#define GOUDA_R(n)                 (((n)&31)<<11)

//gd_vl_input_fmt
#define GOUDA_FORMAT(n)            (((n)&3)<<0)
#define GOUDA_STRIDE(n)            (((n)&0x1FFF)<<2)
#define GOUDA_ACTIVE               (1<<31)

//gd_vl_tl_ppos
//#define GOUDA_X0(n)              (((n)&0x7FF)<<0)
//#define GOUDA_Y0(n)              (((n)&0x7FF)<<16)

//gd_vl_br_ppos
//#define GOUDA_X1(n)              (((n)&0x7FF)<<0)
//#define GOUDA_Y1(n)              (((n)&0x7FF)<<16)

//gd_vl_extents
#define GOUDA_MAX_LINE(n)          (((n)&0x7FF)<<0)
#define GOUDA_MAX_COL(n)           (((n)&0x7FF)<<16)

//gd_vl_blend_opt
#define GOUDA_CHROMA_KEY_B(n)      (((n)&31)<<0)
#define GOUDA_CHROMA_KEY_B_MASK    (31<<0)
#define GOUDA_CHROMA_KEY_G(n)      (((n)&0x3F)<<5)
#define GOUDA_CHROMA_KEY_G_MASK    (0x3F<<5)
#define GOUDA_CHROMA_KEY_R(n)      (((n)&31)<<11)
#define GOUDA_CHROMA_KEY_R_MASK    (31<<11)
#define GOUDA_CHROMA_KEY_ENABLE    (1<<16)
#define GOUDA_CHROMA_KEY_ENABLE_MASK (1<<16)
#define GOUDA_CHROMA_KEY_MASK(n)   (((n)&7)<<17)
#define GOUDA_CHROMA_KEY_MASK_MASK (7<<17)
#define GOUDA_ALPHA(n)             (((n)&0xFF)<<20)
#define GOUDA_ALPHA_MASK           (0xFF<<20)
#define GOUDA_ROTATION(n)          (((n)&3)<<28)
#define GOUDA_ROTATION_MASK        (3<<28)
#define GOUDA_DEPTH(n)             (((n)&3)<<30)
#define GOUDA_DEPTH_MASK           (3<<30)
#define GOUDA_CHROMA_KEY_COLOR(n)  (((n)&0xFFFF)<<0)
#define GOUDA_CHROMA_KEY_COLOR_MASK (0xFFFF<<0)
#define GOUDA_CHROMA_KEY_COLOR_SHIFT (0)

//gd_vl_y_src
#define GOUDA_ADDR(n)              (((n)&0xFFFFFF)<<2)

//gd_vl_u_src
//#define GOUDA_ADDR(n)            (((n)&0xFFFFFF)<<2)

//gd_vl_v_src
//#define GOUDA_ADDR(n)            (((n)&0xFFFFFF)<<2)

//gd_vl_resc_ratio
#define GOUDA_XPITCH(n)            (((n)&0x7FF)<<0)
#define GOUDA_YPITCH(n)            (((n)&0x7FF)<<16)
#define GOUDA_PRE_FETCH_EN         (1<<29)
#define GOUDA_IY_DCTENABLE         (1<<30)
#define GOUDA_YPITCH_SCALE_ENABLE  (1<<31)

//gd_ol_input_fmt
//#define GOUDA_FORMAT(n)          (((n)&3)<<0)
//#define GOUDA_STRIDE(n)          (((n)&0x1FFF)<<2)
#define GOUDA_PREFETCH             (1<<18)
//#define GOUDA_ACTIVE             (1<<31)

//gd_ol_tl_ppos
//#define GOUDA_X0(n)              (((n)&0x7FF)<<0)
//#define GOUDA_Y0(n)              (((n)&0x7FF)<<16)

//gd_ol_br_ppos
//#define GOUDA_X1(n)              (((n)&0x7FF)<<0)
//#define GOUDA_Y1(n)              (((n)&0x7FF)<<16)

//gd_ol_blend_opt
//#define GOUDA_CHROMA_KEY_B(n)    (((n)&31)<<0)
//#define GOUDA_CHROMA_KEY_G(n)    (((n)&0x3F)<<5)
//#define GOUDA_CHROMA_KEY_R(n)    (((n)&31)<<11)
//#define GOUDA_CHROMA_KEY_ENABLE  (1<<16)
//#define GOUDA_CHROMA_KEY_MASK(n) (((n)&7)<<17)
//#define GOUDA_ALPHA(n)           (((n)&0xFF)<<20)
//#define GOUDA_CHROMA_KEY_COLOR(n) (((n)&0xFFFF)<<0)
//#define GOUDA_CHROMA_KEY_COLOR_MASK (0xFFFF<<0)
//#define GOUDA_CHROMA_KEY_COLOR_SHIFT (0)

//gd_ol_rgb_src
//#define GOUDA_ADDR(n)            (((n)&0xFFFFFF)<<2)

//gd_lcd_ctrl
#define GOUDA_DESTINATION_LCD_CS_0 (0<<0)
#define GOUDA_DESTINATION_LCD_CS_1 (1<<0)
#define GOUDA_DESTINATION_MEMORY_LCD_TYPE (2<<0)
#define GOUDA_DESTINATION_MEMORY_RAM (3<<0)
#define GOUDA_OUTPUT_FORMAT_8_BIT_RGB332 (0<<4)
#define GOUDA_OUTPUT_FORMAT_8_BIT_RGB444 (1<<4)
#define GOUDA_OUTPUT_FORMAT_8_BIT_RGB565 (2<<4)
#define GOUDA_OUTPUT_FORMAT_16_BIT_RGB332 (4<<4)
#define GOUDA_OUTPUT_FORMAT_16_BIT_RGB444 (5<<4)
#define GOUDA_OUTPUT_FORMAT_16_BIT_RGB565 (6<<4)
#define GOUDA_HIGH_BYTE            (1<<7)
#define GOUDA_CS0_POLARITY         (1<<8)
#define GOUDA_CS1_POLARITY         (1<<9)
#define GOUDA_RS_POLARITY          (1<<10)
#define GOUDA_WR_POLARITY          (1<<11)
#define GOUDA_RD_POLARITY          (1<<12)
#define GOUDA_NB_COMMAND(n)        (((n)&0x3F)<<16)
#define GOUDA_START_COMMAND        (1<<24)
#define GOUDA_LCD_RESETB           (1<<25)

//gd_lcd_timing
#define GOUDA_TAS(n)               (((n)&7)<<0)
#define GOUDA_TAH(n)               (((n)&7)<<4)
#define GOUDA_PWL(n)               (((n)&0x3F)<<8)
#define GOUDA_PWH(n)               (((n)&0x3F)<<16)

//gd_lcd_mem_address
#define GOUDA_ADDR_DST(n)          (((n)&0xFFFFFF)<<2)

//gd_lcd_stride_offset
#define GOUDA_STRIDE_OFFSET(n)     (((n)&0x3FF)<<0)

//gd_lcd_single_access
#define GOUDA_LCD_DATA(n)          (((n)&0xFFFF)<<0)
#define GOUDA_TYPE                 (1<<16)
#define GOUDA_START_WRITE          (1<<17)
#define GOUDA_START_READ           (1<<18)

//gd_spilcd_config
#define GOUDA_SPI_LCD_SELECT       (1<<0)
#define GOUDA_SPI_DEVICE_ID(n)     (((n)&0x3F)<<1)
#define GOUDA_SPI_DEVICE_ID_MASK   (0x3F<<1)
#define GOUDA_SPI_DEVICE_ID_SHIFT  (1)
#define GOUDA_SPI_CLK_DIVIDER(n)   (((n)&0xFF)<<7)
#define GOUDA_SPI_CLK_DIVIDER_MASK (0xFF<<7)
#define GOUDA_SPI_CLK_DIVIDER_SHIFT (7)
#define GOUDA_SPI_DUMMY_CYCLE(n)   (((n)&7)<<15)
#define GOUDA_SPI_DUMMY_CYCLE_MASK (7<<15)
#define GOUDA_SPI_DUMMY_CYCLE_SHIFT (15)
#define GOUDA_SPI_LINE_MASK        (3<<18)
#define GOUDA_SPI_LINE_4           (0<<18)
#define GOUDA_SPI_LINE_3           (1<<18)
#define GOUDA_SPI_LINE_4_START_BYTE (2<<18)
#define GOUDA_SPI_LINE_3_TWO_LANE  (3<<18)
#define GOUDA_SPI_RX_BYTE(n)       (((n)&7)<<20)
#define GOUDA_SPI_RX_BYTE_MASK     (7<<20)
#define GOUDA_SPI_RX_BYTE_SHIFT    (20)
#define GOUDA_SPI_RW_WRITE         (0<<23)
#define GOUDA_SPI_RW_READ          (1<<23)

//gd_spilcd_rd
#define GOUDA_SPI_LCD_RD(n)        (((n)&0xFFFFFFFF)<<0)

//gd_vl_fix_ratio
#define GOUDA_REG_VL_ONLY_SEL      (1<<19)
#define GOUDA_MIRROR               (1<<18)
#define GOUDA_L_YFIXEN             (1<<17)
#define GOUDA_L_XFIXEN             (1<<16)
#define GOUDA_L_YRATIO(n)          (((n)&0xFF)<<8)
#define GOUDA_L_XRATIO(n)          (((n)&0xFF)<<0)

//TECON
#define GOUDA_TE_COUNT2(n)         (((n)&0xFFF)<<16)
#define GOUDA_TE_MODE              (1<<2)
#define GOUDA_TE_EDGE_SEL          (1<<1)
#define GOUDA_TE_EN                (1<<0)

//TECON2
#define GOUDA_TE_COUNT1(n)         (((n)&0x1FFFFFFF)<<0)



// ============================================================================
// GOUDA_SRAM_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_GOUDA_SRAM_BASE        0x01AE0000

typedef volatile struct
{
    UINT8 sram_array[11840];                    //0x00000000
} HWP_GOUDA_SRAM_T;

#define hwp_goudaSram              ((HWP_GOUDA_SRAM_T*) KSEG1(REG_GOUDA_SRAM_BASE))






#endif

