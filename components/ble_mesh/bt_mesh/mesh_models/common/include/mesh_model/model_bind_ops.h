/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _MODEL_BOUND_OPERATION_H
#define _MODEL_BOUND_OPERATION_H

#include "mesh_model.h"


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
    B_GEN_PWR_ACTUAL_ID,
    B_GEN_PWR_LAST_ID,
    B_GEN_PWR_DFT_ID,
    B_GEN_PWR_RANGE_ID,

    /* Lightness sev model*/
    B_GEN_LIGHTNESS_LINEAR_ID,
    B_GEN_LIGHTNESS_ACTUAL_ID,

    /* !!!END!!! --- Don't add new ID after this ID */
    B_OPS_END_ID

} BIND_OPERATION_ID;


#endif //_MODEL_BOUND_OPERATION_H
