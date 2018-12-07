/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <misc/util.h>
#include <misc/byteorder.h>

#include <net/buf.h>

#include <bluetooth/hci.h>
#include <api/mesh.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_TRANS)
#include "common/log.h"

#include "mesh_crypto.h"
#include "adv.h"
#include "mesh.h"
#include "net.h"
#include "lpn.h"
#include "friend.h"
#include "access.h"
#include "foundation.h"
#include "transport.h"
#include "bt_mesh_custom_log.h"

#define AID_MASK                    ((u8_t)(BIT_MASK(6)))

#define SEG(data)                   ((data)[0] >> 7)
#define AKF(data)                   (((data)[0] >> 6) & 0x01)
#define AID(data)                   ((data)[0] & AID_MASK)
#define ASZMIC(data)                (((data)[1] >> 7) & 1)

#define APP_MIC_LEN(aszmic)         ((aszmic) ? 8 : 4)

#define UNSEG_HDR(akf, aid)         ((akf << 6) | (aid & AID_MASK))
#define SEG_HDR(akf, aid)           (UNSEG_HDR(akf, aid) | 0x80)

#define BLOCK_COMPLETE(seg_n)       (u32_t)(((u64_t)1 << (seg_n + 1)) - 1)

#define SEQ_AUTH(iv_index, seq)     (((u64_t)iv_index) << 24 | (u64_t)seq)

/* Retransmit timeout after which to retransmit unacked segments */
#define SEG_RETRANSMIT_TIMEOUT      K_MSEC(400)

/* Number of retransmit attempts (after the initial transmit) per segment */
#define SEG_RETRANSMIT_ATTEMPTS     4

/* How long to wait for available buffers before giving up */
#define BUF_TIMEOUT                 K_NO_WAIT

static struct seg_tx {
	struct bt_mesh_subnet   *sub;
	struct net_buf          *seg[BT_MESH_TX_SEG_COUNT];
	u64_t                    seq_auth;
	u16_t                    dst;
	u8_t                     seg_n:5,       /* Last segment index */
				 new_key:1;     /* New/old key */
	u8_t                     nack_count;    /* Number of unacked segs */
	const struct bt_mesh_send_cb *cb;
	void                    *cb_data;
	struct k_delayed_work    retransmit;    /* Retransmit timer */
} seg_tx[CONFIG_BT_MESH_TX_SEG_MSG_COUNT];

static struct seg_rx {
	struct bt_mesh_subnet   *sub;
	u64_t                    seq_auth;
	u8_t                     seg_n:5,
				 ctl:1,
				 in_use:1,
				 obo:1;
	u8_t                     hdr;
	u8_t                     ttl;
	u16_t                    src;
	u16_t                    dst;
	u32_t                    block;
	u32_t                    last;
	struct k_delayed_work    ack;
	struct net_buf_simple    buf;
	u8_t                     buf_data[CONFIG_BT_MESH_RX_SDU_MAX];
} seg_rx[CONFIG_BT_MESH_RX_SEG_MSG_COUNT] = {
	[0 ... (CONFIG_BT_MESH_RX_SEG_MSG_COUNT - 1)] = {
		.buf.size = CONFIG_BT_MESH_RX_SDU_MAX,
	},
};

static u16_t hb_sub_dst = BT_MESH_ADDR_UNASSIGNED;

void bt_mesh_set_hb_sub_dst(u16_t addr)
{
	hb_sub_dst = addr;
}

static int send_unseg(struct bt_mesh_net_tx *tx, struct net_buf_simple *sdu,
		      const struct bt_mesh_send_cb *cb, void *cb_data)
{
	struct net_buf *buf;

	BT_DBG("src 0x%04x dst 0x%04x app_idx 0x%04x sdu_len %u",
	       tx->src, tx->ctx->addr, tx->ctx->app_idx, sdu->len);

	buf = bt_mesh_adv_create(BT_MESH_ADV_DATA,
				 BT_MESH_TRANSMIT_COUNT(tx->xmit),
				 BT_MESH_TRANSMIT_INT(tx->xmit), BUF_TIMEOUT);
	if (!buf) {
		BT_ERR("Out of network buffers");
		return -ENOBUFS;
	}

	net_buf_reserve(buf, BT_MESH_NET_HDR_LEN);

	if (tx->ctx->app_idx == BT_MESH_KEY_DEV) {
		net_buf_add_u8(buf, UNSEG_HDR(0, 0));
	} else {
		net_buf_add_u8(buf, UNSEG_HDR(1, tx->aid));
	}

	net_buf_add_mem(buf, sdu->data, sdu->len);

	if (IS_ENABLED(CONFIG_BT_MESH_FRIEND)) {
		if (bt_mesh_friend_enqueue_tx(tx, BT_MESH_FRIEND_PDU_SINGLE,
					      NULL, &buf->b) &&
		    BT_MESH_ADDR_IS_UNICAST(tx->ctx->addr)) {
			/* PDUs for a specific Friend should only go
			 * out through the Friend Queue.
			 */
			net_buf_unref(buf);
			return 0;
		}
	}

	return bt_mesh_net_send(tx, buf, cb, cb_data);
}

bool bt_mesh_tx_in_progress(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(seg_tx); i++) {
		if (seg_tx[i].nack_count) {
			return true;
		}
	}

	return false;
}

static void seg_tx_reset(struct seg_tx *tx)
{
	int i;

	k_delayed_work_cancel(&tx->retransmit);

	tx->cb = NULL;
	tx->cb_data = NULL;
	tx->seq_auth = 0;
	tx->sub = NULL;
	tx->dst = BT_MESH_ADDR_UNASSIGNED;

	if (!tx->nack_count) {
		return;
	}

	for (i = 0; i <= tx->seg_n; i++) {
		if (!tx->seg[i]) {
			continue;
		}

		net_buf_unref(tx->seg[i]);
		tx->seg[i] = NULL;
	}

	tx->nack_count = 0;

	if (bt_mesh.pending_update) {
		BT_DBG("Proceding with pending IV Update");
		bt_mesh.pending_update = 0;
		/* bt_mesh_net_iv_update() will re-enable the flag if this
		 * wasn't the only transfer.
		 */
		if (bt_mesh_net_iv_update(bt_mesh.iv_index, false)) {
			bt_mesh_net_sec_update(NULL);
		}
	}
}

static inline void seg_tx_complete(struct seg_tx *tx, int err)
{
	if (tx->cb && tx->cb->end) {
		tx->cb->end(err, tx->cb_data);
	}

	seg_tx_reset(tx);
}

static void seg_send_start(u16_t duration, int err, void *user_data)
{
	struct seg_tx *tx = user_data;

	if (tx->cb && tx->cb->start) {
		tx->cb->start(duration, err, tx->cb_data);
	}
}

static void seg_sent(int err, void *user_data)
{
	struct seg_tx *tx = user_data;

	k_delayed_work_submit(&tx->retransmit, SEG_RETRANSMIT_TIMEOUT);
}

static const struct bt_mesh_send_cb first_sent_cb = {
	.start = seg_send_start,
	.end = seg_sent,
};

static const struct bt_mesh_send_cb seg_sent_cb = {
	.end = seg_sent,
};

static void seg_tx_send_unacked(struct seg_tx *tx)
{
	int i, err;

	for (i = 0; i <= tx->seg_n; i++) {
		struct net_buf *seg = tx->seg[i];

		if (!seg) {
			continue;
		}

		if (BT_MESH_ADV(seg)->busy) {
			BT_DBG("Skipping segment that's still advertising");
			continue;
		}

		if (!(BT_MESH_ADV(seg)->seg.attempts--)) {
			BT_ERR("Ran out of retransmit attempts");
			seg_tx_complete(tx, -ETIMEDOUT);
			return;
		}

		BT_DBG("resending %u/%u", i, tx->seg_n);

		err = bt_mesh_net_resend(tx->sub, seg, tx->new_key,
					 &seg_sent_cb, tx);
		if (err) {
			BT_ERR("Sending segment failed");
			seg_tx_complete(tx, -EIO);
			return;
		}
	}
}

static void seg_retransmit(struct k_work *work)
{
	struct seg_tx *tx = CONTAINER_OF(work, struct seg_tx, retransmit);

	seg_tx_send_unacked(tx);
}

static int send_seg(struct bt_mesh_net_tx *net_tx, struct net_buf_simple *sdu,
		    const struct bt_mesh_send_cb *cb, void *cb_data)
{
	u8_t seg_hdr, seg_o;
	u16_t seq_zero;
	struct seg_tx *tx;
	int i;

	BT_DBG("src 0x%04x dst 0x%04x app_idx 0x%04x aszmic %u sdu_len %u",
	       net_tx->src, net_tx->ctx->addr, net_tx->ctx->app_idx,
	       net_tx->aszmic, sdu->len);

	if (sdu->len < 1) {
		BT_ERR("Zero-length SDU not allowed");
		return -EINVAL;
	}

	if (sdu->len > BT_MESH_TX_SDU_MAX) {
		BT_ERR("Not enough segment buffers for length %u", sdu->len);
		return -EMSGSIZE;
	}

	for (tx = NULL, i = 0; i < ARRAY_SIZE(seg_tx); i++) {
		if (!seg_tx[i].nack_count) {
			tx = &seg_tx[i];
			break;
		}
	}

	if (!tx) {
		BT_ERR("No multi-segment message contexts available");
		return -EBUSY;
	}

	if (net_tx->ctx->app_idx == BT_MESH_KEY_DEV) {
		seg_hdr = SEG_HDR(0, 0);
	} else {
		seg_hdr = SEG_HDR(1, net_tx->aid);
	}

	seg_o = 0;
	tx->dst = net_tx->ctx->addr;
	tx->seg_n = (sdu->len - 1) / 12;
	tx->nack_count = tx->seg_n + 1;
	tx->seq_auth = SEQ_AUTH(BT_MESH_NET_IVI_TX, bt_mesh.seq);
	tx->sub = net_tx->sub;
	tx->new_key = net_tx->sub->kr_flag;
	tx->cb = cb;
	tx->cb_data = cb_data;

	seq_zero = tx->seq_auth & 0x1fff;

	BT_DBG("SeqZero 0x%04x", seq_zero);

	for (seg_o = 0; sdu->len; seg_o++) {
		struct net_buf *seg;
		u16_t len;
		int err;

		seg = bt_mesh_adv_create(BT_MESH_ADV_DATA,
					 BT_MESH_TRANSMIT_COUNT(net_tx->xmit),
					 BT_MESH_TRANSMIT_INT(net_tx->xmit),
					 BUF_TIMEOUT);
		if (!seg) {
			BT_ERR("Out of segment buffers");
			seg_tx_reset(tx);
			return -ENOBUFS;
		}

		BT_MESH_ADV(seg)->seg.attempts = SEG_RETRANSMIT_ATTEMPTS;

		net_buf_reserve(seg, BT_MESH_NET_HDR_LEN);

		net_buf_add_u8(seg, seg_hdr);
		net_buf_add_u8(seg, (net_tx->aszmic << 7) | seq_zero >> 6);
		net_buf_add_u8(seg, (((seq_zero & 0x3f) << 2) |
				     (seg_o >> 3)));
		net_buf_add_u8(seg, ((seg_o & 0x07) << 5) | tx->seg_n);

		len = min(sdu->len, 12);
		net_buf_add_mem(seg, sdu->data, len);
		net_buf_simple_pull(sdu, len);

		tx->seg[seg_o] = net_buf_ref(seg);

		if (IS_ENABLED(CONFIG_BT_MESH_FRIEND)) {
			enum bt_mesh_friend_pdu_type type;

			if (seg_o == tx->seg_n) {
				type = BT_MESH_FRIEND_PDU_COMPLETE;
			} else {
				type = BT_MESH_FRIEND_PDU_PARTIAL;
			}

			if (bt_mesh_friend_enqueue_tx(net_tx, type,
						      &tx->seq_auth,
						      &seg->b) &&
			    BT_MESH_ADDR_IS_UNICAST(net_tx->ctx->addr)) {
				/* PDUs for a specific Friend should only go
				 * out through the Friend Queue.
				 */
				net_buf_unref(seg);
				return 0;
			}
		}

		BT_DBG("Sending %u/%u", seg_o, tx->seg_n);

		err = bt_mesh_net_send(net_tx, seg,
				       seg_o ? &seg_sent_cb : &first_sent_cb,
				       tx);
		if (err) {
			BT_ERR("Sending segment failed");
			seg_tx_reset(tx);
			return err;
		}
	}

	if (bt_mesh_lpn_established()) {
		bt_mesh_lpn_poll();
	}

	return 0;
}

struct bt_mesh_app_key *bt_mesh_app_key_find(u16_t app_idx)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
		struct bt_mesh_app_key *key = &bt_mesh.app_keys[i];

		if (key->net_idx != BT_MESH_KEY_UNUSED &&
		    key->app_idx == app_idx) {
			return key;
		}
	}

	return NULL;
}

int bt_mesh_trans_send(struct bt_mesh_net_tx *tx, struct net_buf_simple *msg,
		       const struct bt_mesh_send_cb *cb, void *cb_data)
{
	const u8_t *key;
	u8_t *ad;
	int err;

	if (net_buf_simple_tailroom(msg) < 4) {
		BT_ERR("Insufficient tailroom for Transport MIC");
		return -EINVAL;
	}

	if (msg->len > 11) {
		tx->ctx->send_rel = 1;
	}

	BT_DBG("net_idx 0x%04x app_idx 0x%04x dst 0x%04x", tx->sub->net_idx,
	       tx->ctx->app_idx, tx->ctx->addr);
	BT_DBG("len %u: %s", msg->len, bt_hex(msg->data, msg->len));

	if (tx->ctx->app_idx == BT_MESH_KEY_DEV) {
		key = bt_mesh.dev_key;
		tx->aid = 0;
	} else {
		struct bt_mesh_app_key *app_key;

		app_key = bt_mesh_app_key_find(tx->ctx->app_idx);
		if (!app_key) {
			return -EINVAL;
		}

		if (tx->sub->kr_phase == BT_MESH_KR_PHASE_2 &&
		    app_key->updated) {
			key = app_key->keys[1].val;
			tx->aid = app_key->keys[1].id;
		} else {
			key = app_key->keys[0].val;
			tx->aid = app_key->keys[0].id;
		}
	}

	if (!tx->ctx->send_rel || net_buf_simple_tailroom(msg) < 8) {
		tx->aszmic = 0;
	} else {
		tx->aszmic = 1;
	}

	if (BT_MESH_ADDR_IS_VIRTUAL(tx->ctx->addr)) {
		ad = bt_mesh_label_uuid_get(tx->ctx->addr);
	} else {
		ad = NULL;
	}

	err = bt_mesh_app_encrypt(key, tx->ctx->app_idx == BT_MESH_KEY_DEV,
				  tx->aszmic, msg, ad, tx->src,
				  tx->ctx->addr, bt_mesh.seq,
				  BT_MESH_NET_IVI_TX);
	if (err) {
		return err;
	}

	if (tx->ctx->send_rel) {
		err = send_seg(tx, msg, cb, cb_data);
	} else {
		err = send_unseg(tx, msg, cb, cb_data);
	}

	return err;
}

int bt_mesh_trans_resend(struct bt_mesh_net_tx *tx, struct net_buf_simple *msg,
			 const struct bt_mesh_send_cb *cb, void *cb_data)
{
	struct net_buf_simple_state state;
	int err;

	net_buf_simple_save(msg, &state);

	if (tx->ctx->send_rel || msg->len > 15) {
		err = send_seg(tx, msg, cb, cb_data);
	} else {
		err = send_unseg(tx, msg, cb, cb_data);
	}

	net_buf_simple_restore(msg, &state);

	return err;
}

static bool is_replay(struct bt_mesh_net_rx *rx)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
		struct bt_mesh_rpl *rpl = &bt_mesh.rpl[i];

		/* Empty slot */
		if (!rpl->src) {
			rpl->src = rx->ctx.addr;
			rpl->seq = rx->seq;
			rpl->old_iv = rx->old_iv;
			return false;
		}

		/* Existing slot for given address */
		if (rpl->src == rx->ctx.addr) {
			if (rx->old_iv && !rpl->old_iv) {
				return true;
			}

			if ((!rx->old_iv && rpl->old_iv) ||
			    rpl->seq < rx->seq) {
				rpl->seq = rx->seq;
				rpl->old_iv = rx->old_iv;
				return false;
			} else {
				return true;
			}
		}
	}

	BT_ERR("RPL is full!");
	return true;
}

static int sdu_recv(struct bt_mesh_net_rx *rx, u8_t hdr, u8_t aszmic,
		    struct net_buf_simple *buf)
{
	struct net_buf_simple *sdu =
		NET_BUF_SIMPLE(CONFIG_BT_MESH_RX_SDU_MAX - 4);
	u8_t *ad;
	u16_t i;
	int err;

	BT_DBG("ASZMIC %u AKF %u AID 0x%02x", aszmic, AKF(&hdr), AID(&hdr));
	BT_DBG("len %u: %s", buf->len, bt_hex(buf->data, buf->len));

	if (buf->len < 1 + APP_MIC_LEN(aszmic)) {
		BT_ERR("Too short SDU + MIC");
		return -EINVAL;
	}

	if (IS_ENABLED(CONFIG_BT_MESH_FRIEND) && !rx->local_match) {
		BT_DBG("Ignoring PDU for LPN 0x%04x of this Friend", rx->dst);
		return 0;
	}

	if (BT_MESH_ADDR_IS_VIRTUAL(rx->dst)) {
		ad = bt_mesh_label_uuid_get(rx->dst);
	} else {
		ad = NULL;
	}

	/* Adjust the length to not contain the MIC at the end */
	buf->len -= APP_MIC_LEN(aszmic);

	if (!AKF(&hdr)) {
		net_buf_simple_init(sdu, 0);
		err = bt_mesh_app_decrypt(bt_mesh.dev_key, true, aszmic, buf,
					  sdu, ad, rx->ctx.addr, rx->dst,
					  rx->seq, BT_MESH_NET_IVI_RX(rx));
		if (err) {
			BT_ERR("Unable to decrypt with DevKey");
			return -EINVAL;
		}

		rx->ctx.app_idx = BT_MESH_KEY_DEV;
		bt_mesh_model_recv(rx, sdu);
		return 0;
	}

	for (i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
		struct bt_mesh_app_key *key = &bt_mesh.app_keys[i];
		struct bt_mesh_app_keys *keys;

		/* Check that this AppKey matches received net_idx */
		if (key->net_idx != rx->sub->net_idx) {
			continue;
		}

		if (rx->new_key && key->updated) {
			keys = &key->keys[1];
		} else {
			keys = &key->keys[0];
		}

		/* Check that the AppKey ID matches */
		if (AID(&hdr) != keys->id) {
			continue;
		}

		net_buf_simple_init(sdu, 0);
		err = bt_mesh_app_decrypt(keys->val, false, aszmic, buf,
					  sdu, ad, rx->ctx.addr, rx->dst,
					  rx->seq, BT_MESH_NET_IVI_RX(rx));
		if (err) {
			BT_WARN("Unable to decrypt with AppKey %u", i);
			continue;

		}

		rx->ctx.app_idx = key->app_idx;

		bt_mesh_model_recv(rx, sdu);
		return 0;
	}

	BT_WARN("No matching AppKey");

	return -EINVAL;
}

static struct seg_tx *seg_tx_lookup(u16_t seq_zero, u8_t obo, u16_t addr)
{
	struct seg_tx *tx;
	int i;

	for (i = 0; i < ARRAY_SIZE(seg_tx); i++) {
		tx = &seg_tx[i];

		if ((tx->seq_auth & 0x1fff) != seq_zero) {
			continue;
		}

		if (tx->dst == addr) {
			return tx;
		}

		/* If the expected remote address doesn't match,
		 * but the OBO flag is set and this is the first
		 * acknowledgement, assume it's a Friend that's
		 * responding and therefore accept the message.
		 */
		if (obo && tx->nack_count == tx->seg_n + 1) {
			tx->dst = addr;
			return tx;
		}
	}

	return NULL;
}

static int trans_ack(struct bt_mesh_net_rx *rx, u8_t hdr,
		     struct net_buf_simple *buf, u64_t *seq_auth)
{
	struct seg_tx *tx;
	unsigned int bit;
	u32_t ack;
	u16_t seq_zero;
	u8_t obo;

	if (buf->len < 6) {
		BT_ERR("Too short ack message");
		return -EINVAL;
	}

	seq_zero = net_buf_simple_pull_be16(buf);
	obo = seq_zero >> 15;
	seq_zero = (seq_zero >> 2) & 0x1fff;

	if (IS_ENABLED(CONFIG_BT_MESH_FRIEND) && rx->friend_match) {
		BT_DBG("Ack for LPN 0x%04x of this Friend", rx->dst);
		/* Best effort - we don't have enough info for true SeqAuth */
		*seq_auth = SEQ_AUTH(BT_MESH_NET_IVI_RX(rx), seq_zero);
		return 0;
	}

	ack = net_buf_simple_pull_be32(buf);

	BT_DBG("OBO %u seq_zero 0x%04x ack 0x%08x", obo, seq_zero, ack);

	tx = seg_tx_lookup(seq_zero, obo, rx->ctx.addr);
	if (!tx) {
		BT_WARN("No matching TX context for ack");
		return -EINVAL;
	}

	*seq_auth = tx->seq_auth;

	if (!ack) {
		BT_WARN("SDU canceled");
		seg_tx_complete(tx, -ECANCELED);
		return 0;
	}

	if (find_msb_set(ack) - 1 > tx->seg_n) {
		BT_ERR("Too large segment number in ack");
		return -EINVAL;
	}

	k_delayed_work_cancel(&tx->retransmit);

	while ((bit = find_lsb_set(ack))) {
		if (tx->seg[bit - 1]) {
			BT_DBG("seg %u/%u acked", bit - 1, tx->seg_n);
			net_buf_unref(tx->seg[bit - 1]);
			tx->seg[bit - 1] = NULL;
			tx->nack_count--;
		}

		ack &= ~BIT(bit - 1);
	}

	if (tx->nack_count) {
		seg_tx_send_unacked(tx);
	} else {
		BT_DBG("SDU TX complete");
		seg_tx_complete(tx, 0);
	}

	return 0;
}

static int trans_heartbeat(struct bt_mesh_net_rx *rx,
			   struct net_buf_simple *buf)
{
	u8_t init_ttl, hops;
	u16_t feat;

	if (buf->len < 3) {
		BT_ERR("Too short heartbeat message");
		return -EINVAL;
	}

	if (rx->dst != hb_sub_dst) {
		BT_WARN("Ignoring heartbeat to non-subscribed destination");
		return 0;
	}

	init_ttl = (net_buf_simple_pull_u8(buf) & 0x7f);
	feat = net_buf_simple_pull_be16(buf);

	hops = (init_ttl - rx->ctx.recv_ttl + 1);

	BT_DBG("src 0x%04x TTL %u InitTTL %u (%u hop%s) feat 0x%04x",
	       rx->ctx.addr, rx->ctx.recv_ttl, init_ttl, hops,
	       (hops == 1) ? "" : "s", feat);

	bt_mesh_heartbeat(rx->ctx.addr, rx->dst, hops, feat);

	return 0;
}

static int ctl_recv(struct bt_mesh_net_rx *rx, u8_t hdr,
		    struct net_buf_simple *buf, u64_t *seq_auth)
{
	u8_t ctl_op = TRANS_CTL_OP(&hdr);

	BT_DBG("OpCode 0x%02x len %u", ctl_op, buf->len);

	switch (ctl_op) {
	case TRANS_CTL_OP_ACK:
		return trans_ack(rx, hdr, buf, seq_auth);
	case TRANS_CTL_OP_HEARTBEAT:
		return trans_heartbeat(rx, buf);
	}

	BT_DBG("%s %d", __func__, __LINE__);

	/* Only acks and heartbeats may need processing without local_match */
	if (!rx->local_match) {
		return 0;
	}

	bool estblshed = bt_mesh_lpn_established();
	BT_DBG("%s %d, estblshed: %d", __func__, __LINE__, estblshed);

	if (IS_ENABLED(CONFIG_BT_MESH_FRIEND) && !bt_mesh_lpn_established()) {
		switch (ctl_op) {
		case TRANS_CTL_OP_FRIEND_POLL:
			return bt_mesh_friend_poll(rx, buf);
		case TRANS_CTL_OP_FRIEND_REQ:
			return bt_mesh_friend_req(rx, buf);
		case TRANS_CTL_OP_FRIEND_CLEAR:
			return bt_mesh_friend_clear(rx, buf);
		case TRANS_CTL_OP_FRIEND_CLEAR_CFM:
			return bt_mesh_friend_clear_cfm(rx, buf);
		case TRANS_CTL_OP_FRIEND_SUB_ADD:
			return bt_mesh_friend_sub_add(rx, buf);
		case TRANS_CTL_OP_FRIEND_SUB_REM:
			return bt_mesh_friend_sub_rem(rx, buf);
		}
	}

#if defined(CONFIG_BT_MESH_LOW_POWER)
	if (ctl_op == TRANS_CTL_OP_FRIEND_OFFER) {
		BT_DBG("%s friend offer pkt received.", __func__);
		return bt_mesh_lpn_friend_offer(rx, buf);
	}

	if (rx->ctx.addr == bt_mesh.lpn.frnd) {
		if (ctl_op == TRANS_CTL_OP_FRIEND_CLEAR_CFM) {
			return bt_mesh_lpn_friend_clear_cfm(rx, buf);
		}

		if (!rx->friend_cred) {
			BT_WARN("Message from friend with wrong credentials");
			return -EINVAL;
		}

		switch (ctl_op) {
		case TRANS_CTL_OP_FRIEND_UPDATE:
			return bt_mesh_lpn_friend_update(rx, buf);
		case TRANS_CTL_OP_FRIEND_SUB_CFM:
			return bt_mesh_lpn_friend_sub_cfm(rx, buf);
		}
	}
#endif /* CONFIG_BT_MESH_LOW_POWER */

	BT_WARN("Unhandled TransOpCode 0x%02x", ctl_op);

	return -ENOENT;
}

static int trans_unseg(struct net_buf_simple *buf, struct bt_mesh_net_rx *rx,
		       u64_t *seq_auth)
{
	u8_t hdr;

	BT_DBG("AFK %u AID 0x%02x", AKF(buf->data), AID(buf->data));

	if (buf->len < 1) {
		BT_ERR("Too small unsegmented PDU");
		return -EINVAL;
	}

	if (rx->local_match && is_replay(rx)) {
		BT_WARN("Replay: src 0x%04x dst 0x%04x seq 0x%06x",
			rx->ctx.addr, rx->dst, rx->seq);
		return -EINVAL;
	}

	hdr = net_buf_simple_pull_u8(buf);

	if (rx->ctl) {
		return ctl_recv(rx, hdr, buf, seq_auth);
	} else {
		/* SDUs must match a local element or an LPN of this Friend. */
		if (!rx->local_match && !rx->friend_match) {
			return 0;
		}

		return sdu_recv(rx, hdr, 0, buf);
	}
}

static inline s32_t ack_timeout(struct seg_rx *rx)
{
	s32_t to;

	/* The acknowledgment timer shall be set to a minimum of
	 * 150 + 50 * TTL milliseconds.
	 */
	to = K_MSEC(150 + (50 * rx->ttl));

	/* 100 ms for every not yet received segment */
	to += K_MSEC(((rx->seg_n + 1) - popcount(rx->block)) * 100);

	/* Make sure we don't send more frequently than the duration for
	 * each packet (default is 300ms).
	 */
	return max(to, K_MSEC(400));
}

int bt_mesh_ctl_send(struct bt_mesh_net_tx *tx, u8_t ctl_op, void *data,
		     size_t data_len, u64_t *seq_auth,
		     const struct bt_mesh_send_cb *cb, void *cb_data)
{
	struct net_buf *buf;

	BT_DBG("src 0x%04x dst 0x%04x ttl 0x%02x ctl 0x%02x", tx->src,
	       tx->ctx->addr, tx->ctx->send_ttl, ctl_op);
	BT_DBG("len %zu: %s", data_len, bt_hex(data, data_len));

	buf = bt_mesh_adv_create(BT_MESH_ADV_DATA,
				 BT_MESH_TRANSMIT_COUNT(tx->xmit),
				 BT_MESH_TRANSMIT_INT(tx->xmit), BUF_TIMEOUT);
	if (!buf) {
		BT_ERR("Out of transport buffers");
		return -ENOBUFS;
	}

	net_buf_reserve(buf, BT_MESH_NET_HDR_LEN);

	net_buf_add_u8(buf, TRANS_CTL_HDR(ctl_op, 0));

	net_buf_add_mem(buf, data, data_len);

	if (IS_ENABLED(CONFIG_BT_MESH_FRIEND)) {
		if (bt_mesh_friend_enqueue_tx(tx, BT_MESH_FRIEND_PDU_SINGLE,
					      seq_auth, &buf->b) &&
		    BT_MESH_ADDR_IS_UNICAST(tx->ctx->addr)) {
			/* PDUs for a specific Friend should only go
			 * out through the Friend Queue.
			 */
			net_buf_unref(buf);
			return 0;
		}
	}

	return bt_mesh_net_send(tx, buf, cb, cb_data);
}

static int send_ack(struct bt_mesh_subnet *sub, u16_t src, u16_t dst,
		    u8_t ttl, u64_t *seq_auth, u32_t block, u8_t obo)
{
	struct bt_mesh_msg_ctx ctx = {
		.net_idx = sub->net_idx,
		.app_idx = BT_MESH_KEY_UNUSED,
		.addr = dst,
		.send_ttl = ttl,
	};
	struct bt_mesh_net_tx tx = {
		.sub = sub,
		.ctx = &ctx,
		.src = obo ? bt_mesh_primary_addr() : src,
		.xmit = bt_mesh_net_transmit_get(),
	};
	u16_t seq_zero = *seq_auth & 0x1fff;
	u8_t buf[6];

	BT_DBG("SeqZero 0x%04x Block 0x%08x OBO %u", seq_zero, block, obo);

	if (bt_mesh_lpn_established()) {
		BT_WARN("Not sending ack when LPN is enabled");
		return 0;
	}

	/* This can happen if the segmented message was destined for a group
	 * or virtual address.
	 */
	if (!BT_MESH_ADDR_IS_UNICAST(src)) {
		BT_WARN("Not sending ack for non-unicast address");
		return 0;
	}

	sys_put_be16(((seq_zero << 2) & 0x7ffc) | (obo << 15), buf);
	sys_put_be32(block, &buf[2]);

	return bt_mesh_ctl_send(&tx, TRANS_CTL_OP_ACK, buf, sizeof(buf),
				NULL, NULL, NULL);
}

static void seg_rx_reset(struct seg_rx *rx)
{
	BT_DBG("rx %p", rx);

	k_delayed_work_cancel(&rx->ack);

	if (IS_ENABLED(CONFIG_BT_MESH_FRIEND) && rx->obo &&
	    rx->block != BLOCK_COMPLETE(rx->seg_n)) {
		BT_WARN("Clearing incomplete buffers from Friend queue");
		bt_mesh_friend_clear_incomplete(rx->sub, rx->src, rx->dst,
						&rx->seq_auth);
	}

	/* We don't reset rx->net and rx->seq_auth here since we need to
	 * be able to send an ack if we receive a segment after we've
	 * already received the full SDU.
	 */

	rx->in_use = 0;
}

static void seg_ack(struct k_work *work)
{
	struct seg_rx *rx = CONTAINER_OF(work, struct seg_rx, ack);

	BT_DBG("rx %p", rx);

	if (k_uptime_get_32() - rx->last > (60 * MSEC_PER_SEC)) {
		BT_WARN("Incomplete timer expired");
		send_ack(rx->sub, rx->dst, rx->src, rx->ttl,
			 &rx->seq_auth, 0, rx->obo);
		seg_rx_reset(rx);
		return;
	}

	send_ack(rx->sub, rx->dst, rx->src, rx->ttl, &rx->seq_auth,
		 rx->block, rx->obo);

	k_delayed_work_submit(&rx->ack, ack_timeout(rx));
}

static inline u8_t seg_len(bool ctl)
{
	if (ctl) {
		return 8;
	} else {
		return 12;
	}
}

static inline bool sdu_len_is_ok(bool ctl, u8_t seg_n)
{
	return ((seg_n * seg_len(ctl) + 1) <= CONFIG_BT_MESH_RX_SDU_MAX);
}

static struct seg_rx *seg_rx_find(struct bt_mesh_net_rx *net_rx,
				  const u64_t *seq_auth)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(seg_rx); i++) {
		struct seg_rx *rx = &seg_rx[i];

		if (rx->src != net_rx->ctx.addr || rx->dst != net_rx->dst) {
			continue;
		}

		if (rx->seq_auth == *seq_auth) {
			return rx;
		}

		if (rx->in_use) {
			BT_WARN("Duplicate SDU from src 0x%04x",
				net_rx->ctx.addr);

			/* Clear out the old context since the sender
			 * has apparently started sending a new SDU.
			 */
			seg_rx_reset(rx);

			/* Return non-match so caller can re-allocate */
			return NULL;
		}
	}

	return NULL;
}

static bool seg_rx_is_valid(struct seg_rx *rx, struct bt_mesh_net_rx *net_rx,
			    const u8_t *hdr, u8_t seg_n)
{
	if (rx->hdr != *hdr || rx->seg_n != seg_n) {
		BT_ERR("Invalid segment for ongoing session");
		return false;
	}

	if (rx->src != net_rx->ctx.addr || rx->dst != net_rx->dst) {
		BT_ERR("Invalid source or destination for segment");
		return false;
	}

	if (rx->ctl != net_rx->ctl) {
		BT_ERR("Inconsistent CTL in segment");
		return false;
	}

	return true;
}

static struct seg_rx *seg_rx_alloc(struct bt_mesh_net_rx *net_rx,
				   const u8_t *hdr, const u64_t *seq_auth,
				   u8_t seg_n)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(seg_rx); i++) {
		struct seg_rx *rx = &seg_rx[i];

		if (rx->in_use) {
			continue;
		}

		rx->in_use = 1;
		net_buf_simple_init(&rx->buf, 0);
		rx->sub = net_rx->sub;
		rx->ctl = net_rx->ctl;
		rx->seq_auth = *seq_auth;
		rx->seg_n = seg_n;
		rx->hdr = *hdr;
		rx->ttl = net_rx->ctx.send_ttl;
		rx->src = net_rx->ctx.addr;
		rx->dst = net_rx->dst;
		rx->block = 0;

		BT_DBG("New RX context. Block Complete 0x%08x",
		       BLOCK_COMPLETE(seg_n));

		return rx;
	}

	return NULL;
}

static int trans_seg(struct net_buf_simple *buf, struct bt_mesh_net_rx *net_rx,
		     enum bt_mesh_friend_pdu_type *pdu_type, u64_t *seq_auth)
{
	struct seg_rx *rx;
	u8_t *hdr = buf->data;
	u16_t seq_zero;
	u8_t seg_n;
	u8_t seg_o;
	int err;

	if (buf->len < 5) {
		BT_ERR("Too short segmented message (len %u)", buf->len);
		return -EINVAL;
	}

	BT_DBG("ASZMIC %u AKF %u AID 0x%02x", ASZMIC(hdr), AKF(hdr), AID(hdr));

	net_buf_simple_pull(buf, 1);

	seq_zero = net_buf_simple_pull_be16(buf);
	seg_o = (seq_zero & 0x03) << 3;
	seq_zero = (seq_zero >> 2) & 0x1fff;
	seg_n = net_buf_simple_pull_u8(buf);
	seg_o |= seg_n >> 5;
	seg_n &= 0x1f;

	BT_DBG("SeqZero 0x%04x SegO %u SegN %u", seq_zero, seg_o, seg_n);

	if (seg_o > seg_n) {
		BT_ERR("SegO greater than SegN (%u > %u)", seg_o, seg_n);
		return -EINVAL;
	}

	*seq_auth = SEQ_AUTH(BT_MESH_NET_IVI_RX(net_rx),
			     (net_rx->seq & 0xffffe000) | seq_zero);

	/* Look for old RX sessions */
	rx = seg_rx_find(net_rx, seq_auth);
	if (rx) {
		if (!seg_rx_is_valid(rx, net_rx, hdr, seg_n)) {
			return -EINVAL;
		}

		if (rx->in_use) {
			BT_DBG("Existing RX context. Block 0x%08x", rx->block);
			goto found_rx;
		}

		if (rx->block == BLOCK_COMPLETE(rx->seg_n)) {
			BT_WARN("Got segment for already complete SDU");
			send_ack(net_rx->sub, net_rx->dst, net_rx->ctx.addr,
				 net_rx->ctx.send_ttl, seq_auth, rx->block,
				 rx->obo);
			return -EALREADY;
		}

		/* We ignore instead of sending block ack 0 since the
		 * ack timer is always smaller than the incomplete
		 * timer, i.e. the sender is misbehaving.
		 */
		BT_WARN("Got segment for canceled SDU");
		return -EINVAL;
	}

	/* Bail out early if we're not ready to receive such a large SDU */
	if (!sdu_len_is_ok(net_rx->ctl, seg_n)) {
		BT_ERR("Too big incoming SDU length");
		send_ack(net_rx->sub, net_rx->dst, net_rx->ctx.addr,
			 net_rx->ctx.send_ttl, seq_auth, 0,
			 net_rx->friend_match);
		return -EMSGSIZE;
	}

	/* Look for free slot for a new RX session */
	rx = seg_rx_alloc(net_rx, hdr, seq_auth, seg_n);
	if (!rx) {
		/* Warn but don't cancel since the existing slots willl
		 * eventually be freed up and we'll be able to process
		 * this one.
		 */
		BT_WARN("No free slots for new incoming segmented messages");
		return -ENOMEM;
	}

	rx->obo = net_rx->friend_match;

found_rx:
	if (BIT(seg_o) & rx->block) {
		BT_WARN("Received already received fragment");
		return -EALREADY;
	}

	/* All segments, except the last one, must either have 8 bytes of
	 * payload (for 64bit Net MIC) or 12 bytes of payload (for 32bit
	 * Net MIC).
	 */
	if (seg_o == seg_n) {
		/* Set the expected final buffer length */
		rx->buf.len = seg_n * seg_len(rx->ctl) + buf->len;
		BT_DBG("Target len %u * %u + %u = %u", seg_n, seg_len(rx->ctl),
		       buf->len, rx->buf.len);

		if (rx->buf.len > CONFIG_BT_MESH_RX_SDU_MAX) {
			BT_ERR("Too large SDU len");
			send_ack(net_rx->sub, net_rx->dst, net_rx->ctx.addr,
				 net_rx->ctx.send_ttl, seq_auth, 0, rx->obo);
			seg_rx_reset(rx);
			return -EMSGSIZE;
		}
	} else {
		if (buf->len != seg_len(rx->ctl)) {
			BT_ERR("Incorrect segment size for message type");
			return -EINVAL;
		}
	}

	/* Reset the Incomplete Timer */
	rx->last = k_uptime_get_32();

	if (!k_delayed_work_remaining_get(&rx->ack) &&
	    !bt_mesh_lpn_established()) {
		k_delayed_work_submit(&rx->ack, ack_timeout(rx));
	}

	/* Location in buffer can be calculated based on seg_o & rx->ctl */
	memcpy(rx->buf_data + (seg_o * seg_len(rx->ctl)), buf->data, buf->len);

	BT_DBG("Received %u/%u", seg_o, seg_n);

	/* Mark segment as received */
	rx->block |= BIT(seg_o);

	if (rx->block != BLOCK_COMPLETE(seg_n)) {
		*pdu_type = BT_MESH_FRIEND_PDU_PARTIAL;
		return 0;
	}

	BT_DBG("Complete SDU");

	if (net_rx->local_match && is_replay(net_rx)) {
		BT_WARN("Replay: src 0x%04x dst 0x%04x seq 0x%06x",
			net_rx->ctx.addr, net_rx->dst, net_rx->seq);
		/* Clear the segment's bit */
		rx->block &= ~BIT(seg_o);
		return -EINVAL;
	}

	*pdu_type = BT_MESH_FRIEND_PDU_COMPLETE;

	/* Set the correct sequence number to be used with the App Nonce */
	net_rx->seq = (rx->seq_auth & 0xffffff);

	k_delayed_work_cancel(&rx->ack);
	send_ack(net_rx->sub, net_rx->dst, net_rx->ctx.addr,
		 net_rx->ctx.send_ttl, seq_auth, rx->block, rx->obo);

	if (net_rx->ctl) {
		err = ctl_recv(net_rx, *hdr, &rx->buf, seq_auth);
	} else {
		err = sdu_recv(net_rx, *hdr, ASZMIC(hdr), &rx->buf);
	}

	seg_rx_reset(rx);

	return err;
}

int bt_mesh_trans_recv(struct net_buf_simple *buf, struct bt_mesh_net_rx *rx)
{
	u64_t seq_auth = TRANS_SEQ_AUTH_NVAL;
	enum bt_mesh_friend_pdu_type pdu_type = BT_MESH_FRIEND_PDU_SINGLE;
	struct net_buf_simple_state state;
	int err;

	if (IS_ENABLED(CONFIG_BT_MESH_FRIEND)) {
		rx->friend_match = bt_mesh_friend_match(rx->sub->net_idx,
							rx->dst);
	} else {
		rx->friend_match = false;
	}

	BT_DBG("src 0x%04x dst 0x%04x seq 0x%08x friend_match %u",
	       rx->ctx.addr, rx->dst, rx->seq, rx->friend_match);

	/* Remove network headers */
	net_buf_simple_pull(buf, BT_MESH_NET_HDR_LEN);

	BT_DBG("Payload %s", bt_hex(buf->data, buf->len));

	/* If LPN mode is enabled messages are only accepted when we've
	 * requested the Friend to send them. The messages must also
	 * be encrypted using the Friend Credentials.
	 */
	if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER) &&
	    bt_mesh_lpn_established() && rx->net_if == BT_MESH_NET_IF_ADV &&
	    (!bt_mesh_lpn_waiting_update() || !rx->friend_cred)) {
		BT_WARN("Ignoring unexpected message in Low Power mode");
		return -EAGAIN;
	}

	/* Save the app-level state so the buffer can later be placed in
	 * the Friend Queue.
	 */
	net_buf_simple_save(buf, &state);

	if (SEG(buf->data)) {
		/* Segmented messages must match a local element or an
		 * LPN of this Friend.
		 */
		if (!rx->local_match && !rx->friend_match) {
			return 0;
		}

		err = trans_seg(buf, rx, &pdu_type, &seq_auth);
	} else {
		err = trans_unseg(buf, rx, &seq_auth);
	}

	/* Notify LPN state machine so a Friend Poll will be sent. If the
	 * message was a Friend Update it's possible that a Poll was already
	 * queued for sending, however that's fine since then the
	 * bt_mesh_lpn_waiting_update() function will return false:
	 * we still need to go through the actual sending to the bearer and
	 * wait for ReceiveDelay before transitioning to WAIT_UPDATE state.
	 *
	 * Another situation where we want to notify the LPN state machine
	 * is if it's configured to use an automatic Friendship establishment
	 * timer, in which case we want to reset the timer at this point.
	 *
	 * ENOENT is a special condition that's only used to indicate that
	 * the Transport OpCode was invalid, in which case we should ignore
	 * the PDU completely, as per MESH/NODE/FRND/LPN/BI-02-C.
	 */
	if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER) && err != -ENOENT &&
	    (bt_mesh_lpn_timer() ||
	     (bt_mesh_lpn_established() && bt_mesh_lpn_waiting_update()))) {
		bt_mesh_lpn_msg_received(rx);
	}

	net_buf_simple_restore(buf, &state);

	if (IS_ENABLED(CONFIG_BT_MESH_FRIEND) && rx->friend_match && !err) {
		if (seq_auth == TRANS_SEQ_AUTH_NVAL) {
			bt_mesh_friend_enqueue_rx(rx, pdu_type, NULL, buf);
		} else {
			bt_mesh_friend_enqueue_rx(rx, pdu_type, &seq_auth, buf);
		}
	}

	return err;
}

void bt_mesh_rx_reset(void)
{
	int i;

	BT_DBG("");

	for (i = 0; i < ARRAY_SIZE(seg_rx); i++) {
		seg_rx_reset(&seg_rx[i]);
		seg_rx[i].src = BT_MESH_ADDR_UNASSIGNED;
		seg_rx[i].dst = BT_MESH_ADDR_UNASSIGNED;
	}
}

void bt_mesh_tx_reset(void)
{
	int i;

	BT_DBG("");

	for (i = 0; i < ARRAY_SIZE(seg_tx); i++) {
		seg_tx_reset(&seg_tx[i]);
	}
}

void bt_mesh_trans_init(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(seg_tx); i++) {
		k_delayed_work_init(&seg_tx[i].retransmit, seg_retransmit);
	}

	for (i = 0; i < ARRAY_SIZE(seg_rx); i++) {
		k_delayed_work_init(&seg_rx[i].ack, seg_ack);
	}
}

void bt_mesh_rpl_clear(void)
{
	BT_DBG("");
	memset(bt_mesh.rpl, 0, sizeof(bt_mesh.rpl));
}
