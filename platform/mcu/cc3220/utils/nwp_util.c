#include <ti/drivers/net/wifi/simplelink.h>

extern int start_nwp(void);
extern int stop_nwp(int timeout);

#define SIMPLELINK_NWP_STARTED 0xFF
#define SIMPLELINK_NWP_STOPPED 0xEE

static int sl_nwp_started = SIMPLELINK_NWP_STOPPED;

int start_nwp(void)
{
    int ret = -1;
    
    if(sl_nwp_started == SIMPLELINK_NWP_STARTED)
        return sl_nwp_started;
   
    csp_printf("Turn on NWP\r\n"); 
    ret = sl_Start(NULL, NULL, NULL);
    
    if(ret >= 0) {
	csp_printf("NWP has been turned on\r\n");
        sl_nwp_started = SIMPLELINK_NWP_STARTED;
    }
    else
	csp_printf("NWP returned --:%d\r\n", ret);
    
    return ret;
}

int stop_nwp(int timeout)
{
    int ret;

    ret = sl_Stop(timeout);

    sl_nwp_started = SIMPLELINK_NWP_STOPPED;

    return ret;
}
