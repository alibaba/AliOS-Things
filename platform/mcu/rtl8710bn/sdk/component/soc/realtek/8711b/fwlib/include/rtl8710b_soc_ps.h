#ifndef _HAL_SOCPS_H_
#define _HAL_SOCPS_H_

typedef struct
{
	u32 Module;
	u32 Status;
} PWRCFG_TypeDef;

typedef struct
{
	u32 Module;
	u32 Status;
	u32 Polarity; /* 1 is high, 0 is low */
} WAKEPIN_TypeDef;

extern const PWRCFG_TypeDef sleep_pwrmgt_config[];
extern const PWRCFG_TypeDef sleep_wevent_config[];
extern const WAKEPIN_TypeDef sleep_wakepin_config[];

extern const PWRCFG_TypeDef dstandby_wevent_config[];
extern const WAKEPIN_TypeDef dstandby_wakepin_config[];

extern const PWRCFG_TypeDef dsleep_wevent_config[];
extern const WAKEPIN_TypeDef dsleep_wakepin_config[];

extern void SOCPS_SleepPG(void);
extern void SOCPS_InitSYSIRQ(void);
extern void SOCPS_SleepDeInit(void);
extern void SOCPS_SleepInit(void);
extern void SOCPS_DstandbyInit(void);
extern void SOCPS_DsleepInit(void);
#endif  //_HAL_SOCPS_H_
