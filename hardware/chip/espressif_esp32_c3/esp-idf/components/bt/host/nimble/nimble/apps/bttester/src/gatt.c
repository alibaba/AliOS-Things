/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.    You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/* gatt.c - Bluetooth GATT Server Tester */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>
#include <assert.h>

#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "console/console.h"
#include "services/gatt/ble_svc_gatt.h"
#include "../../../nimble/host/src/ble_att_priv.h"
#include "../../../nimble/host/src/ble_gatt_priv.h"

#include "bttester.h"

#define CONTROLLER_INDEX 0
#define MAX_BUFFER_SIZE 2048

/* 0000xxxx-8c26-476f-89a7-a108033a69c7 */
#define PTS_UUID_DECLARE(uuid16)                                    \
    ((const ble_uuid_t *) (&(ble_uuid128_t) BLE_UUID128_INIT(   \
	0xc7, 0x69, 0x3a, 0x03, 0x08, 0xa1, 0xa7, 0x89,             \
	0x6f, 0x47, 0x26, 0x8c, uuid16, uuid16 >> 8, 0x00, 0x00     \
    )))

/* 0000xxxx-8c26-476f-89a7-a108033a69c6 */
#define PTS_UUID_DECLARE_ALT(uuid16)                            \
    ((const ble_uuid_t *) (&(ble_uuid128_t) BLE_UUID128_INIT(   \
	0xc6, 0x69, 0x3a, 0x03, 0x08, 0xa1, 0xa7, 0x89,             \
	0x6f, 0x47, 0x26, 0x8c, uuid16, uuid16 >> 8, 0x00, 0x00     \
    )))

#define  PTS_SVC                           0x0001
#define  PTS_CHR_READ                      0x0002
#define  PTS_CHR_WRITE                     0x0003
#define  PTS_CHR_RELIABLE_WRITE            0x0004
#define  PTS_CHR_WRITE_NO_RSP              0x0005
#define  PTS_CHR_READ_WRITE                0x0006
#define  PTS_CHR_READ_WRITE_ENC            0x0007
#define  PTS_CHR_READ_WRITE_AUTHEN         0x0008
#define  PTS_DSC_READ                      0x0009
#define  PTS_DSC_WRITE                     0x000a
#define  PTS_DSC_READ_WRITE                0x000b
#define  PTS_CHR_NOTIFY                    0x0025
#define  PTS_LONG_CHR_READ_WRITE           0x0015
#define  PTS_LONG_CHR_READ_WRITE_ALT       0x0016
#define  PTS_LONG_DSC_READ_WRITE           0x001b
#define  PTS_INC_SVC                       0x001e
#define  PTS_CHR_READ_WRITE_ALT            0x001f

static uint8_t gatt_svr_pts_static_long_val[300];
static uint8_t gatt_svr_pts_static_val[30];
static uint8_t gatt_svr_pts_static_short_val;
static u8_t notify_state;
static u8_t indicate_state;
static uint16_t myconn_handle;
static struct os_callout notify_tx_timer;
uint16_t notify_handle;
uint8_t notify_value = 90;

struct find_attr_data {
                ble_uuid_any_t *uuid;
                int attr_type;
                void *ptr;
                uint16_t handle;
};

static int
gatt_svr_read_write_test(uint16_t conn_handle, uint16_t attr_handle,
			 struct ble_gatt_access_ctxt *ctxt,
			 void *arg);

static int
gatt_svr_read_write_auth_test(uint16_t conn_handle, uint16_t attr_handle,
			      struct ble_gatt_access_ctxt *ctxt,
			      void *arg);

static int
gatt_svr_read_write_enc_test(uint16_t conn_handle, uint16_t attr_handle,
			     struct ble_gatt_access_ctxt *ctxt,
			     void *arg);

static int
gatt_svr_dsc_read_write_test(uint16_t conn_handle, uint16_t attr_handle,
			     struct ble_gatt_access_ctxt *ctxt,
			     void *arg);

static int
gatt_svr_write_no_rsp_test(uint16_t conn_handle, uint16_t attr_handle,
			   struct ble_gatt_access_ctxt *ctxt,
			   void *arg);

static int
gatt_svr_rel_write_test(uint16_t conn_handle, uint16_t attr_handle,
			struct ble_gatt_access_ctxt *ctxt,
			void *arg);

static int
gatt_svr_read_write_long_test(uint16_t conn_handle, uint16_t attr_handle,
			      struct ble_gatt_access_ctxt *ctxt,
			      void *arg);

static int
gatt_svr_dsc_read_test(uint16_t conn_handle, uint16_t attr_handle,
		       struct ble_gatt_access_ctxt *ctxt,
		       void *arg);

static int
gatt_svr_dsc_read_write_long_test(uint16_t conn_handle, uint16_t attr_handle,
				  struct ble_gatt_access_ctxt *ctxt,
				  void *arg);

static const struct ble_gatt_svc_def gatt_svr_inc_svcs[] = {
	{
		.type = BLE_GATT_SVC_TYPE_PRIMARY,
		.uuid = PTS_UUID_DECLARE(PTS_INC_SVC),
		.characteristics = (struct ble_gatt_chr_def[]) {{
			.uuid = PTS_UUID_DECLARE(PTS_CHR_READ_WRITE_ALT),
			.access_cb = gatt_svr_read_write_test,
			.flags = BLE_GATT_CHR_F_WRITE |
				 BLE_GATT_CHR_F_READ,
		}, {
		0,
		} },
	},

	{
		0, /* No more services. */
	},
};

static const struct ble_gatt_svc_def *inc_svcs[] = {
	&gatt_svr_inc_svcs[0],
	NULL,
};

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
	{
		/*** Service: PTS test. */
		.type = BLE_GATT_SVC_TYPE_PRIMARY,
		.uuid = PTS_UUID_DECLARE(PTS_SVC),
		.includes = inc_svcs,
		.characteristics = (struct ble_gatt_chr_def[]) { {
			.uuid = PTS_UUID_DECLARE(PTS_CHR_READ_WRITE),
			.access_cb = gatt_svr_read_write_test,
			.flags = BLE_GATT_CHR_F_READ |
				 BLE_GATT_CHR_F_WRITE,
			.descriptors = (struct ble_gatt_dsc_def[]) { {
				.uuid = PTS_UUID_DECLARE(PTS_DSC_READ_WRITE),
				.access_cb = gatt_svr_dsc_read_write_test,
				.att_flags = BLE_ATT_F_READ |
					     BLE_ATT_F_WRITE,
			}, {
			.uuid = PTS_UUID_DECLARE(PTS_LONG_DSC_READ_WRITE),
			.access_cb = gatt_svr_dsc_read_write_long_test,
			.att_flags = BLE_ATT_F_READ |
				     BLE_ATT_F_WRITE,
			}, {
			.uuid = PTS_UUID_DECLARE(PTS_DSC_READ),
			.access_cb = gatt_svr_dsc_read_test,
			.att_flags = BLE_ATT_F_READ,
			}, {
				0, /* No more descriptors in this characteristic */
			} }
		}, {
			.uuid = PTS_UUID_DECLARE(PTS_CHR_WRITE_NO_RSP),
			.access_cb = gatt_svr_write_no_rsp_test,
			.flags = BLE_GATT_CHR_F_READ |
				 BLE_GATT_CHR_F_WRITE |
				 BLE_GATT_CHR_F_WRITE_NO_RSP,
		}, {
			.uuid = PTS_UUID_DECLARE(PTS_CHR_READ_WRITE_AUTHEN),
			.access_cb = gatt_svr_read_write_auth_test,
			.flags = BLE_GATT_CHR_F_READ_AUTHEN |
				 BLE_GATT_CHR_F_READ |
				 BLE_GATT_CHR_F_WRITE_AUTHEN |
				 BLE_GATT_CHR_F_WRITE |
				 BLE_GATT_CHR_F_WRITE_AUTHEN,
		}, {
			.uuid = PTS_UUID_DECLARE(PTS_CHR_RELIABLE_WRITE),
			.access_cb = gatt_svr_rel_write_test,
			.flags = BLE_GATT_CHR_F_WRITE |
				 BLE_GATT_CHR_F_RELIABLE_WRITE,
		}, {
			.uuid = PTS_UUID_DECLARE(PTS_CHR_READ_WRITE_ENC),
			.access_cb = gatt_svr_read_write_enc_test,
			.flags = BLE_GATT_CHR_F_READ_ENC |
				 BLE_GATT_CHR_F_READ |
				 BLE_GATT_CHR_F_WRITE |
				 BLE_GATT_CHR_F_WRITE_ENC,
			.min_key_size = 16,
		}, {
			.uuid = PTS_UUID_DECLARE(PTS_LONG_CHR_READ_WRITE),
			.access_cb = gatt_svr_read_write_long_test,
			.flags = BLE_GATT_CHR_F_WRITE |
				 BLE_GATT_CHR_F_READ,
		}, {
			.uuid = PTS_UUID_DECLARE(PTS_LONG_CHR_READ_WRITE_ALT),
			.access_cb = gatt_svr_read_write_long_test,
			.flags = BLE_GATT_CHR_F_WRITE |
				 BLE_GATT_CHR_F_READ,
		}, {
			.uuid = PTS_UUID_DECLARE(PTS_CHR_NOTIFY),
			.access_cb = gatt_svr_read_write_test,
			.val_handle = &notify_handle,
			.flags = BLE_GATT_CHR_F_NOTIFY |
				 BLE_GATT_CHR_F_INDICATE,
		}, {
			0, /* No more characteristics in this service. */
		} },
	},

	{
		.type = BLE_GATT_SVC_TYPE_PRIMARY,
		.uuid = PTS_UUID_DECLARE_ALT(PTS_SVC),
		.characteristics = (struct ble_gatt_chr_def[]) { {
			.uuid = PTS_UUID_DECLARE_ALT(PTS_CHR_READ_WRITE),
			.access_cb = gatt_svr_read_write_test,
			.flags = BLE_GATT_CHR_F_WRITE |
				 BLE_GATT_CHR_F_READ,
		}, {
			0, /* No more characteristics in this service */
		} },
	},

	{
		0, /* No more services. */
	},
};

static void attr_value_changed_ev(u16_t handle, struct os_mbuf *data)
{
	struct gatt_attr_value_changed_ev *ev;
	struct os_mbuf *buf = os_msys_get(0, 0);

	SYS_LOG_DBG("");

	net_buf_simple_init(buf, 0);
	ev = net_buf_simple_add(buf, sizeof(*ev));

	ev->handle = sys_cpu_to_le16(handle);
	ev->data_length = sys_cpu_to_le16(os_mbuf_len(data));
	os_mbuf_appendfrom(buf, data, 0, os_mbuf_len(data));

	tester_send_buf(BTP_SERVICE_ID_GATT, GATT_EV_ATTR_VALUE_CHANGED,
			CONTROLLER_INDEX, buf);
}

static int
gatt_svr_chr_write(uint16_t conn_handle, uint16_t attr_handle,
		   struct os_mbuf *om, uint16_t min_len, uint16_t max_len,
		   void *dst, uint16_t *len)
{
	uint16_t om_len;
	int rc;

	om_len = OS_MBUF_PKTLEN(om);
	if (om_len < min_len || om_len > max_len) {
		return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
	}

	rc = ble_hs_mbuf_to_flat(om, dst, max_len, len);
	if (rc != 0) {
		return BLE_ATT_ERR_UNLIKELY;
	}

	attr_value_changed_ev(attr_handle, om);

	return 0;
}

static uint16_t
extract_uuid16_from_pts_uuid128(const ble_uuid_t *uuid)
{
	const uint8_t *u8ptr;
	uint16_t uuid16;

	u8ptr = BLE_UUID128(uuid)->value;
	uuid16 = u8ptr[12];
	uuid16 |= (uint16_t)u8ptr[13] << 8;
	return uuid16;
}

static int
gatt_svr_read_write_test(uint16_t conn_handle, uint16_t attr_handle,
			 struct ble_gatt_access_ctxt *ctxt,
			 void *arg)
{
	uint16_t uuid16;
	int rc;

	uuid16 = extract_uuid16_from_pts_uuid128(ctxt->chr->uuid);
	assert(uuid16 != 0);

	switch (uuid16) {
	case PTS_CHR_READ_WRITE:
	case PTS_CHR_READ_WRITE_ALT:
		if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
			rc = gatt_svr_chr_write(conn_handle, attr_handle,
						ctxt->om, 0,
						sizeof gatt_svr_pts_static_short_val,
						&gatt_svr_pts_static_short_val, NULL);
			return rc;
		} else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
			rc = os_mbuf_append(ctxt->om, &gatt_svr_pts_static_short_val,
					    sizeof gatt_svr_pts_static_short_val);
			return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
		}
	default:
		assert(0);
		return BLE_ATT_ERR_UNLIKELY;
	}
}

static int
gatt_svr_read_write_long_test(uint16_t conn_handle, uint16_t attr_handle,
			      struct ble_gatt_access_ctxt *ctxt,
			      void *arg)
{
	uint16_t uuid16;
	int rc;

	uuid16 = extract_uuid16_from_pts_uuid128(ctxt->chr->uuid);
	assert(uuid16 != 0);

	switch (uuid16) {
	case PTS_LONG_CHR_READ_WRITE:
	case PTS_LONG_CHR_READ_WRITE_ALT:
		if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
			rc = gatt_svr_chr_write(conn_handle, attr_handle,
						ctxt->om, 0,
						sizeof gatt_svr_pts_static_long_val,
						&gatt_svr_pts_static_long_val, NULL);
			return rc;
		} else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
			rc = os_mbuf_append(ctxt->om, &gatt_svr_pts_static_long_val,
					    sizeof gatt_svr_pts_static_long_val);
			return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
		}
	default:
		assert(0);
		return BLE_ATT_ERR_UNLIKELY;
	}
}

static int
gatt_svr_read_write_auth_test(uint16_t conn_handle, uint16_t attr_handle,
			      struct ble_gatt_access_ctxt *ctxt,
			      void *arg)
{
	uint16_t uuid16;
	int rc;

	uuid16 = extract_uuid16_from_pts_uuid128(ctxt->chr->uuid);
	assert(uuid16 != 0);

	switch (uuid16) {
	case PTS_CHR_READ_WRITE_AUTHEN:
		if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
			rc = gatt_svr_chr_write(conn_handle, attr_handle,
						ctxt->om, 0,
						sizeof gatt_svr_pts_static_val,
						&gatt_svr_pts_static_val, NULL);
			return rc;
		} else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
			rc = os_mbuf_append(ctxt->om, &gatt_svr_pts_static_val,
					    sizeof gatt_svr_pts_static_val);
			return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
		}
	default:
		assert(0);
		return BLE_ATT_ERR_UNLIKELY;
	}
}

static int
gatt_svr_read_write_enc_test(uint16_t conn_handle, uint16_t attr_handle,
			     struct ble_gatt_access_ctxt *ctxt,
			     void *arg)
{
	uint16_t uuid16;
	int rc;

	uuid16 = extract_uuid16_from_pts_uuid128(ctxt->chr->uuid);
	assert(uuid16 != 0);

	switch (uuid16) {
	case PTS_CHR_READ_WRITE_ENC:
		if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
			rc = os_mbuf_append(ctxt->om, &gatt_svr_pts_static_val,
					    sizeof gatt_svr_pts_static_val);
			return rc;
		} else if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
			rc = gatt_svr_chr_write(conn_handle, attr_handle,
						ctxt->om, 0,
						sizeof gatt_svr_pts_static_val,
						&gatt_svr_pts_static_val, NULL);
			return rc;
		}
	default:
		assert(0);
		return BLE_ATT_ERR_UNLIKELY;
	}
}

static int
gatt_svr_dsc_read_write_test(uint16_t conn_handle, uint16_t attr_handle,
			     struct ble_gatt_access_ctxt *ctxt,
			     void *arg)
{
	uint16_t uuid16;
	int rc;

	uuid16 = extract_uuid16_from_pts_uuid128(ctxt->chr->uuid);
	assert(uuid16 != 0);

	switch (uuid16) {
	case PTS_DSC_READ_WRITE:
		if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_DSC) {
			rc = gatt_svr_chr_write(conn_handle, attr_handle,
						ctxt->om, 0,
						sizeof gatt_svr_pts_static_short_val,
						&gatt_svr_pts_static_short_val, NULL);
			return rc;
		} else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_DSC) {
			rc = os_mbuf_append(ctxt->om, &gatt_svr_pts_static_short_val,
					    sizeof gatt_svr_pts_static_short_val);
			return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
		}
	default:
		assert(0);
		return BLE_ATT_ERR_UNLIKELY;
	}
}

static int
gatt_svr_dsc_read_write_long_test(uint16_t conn_handle, uint16_t attr_handle,
				  struct ble_gatt_access_ctxt *ctxt,
				  void *arg)
{
	uint16_t uuid16;
	int rc;

	uuid16 = extract_uuid16_from_pts_uuid128(ctxt->chr->uuid);
	assert(uuid16 != 0);

	switch (uuid16) {
	case PTS_LONG_DSC_READ_WRITE:
		if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_DSC) {
			rc = gatt_svr_chr_write(conn_handle, attr_handle,
						ctxt->om, 0,
						sizeof gatt_svr_pts_static_long_val,
						&gatt_svr_pts_static_long_val, NULL);
			return rc;
		} else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_DSC) {
			rc = os_mbuf_append(ctxt->om, &gatt_svr_pts_static_long_val,
					    sizeof gatt_svr_pts_static_long_val);
			return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
		}
	default:
		assert(0);
		return BLE_ATT_ERR_UNLIKELY;
	}
}

static int
gatt_svr_dsc_read_test(uint16_t conn_handle, uint16_t attr_handle,
		       struct ble_gatt_access_ctxt *ctxt,
		       void *arg)
{
	uint16_t uuid16;
	int rc;

	uuid16 = extract_uuid16_from_pts_uuid128(ctxt->chr->uuid);
	assert(uuid16 != 0);

	switch (uuid16) {
	case PTS_DSC_READ:
		if (ctxt->op == BLE_GATT_ACCESS_OP_READ_DSC) {
			rc = os_mbuf_append(ctxt->om, &gatt_svr_pts_static_long_val,
					    sizeof gatt_svr_pts_static_long_val);
			return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
	}
	default:
		assert(0);
		return BLE_ATT_ERR_UNLIKELY;
	}
}

static int
gatt_svr_write_no_rsp_test(uint16_t conn_handle, uint16_t attr_handle,
			   struct ble_gatt_access_ctxt *ctxt,
			   void *arg)
{
	uint16_t uuid16;
	int rc;

	uuid16 = extract_uuid16_from_pts_uuid128(ctxt->chr->uuid);
	assert(uuid16 != 0);

	switch (uuid16) {
	case PTS_CHR_WRITE_NO_RSP:
		if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
			rc = gatt_svr_chr_write(conn_handle, attr_handle,
						ctxt->om, 0,
						sizeof gatt_svr_pts_static_short_val,
						&gatt_svr_pts_static_short_val, NULL);
			return rc;
	} else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
		rc = os_mbuf_append(ctxt->om, &gatt_svr_pts_static_short_val,
				    sizeof gatt_svr_pts_static_short_val);
		return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
	}
	default:
		assert(0);
		return BLE_ATT_ERR_UNLIKELY;
	}
}

static int
gatt_svr_rel_write_test(uint16_t conn_handle, uint16_t attr_handle,
			struct ble_gatt_access_ctxt *ctxt,
			void *arg)
{
	uint16_t uuid16;
	int rc;

	uuid16 = extract_uuid16_from_pts_uuid128(ctxt->chr->uuid);
	assert(uuid16 != 0);

	switch (uuid16) {
	case PTS_CHR_RELIABLE_WRITE:
		if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
			rc = gatt_svr_chr_write(conn_handle, attr_handle,
						ctxt->om, 0,
						sizeof gatt_svr_pts_static_val,
						&gatt_svr_pts_static_val, NULL);
			return rc;
	}

	default:
		assert(0);
		return BLE_ATT_ERR_UNLIKELY;
	}
}

static void start_server(u8_t *data, u16_t len)
{
	struct gatt_start_server_rp rp;

	SYS_LOG_DBG("");

	ble_gatts_show_local();

	ble_svc_gatt_changed(0x0001, 0xffff);

	rp.db_attr_off = 0;
	rp.db_attr_cnt = 0;

	tester_send(BTP_SERVICE_ID_GATT, GATT_START_SERVER, CONTROLLER_INDEX,
		    (u8_t *) &rp, sizeof(rp));
}

/* Convert UUID from BTP command to bt_uuid */
static u8_t btp2bt_uuid(const u8_t *uuid, u8_t len,
			ble_uuid_any_t *bt_uuid)
{
	u16_t le16;

	switch (len) {
	case 0x02: /* UUID 16 */
		bt_uuid->u.type = BLE_UUID_TYPE_16;
		memcpy(&le16, uuid, sizeof(le16));
		BLE_UUID16(bt_uuid)->value = sys_le16_to_cpu(le16);
		break;
	case 0x10: /* UUID 128*/
		bt_uuid->u.type = BLE_UUID_TYPE_128;
		memcpy(BLE_UUID128(bt_uuid)->value, uuid, 16);
	break;
	default:
		return BTP_STATUS_FAILED;
	}
	return BTP_STATUS_SUCCESS;
}

/*
 * gatt_buf - cache used by a gatt client (to cache data read/discovered)
 * and gatt server (to store attribute user_data).
 * It is not intended to be used by client and server at the same time.
 */
static struct {
	u16_t len;
	u8_t buf[MAX_BUFFER_SIZE];
} gatt_buf;

static void *gatt_buf_add(const void *data, size_t len)
{
	void *ptr = gatt_buf.buf + gatt_buf.len;

	if ((len + gatt_buf.len) > MAX_BUFFER_SIZE) {
		return NULL;
	}

	if (data) {
		memcpy(ptr, data, len);
	} else {
		(void)memset(ptr, 0, len);
	}

	gatt_buf.len += len;

	SYS_LOG_DBG("%d/%d used", gatt_buf.len, MAX_BUFFER_SIZE);

	return ptr;
}

static void *gatt_buf_reserve(size_t len)
{
	return gatt_buf_add(NULL, len);
}

static void gatt_buf_clear(void)
{
	(void)memset(&gatt_buf, 0, sizeof(gatt_buf));
}

static void discover_destroy(void)
{
	gatt_buf_clear();
}

static void read_destroy(void)
{
	gatt_buf_clear();
}

static int read_cb(uint16_t conn_handle,
		   const struct ble_gatt_error *error,
		   struct ble_gatt_attr *attr,
		   void *arg)
{
	struct gatt_read_rp *rp = (void *) gatt_buf.buf;
	u8_t btp_opcode = (uint8_t) (int) arg;

	SYS_LOG_DBG("status=%d", error->status);

	if (error->status != 0 && error->status != BLE_HS_EDONE) {
		rp->att_response = (uint8_t) BLE_HS_ATT_ERR(error->status);
		tester_send(BTP_SERVICE_ID_GATT, btp_opcode,
			    CONTROLLER_INDEX, gatt_buf.buf, gatt_buf.len);
		read_destroy();
		return 0;
	}

	if (!gatt_buf_add(attr->om->om_data, attr->om->om_len)) {
		tester_rsp(BTP_SERVICE_ID_GATT, btp_opcode,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);
		read_destroy();
		return 0;
	}

	rp->data_length += attr->om->om_len;
	tester_send(BTP_SERVICE_ID_GATT, btp_opcode,
		    CONTROLLER_INDEX, gatt_buf.buf, gatt_buf.len);
	read_destroy();

	return 0;
}

static void read(u8_t *data, u16_t len)
{
	const struct gatt_read_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	/* Clear buffer */
	read_destroy();

	if (!gatt_buf_reserve(sizeof(struct gatt_read_rp))) {
		goto fail;
	}

	if (ble_gattc_read(conn.conn_handle, sys_le16_to_cpu(cmd->handle),
			   read_cb, (void *)GATT_READ)) {
		read_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_READ, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static int read_long_cb(uint16_t conn_handle,
			const struct ble_gatt_error *error,
			struct ble_gatt_attr *attr,
			void *arg)
{
	struct gatt_read_rp *rp = (void *) gatt_buf.buf;
	u8_t btp_opcode = (uint8_t) (int) arg;

	SYS_LOG_DBG("status=%d", error->status);

	if (error->status != 0 && error->status != BLE_HS_EDONE) {
		rp->att_response = (uint8_t) BLE_HS_ATT_ERR(error->status);
		tester_send(BTP_SERVICE_ID_GATT, btp_opcode,
			    CONTROLLER_INDEX, gatt_buf.buf, gatt_buf.len);
		read_destroy();
		return 0;
	}

	if (error->status == BLE_HS_EDONE) {
		tester_send(BTP_SERVICE_ID_GATT, btp_opcode,
			    CONTROLLER_INDEX, gatt_buf.buf, gatt_buf.len);
		read_destroy();
		return 0;
	}

	if (gatt_buf_add(attr->om->om_data, attr->om->om_len) == NULL) {
		tester_rsp(BTP_SERVICE_ID_GATT, btp_opcode,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);
		read_destroy();
		return BLE_HS_ENOMEM;
	}

	rp->data_length += attr->om->om_len;

	return 0;
}

static void read_long(u8_t *data, u16_t len)
{
	const struct gatt_read_long_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	/* Clear buffer */
	read_destroy();

	if (!gatt_buf_reserve(sizeof(struct gatt_read_rp))) {
		goto fail;
	}

	if (ble_gattc_read_long(conn.conn_handle,
				sys_le16_to_cpu(cmd->handle),
				sys_le16_to_cpu(cmd->offset),
				read_long_cb, (void *)GATT_READ_LONG)) {
		read_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_READ_LONG, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static void read_multiple(u8_t *data, u16_t len)
{
	const struct gatt_read_multiple_cmd *cmd = (void *) data;
	u16_t handles[cmd->handles_count];
	struct ble_gap_conn_desc conn;
	int rc, i;

	SYS_LOG_DBG("");

	for (i = 0; i < ARRAY_SIZE(handles); i++) {
		handles[i] = sys_le16_to_cpu(cmd->handles[i]);
	}

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	/* Clear buffer */
	read_destroy();

	if (!gatt_buf_reserve(sizeof(struct gatt_read_rp))) {
		goto fail;
	}

	if (ble_gattc_read_mult(conn.conn_handle, handles,
				cmd->handles_count, read_cb,
				(void *)GATT_READ_MULTIPLE)) {
		read_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_READ_MULTIPLE, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static void write_without_rsp(u8_t *data, u16_t len, u8_t op, bool sign)
{
	const struct gatt_write_without_rsp_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	u8_t status = BTP_STATUS_SUCCESS;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		status = BTP_STATUS_FAILED;
		goto rsp;
	}

	if (ble_gattc_write_no_rsp_flat(conn.conn_handle,
					sys_le16_to_cpu(cmd->handle), cmd->data,
					sys_le16_to_cpu(cmd->data_length))) {
		status = BTP_STATUS_FAILED;
	}

rsp:
	tester_rsp(BTP_SERVICE_ID_GATT, op, CONTROLLER_INDEX, status);
}

static int write_rsp(uint16_t conn_handle, const struct ble_gatt_error *error,
		     struct ble_gatt_attr *attr,
		     void *arg)
{
	uint8_t err = (uint8_t) error->status;
	u8_t btp_opcode = (uint8_t) (int) arg;

	SYS_LOG_DBG("");

	tester_send(BTP_SERVICE_ID_GATT, btp_opcode,
		    CONTROLLER_INDEX, &err, sizeof(err));
	return 0;
}

static void write(u8_t *data, u16_t len)
{
	const struct gatt_write_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	if (ble_gattc_write_flat(conn.conn_handle, sys_le16_to_cpu(cmd->handle),
				 cmd->data, sys_le16_to_cpu(cmd->data_length),
				 write_rsp, (void *) GATT_WRITE)) {
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_WRITE, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static void write_long(u8_t *data, u16_t len)
{
	const struct gatt_write_long_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	struct os_mbuf *om = NULL;
	int rc = 0;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	om = ble_hs_mbuf_from_flat(cmd->data, sys_le16_to_cpu(cmd->data_length));
	if (!om) {
		SYS_LOG_ERR("Insufficient resources");
		goto fail;
	}

	rc = ble_gattc_write_long(conn.conn_handle,
				  sys_le16_to_cpu(cmd->handle),
				  sys_le16_to_cpu(cmd->offset),
				  om, write_rsp,
				  (void *) GATT_WRITE_LONG);
	if (!rc) {
		return;
	}

fail:
	SYS_LOG_ERR("Failed to send Write Long request, rc=%d", rc);
	os_mbuf_free_chain(om);
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_WRITE_LONG, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static int reliable_write_rsp(uint16_t conn_handle,
			      const struct ble_gatt_error *error,
			      struct ble_gatt_attr *attrs,
			      uint8_t num_attrs,
			      void *arg)
{
	uint8_t err = (uint8_t) error->status;

	SYS_LOG_DBG("Reliable write status %d", err);

	tester_send(BTP_SERVICE_ID_GATT, GATT_RELIABLE_WRITE,
		    CONTROLLER_INDEX, &err, sizeof(err));
	return 0;
}

static void reliable_write(u8_t *data, u16_t len)
{
	const struct gatt_reliable_write_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	struct ble_gatt_attr attr;
	struct os_mbuf *om = NULL;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	om = ble_hs_mbuf_from_flat(cmd->data, sys_le16_to_cpu(cmd->data_length));
	/* This is required, because Nimble checks if
	* the data is longer than offset
	*/
	if (os_mbuf_extend(om, sys_le16_to_cpu(cmd->offset) + 1) == NULL) {
		goto fail;
	}

	attr.handle = sys_le16_to_cpu(cmd->handle);
	attr.offset = sys_le16_to_cpu(cmd->offset);
	attr.om = om;

	if (ble_gattc_write_reliable(conn.conn_handle, &attr, 1,
				     reliable_write_rsp, NULL)) {
		goto fail;
	}

	return;

fail:
	os_mbuf_free_chain(om);

	tester_rsp(BTP_SERVICE_ID_GATT, GATT_WRITE_LONG, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static struct bt_gatt_subscribe_params {
	u16_t ccc_handle;
	u16_t value;
	u16_t value_handle;
} subscribe_params;

static void read_uuid(u8_t *data, u16_t len)
{
	const struct gatt_read_uuid_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	ble_uuid_any_t uuid;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	if (btp2bt_uuid(cmd->uuid, cmd->uuid_length, &uuid)) {
		goto fail;
	}

	/* Clear buffer */
	read_destroy();

	if (!gatt_buf_reserve(sizeof(struct gatt_read_rp))) {
		goto fail;
	}

	if (ble_gattc_read_by_uuid(conn.conn_handle,
				   sys_le16_to_cpu(cmd->start_handle),
				   sys_le16_to_cpu(cmd->end_handle), &uuid.u,
				   read_long_cb, (void *)GATT_READ_UUID)) {
		read_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_READ, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static int disc_prim_uuid_cb(uint16_t conn_handle,
			     const struct ble_gatt_error *error,
			     const struct ble_gatt_svc *gatt_svc, void *arg)
{
	struct gatt_disc_prim_uuid_rp *rp = (void *) gatt_buf.buf;
	struct gatt_service *service;
	const ble_uuid_any_t *uuid;
	u8_t uuid_length;
	u8_t opcode = (u8_t) (int) arg;

	SYS_LOG_DBG("");

	if (error->status != 0 && error->status != BLE_HS_EDONE) {
		tester_rsp(BTP_SERVICE_ID_GATT, opcode,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);
		discover_destroy();
		return 0;
	}

	if (error->status == BLE_HS_EDONE) {
		tester_send(BTP_SERVICE_ID_GATT, opcode,
			    CONTROLLER_INDEX, gatt_buf.buf, gatt_buf.len);
		discover_destroy();
		return 0;
	}

	uuid = &gatt_svc->uuid;
	uuid_length = (uint8_t) (uuid->u.type == BLE_UUID_TYPE_16 ? 2 : 16);

	service = gatt_buf_reserve(sizeof(*service) + uuid_length);
	if (!service) {
		tester_rsp(BTP_SERVICE_ID_GATT, opcode,
		CONTROLLER_INDEX, BTP_STATUS_FAILED);
		discover_destroy();
		return BLE_HS_ENOMEM;
	}

	service->start_handle = sys_cpu_to_le16(gatt_svc->start_handle);
	service->end_handle = sys_cpu_to_le16(gatt_svc->end_handle);
	service->uuid_length = uuid_length;

	if (uuid->u.type == BLE_UUID_TYPE_16) {
		u16_t u16 = sys_cpu_to_le16(BLE_UUID16(uuid)->value);
		memcpy(service->uuid, &u16, uuid_length);
	} else {
		memcpy(service->uuid, BLE_UUID128(uuid)->value,
		uuid_length);
	}

	rp->services_count++;

	return 0;
}

static int disc_all_desc_cb(uint16_t conn_handle,
			    const struct ble_gatt_error *error,
			    uint16_t chr_val_handle,
			    const struct ble_gatt_dsc *gatt_dsc,
			    void *arg)
{
	struct gatt_disc_all_desc_rp *rp = (void *) gatt_buf.buf;
	struct gatt_descriptor *dsc;
	const ble_uuid_any_t *uuid;
	u8_t uuid_length;

	SYS_LOG_DBG("");

	if (error->status != 0 && error->status != BLE_HS_EDONE) {
		tester_rsp(BTP_SERVICE_ID_GATT, GATT_DISC_ALL_DESC,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);
		discover_destroy();
		return 0;
	}

	if (error->status == BLE_HS_EDONE) {
		tester_send(BTP_SERVICE_ID_GATT, GATT_DISC_ALL_DESC,
			    CONTROLLER_INDEX, gatt_buf.buf, gatt_buf.len);
		discover_destroy();
		return 0;
	}

	uuid = &gatt_dsc->uuid;
	uuid_length = (uint8_t) (uuid->u.type == BLE_UUID_TYPE_16 ? 2 : 16);

	dsc = gatt_buf_reserve(sizeof(*dsc) + uuid_length);
	if (!dsc) {
		tester_rsp(BTP_SERVICE_ID_GATT, GATT_DISC_ALL_DESC,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);
		discover_destroy();
		return BLE_HS_ENOMEM;
	}

	dsc->descriptor_handle = sys_cpu_to_le16(gatt_dsc->handle);
	dsc->uuid_length = uuid_length;

	if (uuid->u.type == BLE_UUID_TYPE_16) {
		u16_t u16 = sys_cpu_to_le16(BLE_UUID16(uuid)->value);
		memcpy(dsc->uuid, &u16, uuid_length);
	} else {
		memcpy(dsc->uuid, BLE_UUID128(uuid)->value, uuid_length);
	}

	rp->descriptors_count++;

	return 0;
}

static void disc_all_prim_svcs(u8_t *data, u16_t len)
{
	struct ble_gap_conn_desc conn;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	if (!gatt_buf_reserve(sizeof(struct gatt_disc_all_prim_svcs_rp))) {
		goto fail;
	}

	if (ble_gattc_disc_all_svcs(conn.conn_handle, disc_prim_uuid_cb,
				    (void *) GATT_DISC_ALL_PRIM_SVCS)) {
		discover_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_DISC_ALL_PRIM_SVCS,
		   CONTROLLER_INDEX, BTP_STATUS_FAILED);
}

static void disc_all_desc(u8_t *data, u16_t len)
{
	const struct gatt_disc_all_desc_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	uint16_t start_handle, end_handle;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	if (!gatt_buf_reserve(sizeof(struct gatt_disc_all_desc_rp))) {
		goto fail;
	}

	start_handle = sys_le16_to_cpu(cmd->start_handle) - 1;
	end_handle = sys_le16_to_cpu(cmd->end_handle);

	rc = ble_gattc_disc_all_dscs(conn.conn_handle, start_handle, end_handle,
	disc_all_desc_cb, NULL);

	SYS_LOG_DBG("rc=%d", rc);

	if (rc) {
		discover_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_DISC_ALL_DESC, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static int find_included_cb(uint16_t conn_handle,
			    const struct ble_gatt_error *error,
			    const struct ble_gatt_svc *gatt_svc, void *arg)
{
	struct gatt_find_included_rp *rp = (void *) gatt_buf.buf;
	struct gatt_included *included;
	const ble_uuid_any_t *uuid;
	int service_handle = (int) arg;
	u8_t uuid_length;

	SYS_LOG_DBG("");

	if (error->status != 0 && error->status != BLE_HS_EDONE) {
		tester_rsp(BTP_SERVICE_ID_GATT, GATT_FIND_INCLUDED,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);
		discover_destroy();
		return 0;
	}

	if (error->status == BLE_HS_EDONE) {
		tester_send(BTP_SERVICE_ID_GATT, GATT_FIND_INCLUDED,
			    CONTROLLER_INDEX, gatt_buf.buf, gatt_buf.len);
		discover_destroy();
		return 0;
	}

	uuid = &gatt_svc->uuid;
	uuid_length = (uint8_t) (uuid->u.type == BLE_UUID_TYPE_16 ? 2 : 16);


	included = gatt_buf_reserve(sizeof(*included) + uuid_length);
	if (!included) {
		tester_rsp(BTP_SERVICE_ID_GATT, GATT_FIND_INCLUDED,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);
		discover_destroy();
		return BLE_HS_ENOMEM;
	}

	/* FIXME */
	included->included_handle = sys_cpu_to_le16(service_handle + 1 +
	rp->services_count);
	included->service.start_handle = sys_cpu_to_le16(gatt_svc->start_handle);
	included->service.end_handle = sys_cpu_to_le16(gatt_svc->end_handle);
	included->service.uuid_length = uuid_length;

	if (uuid->u.type == BLE_UUID_TYPE_16) {
		u16_t u16 = sys_cpu_to_le16(BLE_UUID16(uuid)->value);
		memcpy(included->service.uuid, &u16, uuid_length);
	} else {
		memcpy(included->service.uuid, BLE_UUID128(uuid)->value,
		       uuid_length);
	}

	rp->services_count++;

	return 0;
}

static int disc_chrc_cb(uint16_t conn_handle,
			const struct ble_gatt_error *error,
			const struct ble_gatt_chr *gatt_chr, void *arg)
{
	struct gatt_disc_chrc_rp *rp = (void *) gatt_buf.buf;
	struct gatt_characteristic *chrc;
	const ble_uuid_any_t *uuid;
	u8_t btp_opcode = (uint8_t) (int) arg;
	u8_t uuid_length;

	SYS_LOG_DBG("");

	if (error->status != 0 && error->status != BLE_HS_EDONE) {
		tester_rsp(BTP_SERVICE_ID_GATT, btp_opcode,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);
		discover_destroy();
		return 0;
	}

	if (error->status == BLE_HS_EDONE) {
		tester_send(BTP_SERVICE_ID_GATT, btp_opcode,
			    CONTROLLER_INDEX, gatt_buf.buf, gatt_buf.len);
		discover_destroy();
		return 0;
	}

	uuid = &gatt_chr->uuid;
	uuid_length = (uint8_t) (uuid->u.type == BLE_UUID_TYPE_16 ? 2 : 16);

	chrc = gatt_buf_reserve(sizeof(*chrc) + uuid_length);
	if (!chrc) {
		tester_rsp(BTP_SERVICE_ID_GATT, btp_opcode,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);
		discover_destroy();
		return BLE_HS_ENOMEM;
	}

	chrc->characteristic_handle = sys_cpu_to_le16(gatt_chr->def_handle);
	chrc->properties = gatt_chr->properties;
	chrc->value_handle = sys_cpu_to_le16(gatt_chr->val_handle);
	chrc->uuid_length = uuid_length;

	if (uuid->u.type == BLE_UUID_TYPE_16) {
		u16_t u16 = sys_cpu_to_le16(BLE_UUID16(uuid)->value);
		memcpy(chrc->uuid, &u16, uuid_length);
	} else {
		memcpy(chrc->uuid, BLE_UUID128(uuid)->value,
		uuid_length);
	}

	rp->characteristics_count++;

	return 0;
}

static void disc_chrc_uuid(u8_t *data, u16_t len)
{
	const struct gatt_disc_chrc_uuid_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	uint16_t start_handle, end_handle;
	ble_uuid_any_t uuid;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	if (btp2bt_uuid(cmd->uuid, cmd->uuid_length, &uuid)) {
		goto fail;
	}

	if (!gatt_buf_reserve(sizeof(struct gatt_disc_chrc_rp))) {
		goto fail;
	}

	start_handle = sys_le16_to_cpu(cmd->start_handle);
	end_handle = sys_le16_to_cpu(cmd->end_handle);

	if (ble_gattc_disc_chrs_by_uuid(conn.conn_handle, start_handle,
					end_handle, &uuid.u, disc_chrc_cb,
					(void *)GATT_DISC_CHRC_UUID)) {
		discover_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_DISC_CHRC_UUID, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static void disc_prim_uuid(u8_t *data, u16_t len)
{
	const struct gatt_disc_prim_uuid_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	ble_uuid_any_t uuid;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	if (btp2bt_uuid(cmd->uuid, cmd->uuid_length, &uuid)) {
		goto fail;
	}

	if (!gatt_buf_reserve(sizeof(struct gatt_disc_prim_uuid_rp))) {
		goto fail;
	}

	if (ble_gattc_disc_svc_by_uuid(conn.conn_handle,
				       &uuid.u, disc_prim_uuid_cb,
				       (void *) GATT_DISC_PRIM_UUID)) {
		discover_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_DISC_PRIM_UUID, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static void disc_all_chrc(u8_t *data, u16_t len)
{
	const struct gatt_disc_all_chrc_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	uint16_t start_handle, end_handle;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		SYS_LOG_DBG("Conn find failed");
		goto fail;
	}

	if (!gatt_buf_reserve(sizeof(struct gatt_disc_chrc_rp))) {
		SYS_LOG_DBG("Buf reserve failed");
		goto fail;
	}

	start_handle = sys_le16_to_cpu(cmd->start_handle);
	end_handle = sys_le16_to_cpu(cmd->end_handle);

	rc = ble_gattc_disc_all_chrs(conn.conn_handle, start_handle, end_handle,
	disc_chrc_cb, (void *)GATT_DISC_ALL_CHRC);
	if (rc) {
		discover_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_DISC_ALL_CHRC, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static void find_included(u8_t *data, u16_t len)
{
	const struct gatt_find_included_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	uint16_t start_handle, end_handle;
	int service_handle_arg;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	if (!gatt_buf_reserve(sizeof(struct gatt_find_included_rp))) {
		goto fail;
	}

	start_handle = sys_le16_to_cpu(cmd->start_handle);
	end_handle = sys_le16_to_cpu(cmd->end_handle);
	service_handle_arg = start_handle;

	if (ble_gattc_find_inc_svcs(conn.conn_handle, start_handle, end_handle,
				    find_included_cb,
				    (void *)service_handle_arg)) {
		discover_destroy();
		goto fail;
	}

	return;

fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_FIND_INCLUDED, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
}

static int exchange_func(uint16_t conn_handle,
			 const struct ble_gatt_error *error,
			 uint16_t mtu, void *arg)
{
	SYS_LOG_DBG("");

	if (error->status) {
		tester_rsp(BTP_SERVICE_ID_GATT, GATT_EXCHANGE_MTU,
			   CONTROLLER_INDEX, BTP_STATUS_FAILED);

	return 0;
}

	tester_rsp(BTP_SERVICE_ID_GATT, GATT_EXCHANGE_MTU, CONTROLLER_INDEX,
		   BTP_STATUS_SUCCESS);

	return 0;
}

static void exchange_mtu(u8_t *data, u16_t len)
{
	struct ble_gap_conn_desc conn;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		goto fail;
	}

	if (ble_gattc_exchange_mtu(conn.conn_handle, exchange_func, NULL)) {
		goto fail;
	}

	return;
fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_EXCHANGE_MTU,
		   CONTROLLER_INDEX, BTP_STATUS_FAILED);
}

static int enable_subscription(u16_t conn_handle, u16_t ccc_handle,
			       u16_t value)
{
	u8_t op;

	SYS_LOG_DBG("");

	op = (uint8_t) (value == 0x0001 ? GATT_CFG_NOTIFY : GATT_CFG_INDICATE);

	if (ble_gattc_write_flat(conn_handle, ccc_handle,
				 &value, sizeof(value), NULL, NULL)) {
		return -EINVAL;
	}

	subscribe_params.ccc_handle = value;

	tester_rsp(BTP_SERVICE_ID_GATT, op, CONTROLLER_INDEX,
		   BTP_STATUS_SUCCESS);
	return 0;
}

static int disable_subscription(u16_t conn_handle, u16_t ccc_handle)
{
	u16_t value = 0x00;

	SYS_LOG_DBG("");

	/* Fail if CCC handle doesn't match */
	if (ccc_handle != subscribe_params.ccc_handle) {
		SYS_LOG_ERR("CCC handle doesn't match");
		return -EINVAL;
	}

	if (ble_gattc_write_no_rsp_flat(conn_handle, ccc_handle,
					&value, sizeof(value))) {
		return -EINVAL;
	}

	subscribe_params.ccc_handle = 0;
	return 0;
}

static void config_subscription(u8_t *data, u16_t len, u8_t op)
{
	const struct gatt_cfg_notify_cmd *cmd = (void *) data;
	struct ble_gap_conn_desc conn;
	u16_t ccc_handle = sys_le16_to_cpu(cmd->ccc_handle);
	u8_t status;
	int rc;

	SYS_LOG_DBG("");

	rc = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);
	if (rc) {
		tester_rsp(BTP_SERVICE_ID_GATT, op, CONTROLLER_INDEX,
			   BTP_STATUS_FAILED);
		return;
	}

	if (cmd->enable) {
		u16_t value;

		if (op == GATT_CFG_NOTIFY) {
			value = 0x0001;
		} else {
			value = 0x0002;
		}

		/* on success response will be sent from callback */
		if (enable_subscription(conn.conn_handle,
					ccc_handle, value) == 0) {
			return;
		}

		status = BTP_STATUS_FAILED;
	} else {
		if (disable_subscription(conn.conn_handle, ccc_handle) < 0) {
			status = BTP_STATUS_FAILED;
		} else {
			status = BTP_STATUS_SUCCESS;
		}
	}

	SYS_LOG_DBG("Config subscription (op %u) status %u", op, status);

	tester_rsp(BTP_SERVICE_ID_GATT, op, CONTROLLER_INDEX, status);
}

#define BTP_PERM_F_READ                      0x01
#define BTP_PERM_F_WRITE                     0x02
#define BTP_PERM_F_READ_ENC                  0x04
#define BTP_PERM_F_WRITE_ENC                 0x08
#define BTP_PERM_F_READ_AUTHEN               0x10
#define BTP_PERM_F_WRITE_AUTHEN              0x20
#define BTP_PERM_F_READ_AUTHOR               0x40
#define BTP_PERM_F_WRITE_AUTHOR              0x80

static int flags_hs2btp_map[] = {
	BTP_PERM_F_READ,
	BTP_PERM_F_WRITE,
	BTP_PERM_F_READ_ENC,
	BTP_PERM_F_READ_AUTHEN,
	BTP_PERM_F_READ_AUTHOR,
	BTP_PERM_F_WRITE_ENC,
	BTP_PERM_F_WRITE_AUTHEN,
	BTP_PERM_F_WRITE_AUTHOR,
};

static u8_t flags_hs2btp(u8_t flags)
{
	int i;
	u8_t ret = 0;

	for (i = 0; i < 8; ++i) {
		if (flags & BIT(i)) {
			ret |= flags_hs2btp_map[i];
		}
	}

	return ret;
}

static void get_attrs(u8_t *data, u16_t len)
{
	const struct gatt_get_attributes_cmd *cmd = (void *) data;
	struct gatt_get_attributes_rp *rp;
	struct gatt_attr *gatt_attr;
	struct os_mbuf *buf = os_msys_get(0, 0);
	u16_t start_handle, end_handle;
	struct ble_att_svr_entry *entry = NULL;
	ble_uuid_any_t uuid;
	ble_uuid_t *uuid_ptr = NULL;
	u8_t count = 0;
	char str[BLE_UUID_STR_LEN];

	SYS_LOG_DBG("");

	memset(str, 0, sizeof(str));
	memset(&uuid, 0, sizeof(uuid));
	start_handle = sys_le16_to_cpu(cmd->start_handle);
	end_handle = sys_le16_to_cpu(cmd->end_handle);

	if (cmd->type_length) {
		if (btp2bt_uuid(cmd->type, cmd->type_length, &uuid)) {
			goto fail;
		}

		ble_uuid_to_str(&uuid.u, str);
		SYS_LOG_DBG("start 0x%04x end 0x%04x, uuid %s", start_handle,
			    end_handle, str);

		uuid_ptr = &uuid.u;
	} else {
		SYS_LOG_DBG("start 0x%04x end 0x%04x", start_handle, end_handle);
	}

	net_buf_simple_init(buf, 0);
	rp = net_buf_simple_add(buf, sizeof(*rp));

	entry = ble_att_svr_find_by_uuid(entry, uuid_ptr, end_handle);
	while (entry) {

		if (entry->ha_handle_id < start_handle) {
			entry = ble_att_svr_find_by_uuid(entry,
			uuid_ptr, end_handle);
			continue;
		}

		gatt_attr = net_buf_simple_add(buf, sizeof(*gatt_attr));
		gatt_attr->handle = sys_cpu_to_le16(entry->ha_handle_id);
		gatt_attr->permission = flags_hs2btp(entry->ha_flags);

		if (entry->ha_uuid->type == BLE_UUID_TYPE_16) {
			gatt_attr->type_length = 2;
			net_buf_simple_add_le16(buf,
			BLE_UUID16(entry->ha_uuid)->value);
		} else {
			gatt_attr->type_length = 16;
			net_buf_simple_add_mem(buf,
			BLE_UUID128(entry->ha_uuid)->value,
			gatt_attr->type_length);
		}

		count++;

		entry = ble_att_svr_find_by_uuid(entry, uuid_ptr, end_handle);
	}

	rp->attrs_count = count;

	tester_send_buf(BTP_SERVICE_ID_GATT, GATT_GET_ATTRIBUTES,
			CONTROLLER_INDEX, buf);

	goto free;
fail:
	tester_rsp(BTP_SERVICE_ID_GATT, GATT_GET_ATTRIBUTES, CONTROLLER_INDEX,
		   BTP_STATUS_FAILED);
free:
	os_mbuf_free_chain(buf);
}

static void get_attr_val(u8_t *data, u16_t len)
{
	const struct gatt_get_attribute_value_cmd *cmd = (void *) data;
	struct gatt_get_attribute_value_rp *rp;
	struct ble_gap_conn_desc conn;
	struct os_mbuf *buf = os_msys_get(0, 0);
	u16_t handle = sys_cpu_to_le16(cmd->handle);
	uint8_t out_att_err;
	int conn_status;

	conn_status = ble_gap_conn_find_by_addr((ble_addr_t *)data, &conn);

	if (conn_status) {
		net_buf_simple_init(buf, 0);
		rp = net_buf_simple_add(buf, sizeof(*rp));

		ble_att_svr_read_handle(BLE_HS_CONN_HANDLE_NONE,
					handle, 0, buf,
					&out_att_err);

		rp->att_response = out_att_err;
		rp->value_length = os_mbuf_len(buf) - sizeof(*rp);

		tester_send_buf(BTP_SERVICE_ID_GATT, GATT_GET_ATTRIBUTE_VALUE,
				CONTROLLER_INDEX, buf);

		goto free;
	} else {
		net_buf_simple_init(buf, 0);
		rp = net_buf_simple_add(buf, sizeof(*rp));

		ble_att_svr_read_handle(conn.conn_handle,
		handle, 0, buf,
		&out_att_err);

		rp->att_response = out_att_err;
		rp->value_length = os_mbuf_len(buf) - sizeof(*rp);

		tester_send_buf(BTP_SERVICE_ID_GATT, GATT_GET_ATTRIBUTE_VALUE,
				CONTROLLER_INDEX, buf);

		goto free;
	}

free:
	os_mbuf_free_chain(buf);
}

static void change_database(u8_t *data, u16_t len)
{
	const struct gatt_change_database *cmd = (void *) data;

	SYS_LOG_DBG("")

	ble_gatts_show_local();

	ble_svc_gatt_changed(cmd->start_handle, cmd->end_handle);

	tester_rsp(BTP_SERVICE_ID_GATT, GATT_CHANGE_DATABASE, CONTROLLER_INDEX,
		   BTP_STATUS_SUCCESS);

	return;
}

static void supported_commands(u8_t *data, u16_t len)
{
	u8_t cmds[4];
	struct gatt_read_supported_commands_rp *rp = (void *) cmds;

	SYS_LOG_DBG("");

	memset(cmds, 0, sizeof(cmds));

	tester_set_bit(cmds, GATT_READ_SUPPORTED_COMMANDS);
	tester_set_bit(cmds, GATT_START_SERVER);
	tester_set_bit(cmds, GATT_EXCHANGE_MTU);
	tester_set_bit(cmds, GATT_DISC_ALL_PRIM_SVCS);
	tester_set_bit(cmds, GATT_DISC_PRIM_UUID);
	tester_set_bit(cmds, GATT_FIND_INCLUDED);
	tester_set_bit(cmds, GATT_DISC_ALL_CHRC);
	tester_set_bit(cmds, GATT_DISC_CHRC_UUID);
	tester_set_bit(cmds, GATT_DISC_ALL_DESC);
	tester_set_bit(cmds, GATT_READ);
	tester_set_bit(cmds, GATT_READ_LONG);
	tester_set_bit(cmds, GATT_READ_MULTIPLE);
	tester_set_bit(cmds, GATT_WRITE_WITHOUT_RSP);
#if 0
	tester_set_bit(cmds, GATT_SIGNED_WRITE_WITHOUT_RSP);
#endif
	tester_set_bit(cmds, GATT_WRITE);
	tester_set_bit(cmds, GATT_WRITE_LONG);
	tester_set_bit(cmds, GATT_CFG_NOTIFY);
	tester_set_bit(cmds, GATT_CFG_INDICATE);
	tester_set_bit(cmds, GATT_GET_ATTRIBUTES);
	tester_set_bit(cmds, GATT_GET_ATTRIBUTE_VALUE);
	tester_set_bit(cmds, GATT_CHANGE_DATABASE);

	tester_send(BTP_SERVICE_ID_GATT, GATT_READ_SUPPORTED_COMMANDS,
	CONTROLLER_INDEX, (u8_t *) rp, sizeof(cmds));
}

enum attr_type {
	BLE_GATT_ATTR_SVC = 0,
	BLE_GATT_ATTR_CHR,
	BLE_GATT_ATTR_DSC,
};

void tester_handle_gatt(u8_t opcode, u8_t index, u8_t *data,
                                                u16_t len)
{
	switch (opcode) {
	case GATT_READ_SUPPORTED_COMMANDS:
		supported_commands(data, len);
		return;
	case GATT_START_SERVER:
		start_server(data, len);
		return;
	case GATT_EXCHANGE_MTU:
		exchange_mtu(data, len);
		return;
	case GATT_DISC_ALL_PRIM_SVCS:
		disc_all_prim_svcs(data, len);
		return;
	case GATT_DISC_PRIM_UUID:
		disc_prim_uuid(data, len);
		return;
	case GATT_FIND_INCLUDED:
		find_included(data, len);
		return;
	case GATT_DISC_ALL_CHRC:
		disc_all_chrc(data, len);
		return;
	case GATT_DISC_CHRC_UUID:
		disc_chrc_uuid(data, len);
		return;
	case GATT_DISC_ALL_DESC:
		disc_all_desc(data, len);
		return;
	case GATT_CHANGE_DATABASE:
		change_database(data, len);
		return;
	case GATT_READ:
		read(data, len);
		return;
	case GATT_READ_UUID:
		read_uuid(data, len);
		return;
	case GATT_READ_LONG:
		read_long(data, len);
		return;
	case GATT_READ_MULTIPLE:
		read_multiple(data, len);
		return;
	case GATT_WRITE_WITHOUT_RSP:
		write_without_rsp(data, len, opcode, false);
		return;
#if 0
	case GATT_SIGNED_WRITE_WITHOUT_RSP:
		write_without_rsp(data, len, opcode, true);
		return;
#endif
	case GATT_WRITE:
		write(data, len);
		return;
	case GATT_WRITE_LONG:
		write_long(data, len);
		return;
	case GATT_RELIABLE_WRITE:
		reliable_write(data, len);
		return;
	case GATT_CFG_NOTIFY:
	case GATT_CFG_INDICATE:
		config_subscription(data, len, opcode);
		return;
	case GATT_GET_ATTRIBUTES:
		get_attrs(data, len);
		return;
	case GATT_GET_ATTRIBUTE_VALUE:
		get_attr_val(data, len);
		return;
	default:
		tester_rsp(BTP_SERVICE_ID_GATT, opcode, index,
			   BTP_STATUS_UNKNOWN_CMD);
	return;
	}
}

int tester_gatt_notify_rx_ev(u16_t conn_handle, u16_t attr_handle,
			     u8_t indication, struct os_mbuf *om)
{
	struct gatt_notification_ev *ev;
	struct ble_gap_conn_desc conn;
	struct os_mbuf *buf = os_msys_get(0, 0);
	const ble_addr_t *addr;

	SYS_LOG_DBG("");

	if (!subscribe_params.ccc_handle) {
		goto fail;
	}

	if (ble_gap_conn_find(conn_handle, &conn)) {
		goto fail;
	}

	net_buf_simple_init(buf, 0);
	ev = net_buf_simple_add(buf, sizeof(*ev));

	addr = &conn.peer_ota_addr;

	ev->address_type = addr->type;
	memcpy(ev->address, addr->val, sizeof(ev->address));
	ev->type = (u8_t) (indication ? 0x02 : 0x01);
	ev->handle = sys_cpu_to_le16(attr_handle);
	ev->data_length = sys_cpu_to_le16(os_mbuf_len(om));
	os_mbuf_appendfrom(buf, om, 0, os_mbuf_len(om));

	tester_send_buf(BTP_SERVICE_ID_GATT, GATT_EV_NOTIFICATION,
			CONTROLLER_INDEX, buf);

fail:
	os_mbuf_free_chain(buf);
	return 0;
}

void notify_test_stop(void)
{
	os_callout_stop(&notify_tx_timer);
}

void notify_test_reset(void)
{
	int rc;

	rc = os_callout_reset(&notify_tx_timer, OS_TICKS_PER_SEC);
	assert(rc == 0);
}

void notify_test(struct os_event *ev)
{
	static uint8_t ntf[1];
	struct os_mbuf *om;
	int rc;

	if (!notify_state && !indicate_state) {
		notify_test_stop();
		notify_value = 90;
		return;
	}

	ntf[0] = notify_value;

	notify_value++;
	if (notify_value == 160) {
		notify_value = 90;
	}

	om = ble_hs_mbuf_from_flat(ntf, sizeof(ntf));

	if (notify_state) {
		rc = ble_gattc_notify_custom(myconn_handle, notify_handle, om);
		assert(rc == 0);
	}

	if (indicate_state) {
		rc = ble_gattc_indicate_custom(myconn_handle, notify_handle, om);
		assert(rc == 0);
	}
}

int tester_gatt_subscribe_ev(u16_t conn_handle, u16_t attr_handle, u8_t reason,
			     u8_t prev_notify, u8_t cur_notify,
			     u8_t prev_indicate, u8_t cur_indicate)
{
	SYS_LOG_DBG("");
	myconn_handle = conn_handle;

	if (cur_notify == 0 && cur_indicate == 0) {
		SYS_LOG_INF("Unsubscribed");
		memset(&subscribe_params, 0, sizeof(subscribe_params));
		return 0;
	}

	if (cur_notify) {
		SYS_LOG_INF("Subscribed to notifications");
		if (attr_handle == notify_handle) {
			notify_state = cur_notify;
		}
	}

	if (cur_indicate) {
		SYS_LOG_INF("Subscribed to indications");
		if (attr_handle == notify_handle) {
			indicate_state = cur_indicate;
		}
	}


	if (notify_state || indicate_state) {
		notify_test_reset();
	} else {
		notify_test_stop();
	}

	return 0;
}

void
gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
	char buf[BLE_UUID_STR_LEN];

	switch (ctxt->op) {
	case BLE_GATT_REGISTER_OP_SVC:
		MODLOG_DFLT(DEBUG, "registered service %s with handle=%d\n",
		ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
				ctxt->svc.handle);
		break;

	case BLE_GATT_REGISTER_OP_CHR:
		MODLOG_DFLT(DEBUG, "registering characteristic %s with "
			    "def_handle=%d val_handle=%d\n",
			    ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
			    ctxt->chr.def_handle,
			    ctxt->chr.val_handle);
	break;

	case BLE_GATT_REGISTER_OP_DSC:
		MODLOG_DFLT(DEBUG, "registering descriptor %s with handle=%d\n",
			    ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
			    ctxt->dsc.handle);
		break;

	default:
		assert(0);
		break;
	}
}

int gatt_svr_init(void)
{
	int rc;

	rc = ble_gatts_count_cfg(gatt_svr_inc_svcs);
	if (rc != 0) {
		return rc;
	}

	rc = ble_gatts_add_svcs(gatt_svr_inc_svcs);
	if (rc != 0) {
		return rc;
	}

	rc = ble_gatts_count_cfg(gatt_svr_svcs);
	if (rc != 0) {
		return rc;
	}

	rc = ble_gatts_add_svcs(gatt_svr_svcs);
	if (rc != 0) {
		return rc;
	}

	return 0;
}

u8_t tester_init_gatt(void)
{
	os_callout_init(&notify_tx_timer, os_eventq_dflt_get(),
			notify_test, NULL);

	return BTP_STATUS_SUCCESS;
}

u8_t tester_unregister_gatt(void)
{
	return BTP_STATUS_SUCCESS;
}
