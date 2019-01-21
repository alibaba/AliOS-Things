#ifndef POWERSAVE_H_
#define POWERSAVE_H_

typedef enum t_PC_MODE
{
    MORMAL_MODE = 0,
    IRQ_MODE,
} PC_MODE;

typedef enum t_MCU_CFG
{
    MCU_ON = 0,
    MCU_OFF,
} MCU_CFG;

typedef enum t_RFPHY_STA
{
    RFPHY_ON = 0,
    RFPHY_OFF,
    RFPHY_SETTING,
} RFPHY_STA;

typedef struct t_PS_TIMER{
    u8  en;
    u32 tclock;
}PS_TIMER;

typedef struct PWSAVE_NEXTE{
    u32 nextsleeptime;
    u32 nexteventtime;
    u32 nexteventtime_en:8;
    u32 nextevent:8;
    u32 reserved:16;
}PWSAVE_NEXTE;

typedef enum t_POWERSAVE_MSG
{
    GO_TO_ACTIVE = 0,
	DATA_ACTIVE_SUCC,
    DATA_PWSAVE_SUCC,
    DATA_FAULT,
    TURNON_RFPHY,
    TURNOFF_RFPHY,
    UPDATE_TIMEOUT,
    TEMP_ALIVE,
} POWERSAVE_MSG;

typedef enum t_TIMEOUT_EVENT
{
    WAIT_BEACON_TIMEOUT,
    SYS_AWAKE_TIMEOUT,
    SYS_EN_POWERSAVE_TIMEOUT,
    NOEVENT_TIMEOUT,
} TIMEOUT_EVENT;

typedef struct t_PS_runinfo
{
    u16   losscnt:3;
    u16   dataretx:5;
    u16   sleepcountdown:3;
    u16   wakeupsource:2;
    u16   rfoff:2;
    u16   beaconillegal:1;
} PS_runinfo;

typedef struct t_PS_STATUS
{
    u32 usdtimperiod;
    u32 basebeacontime;
    u32 wakeuptime;
    PWSAVE_NEXTE next;
    struct t_PS_runinfo psruninfo;
    u8 userexpstatus;
    u8 STApwstatus;
    u8 opmode;
}PS_STATUS;

void checkpwstate();
void findTIM(u8 *buf, u8 *end, u32 intime);
void pwsave_enque(int msg);
void pwupdatepstime();
void resetpwstatus(u8 clearpstime);
s32 Switch_Power_mode(u8 enable, MCU_CFG opmode);
void powersaving_mgmt_task( void *args );

#endif /* POWERSAVE_H_ */

