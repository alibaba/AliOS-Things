#ifndef _HAL_SOCPS_H_
#define _HAL_SOCPS_H_

typedef struct
{
	u32 Module;
	u32 Status;
} PWRCFG_TypeDef;

typedef struct
{
	u32 Pinmux; /* PINMUX_S0/S1/S2 based on aon_wakepin */
	u32 Status;
	u32 Polarity; /* 1 is high, 0 is low */
} WAKEPIN_TypeDef;

typedef struct
{
	u32 Module;
	u32 Event;
	u32 Status;
} HSWAKEEVENT_TypeDef;

extern u8 aon_wakepin[4][3];

extern PWRCFG_TypeDef sleep_wevent_config[];
extern PWRCFG_TypeDef sleep_aon_wevent_config[];
extern PWRCFG_TypeDef sleep_hsram_config[];
extern PWRCFG_TypeDef sleep_lsram_config[];
extern PWRCFG_TypeDef dsleep_lsram_config[];
extern WAKEPIN_TypeDef sleep_wakepin_config[];
extern PWRCFG_TypeDef km0_pwrmgt_config[];
extern PWRCFG_TypeDef dsleep_aon_wevent_config[];
extern HSWAKEEVENT_TypeDef hs_wakeevent_config[];

extern void SOCPS_InitSYSIRQ_HP(void);
extern void SOCPS_CPUReset(void);
extern void SOCPS_SleepPG(void);
extern void SOCPS_SleepCG(void);

extern void SOCPS_InitSYSIRQ(void);
extern void SOCPS_SleepInit(void);
extern void SOCPS_DsleepInit(void);
extern void SOCPS_SleepDeInit(void);
extern void SOCPS_DsleepWakeStatusSet(u32 DslpWake);
extern u32 SOCPS_DsleepWakeStatusGet(void);

extern void SOCPS_ClearWakeEvent_HP(void);
extern void SOCPS_SetWakeEvent_HP(u32 Option, u32 NewStatus);
extern void SOCPS_AONTimerCmd(u32 NewStatus);
extern int SOCPS_AONWakeReason(void);
extern void SOCPS_AONWakeClear(u32 BitMask);
extern void SOCPS_AONTimer(u32 SDuration);
extern void SOCPS_AONTSF(u32 EarlyBcnUs, u32 WakeIntvalUs);

extern VOID SOCPS_MMUReFill(VOID);


extern void SOCPS_SetWakeEvent(u32 Option, u32 NewStatus);
extern void SOCPS_SetWakeEventAON(u32 Option, u32 NewStatus);
extern void SOCPS_ClearWakePin(void);
extern void SOCPS_ClearWakeEvent(void);
extern void SOCPS_AudioLDO(u32 NewStatus);
extern void SOCPS_SWRLDO_Suspend(u32 new_status);
extern void SOCPS_OSC2M_Cmd(u32 new_status);
extern void SOCPS_SleepCG_RAM(void);
extern void SOCPS_SleepPG_RAM(void);
extern void SOCPS_DeepSleep_RAM(void);
extern int SOCPS_WakePinCheck(void);


#endif  //_HAL_SOCPS_H_
