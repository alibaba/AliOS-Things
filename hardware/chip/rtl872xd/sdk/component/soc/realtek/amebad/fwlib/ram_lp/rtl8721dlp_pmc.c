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
	u32 MMU_ENTRY0_CTRL;			/*!< flash MMU entry_x control */
	u32 MMU_ENTRY0_STRADDR;		/*!< flash MMU entry_x virtual address start */
	u32 MMU_ENTRY0_ENDADDR;		/*!< flash MMU entry_x virtual address end */
	u32 MMU_ENTRY0_OFFSET;
	u32 MMU_ENTRY1_CTRL;			/*!< flash MMU entry_x control */
	u32 MMU_ENTRY1_STRADDR;		/*!< flash MMU entry_x virtual address start */
	u32 MMU_ENTRY1_ENDADDR;		/*!< flash MMU entry_x virtual address end */
	u32 MMU_ENTRY1_OFFSET;
} MMU_Backup_TypeDef;

typedef struct {
	u32 CPURegbackup[25];
	u32 NVICbackup[6];
	u32 SYSTICKbackup[4];
	u32 SCBbackup[4];
	u32 CPUPSP;
	u32 IPCbackup;
	u32 PortSVC_Backup;
	u8 NVICIPbackup[MAX_PERIPHERAL_IRQ_NUM];
	MPU_BackUp_TypeDef MPU_BK;
	MMU_Backup_TypeDef MMU_BK;
} CPU_BackUp_TypeDef;

CPU_BackUp_TypeDef PMC_BK;
u32 WakeEventFlag = _FALSE;
u32 WakeEventAon = 0x00;
u32 WakeEvent = 0x00;

IMAGE2_RAM_TEXT_SECTION
void SOCPS_SWR_PFMForce(u32 NewStatus)
{
	u32 Temp = 0;

	return; //need more test
	
	Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_KM4_CTRL);
	if (Temp & BIT_LSYS_HSOCFUN_FEN){
		return;
	}

	if (NewStatus == ENABLE) {
		//DBG_8195A("PFM \n");
		Temp=HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SWR_PSW_CTRL);
		Temp &= 0x00FF0000;
		Temp |= 0x3F009999;//>0.81V is save for MP
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_SWR_PSW_CTRL,Temp);
			
		Temp=HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG1);
		Temp &= ~BIT_SWR_FPWM_L1;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG1,Temp);
	
		Temp = HAL_READ32(WIFI_REG_BASE, 0x00);//ReadMACRegDWord(0x00);
		Temp &= ~BIT4;
		HAL_WRITE32(WIFI_REG_BASE, 0x00,Temp);//WriteMACRegDWord(0x00, Temp);
	} else { //PWM mode
		//Temp = HAL_READ32(WIFI_REG_BASE, 0x00);//ReadMACRegDWord(0x00);
		//Temp |= BIT4;
		//HAL_WRITE32(WIFI_REG_BASE, 0x00,Temp);//WriteMACRegDWord(0x00, Temp);

		Temp=HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG1);
		Temp |= BIT_SWR_FPWM_L1;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG1,Temp);
		//DBG_8195A("PWM \n");

		Temp=HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SWR_PSW_CTRL);
		Temp &= 0x00FF0000;
		Temp |= 0x3F008754;//>0.81V is save for MP
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_SWR_PSW_CTRL,Temp);

	}
}

/**
  * @brief  set snooze mode disable or enable.
  * @param  bitmask:
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_XTAL_REQ_SNOOZE_MSK: 1: Enable xtal request event to trigger PMC enter snooze mode (for RTC SDM calibration) 
  *            @arg BIT_CAPTOUCH_SNOOZE_MSK: 1: Enable captouch wake event to trigger PMC enter snooze mode
  * @retval None
  */
IMAGE2_RAM_TEXT_SECTION
void SOCPS_SNOOZE_Config(u32 bitmask, u32 status)
{
	u32 temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_SNOOZE_MSK);	

	if (status == ENABLE) {
		temp |= bitmask;
	} else {
		temp &= ~bitmask;
	}

	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_SNOOZE_MSK, temp);
}

/**
  * @brief  set power option for power save mode.
  * @param  pwrmgt_option: power save mode option.
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_LSYS_PST_NORM_EXTL: Enable XTAL when PMC exit from sleep mode0
  *            @arg BIT_LSYS_PST_NORM_EPSW: LS power keeps unchange when PMC exit from sleep mode0
  *            @arg BIT_LSYS_PST_DSLP_EACK: Enable ANA4M when PMC enter into deep sleep mode0
  *            @arg BIT_LSYS_PST_SLEP_XACT: XTAL keeps at active mode when PMC enter into sleep mode0
  *            @arg BIT_LSYS_PST_SLEP_EXTL:  Enable XTAL when PMC enter into sleep mode0
  *            @arg BIT_LSYS_PST_SLEP_ESWR: Enable SWR when PMC enter into sleep mode0
  *            @arg BIT_LSYS_PST_SLEP_EPWM: SWR keeps at PWM mode when PMC enter into sleep mode0
  *            @arg BIT_LSYS_PST_SLEP_DPSW: Keep LS power unchange when PMC enter into sleep mode0
  *            @arg BIT_LSYS_PST_SLEP_ERCK: Enable LS SYSON register clock when PMC enter into sleep mode0
  *            @arg BIT_LSYS_PST_SLEP_LDLM: LPLDO keeps at active mode when PMC enter into sleep mode
  *            @arg BIT_LSYS_PST_SLEP_EMPM: emory power control setting don't change when PMC enter into sleep mode
  *            @arg BIT_LSYS_PST_SLEP_EBUS: Enable platform clock when PMC entro into sleep mode
  *            @arg BIT_LSYS_PST_SLEP_EACK: Enable ANA4M CLK when PMC enter into sleep mode
  *            @arg BIT_LSYS_PST_SLEP_ESOC: power enable SOC platform when PMC enter into sleep mode (clock-gating)
  * @retval None
  */
IMAGE2_RAM_TEXT_SECTION
void SOCPS_PWROption(u32 pwrmgt_option)
{
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LP_PWRMGT_OPTION, pwrmgt_option);
}

/**
  * @brief  get aon wake reason.
  * @param  None
  * @retval BIT(0): wakepin, BIT(1): aon_timer, BIT(2): RTC, BIT(3): tsf_timer, BIT(4): key-scan
  */
IMAGE2_RAM_TEXT_SECTION
int SOCPS_AONWakeReason(void)
{
	int reason = 0;
	
	reason = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_WAKE_OPT_STS);	

	return reason;
}

/**
  * @brief  clear aon wake event.
  * @param  BitMask: aon wakepin index
  *		This parameter can be one or combination of the following values:
  *		 @arg BIT_GPIO_WAKE_STS
  *		 @arg BIT_AON_WAKE_TIM0_STS
  *		 @arg BIT_RTC_WAKE_STS
  *		 @arg BIT_DLPS_TSF_WAKE_STS
  *		 @arg BIT_KEYSCAN_WAKE_STS
  *		 @arg BIT_CAPTOUCH_WAKE_STS
  * @retval none
  */
IMAGE2_RAM_TEXT_SECTION
void SOCPS_AONWakeClear(u32 BitMask)
{
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_WAKE_OPT_STS, BitMask);
}


/**
  * @brief  get wakepin index.
  * @param  None
  * @retval BIT(0): wakepin0, BIT(1): wakepin1, BIT(2): wakepin2, BIT(3): wakepin3
  */
IMAGE2_RAM_TEXT_SECTION
int SOCPS_WakePinCheck(void)
{
	int WakePin = 0;
	
	WakePin = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_WAKE_GPIO_CTRL2);
	WakePin = (WakePin >> BIT_AON_SHIFT_GPIO_WAKE_EVENT) & BIT_AON_MASK_GPIO_WAKE_EVENT;

	return WakePin;
}

/**
  * @brief  set AON timer for wakeup.
  * @param  SDuration: sleep time, unit is ms
  * @note wakeup state:sleep PG & CG &deep standby & deep sleep
  * @note This is 32.768KHz timer, max counter = 0x3FFFFFFF/32768/60 = 546min
  * @retval None
  */
IMAGE2_RAM_TEXT_SECTION
void SOCPS_AONTimer(u32 SDuration)
{
	u32 Rtemp = 0;

	if (SDuration == 0)
		return;

	/* use 32K */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_ISO_CTRL);
	Rtemp &= ~BIT_AUX_FUNC_CLK_SEL;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_ISO_CTRL, Rtemp);	

	//SDuration = ((((double)(SDuration)) * 32768) / 1000);
	//SDuration = SDuration*(32+0.5+0.25+1/64+1/512+ 1/2048)
	SDuration = (SDuration << 5) + (SDuration >> 1) + (SDuration >> 2) + \
	(SDuration >> 6) + (SDuration >> 9) + (SDuration >> 11); 
	
	SDuration &= BIT_AON_MASK_WAKE_TM0_TC;

	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_WAKE_TIMER0, SDuration);
}

/**
  * @brief  get current AON timer.
  * @param  None
  * @note wakeup state:sleep PG & CG &deep standby & deep sleep
  * @retval The current Aon Timer counter, the unit is ms.
  */
IMAGE2_RAM_TEXT_SECTION
u32 SOCPS_AONTimerGet(VOID)
{
	u32 Rtemp = 0;

	/* use 32K */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_WAKE_TIMER0_CNT) & BIT_AON_TIMER_COUNTER;
	//Rtemp = ((((double)(Rtemp)) * 1000) / 32768);
	//Rtemp =((Rtemp & 0xFFFF8000) / 32768) * 1000 + ((Rtemp & 0x7FFF) * 1000) /32768;
	Rtemp =((Rtemp >> 15) * 1000) + (((Rtemp & 0x7FFF) * 1000) >> 15);
	return Rtemp;
}

/**
  * @brief  set AON Timer Disable or Enable.
  * @retval None
  */
IMAGE2_RAM_TEXT_SECTION
void SOCPS_AONTimerCmd(u32 NewStatus)
{
	u32 Rtemp = 0;

	/* Disable or Enable */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_WAKE_TIMER0);
	if (NewStatus == DISABLE) {
		Rtemp &= ~BIT_AON_WAKE_TM0_EN;
	} else {
		Rtemp |= BIT_AON_WAKE_TM0_EN;
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_WAKE_TIMER0, Rtemp);

	/* Clear event */
	SOCPS_AONWakeClear(BIT_AON_WAKE_TIM0_STS);
}

/**
  * @brief  set REGU wakeup_pin (just on wakepin mux to 4 pads).
  * @param  EarlyBcnUs: unit is 32us clock (athough clock is 32.768, but we have ajust it)
  * @param  WakeIntvalUs: unit is 32us clock
  * @retval None
  */
IMAGE2_RAM_TEXT_SECTION
void SOCPS_AONTSF(u32 EarlyBcnUs, u32 WakeIntvalUs)
{
	static u32 enabled = 0;
	u32 Temp = 0;

	if (enabled == 0) {
		enabled = 1;
		RCC_PeriphClockCmd(APBPeriph_TSF, APBPeriph_TSF_CLOCK, ENABLE);
	}
	
	/* 1/32.768 = 30.517578us */
	Temp = WakeIntvalUs / 32;
	Temp &= BIT_MASK_DLPS_BCN_SPACE;
	//DBG_8195A("TSF1:%d \n", Temp);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_TSF_BCN_CFG1, Temp);

	/*
	1,bit[15:0] beaon early interval for dlps in unit of 32us
	2,bit[23:16] compasetion value of mac tsf sync to aon value
	3,bit[24] sync mac tsf to aon tsf
	4,bit[31] sync aon tsf to mac tsf
	*/
	Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_TSF_BCN_CFG2);
	//Temp |= BIT_SYNC_MAC_TSFT_EN;
	Temp &= ~BIT_DLPS_BCN_EARLY;
	Temp |= ((EarlyBcnUs / 32) & BIT_DLPS_BCN_EARLY);
	//DBG_8195A("TSF2:%d %x %d \n", EarlyBcnUs, Temp, Temp & BIT_DLPS_BCN_EARLY);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_TSF_BCN_CFG2, Temp);
}

IMAGE2_RAM_TEXT_SECTION
void SOCPS_AONTSF_Reset(void)
{
#if 0
	u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_TSF_CNT_LOW);
	
	/* clear cnt to 0, or write back it, if not, DSLP will wakeup in next TBTT time, this is not what we want */

	/* dont reset when low 32bit overflow soon */
	if (Temp >= 0xFFFFFFC0) {
		DelayUs(100);
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_TSF_CNT_LOW, 0);//HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_TSF_CNT_LOW));
#else
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_TSF_CNT_LOW, HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_TSF_CNT_LOW));
#endif
}

	
/**
  * @brief  set REGU wakeup_pin (just on wakepin mux to 4 pads).
  * @param  PinMask: aon wakepin index
  *		This parameter can be one of the following values:
  *		 @arg WAKUP_0 :
  *		 @arg WAKUP_1 :
  *		 @arg WAKUP_2 :
  *		 @arg WAKUP_3 :
  * @param  Polarity: aon wakepin Polarity
  *		This parameter can be one of the following values:
  *		 @arg 1 : high wakeup
  *		 @arg 0 : low wakeup
  * @note wakeup state: deep sleep
  * @retval None
  */
IMAGE2_RAM_TEXT_SECTION
void SOCPS_SetWakepin(u32 PinIdx,u32 Polarity)
{
	u32 Rtemp = 0;

	/* clear dsleep event, or SYS IRQ will happen when we open event mask becasue last wakeup event not clear */
	SOCPS_AONWakeClear(BIT_GPIO_WAKE_STS);

	/* disable wakepin */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_WAKE_GPIO_CTRL1);
	Rtemp &= (~BIT_AON_GPIO_WAKE_FEN);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_WAKE_GPIO_CTRL1, Rtemp);

	/* set polarity & internal PU/PD resistence */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_WAKE_GPIO_CTRL2);
	Rtemp &= (~(BIT(PinIdx) << BIT_AON_SHIFT_GPIO_WAKE_POLARITY)); /* clear polarity, set to low active */
	if(Polarity == 1) {
		Rtemp |= (BIT(PinIdx) << BIT_AON_SHIFT_GPIO_WAKE_POLARITY); /* if high active */
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_WAKE_GPIO_CTRL2, Rtemp);

	/* enable wakepin */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_WAKE_GPIO_CTRL1);
	Rtemp |= BIT_AON_GPIO_WAKE_FEN;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_WAKE_GPIO_CTRL1, Rtemp);
}

IMAGE2_RAM_TEXT_SECTION
VOID SOCPS_SYSIrq(VOID *Data)
{
	/* To avoid gcc warnings */
	( void ) Data;

	SOCPS_SleepDeInit();

	if (tickless_debug) {
		//DBG_8195A("SOCPS_SYSIrq %x Aon Wake_Event %x\n", WakeEvent, WakeEventAon);
		if (WakeEventFlag == _FALSE) {
			DBG_8195A("Oops: SOCPS_SYSIrq Error %x !!!!!\n", WakeEvent);
		}
	}
	WakeEventFlag = _FALSE;
}

void SOCPS_vWFSSVCHandler(void)
{
	asm volatile ("mov r0, %0\n"::"r"(PMC_BK.CPUPSP):"r0");
	asm volatile 
	(
		"ldmia r0!, {r4-r7}				\n" /* Pop the high registers. */
		"mov r8, r4						\n"
		"mov r9, r5						\n"
		"mov r10, r6					\n"
		"mov r11, r7					\n"
		"ldmia r0!, {r4-r7}				\n" /* Pop low registers.  */
		"msr psp, r0					\n" /* Remember the new top of stack for the task. */
		"mov r1, r14					\n" /* OR R14 with 0x0d. */
		"movs r0, #0x0d					\n"
#if defined ( __ICCARM__ )
		"orrs r1, r1, r0						\n"
#else
		"orr r1, r0						\n"
#endif
		"bx r1							\n"
	);
}

VOID SOCPS_NVICBackup(void)
{
	int i=0;
	
	PMC_BK.SYSTICKbackup[0] = SysTick->CTRL;
	PMC_BK.SYSTICKbackup[1] = SysTick->LOAD;
	PMC_BK.SYSTICKbackup[2] = SysTick->VAL;

	PMC_BK.NVICbackup[0] = NVIC->ISER[0];
	PMC_BK.NVICbackup[1] = NVIC->ISER[1];
	PMC_BK.NVICbackup[2] = NVIC->ICER[0];
	PMC_BK.NVICbackup[3] = NVIC->ICER[1];

	for(i = 0; i < MAX_PERIPHERAL_IRQ_NUM; i++) {
		PMC_BK.NVICIPbackup[i] = NVIC->IPR[i];
	}

	PMC_BK.NVICbackup[4] = NVIC->ISPR[0];
	PMC_BK.NVICbackup[5] = NVIC->ISPR[1];

#if (__VTOR_PRESENT == 1)
	PMC_BK.SCBbackup[0] = SCB->VTOR;
#endif
}

VOID SOCPS_NVICReFill(VOID)
{
	int i=0;
	
	/* Configure SysTick to interrupt at the requested rate. */
	SysTick->CTRL = PMC_BK.SYSTICKbackup[0];
	SysTick->LOAD = PMC_BK.SYSTICKbackup[1];
	SysTick->VAL = PMC_BK.SYSTICKbackup[2];

	NVIC->ISPR[0] = PMC_BK.NVICbackup[4];
	NVIC->ISPR[1] = PMC_BK.NVICbackup[5];
	NVIC->ICER[0] = PMC_BK.NVICbackup[2];
	NVIC->ICER[1] = PMC_BK.NVICbackup[3];

	for(i = 0; i < MAX_PERIPHERAL_IRQ_NUM; i++) {
		NVIC->IPR[i] = PMC_BK.NVICIPbackup[i];
	}

	NVIC->ISER[0] = PMC_BK.NVICbackup[0];
	NVIC->ISER[1] = PMC_BK.NVICbackup[1];

#if (__VTOR_PRESENT == 1)
	SCB->VTOR = PMC_BK.SCBbackup[0];
#endif
}

VOID SOCPS_MPUBackup(void)
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

VOID SOCPS_MPUReFill(VOID)
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

IMAGE2_RAM_TEXT_SECTION
VOID SOCPS_MMUBackup(void)
{
	RSIP_REG_TypeDef* RSIP = ((RSIP_REG_TypeDef *) RSIP_REG_BASE);
	PMC_BK.MMU_BK.MMU_ENTRY0_CTRL = RSIP->FLASH_MMU[0].MMU_ENTRYx_CTRL;
	PMC_BK.MMU_BK.MMU_ENTRY0_ENDADDR = RSIP->FLASH_MMU[0].MMU_ENTRYx_ENDADDR;
	PMC_BK.MMU_BK.MMU_ENTRY0_OFFSET = RSIP->FLASH_MMU[0].MMU_ENTRYx_OFFSET;
	PMC_BK.MMU_BK.MMU_ENTRY0_STRADDR = RSIP->FLASH_MMU[0].MMU_ENTRYx_STRADDR;

	PMC_BK.MMU_BK.MMU_ENTRY1_CTRL = RSIP->FLASH_MMU[1].MMU_ENTRYx_CTRL;
	PMC_BK.MMU_BK.MMU_ENTRY1_ENDADDR = RSIP->FLASH_MMU[1].MMU_ENTRYx_ENDADDR;
	PMC_BK.MMU_BK.MMU_ENTRY1_OFFSET = RSIP->FLASH_MMU[1].MMU_ENTRYx_OFFSET;
	PMC_BK.MMU_BK.MMU_ENTRY1_STRADDR = RSIP->FLASH_MMU[1].MMU_ENTRYx_STRADDR;	
}

IMAGE2_RAM_TEXT_SECTION
VOID SOCPS_MMUReFill(VOID)
{
	RSIP_REG_TypeDef* RSIP = ((RSIP_REG_TypeDef *) RSIP_REG_BASE);
	RSIP->FLASH_MMU[0].MMU_ENTRYx_CTRL = PMC_BK.MMU_BK.MMU_ENTRY0_CTRL;
	RSIP->FLASH_MMU[0].MMU_ENTRYx_ENDADDR = PMC_BK.MMU_BK.MMU_ENTRY0_ENDADDR;
	RSIP->FLASH_MMU[0].MMU_ENTRYx_OFFSET = PMC_BK.MMU_BK.MMU_ENTRY0_OFFSET;
	RSIP->FLASH_MMU[0].MMU_ENTRYx_STRADDR = PMC_BK.MMU_BK.MMU_ENTRY0_STRADDR;

	RSIP->FLASH_MMU[1].MMU_ENTRYx_CTRL = PMC_BK.MMU_BK.MMU_ENTRY1_CTRL;
	RSIP->FLASH_MMU[1].MMU_ENTRYx_ENDADDR = PMC_BK.MMU_BK.MMU_ENTRY1_ENDADDR;
	RSIP->FLASH_MMU[1].MMU_ENTRYx_OFFSET = PMC_BK.MMU_BK.MMU_ENTRY1_OFFSET;
	RSIP->FLASH_MMU[1].MMU_ENTRYx_STRADDR = PMC_BK.MMU_BK.MMU_ENTRY1_STRADDR;
}

IMAGE2_RAM_TEXT_SECTION
VOID SOCPS_WakeFromPG(VOID)
{
	/*MMU registers must be refilled first.*/
	SOCPS_MMUReFill();

	km4_flash_highspeed_resume(FALSE);

	if (ps_config.km0_tickles_debug) {
		DBG_8195A("SOCPS_WakeFromPG \n");
	}

	/* Make PendSV, CallSV and SysTick the same priroity as the kernel. */
	/* uint8_t  SHP[12]: Offset: 0x018 (R/W)  System Handlers Priority Registers (4-7, 8-11, 12-15) */
	/* SCB->SHP[8~11] */
	HAL_WRITE32(0, 0xE000ED20, 0xF0F00000);

	PMC_BK.PortSVC_Backup = (u32)NewVectorTable[11];
	NewVectorTable[11] = (HAL_VECTOR_FUN)SOCPS_vWFSSVCHandler;

	/* push cpu register into stack based on SOCPS_vWFSSVCHandler */
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP-4) ) )= (PMC_BK.CPURegbackup[16] | 0x1000000); //PSR
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP-8) ) )= PMC_BK.CPURegbackup[15]; //PC
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP-12) ) )= PMC_BK.CPURegbackup[14]; //LR
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP-16) ) )= PMC_BK.CPURegbackup[12]; //R12
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP-20) ) )= PMC_BK.CPURegbackup[3];  //R3
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP-24) ) )= PMC_BK.CPURegbackup[2];  //R2
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP-28) ) )= PMC_BK.CPURegbackup[1];  //R1
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP-32) ) )= PMC_BK.CPURegbackup[0];  //R0
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP-36) ) )= PMC_BK.CPURegbackup[7]; //R11
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP-40) ) )= PMC_BK.CPURegbackup[6]; //R10
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP-44) ) )= PMC_BK.CPURegbackup[5];  //R9
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP-48) ) )= PMC_BK.CPURegbackup[4];  //R8
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP-52) ) )= PMC_BK.CPURegbackup[11];  //R7
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP-56) ) )= PMC_BK.CPURegbackup[10];  //R6
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP-60) ) )= PMC_BK.CPURegbackup[9];  //R5
	( * ( ( volatile unsigned long * ) (PMC_BK.CPUPSP-64) ) )= PMC_BK.CPURegbackup[8];  //R4
	PMC_BK.CPUPSP = PMC_BK.CPUPSP-64;  //PSP

	asm volatile(
		" cpsie i				\n" /* Globally enable interrupts. */
		" svc 0					\n" /* System call to start first task. */
		" nop					\n"
		);
}

IMAGE2_RAM_TEXT_SECTION
VOID SOCPS_TimingTest(VOID)
{
	u32 WAKE2RFON = (ps_config.km0_rf_on_time - ps_config.km0_wake_time);
	u32 RFON2RFOFF = (ps_config.km0_rf_off_time - ps_config.km0_rf_on_time);
	u32 RFOFF2CG = (ps_config.km0_gating_time - ps_config.km0_rf_off_time);
	//u32 RFOFF2WIFI32K = (ps_config.km0_clk_down_time - ps_config.km0_rf_off_time);
	u32 WIFI32K2CG = (ps_config.km0_gating_time - ps_config.km0_clk_down_time);

	static u32 TIM_TEST_WAKE2RFON_AR[20];
	static u32 TIM_TEST_RFON2RFOFF_AR[20];
	static u32 TIM_TEST_RFOFF2CG_AR[20];
	static u32 TIM_TEST_WIFI32K2CG_AR[20];

	static u32 WAKE2RFON_SUM = 0;
	static u32 RFON2RFOFF_SUM = 0;
	static u32 RFOFF2CG_SUM = 0;
	static u32 WIFI32K2CG_SUM = 0;

	u32 WAKE2RFON_AVER;
	u32 RFON2RFOFF_AVER;
	u32 RFOFF2CG_AVER;
	u32 WIFI32K2CG_AVER;
	static u32 AR_Index = 0;

	if (AR_Index >= 20) {
		AR_Index = 0;
	}

	/* fast timer over flow */
	if ((RFOFF2CG < 10000) && (WIFI32K2CG < 10000) && (RFON2RFOFF < 10000) && (WAKE2RFON < 100000)) {
		WAKE2RFON_SUM -= TIM_TEST_WAKE2RFON_AR[AR_Index];
		RFON2RFOFF_SUM -= TIM_TEST_RFON2RFOFF_AR[AR_Index];
		RFOFF2CG_SUM -= TIM_TEST_RFOFF2CG_AR[AR_Index];
		WIFI32K2CG_SUM -= TIM_TEST_WIFI32K2CG_AR[AR_Index];

		TIM_TEST_WAKE2RFON_AR[AR_Index] = WAKE2RFON;
		TIM_TEST_RFON2RFOFF_AR[AR_Index] = RFON2RFOFF;
		TIM_TEST_RFOFF2CG_AR[AR_Index] = RFOFF2CG;
		TIM_TEST_WIFI32K2CG_AR[AR_Index] = WIFI32K2CG;

		WAKE2RFON_SUM += WAKE2RFON;
		RFON2RFOFF_SUM += RFON2RFOFF;
		RFOFF2CG_SUM += RFOFF2CG;
		WIFI32K2CG_SUM += WIFI32K2CG;		
		AR_Index++;
	}

	WAKE2RFON_AVER = WAKE2RFON_SUM/20;
	RFON2RFOFF_AVER = RFON2RFOFF_SUM/20;
	RFOFF2CG_AVER = RFOFF2CG_SUM/20;
	WIFI32K2CG_AVER = WIFI32K2CG_SUM/20;

	//993(1003) 5338(2878) 909(938) 546(548)->
	//922(919) 2047(2178) 879(867) 490(494) 
	DBG_8195A("%d(%d) ", WAKE2RFON, WAKE2RFON_AVER);
	DBG_8195A("%d(%d) ",RFON2RFOFF, RFON2RFOFF_AVER);
	DBG_8195A("%d(%d) ", RFOFF2CG, RFOFF2CG_AVER);
	//DBG_8195A("%d ", RFOFF2WIFI32K);
	DBG_8195A("%d(%d) ", WIFI32K2CG, WIFI32K2CG_AVER);
	//DBG_8195A("%d ",ps_config.km0_dur1*30);//WAKE->RFON
	DBG_8195A("\n");

}

IMAGE2_RAM_TEXT_SECTION
u32 SOCPS_KM4Wake(VOID)
{
	int i = 0;
	static u32 lp_km4wevent_config_val = 0;
	static u32 aon_km4wevent_config_val = 0;

	for (i = 0;;) {
		/*  Check if search to end */
		if (hs_wakeevent_config[i].Module == 0xFFFFFFFF) {
			break;
		}

		if (hs_wakeevent_config[i].Status == ON) {
			if(hs_wakeevent_config[i].Module == REG_LP_SLP_WAKE_EVENT_STATUS0) {
				lp_km4wevent_config_val |= hs_wakeevent_config[i].Event;
			}

			if(hs_wakeevent_config[i].Module == REG_AON_WAKE_OPT_STS) {
				aon_km4wevent_config_val |= hs_wakeevent_config[i].Event;
			}
		} else {
			if(hs_wakeevent_config[i].Module == REG_LP_SLP_WAKE_EVENT_STATUS0) {
				lp_km4wevent_config_val &= ~hs_wakeevent_config[i].Event;
			}

			if(hs_wakeevent_config[i].Module == REG_AON_WAKE_OPT_STS) {
				aon_km4wevent_config_val &= ~hs_wakeevent_config[i].Event;
			}
		}

		i++;
	}

	if((lp_km4wevent_config_val & (WakeEvent & HAL_READ32(SYSTEM_CTRL_BASE, REG_LP_SLP_WAKE_EVENT_MSK0))) | \
		(aon_km4wevent_config_val & (WakeEventAon & HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_WAKE_OPT_MSK)))) {
		return TRUE;
	} else {
		return FALSE;
	}
}

IMAGE2_RAM_TEXT_SECTION
VOID SOCPS_SleepCG(VOID)
{
	u32 nDeviceIdOffset = 0;
	u32 Rtemp;

	if (ps_config.km0_tickles_debug) {
		DBG_8195A("KM0CG \n");
	}

	SOCPS_DsleepWakeStatusSet(FALSE);
	
	/* exec sleep hook functions */
	nDeviceIdOffset = pmu_exec_sleep_hook_funs();
	if (nDeviceIdOffset != PMU_MAX) {
		pmu_exec_wakeup_hook_funs(nDeviceIdOffset);
		DBG_8195A("Oops: Sleep Fail %x !!!!!\n", nDeviceIdOffset);
		SOCPS_SleepDeInit();
		return;
	}
	
	ps_config.km0_gating_time = RTIM_TestTimer_GetCount() + 300;
	//SOCPS_TimingTest();

	WakeEventFlag = _TRUE;

	//switch captouch reg/intr clock to 131K
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_ISO_CTRL);
	Rtemp &= ~BIT_AON_CTOUCH_CK_SEL;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_ISO_CTRL, Rtemp);

	//Enable low power mode
	SOCPS_SleepCG_RAM();

	//switch captouch reg/intr clock to apb clock
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_ISO_CTRL);
	Rtemp |= BIT_AON_CTOUCH_CK_SEL;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_ISO_CTRL, Rtemp);

	/*Get wake event*/
	WakeEvent = HAL_READ32(SYSTEM_CTRL_BASE, REG_LP_SLP_WAKE_EVENT_STATUS0);
	WakeEventAon = SOCPS_AONWakeReason();

	/* exec sleep hook functions */
	pmu_exec_wakeup_hook_funs(PMU_MAX);

	if (ps_config.km0_tickles_debug) {
		DBG_8195A("KM0CG- %x %x\n", WakeEventAon, WakeEvent);
	}

	/* open KM4 */
	if(SOCPS_KM4Wake() == TRUE) {
		km4_set_wake_event(BIT_HP_WEVT_MISC_STS);
	}
}

/* keep power functions: UART/I2C/RTC/GPIO/Gtimer/REGU/ANAtimer */
/* close power functions: GDMA */
_OPTIMIZE_NONE_
VOID SOCPS_SleepPG(VOID)
{
	u32 nDeviceIdOffset = 0;
	u32 Rtemp = 0;

	if (ps_config.km0_tickles_debug) {
		DBG_8195A("SOCPS_SleepPG \n");
	}

	SOCPS_DsleepWakeStatusSet(FALSE);
	
	/* exec sleep hook functions */
	nDeviceIdOffset = pmu_exec_sleep_hook_funs();
	if (nDeviceIdOffset != PMU_MAX) {
		pmu_exec_wakeup_hook_funs(nDeviceIdOffset);
		DBG_8195A("Oops: Sleep Fail %x !!!!!\n", nDeviceIdOffset);
		SOCPS_SleepDeInit();
		return;
	}

	/* enable boot from ps */
	Rtemp = HAL_READ32(PERI_ON_BASE, REG_SOC_FUNC_EN);
	Rtemp |= BIT_SOC_BOOT_WAKE_FROM_PS;
	HAL_WRITE32(PERI_ON_BASE, REG_SOC_FUNC_EN, Rtemp);

	WakeEventFlag = _FALSE;

	/* Clear event */
	//SOCPS_ClearWakeEvent();
	
	/*Backup KM0 IPC configuration*/
	PMC_BK.IPCbackup = IPC_IERGet(IPCM0_DEV);
	
	/* backup registgers */
	SOCPS_NVICBackup();
	SOCPS_MPUBackup();
	SOCPS_MMUBackup();

	LOGUART_WaitBusy();

	//asm volatile("SLPPG_WAKEUP_POINT:\n");
	asm volatile("MOV %0, r0\n"	:"=r"(PMC_BK.CPURegbackup[0])::"memory");
	asm volatile("MOV %0, r1\n"	:"=r"(PMC_BK.CPURegbackup[1])::"memory");
	asm volatile("MOV %0, r2\n"	:"=r"(PMC_BK.CPURegbackup[2])::"memory");
	asm volatile("MOV %0, r3\n"	:"=r"(PMC_BK.CPURegbackup[3])::"memory");
	asm volatile("MOV %0, r4\n"	:"=r"(PMC_BK.CPURegbackup[4])::"memory");
	asm volatile("MOV %0, r5\n"	:"=r"(PMC_BK.CPURegbackup[5])::"memory");
	asm volatile("MOV %0, r6\n"	:"=r"(PMC_BK.CPURegbackup[6])::"memory");
	asm volatile("MOV %0, r7\n"	:"=r"(PMC_BK.CPURegbackup[7])::"memory");
	asm volatile("MOV %0, r8\n"	:"=r"(PMC_BK.CPURegbackup[8])::"memory");
	asm volatile("MOV %0, r9\n"	:"=r"(PMC_BK.CPURegbackup[9])::"memory");
	asm volatile("MOV %0, r10\n":"=r"(PMC_BK.CPURegbackup[10])::"memory");
	asm volatile("MOV %0, r11\n":"=r"(PMC_BK.CPURegbackup[11])::"memory");
	asm volatile("MOV %0, r12\n":"=r"(PMC_BK.CPURegbackup[12])::"memory");
	asm volatile("MOV %0, r13\n":"=r"(PMC_BK.CPURegbackup[13])::"memory");//PSP
	asm volatile("MOV %0, r14\n" :"=r"(PMC_BK.CPURegbackup[14])::"memory");
	asm volatile("MOV %0, pc\n" :"=r"(PMC_BK.CPURegbackup[15])::"memory");
	asm volatile("MRS %0, PSR\n":"=r"(PMC_BK.CPURegbackup[16])	::"memory");
	asm volatile("NOP");
	asm volatile("NOP");

	PMC_BK.CPURegbackup[14] = PMC_BK.CPURegbackup[15];
	PMC_BK.CPUPSP = PMC_BK.CPURegbackup[13];

	/* Enable low power mode:  */
	if (WakeEventFlag != _TRUE){		
		WakeEventFlag = _TRUE;

		//switch captouch reg/intr clock to 131K
		Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_ISO_CTRL);
		Rtemp &= ~BIT_AON_CTOUCH_CK_SEL;
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_ISO_CTRL, Rtemp);

		SOCPS_SleepPG_RAM();
	}

	//switch captouch reg/intr clock to apb clock
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_ISO_CTRL);
	Rtemp |= BIT_AON_CTOUCH_CK_SEL;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_ISO_CTRL, Rtemp);

	//DBG_8195A("SOCPS_SleepPG wakeup \n");

	if (ps_config.km0_osc2m_close) {
		SOCPS_OSC2M_Cmd(ENABLE);
	}

	/* disable boot from ps */
	Rtemp = HAL_READ32(PERI_ON_BASE, REG_SOC_FUNC_EN);
	Rtemp &= ~BIT_SOC_BOOT_WAKE_FROM_PS;		
	HAL_WRITE32(PERI_ON_BASE, REG_SOC_FUNC_EN, Rtemp);

	NewVectorTable[11] = (HAL_VECTOR_FUN)PMC_BK.PortSVC_Backup;

	//DBG_8195A("IPCbackup = %x \n", PMC_BK.IPCbackup);

	/*Refill KM4 IPC configuration*/
	IPC_IERSet(IPCM0_DEV, PMC_BK.IPCbackup);

	/*Get wake event*/
	WakeEvent = HAL_READ32(SYSTEM_CTRL_BASE, REG_LP_SLP_WAKE_EVENT_STATUS0);
	WakeEventAon = SOCPS_AONWakeReason();

	/* ReFill registers */
	SOCPS_NVICReFill();	
	SOCPS_MPUReFill();

	pinmap_wake();

	/* exec sleep hook functions */
	pmu_exec_wakeup_hook_funs(PMU_MAX);

	/* open KM4 */
	if(SOCPS_KM4Wake() == TRUE) {
		km4_set_wake_event(BIT_HP_WEVT_MISC_STS);
	}
}

VOID SOCPS_InitSYSIRQ(VOID)
{
	Img2EntryFun0.RamWakeupFun = SOCPS_WakeFromPG;

	/* clear wake event IMR */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LP_SLP_WAKE_EVENT_MSK0, 0);
	
	/* clear wake event ISR */
	if (SOCPS_DsleepWakeStatusGet() == FALSE) {
		SOCPS_ClearWakeEvent();
	}

	/* open snooze mode */
	SOCPS_SNOOZE_Config((BIT_XTAL_REQ_SNOOZE_MSK | BIT_CAPTOUCH_SNOOZE_MSK), ENABLE);
	
	InterruptRegister((IRQ_FUN) SOCPS_SYSIrq, SYSTEM_ON_IRQ_LP, NULL, 0);
	InterruptEn(SYSTEM_ON_IRQ_LP, 0);
}

/**
  *  @brief clear work modules/wake up event after resume.
  *  @retval None
  */
IMAGE2_RAM_TEXT_SECTION
void SOCPS_SleepDeInit(void)
{
	/* disable regu timer */
	SOCPS_AONTimerCmd(DISABLE);

	/* clear wake event IMR */
	SOCPS_SetWakeEvent(0xFFFFFFFF, DISABLE);

	/* clear regu wakepin event*/
	SOCPS_ClearWakePin();

	/* clear wake event ISR */
	SOCPS_ClearWakeEvent();

	/* Clear ARM Cortex-M3 IRQ */
	NVIC_ClearPendingIRQ(SYSTEM_ON_IRQ_LP);
}

/**
  *  @brief set work modules/wake up event after clock gating.
  *  @retval None       
  */
void SOCPS_SleepInit(void)
{
	int i = 0;
	static u32 sleep_wevent_config_val;
	static u32 sleep_aon_wevent_config_val;
	static u32 sleep_hsram_config_val;
	static u32 sleep_lsram_config_val;
	static u32 dsleep_lsram_config_val;
	//static u32 sleep_wakepin_config_val;
	static u32 km0cg_pwrmgt_config_val;
	//static u32 dsleep_wevent_config_val;

	/* shrink process time */
	if (km0cg_pwrmgt_config_val != 0) {
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LP_PWRMGT_OPTION, km0cg_pwrmgt_config_val);
		SOCPS_SetWakeEvent(sleep_wevent_config_val, ENABLE);
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_WAKE_OPT_MSK, sleep_aon_wevent_config_val);

		goto end;
	}

	km0cg_pwrmgt_config_val = HAL_READ32(SYSTEM_CTRL_BASE, REG_LP_PWRMGT_OPTION);
	for (i = 0;;) {
		/*  Check if search to end */
		if (km0_pwrmgt_config[i].Module == 0xFFFFFFFF) {
			break;
		}

		if (km0_pwrmgt_config[i].Status == ON) {
			km0cg_pwrmgt_config_val |= km0_pwrmgt_config[i].Module;
		} else {
			km0cg_pwrmgt_config_val &= ~km0_pwrmgt_config[i].Module;
		}
		
		i++;
	}
	/* If dont switch SWR to LPLDO when enter sleep, then disable switch when wakeup */
	if (km0cg_pwrmgt_config_val & BIT_LSYS_PST_SLEP_DPSW) { /* 0 is enable */
		km0cg_pwrmgt_config_val &= ~BIT_LSYS_PST_NORM_EPSW; /* 1 is enable */
	} else {
		km0cg_pwrmgt_config_val |= BIT_LSYS_PST_NORM_EPSW; /* 1 is enable */
	}
	if (km0cg_pwrmgt_config_val & BIT_LSYS_PST_SLEP_EXTL) {
		km0cg_pwrmgt_config_val &= ~BIT_LSYS_PST_NORM_EXTL;
	} else {
		km0cg_pwrmgt_config_val |= BIT_LSYS_PST_NORM_EXTL;
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LP_PWRMGT_OPTION, km0cg_pwrmgt_config_val);

	sleep_wevent_config_val = 0;
	for (i = 0;;) {
		/*  Check if search to end */
		if (sleep_wevent_config[i].Module == 0xFFFFFFFF) {
			break;
		}

		if (sleep_wevent_config[i].Status == ON) {
			sleep_wevent_config_val |= sleep_wevent_config[i].Module;
		} else {
			sleep_wevent_config_val &= ~sleep_wevent_config[i].Module;
		}
		
		i++;
	}
	SOCPS_SetWakeEvent(sleep_wevent_config_val, ENABLE);

	/* set aon wake event */
	sleep_aon_wevent_config_val = 0;
	for (i = 0;;) {
		/*  Check if search to end */
		if (sleep_aon_wevent_config[i].Module == 0xFFFFFFFF) {
			break;
		}

		if (sleep_aon_wevent_config[i].Status == ON) {
			sleep_aon_wevent_config_val |= sleep_aon_wevent_config[i].Module;
		} else {
			sleep_aon_wevent_config_val &= ~sleep_aon_wevent_config[i].Module;
		}
		
		i++;
	}
	SOCPS_SetWakeEventAON(sleep_aon_wevent_config_val, ENABLE);

	/* hs sram option init */
	sleep_hsram_config_val = HAL_READ32(SYSTEM_CTRL_BASE_LP, (REG_HS_MEM_CTRL2 + LP_RW_HSYSON_OFFSET));
	for (i = 0;;) {
		/*  Check if search to end */
		if (sleep_hsram_config[i].Module == 0xFFFFFFFF) {
			break;
		}

		if (sleep_hsram_config[i].Status == ON) {
			sleep_hsram_config_val |= sleep_hsram_config[i].Module;
		} else {
			sleep_hsram_config_val &= ~sleep_hsram_config[i].Module;
		}
		
		i++;
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, (REG_HS_MEM_CTRL2 + LP_RW_HSYSON_OFFSET), sleep_hsram_config_val);

	/* ls sram option init */
	sleep_lsram_config_val = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_MEM_CTRL2);
	for (i = 0;;) {
		/*  Check if search to end */
		if (sleep_lsram_config[i].Module == 0xFFFFFFFF) {
			break;
		}

		if (sleep_lsram_config[i].Status == ON) {
			sleep_lsram_config_val |= sleep_lsram_config[i].Module;
		} else {
			sleep_lsram_config_val &= ~sleep_lsram_config[i].Module;
		}
		
		i++;
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_MEM_CTRL2, sleep_lsram_config_val);

	/* dslp retension sram option init */
	dsleep_lsram_config_val = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_MEM_PMCTRL_DSLP);
	for (i = 0;;) {
		/*  Check if search to end */
		if (dsleep_lsram_config[i].Module == 0xFFFFFFFF) {
			break;
		}

		if (dsleep_lsram_config[i].Status == ON) {
			dsleep_lsram_config_val |= dsleep_lsram_config[i].Module;
		} else {
			dsleep_lsram_config_val &= ~dsleep_lsram_config[i].Module;
		}
		
		i++;
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_MEM_PMCTRL_DSLP, dsleep_lsram_config_val);
	
	/* set aon wake pin */
	/* clear all wakeup pin first, and then enable by config */
	SOCPS_ClearWakePin();
	for (i = 0;;) {
		/*  Check if search to end */
		if (sleep_wakepin_config[i].Pinmux == 0xFFFFFFFF) {
			break;
		}

		if (sleep_wakepin_config[i].Status == ON) {
			u32 pinmux = sleep_wakepin_config[i].Pinmux;

			if(sleep_wakepin_config[i].Polarity) {
				PAD_PullCtrl(aon_wakepin[i][pinmux], GPIO_PuPd_DOWN);
			} else {
				PAD_PullCtrl(aon_wakepin[i][pinmux], GPIO_PuPd_UP);
			}

			Pinmux_Config(aon_wakepin[i][pinmux], PINMUX_FUNCTION_WAKEUP);
			SOCPS_SetWakepin(i, sleep_wakepin_config[i].Polarity);
		}
		
		i++;
	}
end:
	;
}

/**
  *  @brief set work modules/wake up event after dsleep.
  *  @retval None       
  */
void SOCPS_DsleepInit(void)
{
	int i = 0;
	u32 option = 0;

	/*if enable AON wake event, DLPS may can't stop at WFI. so we Disable AON wake event*/
	SOCPS_SetWakeEvent(BIT_LP_WEVT_AON_MSK, DISABLE);

	/* set aon wake event */
	option = 0;
	for (i = 0;;) {
		/*  Check if search to end */
		if (dsleep_aon_wevent_config[i].Module == 0xFFFFFFFF) {
			break;
		}

		if (dsleep_aon_wevent_config[i].Status == ON) {
			option |= dsleep_aon_wevent_config[i].Module;
		} else {
			option &= ~dsleep_aon_wevent_config[i].Module;
		}
		
		i++;
	}
	SOCPS_SetWakeEventAON(option, ENABLE);

	/* set aon wake pin */
	/* clear all wakeup pin first, and then enable by config */
	SOCPS_ClearWakePin();
	for (i = 0;;) {
		/*  Check if search to end */
		if (sleep_wakepin_config[i].Pinmux == 0xFFFFFFFF) {
			break;
		}

		if (sleep_wakepin_config[i].Status == ON) {
			u32 pinmux = sleep_wakepin_config[i].Pinmux;

			if(sleep_wakepin_config[i].Polarity) {
				PAD_PullCtrl(aon_wakepin[i][pinmux], GPIO_PuPd_DOWN);
			} else {
				PAD_PullCtrl(aon_wakepin[i][pinmux], GPIO_PuPd_UP);
			}

			Pinmux_Config(aon_wakepin[i][pinmux], PINMUX_FUNCTION_WAKEUP);
			SOCPS_SetWakepin(i, sleep_wakepin_config[i].Polarity);
		}
		
		i++;
	}
}

/**
  *  @brief set dslp wake status.
	* @param  DslpWake:
	*			 @arg TRUE: boot from deep sleep 
	*			 @arg FALSE: boot from power on
	* @retval None    
  */
void SOCPS_DsleepWakeStatusSet(u32 DslpWake)
{
	if (DslpWake == TRUE) {
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_DSLP_INFO_SW, BIT(0));
	} else {
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_DSLP_INFO_SW, 0);
	}
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

