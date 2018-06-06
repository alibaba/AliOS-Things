/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/


#ifndef _CALENDAR_ASM_H_
#define _CALENDAR_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'calendar'."
#endif



//==============================================================================
// calendar
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_CALENDAR_BASE          0x01A0A000

#define REG_CALENDAR_BASE_HI       BASE_HI(REG_CALENDAR_BASE)
#define REG_CALENDAR_BASE_LO       BASE_LO(REG_CALENDAR_BASE)

#define REG_CALENDAR_CTRL          REG_CALENDAR_BASE_LO + 0x00000000
#define REG_CALENDAR_CMD           REG_CALENDAR_BASE_LO + 0x00000004
#define REG_CALENDAR_STATUS        REG_CALENDAR_BASE_LO + 0x00000008
#define REG_CALENDAR_CALENDAR_LOADVAL_L REG_CALENDAR_BASE_LO + 0x0000000C
#define REG_CALENDAR_CALENDAR_LOADVAL_H REG_CALENDAR_BASE_LO + 0x00000010
#define REG_CALENDAR_CALENDAR_CURVAL_L REG_CALENDAR_BASE_LO + 0x00000014
#define REG_CALENDAR_CALENDAR_CURVAL_H REG_CALENDAR_BASE_LO + 0x00000018
#define REG_CALENDAR_ALARMVAL_L    REG_CALENDAR_BASE_LO + 0x0000001C
#define REG_CALENDAR_ALARMVAL_H    REG_CALENDAR_BASE_LO + 0x00000020

//Ctrl
#define CALENDAR_INTERVAL(n)       (((n)&3)<<0)
#define CALENDAR_INTERVAL_DISABLE  (0<<0)
#define CALENDAR_INTERVAL_PER_SEC  (1<<0)
#define CALENDAR_INTERVAL_PER_MIN  (2<<0)
#define CALENDAR_INTERVAL_PER_HOUR (3<<0)

//Cmd
#define CALENDAR_CALENDAR_LOAD     (1<<0)
#define CALENDAR_ALARM_LOAD        (1<<4)
#define CALENDAR_ALARM_ENABLE_SET  (1<<5)
#define CALENDAR_ALARM_ENABLE_CLR  (1<<6)
#define CALENDAR_ALARM_CLR         (1<<8)
#define CALENDAR_ITV_IRQ_CLR       (1<<9)
#define CALENDAR_ITV_IRQ_MASK_SET  (1<<16)
#define CALENDAR_ITV_IRQ_MASK_CLR  (1<<17)
#define CALENDAR_CALENDAR_NOT_VALID (1<<31)

//Status
#define CALENDAR_ITV_IRQ_CAUSE     (1<<0)
#define CALENDAR_ALARM_IRQ_CAUSE   (1<<1)
#define CALENDAR_FORCE_WAKEUP      (1<<8)
#define CALENDAR_CHG_MASK          (1<<12)
#define CALENDAR_ITV_IRQ_STATUS    (1<<16)
#define CALENDAR_ALARM_ENABLE      (1<<20)
#define CALENDAR_CALENDAR_NOT_PROG (1<<31)

//Calendar_LoadVal_L
#define CALENDAR_SEC(n)            (((n)&0x3F)<<0)
#define CALENDAR_MIN(n)            (((n)&0x3F)<<8)
#define CALENDAR_HOUR(n)           (((n)&31)<<16)

//Calendar_LoadVal_H
#define CALENDAR_DAY(n)            (((n)&31)<<0)
#define CALENDAR_MON(n)            (((n)&15)<<8)
#define CALENDAR_YEAR(n)           (((n)&0x7F)<<16)
#define CALENDAR_WEEKDAY(n)        (((n)&7)<<24)

//Calendar_CurVal_L
//#define CALENDAR_SEC(n)          (((n)&0x3F)<<0)
#define CALENDAR_SEC_MASK          (0x3F<<0)
#define CALENDAR_SEC_SHIFT         (0)
//#define CALENDAR_MIN(n)          (((n)&0x3F)<<8)
#define CALENDAR_MIN_MASK          (0x3F<<8)
#define CALENDAR_MIN_SHIFT         (8)
//#define CALENDAR_HOUR(n)         (((n)&31)<<16)
#define CALENDAR_HOUR_MASK         (31<<16)
#define CALENDAR_HOUR_SHIFT        (16)

//Calendar_CurVal_H
//#define CALENDAR_DAY(n)          (((n)&31)<<0)
#define CALENDAR_DAY_MASK          (31<<0)
#define CALENDAR_DAY_SHIFT         (0)
//#define CALENDAR_MON(n)          (((n)&15)<<8)
#define CALENDAR_MON_MASK          (15<<8)
#define CALENDAR_MON_SHIFT         (8)
//#define CALENDAR_YEAR(n)         (((n)&0x7F)<<16)
#define CALENDAR_YEAR_MASK         (0x7F<<16)
#define CALENDAR_YEAR_SHIFT        (16)
//#define CALENDAR_WEEKDAY(n)      (((n)&7)<<24)
#define CALENDAR_WEEKDAY_MASK      (7<<24)
#define CALENDAR_WEEKDAY_SHIFT     (24)

//AlarmVal_L
//#define CALENDAR_SEC(n)          (((n)&0x3F)<<0)
//#define CALENDAR_MIN(n)          (((n)&0x3F)<<8)
//#define CALENDAR_HOUR(n)         (((n)&31)<<16)

//AlarmVal_H
//#define CALENDAR_DAY(n)          (((n)&31)<<0)
//#define CALENDAR_MON(n)          (((n)&15)<<8)
//#define CALENDAR_YEAR(n)         (((n)&0x7F)<<16)




#endif
