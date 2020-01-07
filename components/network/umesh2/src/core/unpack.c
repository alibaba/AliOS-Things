#include <stdint.h>
#include <stddef.h>
#include "buffer.h"
#include "state.h"
#include "unpack.h"
#include "ieee80211.h"
#include "log.h"

extern const uint8_t UMESH_BSSID[];
static int umesh_parse_hdr(const struct buf *frame, uint16_t *seq, umesh_frame_type_t *frame_type, umesh_peer_type_t *peer_type) {
	const struct umesh_header *af;
	int valid;
    if(frame == NULL || seq == NULL || frame_type == NULL || peer_type == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }

	READ_BYTES(frame, 0, NULL, sizeof(struct umesh_header));

	af = (const struct umesh_header *) buf_data(frame);

	valid = af->category == IEEE80211_VENDOR_SPECIFIC &&
	        !memcmp(&af->oui, (uint8_t *)UMESH_OUI, 3);
    
    if(!valid) {
       log_e("parse umesh hdr err!");
       return UMESH_RX_UNEXPECTED_TYPE;
    }

    *seq = af->frame_seq;
    *frame_type  = af->flag & 0x0f;  
    *peer_type =  (af->flag>>4) &0x03;
    return 0;
buffer_error:
	return UMESH_RX_TOO_SHORT;
}

static int umesh_parse_join_tlv_resp(umesh_peer_type_t peer_type, uint8_t *from,const struct buf * tlv_buf, umesh_state_t *state)
{
    int ret = UMESH_PARSE_TLV_FAILED;
    struct umesh_peer *peer = NULL;
    uint8_t from_rand_array[UMESH_PEER_RANDOM_LEN];
    uint8_t to_rand_array[UMESH_PEER_RANDOM_LEN];
    uint32_t session_id = 0;
    uint16_t offset = 0;
 

    umesh_produce_random(to_rand_array, UMESH_PEER_RANDOM_LEN);
    READ_LE32(tlv_buf, offset, &session_id);
    offset += sizeof(session_id);
    READ_BYTES_COPY(tlv_buf, offset, from_rand_array, UMESH_PEER_RANDOM_LEN); 

    ret = umesh_peer_get(state->peers_state.peers, from, &peer);
    if (ret < 0) {/*新peer:如果 peer 不存在， 创建新peer,回复信息 */
        peer = umesh_peer_add2(state->peers_state.peers, from, peer_type, UMESH_PHASE_IDENTIFY,
                                umesh_now_ms(), session_id, from_rand_array, to_rand_array);

        if(peer == NULL) {
            char temp[IEEE80211_MAC_ADDR_LEN *3 + 1] = {0};
            umesh_hex_to_str(from, IEEE80211_MAC_ADDR_LEN, temp);
            log_e("create peer failed , mac = %s , ret = %d",temp, ret);
            ret = UMESH_PEER_ADD_FAILED;
            return ret;
        }
    } else {/*单向旧peer:如果 peer 已经存在，更新peer 信息 ，回复信息*/
            memcpy(peer->from_random, from_rand_array, UMESH_PEER_RANDOM_LEN);
            memcpy(peer->to_random, to_rand_array, UMESH_PEER_RANDOM_LEN);
            peer->session_id = session_id;
            peer->last_update = umesh_now_ms();
        }
        /*send finish to dst peer*/

    ret = umesh_send_frame_join_finish(state, from);
    if(ret == 0) {
        if(state->peer_add_cb) {
            state->peer_add_cb(peer, state);
        }
        /*update */
    }
    return ret;
buffer_error:
    return UMESH_RX_TOO_SHORT;
}

static int umesh_parse_join_tlv_finish(umesh_peer_type_t peer_type, uint8_t *from,const struct buf * tlv_buf, umesh_state_t *state)
{
    int ret = UMESH_PARSE_TLV_FAILED;
    struct umesh_peer *peer = NULL;
    umesh_buf_t *send_buf = NULL;
    uint8_t from_rand_array[UMESH_PEER_RANDOM_LEN];
    uint8_t to_rand_array[UMESH_PEER_RANDOM_LEN];
    uint32_t session_id = 0;
    uint16_t offset = 0;
 
    READ_LE32(tlv_buf, offset, &session_id);
    offset += sizeof(session_id);
    READ_BYTES_COPY(tlv_buf, offset, from_rand_array, UMESH_PEER_RANDOM_LEN); 

    ret = umesh_peer_get(state->peers_state.peers, from, &peer);
    if (ret < 0) {/*如果peer 不存在，说明 应答有错误，不应该应答*/

    } else {/*单向旧peer:如果 peer 已经存在，更新peer 信息 ，回复信息*/

            if(peer->session_id != session_id) {
                log_w("get tlv finish frame, self session_id = %d,recv one = %d ",peer->session_id, session_id );
            }
            memcpy(peer->from_random, from_rand_array, UMESH_PEER_RANDOM_LEN);

            peer->session_id = session_id; /*update session id*/
            peer->last_update = umesh_now_ms();
            peer->step = UMESH_PEER_IDENTIFY_FINISH;
            if(state->peer_add_cb) { /*new peer add finished */
                state->peer_add_cb(peer, state);
            }
            ret = 0;
        }

    return ret;
buffer_error:
    return UMESH_RX_TOO_SHORT;
}

static int umesh_parse_heart_tlv(umesh_peer_type_t peer_type, uint8_t *from,const struct buf *tlv_buf, umesh_state_t *state)
{
    int ret = UMESH_PARSE_TLV_FAILED;

    return ret;
}

static int umesh_parse_data_tlv(umesh_peer_type_t peer_type, uint8_t *from,const struct buf *tlv_buf, umesh_state_t *state)
{
    int ret = UMESH_PARSE_TLV_FAILED;

    return ret;
}

int umesh_handle_tlv(umesh_frame_type_t frame_type, umesh_peer_type_t peer_type,uint8_t * from, 
                     umesh_tlvs_t tlv_type,const struct buf * tlv_buf, umesh_state_t *state)
{
    int ret = UMESH_RX_UNEXPECTED_TYPE;
    switch(tlv_type) {
        case UMESH_VENDOR_TLV:
        break;
        case UMESH_JOIN_TLV:
        if(frame_type == UMESH_FRAME_JOIN_PERMIT ) {
            ret = umesh_parse_join_tlv_resp(peer_type, from, tlv_buf, state);
        }
        else if(frame_type == UMESH_FRAME_JOIN_FINISH ) {
            ret = umesh_parse_join_tlv_finish(peer_type, from, tlv_buf, state);
        }
        break;
        case UMESH_HEART_BEAT_TLV:
        if(frame_type < UMESH_FRAME_JOIN_PERMIT) {
            ret = umesh_parse_heart_tlv(peer_type, from, tlv_buf, state);
            break;
        }

        break;
        case UMESH_DATA_TLV:
        if(frame_type != UMESH_FRAME_DATA) {
            ret = umesh_parse_data_tlv(peer_type, from, tlv_buf, state);
            break;
        }

        break;
        case UMESH_ZERO_REQ_TLV:
        break;
        case UMESH_ZERO_RESP_TLV:
        break;
        case UMESH_ZERO_FINISH_TLV:
        break;
    }
    return ret;
}


int umesh_rx_ieee80211_payload(const struct buf *frame, const uint8_t *from, const uint8_t *to, umesh_state_t *state)
{
    int ret;
	uint8_t tlv_type;
	uint16_t tlv_len;  
	const uint8_t *tlv_value;
	int left_len;
    int len;
    uint16_t seq; 
    umesh_frame_type_t frame_type;
     umesh_peer_type_t peer_type;
    
    ret = umesh_parse_hdr(frame, &seq, &frame_type, &peer_type);
	if (ret < 0) {
		log_e("not umesh frame"); /* could be block ACK */
		return UMESH_RX_UNEXPECTED_TYPE;
	}
    buf_strip(frame, sizeof(struct umesh_header));



    left_len = buf_len(frame);
    while ((len = read_tlv(frame, 0, &tlv_type, &tlv_len, &tlv_value)) > 0) {
        const struct buf *tlv_buf = buf_new_const(tlv_value, tlv_len);
        int result = umesh_handle_tlv(frame_type, peer_type, from, tlv_type, tlv_buf, state);
        // if (state->tlv_cb)
        //     state->tlv_cb(peer, tlv_type, tlv_buf, state, state->tlv_cb_data);
        buf_free(tlv_buf);

        if (result < 0) {
            log_w("awdl_action: parsing error %s", umesh_tlv_as_str(tlv_type));
            return UMESH_RX_UNEXPECTED_TYPE;
        }
        left_len -= len;
        log_w("cur offset = %d, left_len = %d ",len , left_len);
        buf_strip(frame, len);
    }


    switch(frame_type) {
        case UMESH_FRAME_DISCOVERY_REQ:
            ret = umesh_send_frame_discovery_resp(state, from);    
            break;
        case UMESH_FRAME_DISCOVERY_RESP:
            if(state->self_phase == UMESH_PHASE_SCAN) {  
                ret = umesh_add_scaned_count(state,peer_type);
            }
        break;
        case UMESH_FRAME_JOIN_REQ: {
        /*看下sa是否在当前peers列表中，如果不在则创建一个peer,给其分配session id/ token值*/
            struct umesh_peer *peer = NULL;
            uint8_t rand_array[UMESH_PEER_RANDOM_LEN] = {0};
            ret = umesh_peer_get(&state->peers_state, from, &peer);
            if(ret == 0 && peer != NULL) {
                if(peer->step > UMESH_PEER_IDENTIFY_RESP) { /* no need action*/
                    ret = UMESH_NO_ACTION_REQUIRED; 
                    break;
                }  /*send join resp*/
            }
            umesh_produce_random(rand_array, UMESH_PEER_RANDOM_LEN);
            ret = umesh_peer_add2(state->peers_state.peers, from, peer_type, UMESH_PEER_IDENTIFY_REQ,
	                              umesh_now_ms(), state->session_id, NULL, rand_array);

            
            if(ret < 0) {
                ret = UMESH_PEER_ADD_FAILED;
                break;
            }

            ret = umesh_send_frame_join_resp(state, from); 
        break;
        }
        case UMESH_FRAME_JOIN_PERMIT:{
            int len;

        }
        /*parse tlv */
        /*看下sa是否在当前peers列表中，如果不在则创建一个peer,给其分配session id/ token值，并回复finish*/
        break;
        case UMESH_FRAME_JOIN_FINISH:
        /*peer 存在则更新 peer 状态，完成认证*/
        break;
        case UMESH_FRAME_HEART_BEAT:
        /* 更新 peer 上一次 心跳时间*/
        break;
        case UMESH_FRAME_DATA:
        /*解析数据，da不为自己则转发，否则调用回调往上传数据*/
        break;       
    }

    return ret;
buffer_error:
	return UMESH_RX_TOO_SHORT;
}

int umesh_rx_ieee80211_frame(const struct buf *frame,uint8_t rssi, umesh_state_t *state)
{
	const struct umesh_ieee80211_hdr *ieee80211;
	const uint8_t *from, *to, *bssid;
	uint16_t fc;
	uint8_t flags;
    
    log_hex("rx frame",buf_data(frame), buf_len(frame));
	READ_BYTES(frame, 0, NULL, sizeof(struct umesh_ieee80211_hdr));

	ieee80211 = (const struct umesh_ieee80211_hdr *) (buf_data(frame));
	from = ieee80211->addr2;
	to = ieee80211->addr1;
	bssid = ieee80211->addr3;    

	fc = umesh_le16toh(ieee80211->frame_control);

	if (!memcmp(bssid, UMESH_BSSID, IEEE80211_MAC_ADDR_LEN))
		return UMESH_RX_IGNORE_NOT_UMESH; 

	if (!memcmp(from, &state->self_addr, IEEE80211_MAC_ADDR_LEN))
		return UMESH_RX_IGNORE_FROM_SELF; 

    log_hex("from dest ",from, IEEE80211_MAC_ADDR_LEN);
	log_hex("to dest ",to, IEEE80211_MAC_ADDR_LEN);
	if (!(to[0] & 0x01) && memcmp(to, &state->self_addr, IEEE80211_MAC_ADDR_LEN))
		return UMESH_RX_IGNORE_TO_OTHER; 

	BUF_STRIP(frame, sizeof(struct umesh_ieee80211_hdr));

	switch (fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) {
		case IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ACTION:
	    
			return umesh_rx_ieee80211_payload(frame, from, to, state);
		case IEEE80211_FTYPE_DATA | IEEE80211_STYPE_DATA:
			return umesh_rx_ieee80211_payload(frame, from, to, state);
		default:
			log_w("ieee80211: cannot handle type %x and subtype %x",
			         fc & IEEE80211_FCTL_FTYPE, fc & IEEE80211_FCTL_STYPE);
			return UMESH_RX_UNEXPECTED_TYPE;
	}
buffer_error:
	return UMESH_RX_TOO_SHORT;
}


void umesh_wifi_rx_frame(uint8_t *buffer, int len,uint8_t rssi, void *context)
{
    const struct buf * rx_buf = buf_new_const(buffer,len);
    if(rx_buf == NULL) {
        return;
    }
    umesh_rx_ieee80211_frame(rx_buf ,rssi, (umesh_state_t *)context);    
}
