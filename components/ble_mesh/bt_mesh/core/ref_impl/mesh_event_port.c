/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ble_os.h>
#include <string.h>
#include <bt_errno.h>
#include <stdbool.h>
#include <ble_types/types.h>

#include <api/mesh.h>
#include "common/log.h"

#include "net.h"
#include "foundation.h"
#include "mesh_event_port.h"

static mesh_model_evt_cb g_bt_mesh_model_evt_cb = NULL;


int bt_mesh_event_register(mesh_model_evt_cb event_cb)
{
    if (event_cb == NULL) {
        return -1;
    }
    g_bt_mesh_model_evt_cb = event_cb;
    return 0;
}

mesh_model_evt_cb bt_mesh_event_get_cb_func(void)
{
    return g_bt_mesh_model_evt_cb;
}
