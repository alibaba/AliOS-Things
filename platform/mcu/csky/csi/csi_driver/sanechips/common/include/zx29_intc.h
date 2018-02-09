/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __ZX29_INTC_H
#define __ZX29_INTC_H

#define NR_IRQS           32
#define VIC_IPR_VALIDBIT 0x00000003
#define VIC_IRQ_MAXNUM   0x0000001F

#ifdef CONFIG_HAVE_ICU

#define ICU_IRQS                         36
#define INT_PRI_NUM                      15        /* 0-highest, 15-lowest, but 15 is always be masked in gic*/
/*so in fact, only 0-14 can generate interrupt*/
#define ICU_INT_BASE                     0x22008000

#endif

#ifdef CONFIG_HAVE_PCU

#define PCU_AO_BASE_ADDR                  0x20132000
#define PCU_SBY_BASE_ADDR                 0x20141000

#define PCU_AO_TYPE_CONFIG_ADDR              (PCU_AO_BASE_ADDR+0x4c)
#define PCU_AO_POL_CONFIG_ADDR               (PCU_AO_BASE_ADDR+0x50)
#define PCU_AO_CLEAR_ADDR                    (PCU_AO_BASE_ADDR+0x54)
#define PCU_SBY_TYPE_CONFIG_ADDR             (PCU_SBY_BASE_ADDR+0x1c)
#define PCU_SBY_POL_CONFIG_ADDR              (PCU_SBY_BASE_ADDR+0x20)
#define PCU_SBY_CLEAR_ADDR                   (PCU_SBY_BASE_ADDR+0x24)

#endif

/* define vic resesters */
#if 0
#define  ZX29_VIC_BASE 0xE000E100
#define  VIC_ISER        0x0000
#define  VIC_IWER        0x0040
#define  VIC_ICER        0x0080
#define  VIC_IWDR        0x00C0
#define  VIC_ISPR        0x0100
#define  VIC_ICPR        0x0180
#define  VIC_IABR        0x0200
#define  VIC_IPR0        0x0300
#define  VIC_IPR1        0x0304
#define  VIC_IPR2        0x0308
#define  VIC_IPR3        0x030C
#define  VIC_IPR4        0x0310
#define  VIC_IPR5        0x0314
#define  VIC_IPR6        0x0318
#define  VIC_IPR7        0x031C
#define  VIC_ISR         0x0B00
#define  VIC_IPTR        0x0B04

#define ZX29_VIC_ISER (*(volatile unsigned int *)( ZX29_VIC_BASE + VIC_ISER))
#define ZX29_VIC_IWER (*(volatile unsigned int *)( ZX29_VIC_BASE + VIC_IWER))
#define ZX29_VIC_ICER (*(volatile unsigned int *)( ZX29_VIC_BASE + VIC_ICER))
#define ZX29_VIC_IWDR (*(volatile unsigned int *)( ZX29_VIC_BASE + VIC_IWDR))
#define ZX29_VIC_ISPR (*(volatile unsigned int *)( ZX29_VIC_BASE + VIC_ISPR))
#define ZX29_VIC_ICPR (*(volatile unsigned int *)( ZX29_VIC_BASE + VIC_ICPR))
#define ZX29_VIC_IABR (*(volatile unsigned int *)( ZX29_VIC_BASE + VIC_IABR))
#define ZX29_VIC_IPR0 (*(volatile unsigned int *)( ZX29_VIC_BASE + VIC_IPR0))
#define ZX29_VIC_IPR1 (*(volatile unsigned int *)( ZX29_VIC_BASE + VIC_IPR1))
#define ZX29_VIC_IPR2 (*(volatile unsigned int *)( ZX29_VIC_BASE + VIC_IPR2))
#define ZX29_VIC_IPR3 (*(volatile unsigned int *)( ZX29_VIC_BASE + VIC_IPR3))
#define ZX29_VIC_IPR4 (*(volatile unsigned int *)( ZX29_VIC_BASE + VIC_IPR4))
#define ZX29_VIC_IPR5 (*(volatile unsigned int *)( ZX29_VIC_BASE + VIC_IPR5))
#define ZX29_VIC_IPR6 (*(volatile unsigned int *)( ZX29_VIC_BASE + VIC_IPR6))
#define ZX29_VIC_IPR7 (*(volatile unsigned int *)( ZX29_VIC_BASE + VIC_IPR7))
#define ZX29_VIC_ISR  (*(volatile unsigned int *)( ZX29_VIC_BASE + VIC_ISR))
#define ZX29_VIC_IPTR (*(volatile unsigned int *)( ZX29_VIC_BASE + VIC_IPTR))
#endif
/* define vic priority ,smaller priority number higher priority level */
#define PRIORITY_LEVEL1 0
#define PRIORITY_LEVEL2 1
#define PRIORITY_LEVEL3 2
#define PRIORITY_LEVEL4 3


#ifdef CONFIG_HAVE_ICU

typedef volatile struct _T_Int_RegsMap {
    uint32_t                IntVerReg;                  //�汾�żĴ���            0x000
    uint32_t                Reserved0[3];
    uint32_t                IntIrqNumReg;               //��ͨ�жϺżĴ���  0x010����ͨģʽ
    uint32_t                IntFiqNumReg;               //�����жϺżĴ���   0x014  ��ͨģʽ
    uint32_t                IntIrqCurReg;                       //��ͨ�ж��ֳ��Ĵ���    0x018����ͨģʽ
    uint32_t                IntFiqCurReg;               //�����ж��ֳ��Ĵ���    0x01c����ͨģʽ
    uint32_t                Reserved1;
    uint32_t                IntStatusReg;                   //�ж�״̬�Ĵ���        0x024����ͨģʽ
    uint32_t                Reserved2[4];
    uint32_t                IntTestReg ;                    //�жϲ��ԼĴ���        0x038
    uint32_t                IntSetBitReg[4];                //�жϵ�bit��λ�Ĵ���   0x03c~0x048
    uint32_t                IntClearReg[4] ;                //�ж���λ�Ĵ���        0x04c~0x058
    uint32_t                IntEnReg[4];                    //�ж�ʹ�ܼĴ���        0x05c~0x068
    uint32_t                IntDisEnReg[4];                 //�ж�ȥʹ�ܼĴ���  0x06c~0x078
    uint32_t                Reserved3;
    uint32_t                IntModeReg[128];    //�ж�ģʽ�Ĵ���        0x080~0x27c
    uint32_t                Reserved4;
    uint32_t                IcuFiqNum;                  //FIQ�ж����üĴ���     0x284
    uint32_t                IcuEnClockGate ;            //�Զ��ſ�ʹ�ܼĴ���    0x288
    uint32_t                Reserved5;
    uint32_t                IntMaskReg[4] ;             //�ж����μĴ���        0x290
    uint32_t                Reserved6[24];
    uint32_t                IntReqReg[4];               //�ж�����Ĵ���       0x300
    uint32_t                Reserved7[84];
    uint32_t                IntSetReg[4];                   //�ж���λ�Ĵ���        0x460
} T_Int_RegsMap;

typedef enum _T_Int_LineLevel {
    INT_LOWLEVEL = 0x00,        /* 00: low level */
    INT_POSEDGE = 0x01,         /* 01: raise edge */
    INT_NEGEDGE = 0x02,         /* 10: fall edge */
    INT_HIGHLEVEL = 0x03,       /* 11: high level */
    INT_DEFAULT_LEVEL,
}
T_Int_LineLevel;

typedef void (*T_Icu_Callback)(void);

#endif



#ifndef __ASSEMBLY__
#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

#undef EXTERN
#ifdef __cplusplus
}
#endif
#endif

#endif /* __ZX29_INTC_H */

