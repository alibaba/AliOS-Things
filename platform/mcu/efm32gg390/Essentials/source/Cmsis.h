/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH.
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

#ifndef CMSIS_H_
#define CMSIS_H_

/*lint -save -e451 suppression of repeatedly included files in 3rd party code */

#if defined (stm32)
#include "stm32l4xx_hal.h"

#include "stm32l4xx.h"
#include "core_cm4.h"
#include "core_cmInstr.h"
#include "core_cmFunc.h"

#elif defined (efm32)
#include "em_device.h"
#include "core_cm3.h"
#include "core_cmInstr.h"
#include "core_cmFunc.h"


#elif defined (ti_cc26xx)

#else
#error "invalid target"
#endif

/*lint -restore */

#endif /* CMSIS_H_ */

