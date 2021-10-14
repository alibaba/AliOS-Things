/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BT_MESH_EVENT_PORT_H_
#define _BT_MESH_EVENT_PORT_H_

#include <api/mesh.h>
int genie_mesh_port_init(void);

void genie_mesh_link_open(bt_mesh_prov_bearer_t bearer);
void genie_mesh_link_close(bt_mesh_prov_bearer_t bearer);
#endif
