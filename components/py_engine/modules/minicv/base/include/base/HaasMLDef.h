/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef HAAS_ML_DEF_H
#define HAAS_ML_DEF_H

/**@enum MLEngineType_t
* @brief ML Engine Type \n
*/
typedef enum {
    ML_ENGINE_NONE    = -1,
    ML_ENGINE_MNN     = 0,
    ML_ENGINE_CLOUD   = 1,
    ML_ENGINE_ODLA    = 2,
    ML_ENGINE_MAX     = 30,
} MLEngineType_t;
#endif
