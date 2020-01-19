#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "buffer.h"
#include "state.h"
#include "unpack.h"
#include "ieee80211.h"
#include "log.h"
#include "utils.h"
#include "osal.h"
#include "pack.h"
#include "utils_aes.h"

extern const uint8_t UMESH_OUI[];
extern const uint8_t UMESH_BSSID[];
extern const uint8_t ETHER_BROADCAST[IEEE80211_MAC_ADDR_LEN];
static int umesh_parse_hdr(const struct buf *frame, uint16_t *seq, umesh_frame_type_t *frame_type,
                           umesh_peer_type_t *peer_type)
{
    const struct umesh_header *af;
    int valid;
    if (frame == NULL || seq == NULL || frame_type == NULL || peer_type == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }

    READ_BYTES(frame, 0, NULL, sizeof(struct umesh_header));

    af = (const struct umesh_header *) buf_data(frame);

    valid = af->category == IEEE80211_VENDOR_SPECIFIC &&
            !memcmp(&af->oui, (uint8_t *)UMESH_OUI, 3);

    if (!valid) {
        log_e("parse umesh hdr err!");
        return UMESH_RX_UNEXPECTED_TYPE;
    }

    *seq = af->frame_seq;
    *frame_type  = af->flag & 0x0f;
    *peer_type = (af->flag >> 4) & 0x03;
    return 0;
buffer_error:
    return UMESH_RX_TOO_SHORT;
}

static int umesh_parse_join_tlv_resp(umesh_peer_type_t peer_type, const uint8_t *from, const struct buf *tlv_buf,
                                     umesh_state_t *state)
{
    int ret = UMESH_PARSE_TLV_FAILED;
    struct umesh_peer *peer = NULL;
    uint8_t from_rand_array[UMESH_RANDOM_LEN];
    uint8_t to_rand_array[UMESH_RANDOM_LEN];
    uint32_t session_id = 0;
    uint16_t offset = 0;

    log_i("parse resp, tlv len = %d", tlv_buf->len);

    umesh_produce_random(to_rand_array, UMESH_RANDOM_LEN);
    READ_LE32(tlv_buf, offset, &session_id);
    offset += sizeof(session_id);
    READ_BYTES_COPY(tlv_buf, offset, from_rand_array, UMESH_RANDOM_LEN);

    ret = umesh_peer_get(state->peers_state.peers, from, &peer);
    if (ret < 0) {/*新peer:如果 peer 不存在， 创建新peer,回复信息 */
        peer = umesh_peer_add2(state->peers_state.peers, from, peer_type, UMESH_PEER_IDENTIFY_RESP,
                               umesh_now_ms(), session_id, from_rand_array, to_rand_array);

        if (peer == NULL) {
            log_hex("add peer failed", from, 6);
            ret = UMESH_PEER_ADD_FAILED;
            return ret;
        }
    } else {/*单向旧peer:如果 peer 已经存在，更新peer 信息 ，回复信息*/
        memcpy(peer->from_random, from_rand_array, UMESH_RANDOM_LEN);
        memcpy(peer->to_random, to_rand_array, UMESH_RANDOM_LEN);
        peer->session_id = session_id;
        peer->step = UMESH_PEER_IDENTIFY_RESP;
        peer->last_update = umesh_now_ms();
    }
    /*send finish to dst peer*/
    if (state->peer_add_cb) {
        state->peer_add_cb(from, UMESH_PEER_IDENTIFY_RESP, state);
    }
    ret = umesh_send_frame_join_finish(state, from);
    if (ret == 0) {
        /*update */
        peer->step = UMESH_PEER_IDENTIFY_FINISH;
        state->peers_state.joined = 1;
        if (state->peer_add_cb) {
            state->peer_add_cb(from, UMESH_PEER_IDENTIFY_FINISH, state);
        }
        log_i("recv join resp frame!");
        log_hex("peer mac", from, 6);

    }
    return ret;
buffer_error:
    log_e("UMESH_RX_TOO_SHORT!");
    return UMESH_RX_TOO_SHORT;
}

static int umesh_parse_join_tlv_finish(umesh_peer_type_t peer_type, const uint8_t *from, const struct buf *tlv_buf,
                                       umesh_state_t *state)
{
    int ret = UMESH_PARSE_TLV_FAILED;
    struct umesh_peer *peer = NULL;

    uint8_t from_rand_array[UMESH_RANDOM_LEN];
    uint32_t session_id = 0;
    uint16_t offset = 0;

    READ_LE32(tlv_buf, offset, &session_id);
    offset += sizeof(session_id);
    READ_BYTES_COPY(tlv_buf, offset, from_rand_array, UMESH_RANDOM_LEN);

    ret = umesh_peer_get(state->peers_state.peers, from, &peer);
    if (ret < 0) {/*如果peer 不存在，说明 应答有错误，不应该应答*/
        log_w("peer not found");
        log_hex("peer mac", from, 6);
    } else {/*单向旧peer:如果 peer 已经存在，更新peer 信息 ，回复信息*/

        if (peer->session_id != session_id) {
            log_w("get tlv finish frame, self session_id = %d,recv one = %d ", peer->session_id, session_id);
        }
        memcpy(peer->from_random, from_rand_array, UMESH_RANDOM_LEN);

        peer->session_id = session_id; /*update session id*/
        peer->last_update = umesh_now_ms();
        peer->step = UMESH_PEER_IDENTIFY_FINISH;
        state->peers_state.joined = 1;
        if (state->peer_add_cb) {
            state->peer_add_cb(from, UMESH_PEER_IDENTIFY_FINISH, state);
        }

        log_i("recv join finish frame!");
        log_hex("peer mac", from, 6);
        ret = 0;
    }

    return ret;
buffer_error:
    return UMESH_RX_TOO_SHORT;
}

static int umesh_parse_heart_tlv(umesh_peer_type_t peer_type, const uint8_t *from, const struct buf *tlv_buf,
                                 umesh_state_t *state)
{
    int ret = UMESH_PARSE_TLV_FAILED;
    struct umesh_peer *peer = NULL;
    ret = umesh_peer_get(state->peers_state.peers, from, &peer);
    if (ret < 0 || peer == NULL) {
        log_e("can not find the peer");
        return UMESH_ERR_PEER_MISSING;
    }

    READ_LE16(tlv_buf, 0, &peer->heart_interval);
    peer->last_update = umesh_now_ms();
    return ret;
buffer_error:
    return UMESH_ERR_OUT_OF_BOUNDS;
}

static int umesh_parse_zero_req_tlv(const uint8_t *from, const struct buf *tlv_buf,
                                 umesh_state_t *state)
{
    int ret = UMESH_PARSE_TLV_FAILED;
    struct umesh_peer *peer = NULL;
    ret = umesh_peer_get(state->peers_state.peers, from, &peer);
    if (ret < 0 || peer == NULL) {
        log_e("can not find the peer");
        return UMESH_ERR_PEER_MISSING;
    }
    READ_U8(tlv_buf, 0, &peer->zero_flag);
    
    ret = umesh_send_frame_zero_resp(state, from);
    return ret;
buffer_error:
    return UMESH_ERR_OUT_OF_BOUNDS;
}

static int umesh_parse_zero_finish_tlv(const uint8_t *from, const struct buf *tlv_buf,
                                 umesh_state_t *state)
{
    int ret = UMESH_PARSE_TLV_FAILED;
    struct umesh_peer *peer = NULL;
    ret = umesh_peer_get(state->peers_state.peers, from, &peer);
    if (ret < 0 || peer == NULL) {
        log_e("can not find the peer");
        return UMESH_ERR_PEER_MISSING;
    }
    READ_U8(tlv_buf, 0, &peer->zero_flag);
    /*todo*/
   
    return ret;
buffer_error:
    return UMESH_ERR_OUT_OF_BOUNDS;
}

static int umesh_parse_zero_resp_tlv(const uint8_t *from, const uint8_t *to, const struct buf *tlv_buf,
                                 umesh_state_t *state)
{
    uint8_t ssid_len = 0;
    uint8_t pwd_len = 0;
    uint16_t offset = 0;
    int ret = UMESH_PARSE_TLV_FAILED;
    struct umesh_peer *peer = NULL;
    uint8_t *decrypt_data = NULL;
    char ssid[UMESH_MAX_SSID_LEN] = {0};
    char pwd[UMESH_MAX_PASSWD_LEN] = {0};
    char bssid[IEEE80211_MAC_ADDR_LEN] = {0};
    struct umesh_peer *from_peer = NULL;
    uint8_t iv_data[UMESH_AES_KEY_LEN] = {0};
    p_Aes128_t  aes = NULL;
    ret = umesh_peer_get(state->peers_state.peers, from, &peer);
    if (ret < 0 || peer == NULL) {
        log_e("can not find the peer");
        return UMESH_ERR_PEER_MISSING;
    }
    READ_U8(tlv_buf, offset++, &peer->zero_flag);
    READ_U8(tlv_buf, offset++, &ssid_len);

    if(ssid_len == 0) {
        goto buffer_error;
    }
    READ_BYTES_COPY(tlv_buf, offset, ssid , ssid_len);
    log_i("---ssid len = %d, ssid = %s", ssid_len, ssid);
    offset += ssid_len;
    READ_U8(tlv_buf, offset++, &pwd_len);
    if(pwd_len != 0) {
        log_i("---pwd len = %d", pwd_len);
        READ_BYTES_COPY(tlv_buf, offset, pwd , pwd_len); 
        offset += pwd_len;
        decrypt_data = umesh_malloc(pwd_len);
        if(decrypt_data == NULL) {
            return UMESH_ERR_MALLOC_FAILED;
        }
/////////////////
    if (memcmp(to, state->self_addr, IEEE80211_MAC_ADDR_LEN) == 0) { /*ucast data to self*/
        /*decrypt the data*/
        ret = umesh_peer_get(state->peers_state.peers, from, &from_peer);
        if (ret < 0) {
            log_w("can not find the peer");
            return ret;
        }

        umesh_get_ucast_iv(from_peer->from_random, from_peer->to_random, iv_data);

        aes = utils_aes128_init(state->aes_key, iv_data, AES_DECRYPTION);
        if (aes == NULL) {
            umesh_free(decrypt_data);
            return UMESH_ERR_AES_INIT;
        }
        ret = utils_aes128_cfb_decrypt(aes, pwd, pwd_len, decrypt_data);
        utils_aes128_destroy(aes);
        if (ret < 0) {
            log_e("decrypt err, ret = %d", ret);
            umesh_free(decrypt_data);
            return UMESH_ERR_AES_DECRYPT;
        }

    } else if (memcmp(to, ETHER_BROADCAST, IEEE80211_MAC_ADDR_LEN) == 0)  { /* bcast to self*/
        /*if da not in neiborlist, drop it*/
        uint8_t session_array[sizeof(uint32_t)] = {0};
        union array_uint32 temp;

        ret = umesh_peer_get(state->peers_state.peers, from, &from_peer);
        if (ret < 0) {
            log_w("can not find src peer");
            return ret;
        }
        /*else, recypt it and relay to da*/

        temp.value32 = umesh_htole32(from_peer->session_id);
        memcpy(session_array, temp.array, sizeof(session_array));
        umesh_get_bcast_iv(from_peer->addr, session_array, iv_data);

        aes = utils_aes128_init(state->aes_key, iv_data, AES_DECRYPTION);
        if (aes == NULL) {
            umesh_free(decrypt_data);
            return UMESH_ERR_AES_INIT;
        }
        ret = utils_aes128_cfb_decrypt(aes, pwd, pwd_len, decrypt_data);
        utils_aes128_destroy(aes);
        if (ret < 0) {
            log_e("decrypt err, ret = %d", ret);
            umesh_free(decrypt_data);
            return UMESH_ERR_AES_DECRYPT;
        }

    }
    
    
/////////////////
    }

    if(peer->zero_flag & BIT2) {
        READ_BYTES(tlv_buf, offset, bssid , IEEE80211_MAC_ADDR_LEN); 
    }
    if(state->get_ap_info_cb) {
        state->get_ap_info_cb(ssid, decrypt_data, bssid);
    }
    umesh_free(decrypt_data);
    ret = umesh_send_frame_zero_finish(state, from);
    return ret;
buffer_error:
    return UMESH_ERR_OUT_OF_BOUNDS;
}

static int umesh_parse_data_tlv(umesh_peer_type_t peer_type, const uint8_t *from, const uint8_t *to,
                                const struct buf *tlv_buf, umesh_state_t *state)
{
    int ret = UMESH_PARSE_TLV_FAILED;
    uint8_t ext_addr = 0;
    uint16_t seq_num = 0;
    uint16_t offset = 0;
    const uint8_t *sa = NULL;
    const uint8_t *da = NULL;
    const uint8_t *data_org = 0;
    uint16_t data_len = 0;
    struct umesh_peer *to_peer = NULL;
    struct umesh_peer *from_peer = NULL;
    p_Aes128_t  aes = NULL;
    uint8_t iv_data[UMESH_AES_KEY_LEN] = {0};
    uint8_t *decrypt_data = NULL;
    if (state == NULL || from == NULL || to == NULL || tlv_buf == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    const uint8_t *cur = buf_data(tlv_buf);
    ext_addr = cur[0] & BIT0;
    offset += 1;
    READ_LE16(tlv_buf, offset, &seq_num);
    offset += 2;
    /*relayed by others */
    if (ext_addr != 0) {
        /*read sa and da*/
        READ_BYTES(tlv_buf, offset, &sa, IEEE80211_MAC_ADDR_LEN);
        offset += IEEE80211_MAC_ADDR_LEN;
        READ_BYTES(tlv_buf, offset, &da, IEEE80211_MAC_ADDR_LEN);
        offset += IEEE80211_MAC_ADDR_LEN;
    } else {
        sa = from;
        da = to;
    }
    data_len = buf_len(tlv_buf) - offset;
    READ_BYTES(tlv_buf, offset, &data_org, data_len);

    if (memcmp(to, state->self_addr, IEEE80211_MAC_ADDR_LEN) == 0) { /*ucast data to self*/
        /*decrypt the data*/
        ret = umesh_peer_get(state->peers_state.peers, from, &from_peer);
        if (ret < 0) {
            log_w("can not find the peer");
            return ret;
        }

        umesh_get_ucast_iv(from_peer->from_random, from_peer->to_random, iv_data);
        decrypt_data = umesh_malloc(data_len);
        if (decrypt_data == NULL) {
            return UMESH_ERR_MALLOC_FAILED;
        }

        aes = utils_aes128_init(state->aes_key, iv_data, AES_DECRYPTION);
        if (aes == NULL) {
            umesh_free(decrypt_data);
            return UMESH_ERR_AES_INIT;
        }
        ret = utils_aes128_cfb_decrypt(aes, data_org, data_len, decrypt_data);
        utils_aes128_destroy(aes);
        if (ret < 0) {
            log_e("decrypt err, ret = %d", ret);
            umesh_free(decrypt_data);
            return UMESH_ERR_AES_DECRYPT;
        }
        if (state->raw_data_cb) {
            state->raw_data_cb(decrypt_data, data_len, from, sa, da, (void *)state);
        }
        umesh_free(decrypt_data);
    } else if (memcmp(to, ETHER_BROADCAST, IEEE80211_MAC_ADDR_LEN) == 0)  { /* bcast to self*/
        /*if da not in neiborlist, drop it*/
        uint8_t session_array[sizeof(uint32_t)] = {0};
        union array_uint32 temp;

        ret = umesh_peer_get(state->peers_state.peers, from, &from_peer);
        if (ret < 0) {
            log_w("can not find src peer");
            return ret;
        }
        /*else, recypt it and relay to da*/

        temp.value32 = umesh_htole32(from_peer->session_id);
        memcpy(session_array, temp.array, sizeof(session_array));
        umesh_get_bcast_iv(from_peer->addr, session_array, iv_data);

        decrypt_data = umesh_malloc(data_len);
        if (decrypt_data == NULL) {
            return UMESH_ERR_MALLOC_FAILED;
        }

        aes = utils_aes128_init(state->aes_key, iv_data, AES_DECRYPTION);
        if (aes == NULL) {
            umesh_free(decrypt_data);
            return UMESH_ERR_AES_INIT;
        }
        ret = utils_aes128_cfb_decrypt(aes, data_org, data_len, decrypt_data);
        utils_aes128_destroy(aes);
        if (ret < 0) {
            log_e("decrypt err, ret = %d", ret);
            umesh_free(decrypt_data);
            return UMESH_ERR_AES_DECRYPT;
        }

        if (state->raw_data_cb) {
            state->raw_data_cb(decrypt_data, data_len, from, sa, da, (void *)state);
        }
        umesh_free(decrypt_data);
    }
    return ret;
buffer_error:
    return UMESH_ERR_OUT_OF_BOUNDS;
}

static int umesh_handle_tlv(umesh_frame_type_t frame_type, umesh_peer_type_t peer_type, const uint8_t *from,
                            const uint8_t *to, umesh_tlvs_t tlv_type, const struct buf *tlv_buf, umesh_state_t *state)
{
    int ret = UMESH_RX_UNEXPECTED_TYPE;
    switch (tlv_type) {

        case UMESH_VENDOR_TLV:
            break;
        case UMESH_JOIN_TLV:
            if (frame_type == UMESH_FRAME_JOIN_PERMIT) {
                ret = umesh_parse_join_tlv_resp(peer_type, from, tlv_buf, state);
            } else if (frame_type == UMESH_FRAME_JOIN_FINISH) {
                ret = umesh_parse_join_tlv_finish(peer_type, from, tlv_buf, state);
            }
            break;
        case UMESH_HEART_BEAT_TLV:
            if (frame_type < UMESH_FRAME_JOIN_PERMIT) {
                break;
            }
            ret = umesh_parse_heart_tlv(peer_type, from, tlv_buf, state);
            break;
        case UMESH_DATA_TLV:
            if (frame_type == UMESH_FRAME_DATA) {
                ret = umesh_parse_data_tlv(peer_type, from, to, tlv_buf, state);
                break;
            }

            break;
        case UMESH_ZERO_REQ_TLV:
            log_d("-----recv zero req tlv-------");
            ret = umesh_parse_zero_req_tlv(from, tlv_buf, state);       
            break;
        case UMESH_ZERO_RESP_TLV:
            log_d("-----recv zero rsp tlv-------");
            ret = umesh_parse_zero_resp_tlv(from, to, tlv_buf, state);
            break;
        case UMESH_ZERO_FINISH_TLV:
             log_d("-----recv zero finish tlv-------");
            ret = umesh_parse_zero_finish_tlv(from, tlv_buf, state);
            break;
    }
    return ret;
}


int umesh_rx_ieee80211_payload(const struct buf *frame, const uint8_t *from, const uint8_t *to, umesh_state_t *state)
{
    int ret = 0;
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
        int result = umesh_handle_tlv(frame_type, peer_type, from, to, tlv_type, tlv_buf, state);
        buf_free(tlv_buf);

        if (result < 0) {
            log_w("parsing %s error = %d ", umesh_tlv_as_str(tlv_type), result);
            return UMESH_RX_UNEXPECTED_TYPE;
        }
        left_len -= len;
        log_i("cur offset = %d, left_len = %d,cur tlv = %s", len, left_len, umesh_tlv_as_str(tlv_type));
        buf_strip(frame, len);
    }


    switch (frame_type) {
        case UMESH_FRAME_DISCOVERY_REQ:
            ret = umesh_send_frame_discovery_resp(state, from);
            break;
        case UMESH_FRAME_DISCOVERY_RESP:
            if (state->self_phase == UMESH_PHASE_SCAN) {
                ret = umesh_add_scaned_count(state, peer_type);
            }
            break;
        case UMESH_FRAME_JOIN_REQ: {
            /*看下sa是否在当前peers列表中，如果不在则创建一个peer,给其分配session id/ token值*/

            struct umesh_peer *peer = NULL;
            uint8_t rand_array[UMESH_RANDOM_LEN] = {0};
            ret = umesh_peer_get(state->peers_state.peers, from, &peer);
            if (ret == 0 && peer != NULL) {
                if (peer->step > UMESH_PEER_IDENTIFY_RESP) { /* no need action*/
                    ret = UMESH_NO_ACTION_REQUIRED;
                    break;
                }  /*send join resp*/
                peer->step = UMESH_PEER_IDENTIFY_REQ;
            } else {
                umesh_produce_random(rand_array, UMESH_RANDOM_LEN);
                peer = umesh_peer_add2(state->peers_state.peers, from, peer_type, UMESH_PEER_IDENTIFY_REQ,
                                       umesh_now_ms(), 0, NULL, rand_array);
            }

            if (peer == NULL) {
                ret = UMESH_PEER_ADD_FAILED;
                break;
            }
            if (state->peer_add_cb) {
                state->peer_add_cb(from, UMESH_PEER_IDENTIFY_REQ, state);
            }
            ret = umesh_send_frame_join_resp(state, from);
            if (ret == 0) {
                peer->step = UMESH_PEER_IDENTIFY_RESP;
                if (state->peer_add_cb) {
                    state->peer_add_cb(from, UMESH_PEER_IDENTIFY_RESP, state);
                }
            }
            break;
        }
        case UMESH_FRAME_JOIN_PERMIT: {
            /*deal in tlv parse*/

        }
            /*parse tlv */
            /*看下sa是否在当前peers列表中，如果不在则创建一个peer,给其分配session id/ token值，并回复finish*/
        break;
        case UMESH_FRAME_JOIN_FINISH:
            /*deal in tlv parse*/
            /*peer 存在则更新 peer 状态，完成认证*/
            break;
        case UMESH_FRAME_HEART_BEAT:
            /*deal in tlv parse*/
            /* 更新 peer 上一次 心跳时间*/
            break;
        case UMESH_FRAME_DATA:
            /*deal in tlv parse*/
            /*解析数据，da不为自己则转发，否则调用回调往上传数据*/
            break;
        default:
            break;
    }

    return ret;
}

int umesh_rx_ieee80211_frame(const struct buf *frame, uint8_t rssi, umesh_state_t *state)
{
    const struct umesh_ieee80211_hdr *ieee80211;
    const uint8_t *from, *to, *bssid;
    uint16_t fc;

    //READ_BYTES(frame, 0, NULL, sizeof(struct umesh_ieee80211_hdr));

    ieee80211 = (const struct umesh_ieee80211_hdr *)(buf_data(frame));
    from = ieee80211->addr2;
    to = ieee80211->addr1;
    bssid = ieee80211->addr3;

    fc = umesh_le16toh(ieee80211->frame_control);

    if (memcmp(bssid, UMESH_BSSID, IEEE80211_MAC_ADDR_LEN) != 0) {
        return UMESH_RX_IGNORE_NOT_UMESH;
    }

    if (!memcmp(from, &state->self_addr, IEEE80211_MAC_ADDR_LEN)) {
        return UMESH_RX_IGNORE_FROM_SELF;
    }

    log_hex("from dest ", from, IEEE80211_MAC_ADDR_LEN);
    log_hex("to dest ", to, IEEE80211_MAC_ADDR_LEN);
    if (!(to[0] & 0x01) && memcmp(to, &state->self_addr, IEEE80211_MAC_ADDR_LEN)) {
        return UMESH_RX_IGNORE_TO_OTHER;
    }

    BUF_STRIP(frame, sizeof(struct umesh_ieee80211_hdr));

    switch (fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) {
        case IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ACTION:
        case IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_PROBE_REQ:
        case IEEE80211_FTYPE_DATA | IEEE80211_STYPE_DATA:
            log_hex("rx umesh", buf_data(frame), buf_len(frame));
            return umesh_rx_ieee80211_payload(frame, from, to, state);
        default:
            log_w("ieee80211: cannot handle type %x and subtype %x",
                  fc & IEEE80211_FCTL_FTYPE, fc & IEEE80211_FCTL_STYPE);
            return UMESH_RX_UNEXPECTED_TYPE;
    }
buffer_error:
    return UMESH_RX_TOO_SHORT;
}


void umesh_wifi_rx_frame(uint8_t *buffer, int len, uint8_t rssi, void *context)
{
    const struct buf *rx_buf = buf_new_const(buffer, len);
    //log_hex("rx frame",buf_data(rx_buf), buf_len(rx_buf));
    if (rx_buf == NULL) {
        return;
    }

    umesh_rx_ieee80211_frame(rx_buf, rssi, (umesh_state_t *)context);
    buf_free(rx_buf);
}
