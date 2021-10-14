/** @file
 *  @brief Bluetooth Mesh Profile APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BT_MESH_H
#define __BT_MESH_H

#include <stddef.h>
#include <ble_config.h>
#include <ble_os.h>

#include <ble_mesh_config.h>

#include <api/mesh/access.h>
#include <api/mesh/main.h>
#include <api/mesh/cfg_srv.h>
/*[Genie begin] add by wenbing.cwb at 2021-01-21*/
#ifdef CONFIG_BT_MESH_CTRL_RELAY
#include <api/mesh/ctrl_relay.h>
#endif
/*[Genie end] add by wenbing.cwb at 2021-01-21*/
#include <api/mesh/health_srv.h>

#if defined(CONFIG_BT_MESH_CFG_CLI)
#include <api/mesh/cfg_cli.h>
#endif

#if defined(CONFIG_BT_MESH_HEALTH_CLI)
#include <api/mesh/health_cli.h>
#endif

#if defined(CONFIG_BT_MESH_GATT_PROXY)
#include <api/mesh/proxy.h>
#endif

#endif /* __BT_MESH_H */
