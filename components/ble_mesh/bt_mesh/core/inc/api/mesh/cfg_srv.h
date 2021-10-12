/** @file
 *  @brief Bluetooth Mesh Configuration Server Model APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BT_MESH_CFG_SRV_H
#define __BT_MESH_CFG_SRV_H

/**
 * @brief Bluetooth Mesh
 * @defgroup bt_mesh_cfg_srv Bluetooth Mesh Configuration Server Model
 * @ingroup bt_mesh
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** Mesh Configuration Server Model Context */
struct bt_mesh_cfg_srv {
	struct bt_mesh_model *model;

	u8_t net_transmit;         /* Network Transmit state */
	u8_t relay;                /* Relay Mode state */
	u8_t relay_retransmit;     /* Relay Retransmit state */
	u8_t beacon;               /* Secure Network Beacon state */
	u8_t gatt_proxy;           /* GATT Proxy state */
	u8_t frnd;                 /* Friend state */
	u8_t default_ttl;          /* Default TTL */

	/* Heartbeat Publication */
	struct bt_mesh_hb_pub {
		struct k_delayed_work timer;

		u16_t dst;
		u16_t count;
		u8_t  period;
		u8_t  ttl;
		u16_t feat;
		u16_t net_idx;
	} hb_pub;

	/* Heartbeat Subscription */
	struct bt_mesh_hb_sub {
		s64_t  expiry;

		u16_t src;
		u16_t dst;
		u16_t count;
		u8_t  min_hops;
		u8_t  max_hops;

		/* Optional subscription tracking function */
		void (*func)(u8_t hops, u16_t feat);
	} hb_sub;
};
#ifdef CONFIG_BT_MESH_EVENT_CALLBACK
  typedef struct {
    uint8_t status;
    uint16_t netkey_idx;
    uint16_t appkey_idx;
  }appkey_status;
#endif


extern const struct bt_mesh_model_op bt_mesh_cfg_srv_op[];
extern struct bt_mesh_cfg_srv g_cfg_srv;

#define BT_MESH_MODEL_CFG_SRV(srv_data)                                      \
		BT_MESH_MODEL(BT_MESH_MODEL_ID_CFG_SRV,                      \
			      bt_mesh_cfg_srv_op, NULL, srv_data)

#define MESH_MODEL_CFG_SRV_NULL()  BT_MESH_MODEL_CFG_SRV(&g_cfg_srv)

u8_t bt_mesh_mod_bind(struct bt_mesh_model *model, u16_t key_idx);
u8_t bt_mesh_mod_sub_add(struct bt_mesh_model *model, u16_t sub_addr);
u8_t bt_mesh_mod_pub_set(struct bt_mesh_model *model, u16_t pub_addr);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __BT_MESH_CFG_SRV_H */
