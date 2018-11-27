#ifndef ATMEL_START_H_INCLUDED
#define ATMEL_START_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define FBSW_MAGIC    0x46425357U    /* Magic number 'FBSW' for flash bank switch */

#include "driver_init.h"
#include "sd_mmc_start.h"

/**
 * Initializes MCU, drivers and middleware in the project
**/
void atmel_start_init(void);

#ifdef __cplusplus
}
#endif
#endif
