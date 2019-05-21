#include "include.h"
#include "arm_arch.h"

#include "wdt_pub.h"
#include "wdt.h"
#include "icu_pub.h"

#include "drv_model_pub.h"

static SDD_OPERATIONS wdt_op = {
            wdt_ctrl
};
static uint32_t g_wdt_period = 0;

/*******************************************************************/
#if 1
void wdt_init(void)
{
	sddev_register_dev(WDT_DEV_NAME, &wdt_op);
}

void wdt_exit(void)
{
	sddev_unregister_dev(WDT_DEV_NAME);
}

UINT32 wdt_ctrl(UINT32 cmd, void *param)
{
	UINT32 ret;
	UINT32 reg;
	UINT32 parameter;

	ret = WDT_SUCCESS;	
	switch(cmd)
	{		
		case WCMD_POWER_DOWN:
			g_wdt_period = 0;
			
			parameter = PWD_ARM_WATCHDOG_CLK_BIT;
		    ret = sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, (void *)&parameter);
		    ASSERT(ICU_SUCCESS == ret);	
			break;
			
		case WCMD_POWER_UP:
			parameter = PWD_ARM_WATCHDOG_CLK_BIT;
		    ret = sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, (void *)&parameter);
		    ASSERT(ICU_SUCCESS == ret);	
			break;
			
		case WCMD_RELOAD_PERIOD:
			reg = WDT_1ST_KEY << WDT_KEY_POSI;
			reg |= (g_wdt_period & WDT_PERIOD_MASK) << WDT_PERIOD_POSI;
			REG_WRITE(WDT_CTRL_REG, reg);
			
			reg = WDT_2ND_KEY << WDT_KEY_POSI;
			reg |= (g_wdt_period & WDT_PERIOD_MASK) << WDT_PERIOD_POSI;
			REG_WRITE(WDT_CTRL_REG, reg);
			break;

		case WCMD_SET_PERIOD:				
			ASSERT(param);
			g_wdt_period = (*(UINT32 *)param);
			
			reg = WDT_1ST_KEY << WDT_KEY_POSI;
			reg |= ((*(UINT32 *)param) & WDT_PERIOD_MASK) << WDT_PERIOD_POSI;
			REG_WRITE(WDT_CTRL_REG, reg);
			
			reg = WDT_2ND_KEY << WDT_KEY_POSI;
			reg |= ((*(UINT32 *)param) & WDT_PERIOD_MASK) << WDT_PERIOD_POSI;
			REG_WRITE(WDT_CTRL_REG, reg);
			break;
			
		default:
			break;
	}
	
    return ret;
}
#endif

// EOF
