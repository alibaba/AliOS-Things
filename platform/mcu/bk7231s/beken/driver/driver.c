#include "include.h"
#include "driver_pub.h"
#include "dd_pub.h"
#include "drv_model_pub.h"

UINT32 driver_init(void)
{    
    drv_model_init();
    g_dd_init();
    
    return 0;
}

// eof
