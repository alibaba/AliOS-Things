/** @file
 *  @brief Bluetooth Mesh Configuration Client Model APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BT_MESH_CFG_CLI_H
#define __BT_MESH_CFG_CLI_H

/** Mesh Configuration Client Model Context */
struct bt_mesh_cfg_cli {
	struct bt_mesh_model *model;

	struct k_sem          op_sync;
	u32_t                 op_pending;
	void                 *op_param;
};

extern const struct bt_mesh_model_op bt_mesh_cfg_cli_op[];

/* Helper macro to define the Configuration Client Model. */
#define BT_MESH_MODEL_CFG_CLI(cli_data)                                      \
		BT_MESH_MODEL(BT_MESH_MODEL_ID_CFG_CLI,                      \
			      bt_mesh_cfg_cli_op, NULL, cli_data)


#endif /* __BT_MESH_CFG_CLI_H */
