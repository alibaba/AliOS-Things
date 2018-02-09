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

#include <stdbool.h>
#include <stdint.h>
#include <csi_config.h>
#include <drv_intc.h>
#include "zx29_intc.h"
#include "soc.h"
#include <csi_core.h>

extern void *g_irqvector[];
//extern void ramdump_entry (void);

#define ERR_INTC(errno) (CSI_DRV_ERRNO_INTC_BASE | errno)

#define INTC_NULL_PARAM_CHK(para)                  \
    do {                                        \
        if (para == NULL) {                     \
            return ERR_INTC(DRV_ERROR_PARAMETER);   \
        }                                       \
    } while (0)


uint32_t pcu_ao_flag = 0;
#ifdef CONFIG_HAVE_ICU
volatile T_Int_RegsMap *gIcuReg = NULL;
extern void *g_icuvector[];
#endif

# define assert(__e) ((void)0)
#if 0
void abort(void)
{
    ramdump_entry();
}
#endif

typedef unsigned int irqstate_t;

#define set_reg_bit(regName, bitAddr, bitValue)      \
    do{                                                  \
        if(bitValue == true)                             \
            (*(volatile unsigned long*)(regName)) |= (0x1<<bitAddr);            \
        else                                             \
            (*(volatile unsigned long*)(regName)) &= ~(0x1<<bitAddr);           \
    }while(0)

#ifdef CONFIG_HAVE_PCU

typedef struct _T_Pcu_Table {
    const uint16_t intNo;
    const char intTypeOffset;           /*pcu int wake type/pol, 0:pulseint        1:level int
                                                             0:low or negedge  1:hign or posedge*/
    const char intClearOffset;       /*pcu int wake clear, read only reg,    1:clear int*/
    const char intWakeOffset;        /*pcu int wake dis,     0:wakeup int       1:not wakeup int    */
} T_Pcu_Table;

T_Pcu_Table pcu_reg_offset[] = {
    {PIC_IRQ_EXT0,        0,    0,    0},
    {ICU_IRQ_EXT1,        1,    1,    1},
    {ICU_IRQ_EXT2,        2,    2,    2},
    {ICU_IRQ_EXT3,        3,    3,    3},
    {ICU_IRQ_EXT4,        4,    4,    4},
    {ICU_IRQ_EXT5,        5,    5,    5},
    {ICU_IRQ_EXT6,        6,    6,    6},
    {ICU_IRQ_EXT7,        7,    7,    7},
    {PIC_IRQ_LP_TIMER,    8,    8,    10},
    {PIC_IRQ_AD_TIMER0,    9,    9,    17},
    {PIC_IRQ_AD_TIMER1,    10,    10,    18},
    {PIC_IRQ_LP_UART,    11,    12,    11},
    {ICU_IRQ_UART0,        12,    13,    12},
    {ICU_IRQ_LP_UART_RXD, 13, 14,    9},
    {ICU_IRQ_LP_UART_CTS, 14, 15,    8},
    {ICU_IRQ_UART2,        15,    16,    13},
    {PIC_IRQ_ADC,        16,    17,    14},
    {PIC_IRQ_DAC,        17,    18,    15},
    {PIC_IRQ_ICP_DSP2CK, 20,    21,    21},
    {ICU_IRQ_PCU_AO2AP_WAKE, 22,    23,    23},
    {ICU_IRQ_PCU_CPSS2AP, 24, 25, 25},
    {ICU_IRQ_CP_WTD_RST, 25, 26, 26},
    {ICU_IRQ_EX0_DC2,   2,  2,  4},
    {ICU_IRQ_EX1_DC2,   3,  3,  5},
    {ICU_IRQ_EX2_DC2,   4,  4,  6},
    {ICU_IRQ_EX3_DC2,   5,  5,  7},
    {ICU_IRQ_EX4_DC2,   6,  6,  8},
    {ICU_IRQ_EX5_DC2,   7,  7,  9},
    {ICU_IRQ_EX6_DC2,   8,  8,  10},
    {ICU_IRQ_EX7_DC2,   9,  9,  11},
    {ICU_IRQ_WAKE1_SP,  10, 10, 12},
    {ICU_IRQ_WAKE1_LP,  11, 11, 13},
    {ICU_IRQ_WAKEUP1_LLP, 12, 12, 21},
    {PIC_IRQ_RTC_ALARM, 13, 13, 14},
    {PIC_IRQ_RTC_TIMER, 14, 14, 15},
    {PIC_IRQ_RTC_TIMESTAMP, 15, 15, 16},
    {PIC_IRQ_LPM0,      16, 16, 17},
    {ICU_IRQ_IWDT_RST,  19, 19, 20}
};

#endif

static inline unsigned int irqsave(void)
{
    unsigned int flags;
    asm volatile(
        "mfcr   %0, psr  \n"
        "psrclr ie      \n"
        :"=r"(flags) : :"memory");
    return flags;
}

/* Restore saved primask state */

static inline void irqrestore(unsigned int flags)
{
    asm volatile(
        "mtcr    %0, psr  \n"
        : :"r"(flags) :"memory");
}

#ifdef CONFIG_HAVE_ICU
/**
 * This function is used for mask the interrupt
 * @param[in]  line     irq number
 * @no return
 */
static void icu_mask_irq(uint32_t line)
{
    uint32_t  uiRegID  = 0;
    uint32_t  uiBitmap = 0;
    uint32_t  flags;

    flags = irqsave();

    if (line < ICU_INT_START) {
        irqrestore(flags);
        return;
    }

    uiRegID = ((line - ICU_INT_START) >> 5);
    uiBitmap = (1 << ((line - ICU_INT_START) & 0x1F));
    gIcuReg->IntMaskReg[uiRegID] &= ~uiBitmap;

    irqrestore(flags);
}

/**
 * This function is used for unmask the interrupt
 * @param[in]  line     irq number
 * @no return
 */
static void icu_unmask_irq(uint32_t line)
{
    uint32_t  uiRegID, uiBitmap = 0;
    uint32_t  flags;

    flags = irqsave();

    if (line < ICU_INT_START) {
        irqrestore(flags);
        return;
    }

    uiRegID = ((line - ICU_INT_START) >> 5);
    uiBitmap = (1 << ((line - ICU_INT_START) & 0x1F));
    gIcuReg->IntMaskReg[uiRegID] |= uiBitmap;

    irqrestore(flags);

}


/**
* This function is used for set int level
* @param[in]  line     irq number
* @param[in]  level irq trigger mode(low_level,high_level...)
* @no return
*/
static void icu_set_irq_level(uint32_t line, int_trigger_mode_t level)
{
#ifdef CONFIG_ARCH_IRQ_PRIO_TYPE
    uint32_t tmp = 0;
    uint32_t level_trans = 0;

    if (INT_MODE_DOUBLE_EDGE <= level) {
        assert(0);
        return;
    }

    switch (level) {
        case INT_MODE_LOW_LEVEL:
            level_trans = 0;
            break;

        case INT_MODE_HIGH_LEVEL:
            level_trans = 3;
            break;

        case INT_MODE_RISING_EDGE:
            level_trans = 1;
            break;

        case INT_MODE_FALLING_EDGE:
            level_trans = 2;
            break;

        default:
            level_trans = 0;
            break;
    }

    tmp = gIcuReg->IntModeReg[line - ICU_INT_START];
    tmp &= ~(3 << 30);
    tmp |= (level_trans << 30);
    gIcuReg->IntModeReg[line - ICU_INT_START] = tmp;
#endif
}

/**
* This function is used for set int priority
* @param[in]  line    irq number
* @param[in]  pri irq priority
* @no return
*/
static void icu_set_irq_prio(uint32_t line, uint32_t pri)
{
#ifdef CONFIG_ARCH_IRQ_PRIO_TYPE

    uint32_t tmp = 0;

    if (INT_PRI_NUM <= pri) {
        assert(0);
        return;
    }

    if (line < ICU_INT_START) {
        return;
    }

    tmp = gIcuReg->IntModeReg[line - ICU_INT_START];
    tmp &= ~(0xf << 25);
    tmp |= pri;

    gIcuReg->IntModeReg[line - ICU_INT_START] = tmp;
#endif
}

/**
* This function is used for icu vector dispatch
* @param[in]  irq    irq number
* @param[in]  context context of vector
* @no return
*/
void icu_vector_handle(void)
{
    uint32_t  irqVect = gIcuReg->IntIrqNumReg;
    uint32_t  irqCurrent = 0;
    T_Icu_Callback callback = NULL;

    if (irqVect > ICU_IRQS) {
        assert(0);
        return;
    }

    icu_mask_irq(irqVect + ICU_INT_START);

    callback = g_icuvector[irqVect];
    (*callback)();/*jump to callback func*/

    irqCurrent = (gIcuReg->IntIrqCurReg & 0x7f0);

    gIcuReg->IntIrqCurReg = irqCurrent;

    icu_unmask_irq(irqVect + ICU_INT_START);

}
#endif

#ifdef CONFIG_HAVE_PCU

/**
* This function is used for get int level offset
* @param[in]  line irq number
* @return the offset value of irq level reg
*/
static uint32_t pcu_get_type_offset_by_irq(uint32_t line)
{
    uint32_t i = 0;

    for (i = 0; i < sizeof(pcu_reg_offset) / sizeof(T_Pcu_Table); i++) {
        if (line == pcu_reg_offset[i].intNo) {
            break;
        }
    }

    if (i >= sizeof(pcu_reg_offset) / sizeof(T_Pcu_Table)) {
        return 31;    /*31bit reserved*/
    }

    if (i <= 21) {
        pcu_ao_flag = 1;
    }

    return pcu_reg_offset[i].intTypeOffset;
}

/**
* This function is used for get int clear offset
* @param[in]  line irq number
* @return the offset value of irq clear reg
*/
static uint32_t pcu_get_clear_offset_by_irq(uint32_t line)
{
    uint32_t i = 0;

    for (i = 0; i < sizeof(pcu_reg_offset) / sizeof(T_Pcu_Table); i++) {
        if (line == pcu_reg_offset[i].intNo) {
            break;
        }
    }

    if (i >= sizeof(pcu_reg_offset) / sizeof(T_Pcu_Table)) {
        return 31;    /*31bit reserved*/
    }

    if (i <= 21) {
        pcu_ao_flag = 1;
    }

    return pcu_reg_offset[i].intClearOffset;
}


/**
* This function is used for judge is irq through pcu
* @param[in]  line irq number
* @return 1 irq through pcu, 0 irq not through pcu
*/
static uint32_t pcu_is_irq_through_pcu(uint32_t line)
{
    uint32_t i = 0;

    for (i = 0; i < sizeof(pcu_reg_offset) / sizeof(T_Pcu_Table); i++) {
        if (line == pcu_reg_offset[i].intNo) {
            break;
        }
    }

    if (i >= sizeof(pcu_reg_offset) / sizeof(T_Pcu_Table)) {
        return 0;    /*can not found irq in pcu table*/
    }

    return 1;
}

/**
* This function is used for clear pcu int
* @param[in]  line irq number
* @no return
*/
static void pcu_clear_int(uint32_t line)
{
    uint32_t offset = 0;

    offset = pcu_get_clear_offset_by_irq(line);

    if (pcu_ao_flag == 1) {
        set_reg_bit(PCU_AO_CLEAR_ADDR, offset, 1);
        set_reg_bit(PCU_AO_CLEAR_ADDR, offset, 0);
        pcu_ao_flag = 0;
    } else {
        set_reg_bit(PCU_SBY_CLEAR_ADDR, offset, 1);
        set_reg_bit(PCU_SBY_CLEAR_ADDR, offset, 0);
    }
}

/**
* This function is used for set pcu int level
* @param[in]  line irq number
* @param[in]  level irq trigger mode(low_level,high_level...)
* @no return
*/
static void pcu_set_irq_level(uint32_t line, int_trigger_mode_t level)
{
#ifdef CONFIG_ARCH_IRQ_PRIO_TYPE
    uint32_t type = 0;
    uint32_t pol = 0;
    uint32_t offset = 0;

    if (INT_MODE_DOUBLE_EDGE <= level) {
        assert(0);
        return;
    }

    switch (level) {
        case INT_MODE_LOW_LEVEL:
            type = 1;
            pol = 0;
            break;

        case INT_MODE_HIGH_LEVEL:
            type = 1;
            pol = 1;
            break;

        case INT_MODE_RISING_EDGE:
            type = 0;
            pol = 1;
            break;

        case INT_MODE_FALLING_EDGE:
            type = 0;
            pol = 0;
            break;

        default:
            type = 1;
            pol = 1;
            break;
    }

    offset = pcu_get_type_offset_by_irq(line);

    if (pcu_ao_flag == 1) {
        set_reg_bit(PCU_AO_TYPE_CONFIG_ADDR, offset, type);
        set_reg_bit(PCU_AO_POL_CONFIG_ADDR, offset, pol);
        pcu_ao_flag = 0;
    } else {
        set_reg_bit(PCU_SBY_TYPE_CONFIG_ADDR, offset, type);
        set_reg_bit(PCU_SBY_POL_CONFIG_ADDR, offset, pol);
    }

    pcu_clear_int(line);/*clear fake int*/
#endif
}

#endif

/**
 * This function is used for init irq
 */
void csi_intc_init(void)
{
    /* clear active */
    //ZX29_VIC_IABR = 0;
    // __enable_irq();
#ifdef CONFIG_HAVE_ICU
    gIcuReg = (volatile T_Int_RegsMap *)(ICU_INT_BASE);
#endif
}

/**
 * This function is disable the IRQ specified by 'irq'
 * @param[in]     irq             irq number to operate
 */
void csi_intc_disable_irq(int32_t IRQn)
{
    irqstate_t flags;

#ifndef CONFIG_HAVE_ICU

    if (IRQn < 0 || IRQn >= NR_IRQS) {
        return;
    }

    flags = irqsave();
    csi_vic_disable_irq(IRQn);
    irqrestore(flags);
#else

    if (IRQn < 0 || IRQn >= (NR_IRQS + ICU_IRQS) || IRQn == (NR_IRQS - 1)) {
        return;
    }

    flags = irqsave();

    if (IRQn < NR_IRQS) {
        csi_vic_disable_irq(IRQn);
    } else {
        csi_vic_disable_irq(31);
        icu_mask_irq(IRQn);
    }

    irqrestore(flags);
#endif

    return;
}


/**
 * This function is enable the IRQ specified by 'irq'
 * @param[in]     irq             irq number to operate
 */
void csi_intc_enable_irq(int32_t IRQn)
{
    irqstate_t flags;

#ifndef CONFIG_HAVE_ICU

    if (IRQn < 0 || IRQn >= NR_IRQS) {
        return ;
    }

    flags = irqsave();
    csi_vic_enable_irq(IRQn);
    irqrestore(flags);
#else

    if (IRQn < 0 || IRQn >= (NR_IRQS + ICU_IRQS)) {
        return ;
    }

    //(*(volatile uint32_t*)(0x901ffff0)) = 0x34;
    flags = irqsave();

    if (IRQn < NR_IRQS) {
        csi_vic_enable_irq(IRQn);
    } else {
        csi_vic_enable_irq(31);
        icu_unmask_irq(IRQn);
    }

    irqrestore(flags);
#endif

    return ;
}

/**
 * This function is get current pending irq. this interface is internally used by irq system
 * @param[in]       irq         irq number to operate
 * @return          -1 on failure, other - the pending irq number
 */
uint32_t csi_intc_get_pending_irq(int32_t IRQn)
{
    return 0;
}


/**
 * This function is configure the IRQ subsystem so that IRQ number 'irq' is dispatched to
 * @param[in]   isr             the intterrput service
 * @param[in]   irq             irq number to operate
 * @return      0 on success; -1 on failure
 */

#if 0
int csi_intc_attach_irq(int irq, int isr)
{
#ifdef CONFIG_HAVE_VIC
    return -1;
#else

#ifndef CONFIG_HAVE_ICU

    if (irq < 0 || irq >= NR_IRQS) {
        return -1;
    }

    g_irqvector[irq] = (void *)isr;
#else

    if (irq < 0 || irq >= (NR_IRQS + ICU_IRQS) || irq == (NR_IRQS - 1)) {
        return -1;
    }

    if (irq < NR_IRQS) {
        g_irqvector[irq] = (void *)isr;
    } else {
        g_irqvector[31] = (void *)icu_vector_handle;
        g_icuvector[irq - NR_IRQS] = (void *)isr;
        ZX29_VIC_ISER |= (1 << (31 & VIC_IRQ_MAXNUM));/*enable 31 ext int*/
    }

#endif
    return 0;
#endif
}
#endif

/**
 * This function is set the attributes of an IRQ.
 * @param[in]     irq             irq number to operate
 * @param[in]     priority         interrupt priority
 * @param[in]     trigger_mode     interrupt trigger_mode
 * @return      0 on success; -1 on failure
 */
int csi_intc_set_attribute(int32_t IRQn, uint32_t priority, int_trigger_mode_t trigger_mode)
{
#ifdef CONFIG_ARCH_IRQ_PRIO_TYPE

    // uint8_t    regnum,regbit;
    irqstate_t flags;

#ifndef CONFIG_HAVE_ICU

    if (IRQn < 0 || IRQn >= NR_IRQS) {
        return -1;
    }

    if (priority < 0 || priority >= 4) {
        return -1;
    }

    flags = irqsave();

    csi_vic_set_prio(IRQn, priority);

    irqrestore(flags);

    if (trigger_mode == INT_MODE_HIGH_LEVEL) {
        return 0;
    } else {
        return -1;
    }

#else

    if (IRQn < 0 || IRQn >= (NR_IRQS + ICU_IRQS) || IRQn == (NR_IRQS - 1)) {
        return -1;
    }

    if (priority < 0 || priority >= 4) {
        return -1;
    }

    flags = irqsave();

#ifdef CONFIG_HAVE_PCU

    if (pcu_is_irq_through_pcu(IRQn)) {
        pcu_set_irq_level(IRQn, trigger_mode);
        trigger_mode = INT_MODE_HIGH_LEVEL;
    }

#endif

    if (IRQn < NR_IRQS) {
        csi_vic_set_prio(IRQn, priority);

        if (trigger_mode == INT_MODE_HIGH_LEVEL) {
            irqrestore(flags);
            return 0;
        } else {
            irqrestore(flags);
            return -1;
        }
    } else {
        //ZX29_VIC_IPR7 |= (((priority & VIC_IPR_VALIDBIT)<<6) << (regbit*8));
        csi_vic_set_prio(IRQn, priority);

        icu_set_irq_prio(IRQn, priority);
        icu_set_irq_level(IRQn, trigger_mode);
    }

    irqrestore(flags);
    return 0;
#endif

#endif

    return 0;
}



/**
 * This funciton is acknowledge the IRQ. this interface is internally used by irq system
 * @param[in]       irq         irq number to operate
 * @return          0 on success; -1 on failure
 */
int csi_intc_ack_irq(int32_t IRQn)
{
#ifndef CONFIG_HAVE_ICU

    if (IRQn < 0 || IRQn >= NR_IRQS) {
        return -1 ;
    }

#else

    if (IRQn < 0 || IRQn >= (NR_IRQS + ICU_IRQS) || IRQn == (NR_IRQS - 1)) {
        return -1;
    }

#endif

#ifdef CONFIG_HAVE_PCU

    if (pcu_is_irq_through_pcu(IRQn)) {
        pcu_clear_int(IRQn);
    }

#endif

    return 0;
}


/**
 * This function is get interrupt status
 * @param[in]       irq         irq number to operate
 * @return          0 on success; -1 on failure
 */
int csi_intc_get_state(int32_t IRQn)
{
#ifndef CONFIG_HAVE_ICU

    if (IRQn < 0 || IRQn >= NR_IRQS) {
        return -1 ;
    }

    if (csi_vic_get_active(IRQn)) {
        return 1;
    } else {
        return 0;
    }

#else
    uint32_t regnum;
    uint32_t regbit;
    int int_status;

    if (IRQn < 0 || IRQn >= (NR_IRQS + ICU_IRQS) || IRQn == (NR_IRQS - 1)) {
        return -1;
    }

    if (IRQn < NR_IRQS) {
        if (csi_vic_get_active(IRQn)) {
            return 1;
        } else {
            return 0;
        }
    } else {
        regnum = (IRQn / 32);
        regbit = (IRQn % 32);
        int_status = (gIcuReg->IntReqReg[regnum] >> regbit) & 0x1;
        return int_status;
    }

#endif
}


/**
* This function is used for icu init
* @no return
*/
void tspend_init(void)
{
    gIcuReg = (volatile T_Int_RegsMap *)(ICU_INT_BASE);
}

/**
  \brief   Set Pending Interrupt
  \details Sets the pending bit of an external interrupt.
  \param [in]      IRQn  Interrupt number. Value cannot be negative.
 */
void csi_intc_set_pending_irq(int32_t IRQn)
{
    ;
}

/**
  \brief   Clear Pending Interrupt
  \details Clears the pending bit of an external interrupt.
  \param [in]      IRQn  External interrupt number. Value cannot be negative.
 */
void csi_intc_clear_pending_irq(int32_t IRQn)
{
    ;
}
/**
  \brief   Set Wake up Interrupt
  \details Sets the wake up bit of an external interrupt.
  \param [in]      IRQn  Interrupt number. Value cannot be negative.
 */
void csi_intc_set_wakeup_irq(int32_t IRQn)
{
    ;
}

/**
  \brief   Clear Wake up Interrupt
  \details Clears the wake up bit of an external interrupt.
  \param [in]      IRQn  External interrupt number. Value cannot be negative.
 */
void csi_intc_clear_wakeup_irq(int32_t IRQn)
{
    ;
}

/**
  \brief   Get Active Interrupt
  \details Reads the active register in the INTC and returns the active bit for the device specific interrupt.
  \param [in]      IRQn  Device specific interrupt number.
  \return             0  Interrupt status is not active.
  \return             1  Interrupt status is active.
  \note    IRQn must not be negative.
 */
uint32_t csi_intc_get_active(int32_t IRQn)
{
    return ERR_INTC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief   Set Threshold register
  \details set the threshold register in the INTC.
  \param [in]      VectThreshold  specific vecter threshold.
  \param [in]      PrioThreshold  specific priority threshold.
 */
void csi_intc_set_threshold(uint32_t VectThreshold, uint32_t PrioThreshold)
{
    ;
}

/**
  \brief   Set Interrupt Priority
  \details Sets the priority of an interrupt.
  \note    The priority cannot be set for every core interrupt.
  \param [in]      IRQn  Interrupt number.
  \param [in]  priority  Priority to set.
 */
void csi_intc_set_prio(int32_t IRQn, uint32_t priority)
{
    ;
}

/**
  \brief   Get Interrupt Priority
  \details Reads the priority of an interrupt.
           The interrupt number can be positive to specify an external (device specific) interrupt,
           or negative to specify an internal (core) interrupt.
  \param [in]   IRQn  Interrupt number.
  \return             Interrupt Priority.
                      Value is aligned automatically to the implemented priority bits of the microcontroller.
 */
uint32_t csi_intc_get_prio(int32_t IRQn)
{
    return ERR_INTC(DRV_ERROR_UNSUPPORTED);
}


