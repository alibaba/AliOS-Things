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

#ifndef _GPADC_ASM_H_
#define _GPADC_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'gpadc'."
#endif



//==============================================================================
// gpadc
//------------------------------------------------------------------------------
///
//==============================================================================

//ctrl
#define GPADC_CH0_EN               (1<<0)
#define GPADC_CH1_EN               (1<<1)
#define GPADC_CH2_EN               (1<<2)
#define GPADC_CH3_EN               (1<<3)
#define GPADC_ATP(n)               (((n)&7)<<8)
#define GPADC_ATP_122US            (0<<8)
#define GPADC_ATP_100MS            (1<<8)
#define GPADC_ATP_10MS             (2<<8)
#define GPADC_ATP_1MS              (3<<8)
#define GPADC_ATP_250MS            (4<<8)
#define GPADC_ATP_500MS            (5<<8)
#define GPADC_ATP_1S               (6<<8)
#define GPADC_ATP_2S               (7<<8)
#define GPADC_PD_MODE              (1<<16)
#define GPADC_PD_MODE_SW           (0<<16)
#define GPADC_PD_MODE_HW           (1<<16)
#define GPADC_MANUAL_MODE          (1<<20)
#define GPADC_CH_EN(n)             (((n)&15)<<0)
#define GPADC_CH_EN_MASK           (15<<0)
#define GPADC_CH_EN_SHIFT          (0)

//status
#define GPADC_THRSHD0_IRQ          (1<<0)
#define GPADC_THRSHD1_IRQ          (1<<4)
#define GPADC_EOC                  (1<<8)
#define GPADC_BUSY                 (1<<12)

//data_REG
#define GPADC_DATA(n)              (((n)&0x3FF)<<0)

//data_ch0
//#define GPADC_DATA(n)            (((n)&0x3FF)<<0)

//data_ch1
//#define GPADC_DATA(n)            (((n)&0x3FF)<<0)

//data_ch2
//#define GPADC_DATA(n)            (((n)&0x3FF)<<0)

//data_ch3
//#define GPADC_DATA(n)            (((n)&0x3FF)<<0)

//irq_mask
//#define GPADC_THRSHD0_IRQ        (1<<0)
//#define GPADC_THRSHD1_IRQ        (1<<4)
//#define GPADC_EOC                (1<<8)

//irq_cause
//#define GPADC_THRSHD0_IRQ        (1<<0)
//#define GPADC_THRSHD1_IRQ        (1<<4)
//#define GPADC_EOC                (1<<8)

//irq_clr
//#define GPADC_THRSHD0_IRQ        (1<<0)
//#define GPADC_THRSHD1_IRQ        (1<<4)
//#define GPADC_EOC                (1<<8)

//thrshd
#define GPADC_THRSHD0(n)           (((n)&0x3FF)<<0)
#define GPADC_THRSHD1(n)           (((n)&0x3FF)<<16)

//cmd_set
#define GPADC_GPADC_PD             (1<<0)
#define GPADC_GPADC_RESET          (1<<4)
#define GPADC_START_MEASURE        (1<<8)

//cmd_clr
//#define GPADC_GPADC_PD           (1<<0)
//#define GPADC_GPADC_RESET        (1<<4)




#endif
