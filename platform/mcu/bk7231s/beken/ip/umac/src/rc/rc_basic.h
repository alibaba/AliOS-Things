/**
****************************************************************************************
*
* @file rc.h
*
* Copyright (C) RivieraWaves 2015
*
* @brief Declaration of the initialization functions used in UMAC Basic RC.
*
****************************************************************************************
*/

#ifndef _RC_BASIC_H_
#define _RC_BASIC_H_

// standard includes
#include "co_int.h"

#define ME_RC_PERIOD                 10   // 100
#define ME_UTILS_RC_DEC_RATIO        (2)  // 5
#define ME_UTILS_RC_INC_RATIO        (3)  // 10

// Forward declarations
struct sta_info_tag;

void rc_basic_check(uint8_t sta_idx);
void rc_basic_init(struct sta_info_tag *sta_entry);

#endif /* _RC_BASIC_H_ */

