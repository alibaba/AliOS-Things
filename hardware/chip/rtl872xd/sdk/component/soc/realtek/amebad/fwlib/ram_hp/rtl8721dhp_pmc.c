/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "ameba_soc.h"

typedef struct {
	u32 CTRL;
	u32 MAIR0;
	u32 MAIR1;
	u32 RBAR[MPU_MAX_REGION];
	u32 RLAR[MPU_MAX_REGION];
} MPU_BackUp_TypeDef;

typedef struct {
	u32 CPURegbackup_HP[25];
	u32 CPUPSP_HP;
	u32 PortSVC_Backup_HP;
	u32 NVICbackup_HP[6];
	u32 SYSTICKbackup_HP[4];
	u32 SCBbackup_HP[4];
	u32 IPCbackup_HP;
	u8 NVICIPbackup_HP[MAX_PERIPHERAL_IRQ_NUM];
	MPU_BackUp_TypeDef MPU_BK;
} CPU_BackUp_TypeDef;

CPU_BackUp_TypeDef PMC_BK;
u32 WakeEventFlag_HP = _FALSE;
extern KM4SLEEP_ParamDef sleep_param;

VOID SOCPS_NVICBackup_HP(void)
{
	int i=0;
	
	PMC_BK.SYSTICKbackup_HP[0] = SysTick->CTRL;
	PMC_BK.SYSTICKbackup_HP[1] = SysTick->LOAD;
	PMC_BK.SYSTICKbackup_HP[2] = SysTick->VAL;

	PMC_BK.NVICbackup_HP[0] = NVIC->ISER[0];
	PMC_BK.NVICbackup_HP[1] = NVIC->ISER[1];
	PMC_BK.NVICbackup_HP[2] = NVIC->ICER[0];
	PMC_BK.NVICbackup_HP[3] = NVIC->ICER[1];

	for(i = 0; i < MAX_PERIPHERAL_IRQ_NUM; i++) {
		PMC_BK.NVICIPbackup_HP[i] = NVIC->IPR[i];
	}

	PMC_BK.NVICbackup_HP[4] = NVIC->ISPR[0];
	PMC_BK.NVICbackup_HP[5] = NVIC->ISPR[1];

#if defined(__VTOR_PRESENT) && (__VTOR_PRESENT == 1)
	PMC_BK.SCBbackup_HP[0] = SCB->VTOR;
#endif
}

VOID SOCPS_NVICReFill_HP(VOID)
{
	int i=0;
	
	/* Configure SysTick to interrupt at the requested rate. */
	SysTick->CTRL = PMC_BK.SYSTICKbackup_HP[0];
	SysTick->LOAD = PMC_BK.SYSTICKbackup_HP[1];
	SysTick->VAL = PMC_BK.SYSTICKbackup_HP[2];

	NVIC->ISPR[0] = PMC_BK.NVICbackup_HP[4];
	NVIC->ISPR[1] = PMC_BK.NVICbackup_HP[5];
	NVIC->ICER[0] = PMC_BK.NVICbackup_HP[2];
	NVIC->ICER[1] = PMC_BK.NVICbackup_HP[3];

	for(i = 0; i < MAX_PERIPHERAL_IRQ_NUM; i++) {
		NVIC->IPR[i] = PMC_BK.NVICIPbackup_HP[i];
	}

	NVIC->ISER[0] = PMC_BK.NVICbackup_HP[0];
	NVIC->ISER[1] = PMC_BK.NVICbackup_HP[1];

#if defined(__VTOR_PRESENT) && (__VTOR_PRESENT == 1)
	SCB->VTOR = PMC_BK.SCBbackup_HP[0];
#endif
}


VOID SOCPS_MPUBackup_HP(void)
{
	int i=0;
	PMC_BK.MPU_BK.CTRL = MPU->CTRL;

	PMC_BK.MPU_BK.MAIR0 = MPU->MAIR0;
	PMC_BK.MPU_BK.MAIR1 = MPU->MAIR1;
	for(i = 0;i < MPU_MAX_REGION; i++){
		MPU->RNR = i;
		PMC_BK.MPU_BK.RBAR[i] = MPU->RBAR;
		PMC_BK.MPU_BK.RLAR[i] = MPU->RLAR;
	}
}

VOID SOCPS_MPUReFill_HP(VOID)
{
	int i=0;
	MPU->CTRL = 0;
	MPU->MAIR0 = PMC_BK.MPU_BK.MAIR0;
	MPU->MAIR1 = PMC_BK.MPU_BK.MAIR1;
	for(i = 0;i < MPU_MAX_REGION; i++){
		MPU->RNR = i;
		MPU->RBAR = PMC_BK.MPU_BK.RBAR[i];
		MPU->RLAR = PMC_BK.MPU_BK.RLAR[i];
	}
	MPU->CTRL = PMC_BK.MPU_BK.CTRL;
}

/**
  *  @brief clear work modules/wake up event after resume.
  *  @retval None
  */
void SOCPS_SleepDeInit_HP(void)
{
	/* clear wake event IMR */
	SOCPS_SetWakeEvent_HP(0xFFFFFFFF, DISABLE);

	/* clear wake event ISR */
	SOCPS_ClearWakeEvent_HP();

	/* Clear ARM Cortex-M4 IRQ */
	NVIC_ClearPendingIRQ(SYSTEM_ON_IRQ);
}


int SOCPS_WakeEvent(void)
{
	return HAL_READ32(SYSTEM_CTRL_BASE, REG_HS_WAKE_EVENT_STATUS1);	
}

VOID SOCPS_SYSIrq_HP(VOID *Data)
{
	/* To avoid gcc warnings */
	( void ) Data;
	
	u32 Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_HS_WAKE_EVENT_STATUS0);

	SOCPS_SleepDeInit_HP();

	//if (tickless_debug) {
		DBG_8195A("SOCPS_SYSIrq_HP %x\n", Rtemp);
		if (WakeEventFlag_HP == _FALSE) {
			DBG_8195A("Oops: SOCPS_SYSIrq_HP Error %x !!!!!\n", Rtemp);
		}
	//}
}

_OPTIMIZE_NONE_
VOID SOCPS_SleepPG(VOID)
{
	u32 nDeviceIdOffset = 0;

	//InterruptDis(UART_LOG_IRQ);

	/* exec sleep hook functions */
	nDeviceIdOffset = pmu_exec_sleep_hook_funs();
	if (nDeviceIdOffset != PMU_MAX) {
		pmu_exec_wakeup_hook_funs(nDeviceIdOffset);
		DBG_8195A("DBG: Sleep blocked because Dev %x  busy\n", nDeviceIdOffset);
		return;
	}

	/*Backup KM4 IPC configuration*/
	PMC_BK.IPCbackup_HP = IPC_IERGet(IPCM4_DEV);
	
	/* backup registgers */
	SOCPS_NVICBackup_HP();
	SOCPS_MPUBackup_HP();
	
	//asm volatile("SLPPG_WAKEUP_POINT:\n");
	asm volatile("MOV %0, r0\n"	:"=r"(PMC_BK.CPURegbackup_HP[0])::"memory");
	asm volatile("MOV %0, r1\n"	:"=r"(PMC_BK.CPURegbackup_HP[1])::"memory");
	asm volatile("MOV %0, r2\n"	:"=r"(PMC_BK.CPURegbackup_HP[2])::"memory");
	asm volatile("MOV %0, r3\n"	:"=r"(PMC_BK.CPURegbackup_HP[3])::"memory");
	asm volatile("MOV %0, r4\n"	:"=r"(PMC_BK.CPURegbackup_HP[4])::"memory");
	asm volatile("MOV %0, r5\n"	:"=r"(PMC_BK.CPURegbackup_HP[5])::"memory");
	asm volatile("MOV %0, r6\n"	:"=r"(PMC_BK.CPURegbackup_HP[6])::"memory");
	asm volatile("MOV %0, r7\n"	:"=r"(PMC_BK.CPURegbackup_HP[7])::"memory");
	asm volatile("MOV %0, r8\n"	:"=r"(PMC_BK.CPURegbackup_HP[8])::"memory");
	asm volatile("MOV %0, r9\n"	:"=r"(PMC_BK.CPURegbackup_HP[9])::"memory");
	asm volatile("MOV %0, r10\n":"=r"(PMC_BK.CPURegbackup_HP[10])::"memory");
	asm volatile("MOV %0, r11\n":"=r"(PMC_BK.CPURegbackup_HP[11])::"memory");
	asm volatile("MOV %0, r12\n":"=r"(PMC_BK.CPURegbackup_HP[12])::"memory");
	asm volatile("MOV %0, r13\n":"=r"(PMC_BK.CPURegbackup_HP[13])::"memory");//PSP
	asm volatile("MOV %0, r14\n" :"=r"(PMC_BK.CPURegbackup_HP[14])::"memory");
	asm volatile("MOV %0, pc\n" :"=r"(PMC_BK.CPURegbackup_HP[15])::"memory");
	asm volatile("MRS %0, PSR\n":"=r"(PMC_BK.CPURegbackup_HP[16])	::"memory");
	asm volatile("NOP");
	asm volatile("NOP");

	PMC_BK.CPURegbackup_HP[14] = PMC_BK.CPURegbackup_HP[15];
	PMC_BK.CPUPSP_HP = PMC_BK.CPURegbackup_HP[13];

	/* Enable low power mode:  */
	if (WakeEventFlag_HP != _TRUE){		
		WakeEventFlag_HP = _TRUE;
//		DBG_8195A("SOCPS_SleepPG sleep \n");

		InterruptDis(SYSTEM_ON_IRQ);
		ipc_send_message(IPC_INT_KM4_TICKLESS_INDICATION, (u32)&sleep_param);
		
		//asm volatile ("cpsid i" : : : "memory");
		//asm volatile ("cpsid f" : : : "memory");
		
		asm volatile ("wfe");
		asm volatile ("wfe");

		//asm volatile ("cpsie i" : : : "memory");
		//asm volatile ("cpsie f" : : : "memory");
	} else {
//		DBG_8195A("SOCPS_SleepPG wakeup \n");

		NewVectorTable[11] = (HAL_VECTOR_FUN)PMC_BK.PortSVC_Backup_HP;

		/* ReFill registers */
		InterruptEn(SYSTEM_ON_IRQ, 0);

//		DBG_8195A("IPCbackup_HP = %x \n", PMC_BK.IPCbackup_HP);
		
		/*Refill KM4 IPC configuration*/
		IPC_IERSet(IPCM4_DEV, PMC_BK.IPCbackup_HP);
		
		SOCPS_NVICReFill_HP();
		SOCPS_MPUReFill_HP();
		rtl_cryptoEngine_init();

//		DBG_8195A("SOCPS_SleepPG exit \n");
	
	}

	pmu_exec_wakeup_hook_funs(PMU_MAX);
	WakeEventFlag_HP = _FALSE;
}

void SOCPS_vWFSSVCHandler_HP(void)
{
	asm volatile ("mov r0, %0\n"::"r"(PMC_BK.CPUPSP_HP):"r0");
	asm volatile 
	(
		"ldmia r0!, {r4-r7}				\n" /* Pop the registers that are not automatically saved on exception entry and the critical nesting count. */
		"ldmia r0!, {r8-r11}				\n"
		"msr psp, r0						\n" /* Restore the task stack pointer. */
		"orr r14, r14, #0xc					      \n"
		"bx r14							\n"
	);
}

IMAGE2_RAM_TEXT_SECTION
VOID SOCPS_WakeFromPG_HP(VOID)
{
	u32 Rtemp;

	//DBG_8195A("SOCPS_WakeFromPG_HP\n");
	
	/* Need Modify */
	irq_table_init(MSP_RAM_HP);
	
	/* we should Cache_Flush when we wake */
	Cache_Enable(DISABLE);
	Cache_Enable(ENABLE);

	/* KM0 will set after flash clock switch ok */
	//asm volatile ("wfe");
	
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_BOOT_CFG);
	Rtemp &= (~BIT_SOC_BOOT_WAKE_FROM_PS_HS);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_BOOT_CFG, Rtemp);
	
	/* Make PendSV, CallSV and SysTick the same priroity as the kernel. */
	/* uint8_t  SHP[12]: Offset: 0x018 (R/W)  System Handlers Priority Registers (4-7, 8-11, 12-15) */
	/* SCB->SHP[8~11] */
	HAL_WRITE32(0, 0xE000ED20, 0xF0F00000);
	PMC_BK.PortSVC_Backup_HP = (u32)NewVectorTable[11];
	NewVectorTable[11] = (HAL_VECTOR_FUN)SOCPS_vWFSSVCHandler_HP;

	/* push cpu register into stack based on SOCPS_vWFSSVCHandler */
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-4) ) )= (PMC_BK.CPURegbackup_HP[16] | 0x1000000); //PSR
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-8) ) )= PMC_BK.CPURegbackup_HP[15]; //PC
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-12) ) )= PMC_BK.CPURegbackup_HP[14]; //LR
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-16) ) )= PMC_BK.CPURegbackup_HP[12]; //R12
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-20) ) )= PMC_BK.CPURegbackup_HP[3];  //R3
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-24) ) )= PMC_BK.CPURegbackup_HP[2];  //R2
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-28) ) )= PMC_BK.CPURegbackup_HP[1];  //R1
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-32) ) )= PMC_BK.CPURegbackup_HP[0];  //R0
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-36) ) )= PMC_BK.CPURegbackup_HP[11]; //R11
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-40) ) )= PMC_BK.CPURegbackup_HP[10]; //R10
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-44) ) )= PMC_BK.CPURegbackup_HP[9];  //R9
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-48) ) )= PMC_BK.CPURegbackup_HP[8];  //R8
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-52) ) )= PMC_BK.CPURegbackup_HP[7];  //R7
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-56) ) )= PMC_BK.CPURegbackup_HP[6];  //R6
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-60) ) )= PMC_BK.CPURegbackup_HP[5];  //R5
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-64) ) )= PMC_BK.CPURegbackup_HP[4];  //R4
	PMC_BK.CPUPSP_HP = PMC_BK.CPUPSP_HP-64;  //PSP
	
	asm volatile(
		" cpsie i				\n" /* Globally enable interrupts. */
		" svc 0					\n" /* System call to start first task. */
		" nop					\n"
		);
}


IMAGE2_RAM_TEXT_SECTION
VOID SOCPS_WakeFromReset(VOID)
{
	u32 Rtemp;

	//DBG_8195A("SOCPS_WakeFromPG_HP\n");
	
	/* Need Modify */
	irq_table_init(MSP_RAM_HP);
	
	/* we should Cache_Flush when we wake */
	Cache_Enable(DISABLE);
	Cache_Enable(ENABLE);

	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_BOOT_CFG);
	Rtemp &= (~BIT_SOC_BOOT_WAKE_FROM_PS_HS);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_BOOT_CFG, Rtemp);
	
	/* Make PendSV, CallSV and SysTick the same priroity as the kernel. */
	/* uint8_t  SHP[12]: Offset: 0x018 (R/W)  System Handlers Priority Registers (4-7, 8-11, 12-15) */
	/* SCB->SHP[8~11] */
	HAL_WRITE32(0, 0xE000ED20, 0xF0F00000);
	PMC_BK.PortSVC_Backup_HP = (u32)NewVectorTable[11];
	NewVectorTable[11] = (HAL_VECTOR_FUN)SOCPS_vWFSSVCHandler_HP;

	/* push cpu register into stack based on SOCPS_vWFSSVCHandler */
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-4) ) )= (PMC_BK.CPURegbackup_HP[16] | 0x1000000); //PSR
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-8) ) )= PMC_BK.CPURegbackup_HP[15]; //PC
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-12) ) )= PMC_BK.CPURegbackup_HP[14]; //LR
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-16) ) )= PMC_BK.CPURegbackup_HP[12]; //R12
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-20) ) )= PMC_BK.CPURegbackup_HP[3];  //R3
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-24) ) )= PMC_BK.CPURegbackup_HP[2];  //R2
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-28) ) )= PMC_BK.CPURegbackup_HP[1];  //R1
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-32) ) )= PMC_BK.CPURegbackup_HP[0];  //R0
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-36) ) )= PMC_BK.CPURegbackup_HP[11]; //R11
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-40) ) )= PMC_BK.CPURegbackup_HP[10]; //R10
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-44) ) )= PMC_BK.CPURegbackup_HP[9];  //R9
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-48) ) )= PMC_BK.CPURegbackup_HP[8];  //R8
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-52) ) )= PMC_BK.CPURegbackup_HP[7];  //R7
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-56) ) )= PMC_BK.CPURegbackup_HP[6];  //R6
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-60) ) )= PMC_BK.CPURegbackup_HP[5];  //R5
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP_HP-64) ) )= PMC_BK.CPURegbackup_HP[4];  //R4
	PMC_BK.CPUPSP_HP = PMC_BK.CPUPSP_HP-64;  //PSP
	
	asm volatile(
		" cpsie i				\n" /* Globally enable interrupts. */
		" svc 0					\n" /* System call to start first task. */
		" nop					\n"
		);
}




_OPTIMIZE_NONE_
VOID SOCPS_CPUReset(VOID)
{
	u32 nDeviceIdOffset = 0;

	InterruptDis(UART_LOG_IRQ);

	/* exec sleep hook functions */
	nDeviceIdOffset = pmu_exec_sleep_hook_funs();
	if (nDeviceIdOffset != PMU_MAX) {
		pmu_exec_wakeup_hook_funs(nDeviceIdOffset);
		DBG_8195A("Oops: Sleep Fail %x !!!!!\n", nDeviceIdOffset);
		return;
	}

	/*Backup KM4 IPC configuration*/
	PMC_BK.IPCbackup_HP = IPC_IERGet(IPCM4_DEV);
	
	/* backup registgers */
	SOCPS_NVICBackup_HP();
	SOCPS_MPUBackup_HP();
	
	//asm volatile("SLPPG_WAKEUP_POINT:\n");
	asm volatile("MOV %0, r0\n"	:"=r"(PMC_BK.CPURegbackup_HP[0])::"memory");
	asm volatile("MOV %0, r1\n"	:"=r"(PMC_BK.CPURegbackup_HP[1])::"memory");
	asm volatile("MOV %0, r2\n"	:"=r"(PMC_BK.CPURegbackup_HP[2])::"memory");
	asm volatile("MOV %0, r3\n"	:"=r"(PMC_BK.CPURegbackup_HP[3])::"memory");
	asm volatile("MOV %0, r4\n"	:"=r"(PMC_BK.CPURegbackup_HP[4])::"memory");
	asm volatile("MOV %0, r5\n"	:"=r"(PMC_BK.CPURegbackup_HP[5])::"memory");
	asm volatile("MOV %0, r6\n"	:"=r"(PMC_BK.CPURegbackup_HP[6])::"memory");
	asm volatile("MOV %0, r7\n"	:"=r"(PMC_BK.CPURegbackup_HP[7])::"memory");
	asm volatile("MOV %0, r8\n"	:"=r"(PMC_BK.CPURegbackup_HP[8])::"memory");
	asm volatile("MOV %0, r9\n"	:"=r"(PMC_BK.CPURegbackup_HP[9])::"memory");
	asm volatile("MOV %0, r10\n":"=r"(PMC_BK.CPURegbackup_HP[10])::"memory");
	asm volatile("MOV %0, r11\n":"=r"(PMC_BK.CPURegbackup_HP[11])::"memory");
	asm volatile("MOV %0, r12\n":"=r"(PMC_BK.CPURegbackup_HP[12])::"memory");
	asm volatile("MOV %0, r13\n":"=r"(PMC_BK.CPURegbackup_HP[13])::"memory");//PSP
	asm volatile("MOV %0, r14\n" :"=r"(PMC_BK.CPURegbackup_HP[14])::"memory");
	asm volatile("MOV %0, pc\n" :"=r"(PMC_BK.CPURegbackup_HP[15])::"memory");
	asm volatile("MRS %0, PSR\n":"=r"(PMC_BK.CPURegbackup_HP[16])	::"memory");
	asm volatile("NOP");
	asm volatile("NOP");

	PMC_BK.CPURegbackup_HP[14] = PMC_BK.CPURegbackup_HP[15];
	PMC_BK.CPUPSP_HP = PMC_BK.CPURegbackup_HP[13];

	/* Enable low power mode:  */
	if (WakeEventFlag_HP != _TRUE){		
		WakeEventFlag_HP = _TRUE;
		DBG_8195A("SOCPS_CPUReset reset \n");

		Img2EntryFun0.RamWakeupFun = SOCPS_WakeFromReset;
		NVIC_SystemReset();
	} else {
		DBG_8195A("SOCPS_CPUReset wakeup \n");
		
		/*Refill KM4 IPC configuration*/
		IPC_IERSet(IPCM4_DEV, PMC_BK.IPCbackup_HP);		
		SOCPS_NVICReFill_HP();
		SOCPS_MPUReFill_HP();
		Img2EntryFun0.RamWakeupFun = SOCPS_WakeFromPG_HP;
	
		WakeEventFlag_HP = _FALSE;
	}
}

VOID SOCPS_InitSYSIRQ_HP(VOID)
{
	Img2EntryFun0.RamWakeupFun = SOCPS_WakeFromPG_HP;

	/* clear wake event IMR */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_HS_WAKE_EVENT_MSK0, 0);
	
	/* clear wake event ISR */
	SOCPS_ClearWakeEvent_HP();

	InterruptRegister((IRQ_FUN) SOCPS_SYSIrq_HP, SYSTEM_ON_IRQ, NULL, 0);
	InterruptEn(SYSTEM_ON_IRQ, 0);
}

void SOCPS_SleepCG(void)
{

	u32 nDeviceIdOffset = 0;

	/* exec sleep hook functions */
	nDeviceIdOffset = pmu_exec_sleep_hook_funs();
	if (nDeviceIdOffset != PMU_MAX) {
		pmu_exec_wakeup_hook_funs(nDeviceIdOffset);
		DBG_8195A("Oops: Sleep Fail %x !!!!!\n", nDeviceIdOffset);
		return;
	}

	ipc_send_message(IPC_INT_KM4_TICKLESS_INDICATION, (u32)&sleep_param);

	asm volatile ("wfe");
	asm volatile ("wfe");

		/* exec sleep hook functions */
	pmu_exec_wakeup_hook_funs(PMU_MAX);

	
}

/**
  *  @brief Get dslp wake status.
	* @param none:

	* @retval DslpWake status.
	* 		   @arg TRUE: boot from deep sleep 
	* 		   @arg FALSE: boot from power on
  */
u32 SOCPS_DsleepWakeStatusGet(void)
{
	return HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_DSLP_INFO_SW);
}

/**
  * @brief  set wake up event.
  * @param  Option:
  *   This parameter can be any combination of the following values:
  *		 @arg BIT_KEYSCAN_WAKE_MSK
  *		 @arg BIT_DLPS_TSF_WAKE_MSK
  *		 @arg BIT_RTC_WAKE_MSK
  *		 @arg BIT_AON_WAKE_TIM0_MSK
  *		 @arg BIT_GPIO_WAKE_MSK
  * @param  NewStatus: TRUE/FALSE.
  * @retval None
  */
IMAGE2_RAM_TEXT_SECTION
void SOCPS_SetWakeEventAON(u32 Option, u32 NewStatus)
{
	u32 WakeEvent = 0;
	
	/* Set Event */
	WakeEvent = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_WAKE_OPT_MSK);
	if (NewStatus == ENABLE) {
		WakeEvent |= Option;
	} else {
		WakeEvent &= ~Option;
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_WAKE_OPT_MSK, WakeEvent);
}

/**
  * @brief  get wakepin index.
  * @param  None
  * @retval BIT(0): wakepin0, BIT(1): wakepin1, BIT(2): wakepin2, BIT(3): wakepin3
  *     note:
	wakepin 0 :
		_PA_12,  PINMUX_S0 
		_PA_16,  PINMUX_S1
		_PA_20,  PINMUX_S2 
		
	 wakepin 1 :
		_PA_13,  PINMUX_S0 
		_PA_17, PINMUX_S1
		_PA_21, PINMUX_S2

	 wakepin 2 :
		_PA_14, PINMUX_S0
		_PA_18, PINMUX_S1
		_PA_25, PINMUX_S2

	 wakepin 3 :
		_PA_15, PINMUX_S0
		_PA_19, PINMUX_S1
		_PA_26  PINMUX_S2
  */
IMAGE2_RAM_TEXT_SECTION
int SOCPS_WakePinCheck(void)
{
	int WakePin = 0;
	
	WakePin = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_WAKE_GPIO_CTRL2);
	WakePin = (WakePin >> BIT_AON_SHIFT_GPIO_WAKE_EVENT) & BIT_AON_MASK_GPIO_WAKE_EVENT;

	return WakePin;
}

