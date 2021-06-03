/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdarg.h>

#include "aos_system.h"
#include "amp_defines.h"

#include "smartcard.h"

#include "aliyun_iot_smartcard_export.h"


#define MOD_STR		"Smartcard"

int smartcard_init(void)
{
    int version_major = -1, version_minor = -1;
    aliyun_iot_smartcard_get_version(&version_major, &version_minor);
    amp_info(MOD_STR, "alismartcard version %d.%d", version_major, version_minor);
    return aliyun_iot_smartcard_init();
}

int smartcard_deinit(void)
{
    return aliyun_iot_smartcard_deinit();
}

int smartcard_change_operator(smartcard_change_operator_t type)
{
    aliyun_iot_smartcard_type_t smartcard_operator;

    switch (type) {
    case SMARTCARD_CHANGE_TO_CM:
        smartcard_operator = SWITCH_TO_SIM1;
        break;
    case SMARTCARD_CHANGE_TO_CU:
        smartcard_operator = SWITCH_TO_SIM2;
        break;
    case SMARTCARD_CHANGE_TO_CT:
        smartcard_operator = SWITCH_TO_SIM3;
        break;
    case SMARTCARD_CHANGE_TO_NEXT:
        smartcard_operator = SWITCH_TO_NEXT;
        break;
    default:
        amp_error(MOD_STR, "unknown operator %d", type);
        return -1;
    }

	return aliyun_iot_smartcard_change_operator(smartcard_operator);
}

