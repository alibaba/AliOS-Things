/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef WRAPPER_IHAAS_DATA_INPUT_H
#define WRAPPER_IHAAS_DATA_INPUT_H

#include "base/include/base/HaasDataInputDef.h"
#ifdef __cplusplus
extern "C" {
#endif

    void* DataInputCreateInstance(DataInputType_t type);
    void DataInputDestoryInstance(void* instance);
	int DataInputOpen(void* instance, char*filename);
	int DataInputOpen2(void* instance, int cameraNum);
	int DataInputClose(void* instance);
	bool DataInputCheckDataReady(void* instance);
	int DataInputRequestData(void* instance, ImageBuffer_t** image, int32_t timeout);
	int DataInputReleaseData(void* instance, ImageBuffer_t* image);

#ifdef __cplusplus
};
#endif

#endif
