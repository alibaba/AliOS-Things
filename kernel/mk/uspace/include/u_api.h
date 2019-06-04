/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef U_API_H
#define U_API_H

#define APP_DEFAULT_PRIO    32

#include "k_api.h"
#include "u_app_config.h"
#include "u_app_default_config.h"

#include "umm.h"
#include "utask.h"

#if (RHINO_CONFIG_UTIMER > 0)
#include "utimer.h"
#endif

#if (RHINO_CONFIG_UIPC > 0)
#include "ipc.h"
#endif

#if (RHINO_CONFIG_URES > 0)
#include "res.h"
#include "res_task.h"
#endif

#if (RHINO_CONFIG_UCLI > 0)
#include "cli_api.h"
#include "cli_task.h"
#endif

#if (RHINO_CONFIG_UCALLBACK > 0)
#include "callback.h"
#include "callback_task.h"
#endif

#include "uassert.h"

#endif /* U_API_H */

