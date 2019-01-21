/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */





#ifndef _IOTX_SHADOW_DELTA_H_
#define _IOTX_SHADOW_DELTA_H_

#include "iot_import.h"
#include "shadow.h"
#include "shadow_config.h"
#include "shadow_common.h"
#include "shadow_update.h"


int iotx_shadow_delta_check_existence(iotx_shadow_pt pshadow, const char *attr_name);

void iotx_shadow_delta_entry(
            iotx_shadow_pt pshadow,
            const char *json_doc,
            size_t json_doc_len);

iotx_err_t iotx_shadow_delta_register_attr(
            iotx_shadow_pt pshadow,
            iotx_shadow_attr_pt pattr);

#endif /* _IOTX_SHADOW_DELTA_H_ */
