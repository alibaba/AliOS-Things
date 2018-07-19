#ifndef _SSV6006_TBL_H_
#define _SSV6006_TBL_H_

#define TB_WIRE

#include "ssv6006.h"
#include "ssv6006_aux.h"
#ifdef TB_WIRE
#include "turismoC_wifi_phy_reg_v5.h"
#include "turismoC_rf_reg_v5.h"
#include "turismo_common_v5.h"
#else
#include "turismoC_wifi_phy_reg.h"
#include "turismoC_rf_reg.h"
#include "turismo_common.h"
#endif
#include "turismo_common_hdr.h"

#endif /* _SSV6030_TBL_H_ */

