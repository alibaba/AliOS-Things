/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */

#ifndef _UCLOUD_AI_OBJECTDET_H_
#define _UCLOUD_AI_OBJECTDET_H_
#include "objectdet.h"

int ucloudai_objectdet_object_detect(char *path, objectdet_detect_object_cb_t cb);

#endif //_UCLOUD_AI_OBJECTDET_H_