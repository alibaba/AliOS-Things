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


#ifndef _SPC_SPP_CFG_H_
#define _SPC_SPP_CFG_H_

#include "baseband_defs.h"

///  @defgroup spc_spp_cfg_func SPP Configuration
///@{

#define MAXS_NBIT(n) ((1<<(n-1))-1)
#define MINS_NBIT(n) (-(1<<(n-1)))

// Following defines for equalization
#define _SYMB_SIZE 12 // can be 12 (Jade like) or 13
#define _SYMB_MAX MAXS_NBIT(_SYMB_SIZE)
#define _SYMB_MIN MINS_NBIT(_SYMB_SIZE)

// Following defines the FOF estimation parameters (for NB and SCH)
// The NBurst format:
// _________________________________________________
// |       57        |1|  26   |1|       57        |
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// or SCH burst format:
// _________________________________________________
// |     39      |        64        |      39      |
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// |           |                       |           |
// |<-NB_SYMB->|                       |<-NB_SYMB->|
// |           |                       |           |
// |         STOP1                     |           |
// |<------------ CORR_GAP ----------->|           |
// |                                 START2        |
// START1                                      STOP2

#define _FOF_NB_SYMB     32
#define _FOF_START_1     0
#define _FOF_STOP_2      141
#define _FOF_STOP_1      (_FOF_START_1+_FOF_NB_SYMB)
#define _FOF_START_2     (_FOF_STOP_2-_FOF_NB_SYMB)
#define _FOF_CORR_GAP    (_FOF_START_2-_FOF_START_1)
// This threshold is choosed for samples at 1/10 of full ADC range.
// -> Adv energy per sample :((2^11)/10)^2 = 41943
#define _AGC_THRESHOLD   (24 * 41943)



///@}




#endif //  _SPC_SPP_CFG_H_
