////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2012, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:irqs.h
//
//            maintainer: Halley
//
//		Description	:
//					Declare architectual interrupt definition
//
//		Changelog	:
///////////////////////////////////////////////////////////////////////////////

#ifndef __IRQS_H__
#define __IRQS_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


/*
 * define SOC priority bits depend on the specific family
 */
#define	__NVIC_PRIO_BITS	(3)

typedef enum IRQn
{
    /*
     * CM3 internal interrupt(Exception/Fault)
     */
    RST_IRQn		= -15,
    NMI_IRQn		= -14,
    HARDFLT_IRQn	= -13,
    MMFLT_IRQn		= -12,//programmable as below
    BUSFLT_IRQn		= -11,
    USGFLT_IRQn		= -10,
    SVCALL_IRQn		= -5,
    DBGMON_IRQn		= -4,
    PENDSV_IRQn		= -2,
    SysTick_IRQn	= -1,
    /*
     * SOC interrupt(External Interrupt)
     */
    GPIO_IRQn		= 0,
    RTC_IRQn		= 1,
    IR_IRQn			= 2,
    FUART_IRQn		= 3,
    BUART_IRQn		= 4,
    PWC_IRQn		= 5,
    TMR0_IRQn		= 6,
    USB_IRQn		= 7,
    DMACH0_IRQn		= 8,
    DMACH1_IRQn		= 9,
    DECODER_IRQn	= 10,
    SPIS_IRQn		= 11,
    SD_IRQn			= 12,
    SPIM_IRQn		= 13,
    TMR1_IRQn		= 14,
    WDG_IRQn		= 15,
} IRQn_Type;

#define	IRQ_PRIO_HIGHEST	(1)/* 0 for reserved,do not change the highest exteran interrupt priority at will!!!*/
#define	IRQ_PRIO_LOWEST		((1 << __NVIC_PRIO_BITS) - 1)

typedef enum IRQn_PRIO
{
    /*
     * CM3 internal interrupt(Exception/Fault)
     */
    RST_IRQn_PRIO		= -3,
    NMI_IRQn_PRIO		= -2,
    HARDFLT_IRQn_PRIO	= -1,
    MMFLT_IRQn_PRIO		= 0,
    BUSFLT_IRQn_PRIO	= 0,
    USGFLT_IRQn_PRIO	= 0,
    SVCALL_IRQn_PRIO	= 1,
    DBGMON_IRQn_PRIO	= 0,
    PENDSV_IRQn_PRIO	= IRQ_PRIO_LOWEST/*must be the lowest*/,
    SYSTICK_IRQn_PRIO	= IRQ_PRIO_LOWEST/*must be the lowest*/,
    /*
     * SOC interrupt(External Interrupt)
     */
    GPIO_IRQn_PRIO		= IRQ_PRIO_LOWEST,
    RTC_IRQn_PRIO		= IRQ_PRIO_LOWEST,
    IR_IRQn_PRIO		= 2,
    FUART_IRQn_PRIO		= IRQ_PRIO_HIGHEST,
    BUART_IRQn_PRIO		= 1,
    PWC_IRQn_PRIO		= 6,
    TMR0_IRQn_PRIO		= IRQ_PRIO_LOWEST,
    USB_IRQn_PRIO		= 2,
    DMACH0_IRQn_PRIO	= 4,
    DMACH1_IRQn_PRIO	= 5,
    DECODER_IRQn_PRIO	= 3/*IRQ_PRIO_HIGHEST*/,
    SPIS_IRQn_PRIO		= 5,
    SD_IRQn_PRIO		= 4,
    SPIM_IRQn_PRIO		= 3,
    TMR1_IRQn_PRIO		= IRQ_PRIO_LOWEST,
    WDG_IRQn_PRIO		= IRQ_PRIO_LOWEST,
} IRQn_PRIO_TYPE;

#if defined(__ICCARM__)
static void __ENABLE_IRQ(void)
{
	asm("cpsie	i ");
	asm("bx		lr");
}

static void __DISABLE_IRQ(void)
{
	asm("cpsid	i ");
	asm("bx		lr");
}

static void __ENABLE_FAULT_IRQ(void)
{
	asm("cpsie	f ");
	asm("bx		lr");
}

static void __DISABLE_FAULT_IRQ(void)
{
	asm("cpsid	f ");
	asm("bx		lr");
}

#elif defined(__CC_ARM)

static __asm void __ENABLE_IRQ(void)
{
	cpsie	i
	bx		lr
}

static __asm void __DISABLE_IRQ(void)
{
	cpsid	i
	bx		lr
}

static __asm void __ENABLE_FAULT_IRQ(void)
{
	cpsie	f
	bx		lr
}

static __asm void __DISABLE_FAULT_IRQ(void)
{
	cpsid	f
	bx		lr
}

#elif defined(__GNUC__)

static void __ENABLE_IRQ(void)
{
	__asm("cpsie	i \n"
			"bx		lr \n");

}

static void __DISABLE_IRQ(void)
{
	__asm("cpsid	i \n"
				"bx		lr \n");
}

static void __ENABLE_FAULT_IRQ(void)
{
	__asm("cpsie	f \n"
				"bx		lr \n");
}

static void __DISABLE_FAULT_IRQ(void)
{
	__asm("cpsid	f \n"
					"bx		lr \n");
}

#endif

#define	__DISABLE_IRQ_FROMISR(Flag) \
	do{\
		(Flag) = __get_BASEPRI(); \
		__set_BASEPRI(0x20); \
	}while(0)

#define	__ENABLE_IRQ_FROMISR(Flag)	__set_BASEPRI(Flag)

#define	__ENABLE_FIRQ_FROMISR(Flag)	__ENABLE_IRQ_FROMISR(Flag)

#define	__DISABLE_FIRQ_FROMISR(Flag)	__DISABLE_IRQ_FROMISR(Flag)

#include <../../CMSIS/Include/core_cm3.h>
/** \brief  Get External Interrupt Control Status
 *
 *  The function get a device-specific interrupt enable or disable status 
 *  in the NVIC interrupt controller.
 *
 *  \param [in]   IRQn  External interrupt number. Value cannot be negative.
 *
 */
static  bool NVIC_GetIRQCtlStat(IRQn_Type IRQn)
{
	return (NVIC->ISER[((uint32_t)(IRQn) >> 5)] & (1 << (IRQn & 0x1F))) >> (IRQn & 0x1F);
}


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif //__IRQS_H__
