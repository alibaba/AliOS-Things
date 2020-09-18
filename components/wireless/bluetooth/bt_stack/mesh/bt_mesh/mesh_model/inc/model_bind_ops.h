//
// Created by ethan on 2019/7/8.
//

#ifndef _MODEL_BOUND_OPERATION_H
#define _MODEL_BOUND_OPERATION_H

#include "genie_mesh.h"

typedef enum _BIND_OPERATION_ID_ {
/* !!!START!!! --- Don't add new ID before this ID */
    B_OPS_START_ID = -1,

/* Generic OnOff */
    B_GEN_ONOFF_ID = 0,

/* Generic Level */
    B_GEN_LEVEL_ID,
    B_GEN_DELTA_ID,
    B_GEN_MOVE_ID,

/* Generic Default Transition Time */
    B_GEN_DFT_TRANS_TIME_ID,

/* Generic Power OnOff */
    B_GEN_ON_PWR_UP_ID,

/* Generic Power Level */
    B_GEN_PWR_LEVEL_ID,
    B_GEN_PWR_LAST_ID,
    B_GEN_PWR_DFT_ID,
    B_GEN_PWR_RANGE_ID,

/* Light Lightness */
    B_LIGHTNESS_ID,
    B_LIGHTNESS_LINEAR_ID,
    B_LIGHTNESS_LAST_ID,
    B_LIGHTNESS_DFT_ID,
    B_LIGHTNESS_RANGE_ID,

/* Light CTL */

    B_LIGHT_CTL_ID,
    B_LIGHT_CTL_TEMP_ID,
    B_LIGHT_CTL_DFT_TEMP_ID,
    B_LIGHT_CTL_TEMP_RANGE_ID,

/* !!!END!!! --- Don't add new ID after this ID */
    B_OPS_END_ID

} BIND_OPERATION_ID;

typedef u16_t (*BIND_OPS_HANDLER)(S_ELEM_STATE *p_elem, u8_t type);

u16_t model_bind_operation(BIND_OPERATION_ID id, S_ELEM_STATE *p_elem, u8_t type);


#endif //_MODEL_BOUND_OPERATION_H
