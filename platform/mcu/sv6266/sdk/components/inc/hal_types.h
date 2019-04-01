#ifndef HAL_TYPES_H
#define HAL_TYPES_H

#define SSV6006     (6006)

#if (CHIP_ID == SSV6006)
#include "ssv6006/types.h"
#else   /* CHIP_ID */
#error "please define CHIP_ID"
#endif  /*CHIP_ID */


#endif /* end of include guard: HAL_TYPES_H */


