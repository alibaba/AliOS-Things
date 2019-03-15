/*
    FreeRTOS V7.0.0 - Copyright (C) 2011 Real Time Engineers Ltd.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************
*/
//maintainer Halley
#ifndef __FREERTOSCFG_H__
#define __FREERTOSCFG_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//#ifdef	DEBUG
#define	MMM_DEBUG
#define	CFG_SHELL_DEBUG
#ifdef CFG_SHELL_DEBUG
#define	CFG_QUEUE_DEBUG_EN				(1)
#define	CFG_QUEUE_DEBUG_TSKMSG_EN   	(0)
#endif
#define CFG_GENERATE_RUN_TIME_STATS		(1)
//#define	ASSERT(x)
//#endif	//DEBUG

#define	CFG_PM_EN						(1)

//HCLK/FCLK setting
//#define CFG_CLK_HZ_96M			(96000000)
//#define CFG_CLK_HZ_80M			(80000000)
//#define CFG_CLK_HZ_72M			(72000000)
//#define CFG_CLK_HZ_48M			(48000000)
//#define CFG_CPU_CLK_HZ			(CFG_CLK_HZ_48M)

#define CFG_RAM_ONCHIP_SIZE			(64 * 1024)
#define CFG_ROM_ONCHIP_SIZE			(192 * 1024)
#define CFG_OTP_ONCHIP_SIZE			(32 * 1024)
//#define CFG_SDRAM_OFFCHIP_MAXSZ	(16 * 1024 * 1024)
//#define CFG_FLASH_OFFCHIP_MAXSZ	(CFG_SDRAM_OFFCHIP_MAXSZ)
#define CFG_MINI_HEAP_SIZE			(5 * 1024)//minimum system memory requirement,cannot less than

#define	CFG_MAX_TICKS_DELAY			(~0UL)
#define	CFG_TICK_RATE				(1000 / (HZ))

// NVIC priority configuration
#define CFG_PRIO_BITS				(__NVIC_PRIO_BITS)	//preempt 2bit,subpriority 1bit
#define CFG_KERNEL_IRQMIN_LEVEL		(((1 << CFG_PRIO_BITS) - 1) << (8 - CFG_PRIO_BITS))
#define CFG_SYSCALL_IRQMAX_PRIO		(1 << (8 - CFG_PRIO_BITS))
#define	CFG_SYSTICK_PRIO_LEVEL		((1 << CFG_PRIO_BITS) - 1)

//timer configuration
#define CFG_USE_TIMERS					(1)
#define CFG_TIMER_QUEUE_LENGTH			(16)
#define	CFG_TIMER_TASK_STACK_DEPTH		(1024)
#define	CFG_TIMER_TASK_PRIORITY			(CFG_MIN_PRIORITIES + 1)

//queue configuration
//#define CFG_QUEUE_TMOCHK_PERIOD			(1000 / HZ * 4)/* (1s) must less or equal to 255 */
#define CFG_QUEUE_TIMEOUT_MAX			(10)/* (1s) must less or equal to 255 */

//task resource configuration
#define	CFG_MAX_TASKS_NUM				(16)
#define CFG_TASK_STACK_SIZE				(640)
#define CFG_SYS_STACK_SIZE				(1024)
#define CFG_INITTASK_STACK_SIZE			(640)
#define CFG_MAX_TASK_NAME_LEN			(12)
#define CFG_STACK_GROWTH				(-1)
#define	CFG_BYTE_ALIGN					(4)
#define	CFG_BYTE_ALIGN_MASK				((CFG_BYTE_ALIGN) - 1)
#define	MAX_SCHEDULE_TIMEOUT			(CFG_MAX_TICKS_DELAY)
#define CFG_INITTASK_PRIORITY			(1)


// task module switch configuration,0 for off,1 for on
#define CFG_TASK_PRIORITY_SET				(1)
#define CFG_TASK_PRIORITY_GET				(1)
#define CFG_TASK_DELETE						(1)
#define CFG_TASK_RESRC_CLEANUP				(1)
#define CFG_TASK_SUSPEND					(1)
#define CFG_TASK_SLEEP_UNTIL				(1)
#define CFG_TASK_DELAY						(1)
#define CFG_GET_STACK_HI_WATERMARK			(1)
#define CFG_CRITICAL_NESTING_IN_TCB			(0)
#define	CFG_SCHEDULER_STATE					(1)
#define	CFG_GET_CURRENT_TASK_HANDLE			(0)
#define	CFG_PRIVILEGE_BIT					(0)
#define	CFG_USE_MPU_WRAPPERS				(0)

//kernel task  configuration
#define CFG_USE_PREEMPTION		(1)
#define CFG_USE_IDLE_HOOK		(0)
#define CFG_MAX_PRIORITIES	    (5)
#define CFG_MIN_PRIORITIES	    (0)
#define CFG_USE_TICK_HOOK		(0)
#define CFG_USE_TRACE_FACILITY	(1)
#define CFG_IDLE_SHOULD_YIELD	(1)
#define CFG_USE_MUTEXES			(1)

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif /* FREERTOSCFG_H */
