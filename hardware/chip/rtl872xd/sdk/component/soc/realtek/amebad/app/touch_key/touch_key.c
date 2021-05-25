/*
 *  xModem protocol
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include <ameba_soc.h>
#include "touch_key.h"

#define HP_JACK_DET _PA_21 /*headphone detect pin*/
#define HP_JACK_INSET_LEVEL GPIO_PIN_LOW /*insert level*/

typedef struct
{
	u32 pinmux; /* PAx or PBx */
	u32 valid; /* 0 or 1 */
} KeyMatrix_TypeDef;

#if defined (ARM_CORE_CM4)
static CapTouch_CHInitTypeDef CTCChan[4] =
{
	/*DiffThrehold, MbiasCurrent, ETCNNoiseThr, ETCPNoiseThr, CHEnable*/
	{35,			0x16,		3,			18,			ENABLE},  /* Channel 0 */
	{500,		0x08,		250,			250,			DISABLE}, /* Channel 1 */
	{500,		0x08,		250,			250,			DISABLE}, /* Channel 2 */
	{500,		0x0b,		250,			250,			DISABLE}, /* Channel 3 */
};

static KeyMatrix_TypeDef KeyRow[8] = 
{
	/* pinmux,	valid */
	{_PA_12,	1}, /* row 0 */
	{_PA_13,	1}, /* row 1 */
	{_PA_14,	1}, /* row 2 */
	{_PA_15,	1}, /* row 3 */
	{_PA_16,	1}, /* row 4 */
	{_PA_18,	0}, /* row 5 */
	{_PA_17,	0}, /* row 6 */
	{_PA_21,	0}, /* row 7 */
};

static KeyMatrix_TypeDef KeyColumn[8] =
{
	/* pinmux, 	valid */
	{_PA_26,	1}, /* Col 0 */
	{_PA_25,	1}, /* Col 1 */
	{_PA_19,	1}, /* Col 2 */
	{_PA_17,	1}, /* Col 3 */
	{_PA_18,	1}, /* Col 4 */
	{_PA_16,	0}, /* Col 5 */
	{_PA_15,	0}, /* Col 6 */
	{_PA_20,	1}, /* Col 7 */
};




#define HP_JACK_DELAY_DET
/*detect headphone insert use a  delay timer*/
#ifdef HP_JACK_DELAY_DET
#include <timers.h>
#define HP_JACK_DELAY 50
static TimerHandle_t xTimers = NULL;

static void app_hp_jack_timer_callback( TimerHandle_t pxExpiredTimer )
{
	/* To avoid gcc warinigs */
	( void ) pxExpiredTimer;
	
	u32 level = GPIO_ReadDataBit(HP_JACK_DET);

	if (level == GPIO_PIN_LOW) {
		GPIO_INTMode(HP_JACK_DET, ENABLE, GPIO_INT_Trigger_LEVEL, GPIO_INT_POLARITY_ACTIVE_HIGH, GPIO_INT_DEBOUNCE_ENABLE);
		DBG_8195A("hp jack insert \n");
	} else {
		GPIO_INTMode(HP_JACK_DET, ENABLE, GPIO_INT_Trigger_LEVEL, GPIO_INT_POLARITY_ACTIVE_LOW, GPIO_INT_DEBOUNCE_ENABLE);
		DBG_8195A("hp jack pull out\n");
	}
	/*Should delay at least 63us between GPIO_INTMode & GPIO_INTConfig*/
	DelayUs(70);
	GPIO_INTConfig(HP_JACK_DET, ENABLE);
}
#endif

static void app_hp_jack_int_handler(uint32_t id, u32 event)
{
	/* To avoid gcc warinigs */
	( void ) id;
	
	u32 level = event & BIT(0);

	GPIO_INTConfig(HP_JACK_DET, DISABLE);
	
#ifdef HP_JACK_DELAY_DET
	portBASE_TYPE taskWoken = pdFALSE;
	if (xTimers) {
		if (xTimerChangePeriodFromISR(xTimers, HP_JACK_DELAY, &taskWoken) == pdPASS) {
			pmu_set_sysactive_time(HP_JACK_DELAY);
			xTimerStartFromISR(xTimers, &taskWoken);
			portEND_SWITCHING_ISR(taskWoken);
			return;
		}
	} else
#endif

	if (level == GPIO_INT_POLARITY_ACTIVE_HIGH) {
		GPIO_INTMode(HP_JACK_DET, ENABLE, GPIO_INT_Trigger_LEVEL, GPIO_INT_POLARITY_ACTIVE_LOW, GPIO_INT_DEBOUNCE_ENABLE);
	} else {
		GPIO_INTMode(HP_JACK_DET, ENABLE, GPIO_INT_Trigger_LEVEL, GPIO_INT_POLARITY_ACTIVE_HIGH, GPIO_INT_DEBOUNCE_ENABLE);
	}
	DelayUs(70);
	GPIO_INTConfig(HP_JACK_DET, ENABLE);
}

void app_hp_jack_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct_jack;

#ifdef HP_JACK_DELAY_DET
	xTimers = xTimerCreate("Jack_Timer", HP_JACK_DELAY, pdFALSE,NULL,app_hp_jack_timer_callback);
	if(xTimers ==NULL)
		DBG_8195A("HP timer create error\n");
#endif

	GPIO_INTConfig(HP_JACK_DET, DISABLE);

	GPIO_InitStruct_jack.GPIO_Pin = HP_JACK_DET;
	GPIO_InitStruct_jack.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct_jack.GPIO_Mode = GPIO_Mode_INT;
	GPIO_InitStruct_jack.GPIO_ITTrigger= GPIO_INT_Trigger_LEVEL;
	GPIO_InitStruct_jack.GPIO_ITPolarity = HP_JACK_INSET_LEVEL;

	/*GPIOA or GPIOB */
	if (HP_JACK_DET < _PB_0) {
		InterruptRegister(GPIO_INTHandler, GPIOA_IRQ, (u32)GPIOA_BASE, 5);
		InterruptEn(GPIOA_IRQ, 5);
	 } else { 
		InterruptRegister(GPIO_INTHandler, GPIOB_IRQ, (u32)GPIOB_BASE, 5);
		InterruptEn(GPIOB_IRQ, 5);
	 }

	GPIO_Init(&GPIO_InitStruct_jack);
	GPIO_UserRegIrq(HP_JACK_DET, (VOID *)app_hp_jack_int_handler, NULL);

	GPIO_INTMode(HP_JACK_DET, ENABLE, GPIO_INT_Trigger_LEVEL, HP_JACK_INSET_LEVEL, GPIO_INT_DEBOUNCE_ENABLE);
	GPIO_INTConfig(HP_JACK_DET, ENABLE);
}

/*captouch related function and interrupt handler*/
static void app_captouch_irq_handler(void)
{
	u8 ch;
	u32 IntStatus;
	
	IntStatus = CapTouch_GetISR(CAPTOUCH_DEV);
	CapTouch_INTClearPendingBit(CAPTOUCH_DEV, IntStatus);
	
	for(ch=0; ch<5; ch++) {
		if(IntStatus & CT_CHX_PRESS_INT(ch)) {
			DBG_8195A("Key	%x press \n",ch);
			pmu_set_sysactive_time(5);
		}

		if(IntStatus & CT_CHX_RELEASE_INT(ch)) {
			DBG_8195A("Key	%x release \n", ch);
			pmu_set_sysactive_time(5);
		}
	}

	if(IntStatus & BIT_CT_FIFO_OVERFLOW_INT) {
		DBG_8195A("BIT_CT_FIFO_OVERFLOW_INT \n");
	}

	/* User do not need care the Noise interrupt */
	if(IntStatus & BIT_CT_OVER_P_NOISE_THRESHOLD_INT) {		
		CapTouch_Cmd(CAPTOUCH_DEV, DISABLE);
		CapTouch_Cmd(CAPTOUCH_DEV, ENABLE);
	}
	
}

void app_captouch_init(void)
{
	u32 dslp_wakeup = SOCPS_DsleepWakeStatusGet();
	u32 temp = 0;
	u32 ch = 0;

	if (dslp_wakeup == FALSE) {
		CapTouch_InitTypeDef Touch_InitStruct;

		CapTouch_StructInit(&Touch_InitStruct);

		_memcpy((void*)Touch_InitStruct.CT_Channel, (void*)CTCChan, 4 * sizeof(CapTouch_CHInitTypeDef));

		for (ch = 0; ch < 4; ch++) {
			if (CTCChan[ch].CT_CHEnable == ENABLE) {
				PAD_PullCtrl((_PB_4 + ch), GPIO_PuPd_NOPULL);
			}
		}
		
		CapTouch_Init(CAPTOUCH_DEV, &Touch_InitStruct);
		CapTouch_Cmd(CAPTOUCH_DEV, ENABLE);
		CapTouch_INTConfig(CAPTOUCH_DEV, BIT_CT_ALL_INT_EN, ENABLE);

		//DelayMs(10);
		//CapTouch_DbgDumpETC(CAPTOUCH_DEV, 0);
	}
	
	/* Register interrupt Callback function */
	InterruptRegister((IRQ_FUN) app_captouch_irq_handler, CTOUCH_IRQ, (u32)NULL, 5);
	InterruptEn(CTOUCH_IRQ, 5);

	/* enable cap-touch wakeup */
	SOCPS_SetWakeEventAON(BIT_CAPTOUCH_WAKE_MSK, ENABLE);
	/* fix boot reason issue, when cap-touch wakeup */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_PM_OPT);
	temp &= ~BIT_AON_PMC_EN_SNOZ2NORM;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_PM_OPT, temp);

	//DBG_8195A("km4 app_captouch_init dslp_wakeup:%x \n", dslp_wakeup);
}

static void app_keyscan_data_process(u32 ksdata)
{
	u32 row = (ksdata & BIT_KS_ROW_INDEX) >> 4;
	u32 col = (ksdata & BIT_KS_COL_INDEX) >> 0;
	u32 press = (ksdata & BIT_KS_PRESS_EVENT) >> 8;
	
	DBG_8195A("ksdata:%x (row:%x col:%x press:%x) \n", ksdata, row, col, press);
}

static void app_keyscan_irq_handler(void)
{
	u32 intr_status = KeyScan_GetINT(KEYSCAN_DEV); 
	u32 temp;
	u32 tempi;
	u32 ksdata[16];

	/*clear intr*/
	KeyScan_ClearINT(KEYSCAN_DEV, intr_status);
	
	if (intr_status & BIT_KS_SCAN_EVENT_INT_STATUS) { //auto cleared
		/*clear keyscan wakeevent*/
		SOCPS_AONWakeClear(BIT_KEYSCAN_WAKE_STS);
		temp = KeyScan_GetDataNum(KEYSCAN_DEV);
		KeyScan_Read(KEYSCAN_DEV, ksdata, temp);
		pmu_acquire_wakelock(PMU_KEYSCAN_DEVICE);
		//DBG_8195A("SCAN_EVENT_INT FIFO Data:");
		for(tempi = 0; tempi < temp; tempi++) {
			app_keyscan_data_process(ksdata[tempi]);
		}
	}

	if (intr_status & BIT_KS_SCAN_FINISH_INT_STATUS) {
		DBG_8195A("KEYSCAN BIT_KS_SCAN_FINISH_INT_STATUS Intr\n");
	}

	if (intr_status & BIT_KS_ALL_RELEASE_INT_STATUS) {
		temp = KeyScan_GetDataNum(KEYSCAN_DEV);
		KeyScan_Read(KEYSCAN_DEV, ksdata, temp);
		pmu_release_wakelock(PMU_KEYSCAN_DEVICE);
		//DBG_8195A("ALL RELEASE \n");
		for(tempi = 0; tempi < temp; tempi++) {
			app_keyscan_data_process(ksdata[tempi]);
		}
	}
}

static u32 app_keyscan_suspend(u32 expected_idle_time, void *param)
{
	/* To avoid gcc warning */
	( void ) expected_idle_time;
	( void ) param;
	
	if(KEYSCAN_DEV->KS_CTRL & BIT_KS_BUSY_STATUS) {
		return FALSE;
	}

	/*reset keyscan for glitch issue*/
	if(SOCPS_AONWakeReason() & BIT_KEYSCAN_WAKE_STS) {
		SOCPS_AONWakeClear(BIT_KEYSCAN_WAKE_STS);
		//DBG_8195A("ks reset\n");
		app_keyscan_init(TRUE);
	}

	return TRUE;
}

static u32 app_keyscan_resume(u32 expected_idle_time, void *param)
{
	/* To avoid gcc warning */
	( void ) expected_idle_time;
	( void ) param;

	if(SOCPS_AONWakeReason() & BIT_KEYSCAN_WAKE_STS) {
		pmu_set_sysactive_time(20);
	}

	return TRUE;
}
void app_keyscan_init(u8 reset_status)
{
	u32 dslp_wakeup = SOCPS_DsleepWakeStatusGet();
	u32 index = 0;
	u32 column_sel = 0;
	u32 row_sel = 0;

	for(index = 0; index < 8; index++) {
		if (KeyColumn[index].valid) {
			if(FALSE == reset_status) {
				PAD_PullCtrl(KeyColumn[index].pinmux, GPIO_PuPd_NOPULL);
				Pinmux_Config(KeyColumn[index].pinmux, PINMUX_FUNCTION_KEYSCAN_COL);
			}
			column_sel |= BIT(index);			
		}
	}
	
	for(index = 0; index < 8; index++) {
		if (KeyRow[index].valid) {
			if(FALSE == reset_status) {
				PAD_PullCtrl(KeyRow[index].pinmux, GPIO_PuPd_UP);
				Pinmux_Config(KeyRow[index].pinmux, PINMUX_FUNCTION_KEYSCAN_ROW);
			}
			row_sel |= BIT(index);			
		}
	}

	//DBG_8195A("app_keyscan_init column_sel:%08x, row_sel:%08x\n", column_sel, row_sel);

	/* keyscan not used */
	if ((column_sel == 0) || (row_sel == 0)) {
		return;
	}

	if ((dslp_wakeup == FALSE) || ((dslp_wakeup == TRUE) && (0 == KeyScan_GetDataNum(KEYSCAN_DEV)))) {
		KeyScan_InitTypeDef  KeyScan_InitStruct;

		/*reset keyscan when dslp keyscan wakeup while no key is scanned*/
		RCC_PeriphClockCmd_AON(APBPeriph_KEYSCAN, APBPeriph_KEYSCAN_CLOCK, DISABLE);
		RCC_PeriphClockCmd_AON(APBPeriph_KEYSCAN, APBPeriph_KEYSCAN_CLOCK, ENABLE);

		KeyScan_StructInit(&KeyScan_InitStruct);

		KeyScan_InitStruct.KS_ClkDiv = 49; /* KEYSCAN_2M */
		KeyScan_InitStruct.KS_ColSel = column_sel;
		KeyScan_InitStruct.KS_RowSel = row_sel;
		KeyScan_InitStruct.KS_WorkMode = KS_EVENT_TRIGGER_MODE;
		KeyScan_InitStruct.KS_LimitLevel = 2;
		KeyScan_Init(KEYSCAN_DEV, &KeyScan_InitStruct);

		//KeyScan_RegDump();
		//KeyScan_INTConfig(KEYSCAN_DEV, BIT_KS_FIFO_FULL_INT_MSK| BIT_KS_ALL_RELEASE_INT_MSK, ENABLE);
		KeyScan_INTConfig(KEYSCAN_DEV, BIT_KS_ALL_RELEASE_INT_MSK | BIT_KS_SCAN_EVENT_INT_MSK, ENABLE);
		KeyScan_Cmd(KEYSCAN_DEV, ENABLE);
	}

	if(FALSE == reset_status) {
		pmu_register_sleep_callback(PMU_KEYSCAN_DEVICE, (PSM_HOOK_FUN)app_keyscan_suspend, NULL, (PSM_HOOK_FUN)app_keyscan_resume, NULL);
		InterruptRegister((IRQ_FUN) app_keyscan_irq_handler, KEYSCAN_IRQ, NULL, 5);
		InterruptEn(KEYSCAN_IRQ, 5);
	}
}

#else /*KM0*/
/*if stimuli is used to wakeup KM4, Two things must be configured
 * 1 the wake up source must be configured  in hs_wakeevent_config[]
 * 2 in case KM0 can't fall sleep dure to some wakelock, user should implement
 *    its own wake event check in km4_wake_event_check()
*/


#endif
