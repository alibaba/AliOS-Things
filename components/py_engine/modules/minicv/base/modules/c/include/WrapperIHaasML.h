
/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef WRAPPER_IHAAS_ML_H
#define WRAPPER_IHAAS_ML_H

#include "base/include/base/HaasMLDef.h"
#ifdef __cplusplus
extern "C" {
#endif

    void* MLCreateInstance(MLEngineType_t type);
    void MLDestoryInstance(void *instance);
    int MLConfig(void *instance, char *key, char *secret, char *endpoint,
              char *bucket);
    int MLSetInputData(void *instance, const char *dataPath, const char *compareDataPath);
    int MLLoadNet(void *instance, const char *modePath);
    int MLPredict(void *instance);
    int MLGetPredictResponses(void *instance, char *outResult, int len);
    int MLUnLoadNet(void *instance);

#ifdef __cplusplus
};
#endif

#endif
