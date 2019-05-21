#ifndef _BK7011_CAL_H_
#define _BK7011_CAL_H_

#if (CFG_SOC_NAME == SOC_BK7231)
#include "bk7231_cal.h"
#elif (CFG_SOC_NAME == SOC_BK7231U)
#include "bk7231U_cal.h"
#elif (CFG_SOC_NAME == SOC_BK7221U)
#include "bk7221U_cal.h"
#endif

#endif // _BK7011_CAL_H_
