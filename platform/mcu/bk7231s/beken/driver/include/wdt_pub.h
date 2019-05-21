#ifndef _WDT_PUB_H_
#define _WDT_PUB_H_

#define WDT_FAILURE                (1)
#define WDT_SUCCESS                (0)

#define WDT_DEV_NAME                "wdt"

#define WDT_CMD_MAGIC              (0xe330000)
enum
{
    WCMD_POWER_UP = WDT_CMD_MAGIC + 1,
	WCMD_SET_PERIOD,
	WCMD_RELOAD_PERIOD,
    WCMD_POWER_DOWN
};

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern void wdt_init(void);
extern void wdt_exit(void);

#endif //_WDT_PUB_H_ 

