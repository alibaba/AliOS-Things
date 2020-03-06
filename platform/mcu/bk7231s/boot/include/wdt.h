#ifndef _WDT_H_
#define _WDT_H_

#define WDT_DEBUG

#ifdef WDT_DEBUG
    #define WDT_PRT      os_printf
	#define WDT_WARN     warning_prf
	#define WDT_FATAL    fatal_prf
#else
    #define WDT_PRT      null_prf
	#define WDT_WARN     null_prf
	#define WDT_FATAL    null_prf
#endif

#define WDT_BASE                                     (0x00802900)

#define WDT_CTRL_REG                                     (WDT_BASE + 0 * 4)
#define WDT_KEY_POSI                                              (16)
#define WDT_KEY_MASK                                              (0xFF)
#define WDT_1ST_KEY                                               (0x5A)
#define WDT_2ND_KEY                                               (0xA5)

#define WDT_PERIOD_POSI                                           (0)
#define WDT_PERIOD_MASK                                           (0xFFFF)

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern UINT32 wdt_ctrl(UINT32 cmd, void *param);

#endif //_WDT_H_ 

