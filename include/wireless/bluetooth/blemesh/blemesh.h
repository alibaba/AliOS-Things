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
#include <net/buf.h>

#include "access.h"
#include "blemesh_main.h"
#include "cfg_srv.h"
#include "health_srv.h"

#if defined(CONFIG_BT_MESH_CFG_CLI)
#include "cfg_cli.h"
#endif

#if defined(CONFIG_BT_MESH_HEALTH_CLI)
#include "health_cli.h"
#endif

#if defined(CONFIG_BT_MESH_SHELL)
#include "shell.h"
#endif

#endif /* __BT_MESH_H */
