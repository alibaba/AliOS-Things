
#include <stdint.h>
#include <string.h>

#include "frame.h"
#include "crc32.h"
#include "define.h"
#include "state.h"
#include "peers.h"
#include "ieee80211.h"
#include "log.h"
#include "utils.h"
#include "osal.h"
#include "utils_aes.h"

const uint8_t UMESH_BSSID[]  = {0xb0, 0xf8, 0x93, 0x00, 0x00, 0x07};
const uint8_t BCAST_ADDR[]  = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
const uint8_t UMESH_OUI[]   = {0xa1, 0x1b, 0xba};
extern const uint8_t ETHER_BROADCAST[IEEE80211_MAC_ADDR_LEN];

static int umesh_init_header(uint8_t *buf, struct umesh_state *state, umesh_frame_type_t frame_type)
{
    struct umesh_header *af = (struct umesh_header *) buf;

    af->category = IEEE80211_VENDOR_SPECIFIC; /* vendor specific */
    memcpy(af->oui, (uint8_t *)UMESH_OUI, 3);
    /* af->frame_seq = umesh_get_next_frame_seq(state); */ /*maybe redundancy */
    af->flag |= (0x0f & frame_type); /*set frame*/
    af->flag |= (0x30 & state->self_type); /*node type*/

    return sizeof(struct umesh_header);
}

// static int umesh_init_join_tlv(uint8_t *buf, const struct umesh_state *state, umesh_peer_t *peer) {

//  int len;
//  int ret;

//  if(buf == NULL || state == NULL || peer == NULL) {
//      return 0;
//  }

//  struct umesh_join_paras_tlv *tlv = (struct umesh_join_paras_tlv *) buf;

//  tlv->type = UMESH_JOIN_TLV;

//  tlv->session_id = htole32(peer->session_id);
//     memcpy(tlv->random, peer->to_random, UMESH_RANDOM_LEN);
//  len = sizeof(struct umesh_join_paras_tlv);
//  tlv->length = htole16(len - sizeof(struct tl));
//  return len;
// }
static int umesh_init_join_tlv(uint8_t *buf, const struct umesh_state *state, uint8_t *da)
{

    int len;
    int ret;
    umesh_peer_t *peer = NULL;
    if (buf == NULL || state == NULL || da == NULL) {
        return 0;
    }

    struct umesh_join_paras_tlv *tlv = (struct umesh_join_paras_tlv *) buf;

    tlv->type = UMESH_JOIN_TLV;


    ret = umesh_peer_get(state->peers_state.peers, da, &peer);
    if (ret < 0) {
        return ret;
    }

    tlv->session_id = umesh_htole32(peer->session_id);
    memcpy(tlv->random, peer->to_random, UMESH_RANDOM_LEN);
    len = sizeof(struct umesh_join_paras_tlv);
    tlv->length = umesh_htole16(len - sizeof(struct tl));
    return len;
}

static int umesh_init_heartbeat_tlv(uint8_t *buf, const struct umesh_state *state)
{
    int len;
    if (buf == NULL || state == NULL) {
        return 0;
    }
    struct umesh_heart_beat_tlv *tlv = (struct umesh_heart_beat_tlv *) buf;
    tlv->type = UMESH_HEART_BEAT_TLV;
    tlv->internal = umesh_htole16(state->heart_duration);

    len = sizeof(struct umesh_heart_beat_tlv);

    tlv->length = umesh_htole16(len - sizeof(struct tl));

    return len;
}


static int umesh_init_data_tlv(uint8_t *buf, struct umesh_state *state,
                               uint8_t *src, uint8_t *dst, uint8_t *data, uint16_t data_len)
{
    int len;
    if (buf == NULL || state == NULL || data == NULL) {
        return 0;
    }
    struct umesh_data_fixed_tlv *tlv = (struct umesh_data_fixed_tlv *) buf;
    tlv->type = UMESH_DATA_TLV;
    tlv->seq_num = umesh_htole16(umesh_data_next_sequence_number(state));
    len = sizeof(struct umesh_data_fixed_tlv);

    if (src != NULL && dst != NULL) {
        tlv->flag |= BIT0;
        memcpy(buf + len, src, IEEE80211_MAC_ADDR_LEN);
        len +=  IEEE80211_MAC_ADDR_LEN;
        memcpy(buf + len, dst, IEEE80211_MAC_ADDR_LEN);
        len +=  IEEE80211_MAC_ADDR_LEN;
    }

    memcpy(buf + len, data, data_len);
    len += data_len;

    tlv->length = umesh_htole16(len - sizeof(struct tl));

    return len;
}


static int ieee80211_init_hdr(uint8_t *buf, const uint8_t  *src, const uint8_t  *dst,
                              uint16_t seq_ctrl, uint16_t type)
{
    struct umesh_ieee80211_hdr *hdr = (struct umesh_ieee80211_hdr *) buf;

    hdr->frame_control = umesh_htole16(type);
    hdr->duration_id = umesh_htole16(0);
    memcpy(hdr->addr1, dst, IEEE80211_MAC_ADDR_LEN);
    memcpy(hdr->addr2, src, IEEE80211_MAC_ADDR_LEN);
    memcpy(hdr->addr3, (uint8_t *)UMESH_BSSID, IEEE80211_MAC_ADDR_LEN);

    hdr->seq_ctrl = umesh_htole16(seq_ctrl) << 4;

    return sizeof(struct umesh_ieee80211_hdr);
}

static int umesh_ieee80211_init_hdr(uint8_t *buf, struct umesh_state *state, const uint8_t  *dst)
{
#ifdef USE_ACTION_FRAME
    return ieee80211_init_hdr(buf, state->self_addr, dst, umesh_ieee80211_next_sequence_number(state),
                              IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_PROBE_REQ);
#else
    return ieee80211_init_hdr(buf, state->self_addr, dst, umesh_ieee80211_next_sequence_number(state),
                              IEEE80211_FTYPE_DATA | IEEE80211_STYPE_DATA);
#endif
}


static int umesh_ieee80211_add_fcs(const uint8_t *start, uint8_t *end)
{
    union array_uint32 temp;

    temp.value32 = umesh_htole32(crc32(start, end - start));

    //get_fcs(start, end - start + 4);
    memcpy(end, temp.array, sizeof(uint32_t));
    return sizeof(uint32_t);
}

int umesh_init_full_frame(umesh_buf_t *buf, struct umesh_state *state, struct umesh_send_message *message)
{
    uint8_t *ptr = (uint8_t *)buf_data(buf);
    int len = buf_len(buf);
    int request_len =  IEEE80211_MAC_HEADER_LEN + sizeof(struct umesh_header) + UMESH_FCS_LEN;

    if (buf == NULL || state == NULL || message == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }

    if (len < request_len) {
        return UMESH_ERR_OUT_OF_BOUNDS;
    }

    ptr += umesh_ieee80211_init_hdr(ptr, state, message->ra_enable ? message->ra : message->da);

    ptr += umesh_init_header(ptr, state, message->type);
    switch (message->type) {
        case UMESH_FRAME_DISCOVERY_REQ:
            break;
        case UMESH_FRAME_DISCOVERY_RESP:
            break;
        case UMESH_FRAME_JOIN_REQ:
            break;
        case UMESH_FRAME_JOIN_PERMIT:
        case UMESH_FRAME_JOIN_FINISH:

            request_len += sizeof(struct umesh_join_paras_tlv) + sizeof(struct umesh_heart_beat_tlv);
            if (len < request_len) {
                log_e("buf len = %d , request len = %d", len, request_len);
                return UMESH_ERR_OUT_OF_BOUNDS;
            }
            ptr += umesh_init_join_tlv(ptr, state, message->da);
            ptr += umesh_init_heartbeat_tlv(ptr, state);
            break;
        case UMESH_FRAME_HEART_BEAT:
            request_len += sizeof(struct umesh_heart_beat_tlv);
            if (len < request_len) {
                log_e("buf len = %d , request len = %d", len, request_len);
                return UMESH_ERR_OUT_OF_BOUNDS;
            }
            ptr += umesh_init_heartbeat_tlv(ptr, state);
            break;
        case UMESH_FRAME_DATA:
            request_len += sizeof(struct umesh_heart_beat_tlv) + sizeof(struct umesh_data_fixed_tlv) + message->data_len +
                           message->ra_enable ? IEEE80211_MAC_ADDR_LEN * 2 : 0;

            if (len < request_len) {
                log_e("buf len = %d , request len = %d", len, request_len);
                return UMESH_ERR_OUT_OF_BOUNDS;
            }
            ptr += umesh_init_heartbeat_tlv(ptr, state);
            ptr += umesh_init_data_tlv(ptr, state, message->ra_enable ? state->self_addr : NULL,
                                       message->ra_enable ? message->da : NULL, message->data, message->data_len);
            break;

    }

    ptr += umesh_ieee80211_add_fcs(buf_data(buf), ptr);
    //log_hex("umesh_init_full_frame:",buf_data(buf),ptr - buf_data(buf));
    return (int)(ptr - buf_data(buf));
}


int umesh_init_frame_discovery(umesh_state_t *state, umesh_buf_t **buf)
{
    int ret = 0;
    struct umesh_send_message message;
    int request_len = IEEE80211_MAC_HEADER_LEN + sizeof(struct umesh_header) + UMESH_FCS_LEN;
    umesh_buf_t *send_buf;


    if (state == NULL || buf == NULL) {
        log_e("state = %p ,buf = %p", state, buf);
        return UMESH_ERR_NULL_POINTER;
    }

    send_buf = buf_new_owned(request_len);

    if (send_buf == NULL) {
        return UMESH_ERR_MALLOC_FAILED;
    }
    *buf = send_buf;
    memset(send_buf->data, 0, send_buf->len);
    memset(&message, 0, sizeof(struct umesh_send_message));
    memcpy(message.da, BCAST_ADDR, IEEE80211_MAC_ADDR_LEN);
    message.type = UMESH_FRAME_DISCOVERY_REQ;

    ret = umesh_init_full_frame(send_buf, state, &message);
    if (ret < 0) {
        buf_free(send_buf);
    }
    return ret;
}

int umesh_init_frame_discovery_resp(umesh_state_t *state, const uint8_t *dst, umesh_buf_t **buf)
{
    int ret = 0;
    struct buf *send_buf;
    int request_len = IEEE80211_MAC_HEADER_LEN + sizeof(struct umesh_header) + UMESH_FCS_LEN;
    struct umesh_send_message message;
    if (dst == NULL || state == NULL || buf == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    send_buf = buf_new_owned(request_len);

    if (send_buf == NULL) {
        return UMESH_ERR_MALLOC_FAILED;
    }
    *buf = send_buf;
    memset(send_buf->data, 0, send_buf->len);
    memset(&message, 0, sizeof(struct umesh_send_message));
    message.type = UMESH_FRAME_DISCOVERY_RESP;
    memcpy(message.da, dst, IEEE80211_MAC_ADDR_LEN);
    ret = umesh_init_full_frame(send_buf, state, &message);
    if (ret < 0) {
        buf_free(send_buf);
    }

    return ret;
}

int umesh_init_frame_join_request(umesh_state_t *state, umesh_buf_t **buf)
{
    int ret = 0;
    int request_len = IEEE80211_MAC_HEADER_LEN + sizeof(struct umesh_header) + UMESH_FCS_LEN;
    struct buf *send_buf;
    struct umesh_send_message message;
    if (state == NULL || buf == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    send_buf = buf_new_owned(request_len);
    if (send_buf == NULL) {
        return UMESH_ERR_MALLOC_FAILED;
    }
    *buf = send_buf;

    memset(send_buf->data, 0, send_buf->len);
    memset(&message, 0, sizeof(struct umesh_send_message));
    memcpy(message.da, BCAST_ADDR, IEEE80211_MAC_ADDR_LEN);
    message.type = UMESH_FRAME_JOIN_REQ;
    ret = umesh_init_full_frame(send_buf, state, &message);
    if (ret < 0) {
        buf_free(send_buf);
    }
    return ret;
}

int umesh_init_frame_join_resp(umesh_state_t *state, const uint8_t *dst, umesh_buf_t **buf)
{
    int ret = 0;
    struct buf *send_buf;
    int request_len = IEEE80211_MAC_HEADER_LEN + sizeof(struct umesh_header) + sizeof(struct umesh_join_paras_tlv) + sizeof(
                                  struct umesh_heart_beat_tlv) + UMESH_FCS_LEN;
    struct umesh_send_message message;
    if (dst == NULL || state == NULL || buf == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }

    send_buf = buf_new_owned(request_len);

    if (send_buf == NULL) {
        return UMESH_ERR_MALLOC_FAILED;
    }
    *buf = send_buf;
    memset(send_buf->data, 0, send_buf->len);
    memset(&message, 0, sizeof(struct umesh_send_message));
    message.type = UMESH_FRAME_JOIN_PERMIT;
    memcpy(message.da, dst, IEEE80211_MAC_ADDR_LEN);
    ret = umesh_init_full_frame(send_buf, state, &message);
    if (ret < 0) {
        buf_free(send_buf);
    }

    return ret;
}

int umesh_init_frame_join_finish(umesh_state_t *state, const uint8_t *dst, umesh_buf_t **buf)
{
    int ret = 0;
    struct buf *send_buf;
    int request_len = IEEE80211_MAC_HEADER_LEN + sizeof(struct umesh_header) + sizeof(struct umesh_join_paras_tlv) + sizeof(
                                  struct umesh_heart_beat_tlv) + UMESH_FCS_LEN;
    struct umesh_send_message message;
    if (dst == NULL || state == NULL || buf == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }

    send_buf = buf_new_owned(request_len);

    if (send_buf == NULL) {
        return UMESH_ERR_MALLOC_FAILED;
    }
    *buf = send_buf;
    memset(send_buf->data, 0, send_buf->len);
    memset(&message, 0, sizeof(struct umesh_send_message));
    message.type = UMESH_FRAME_JOIN_FINISH;
    memcpy(message.da, dst, IEEE80211_MAC_ADDR_LEN);
    ret = umesh_init_full_frame(send_buf, state, &message);
    if (ret < 0) {
        buf_free(send_buf);
    }
    return ret;
}

int umesh_init_frame_heart_beat(umesh_state_t *state, umesh_buf_t **buf)
{
    int ret = 0;
    struct buf *send_buf;
    int request_len = IEEE80211_MAC_HEADER_LEN + sizeof(struct umesh_header) + sizeof(struct umesh_heart_beat_tlv) +
                      UMESH_FCS_LEN;
    struct umesh_send_message message;
    if (state == NULL || buf == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }

    send_buf = buf_new_owned(request_len);

    if (send_buf == NULL) {
        return UMESH_ERR_MALLOC_FAILED;
    }
    *buf = send_buf;
    memset(send_buf->data, 0, send_buf->len);
    memset(&message, 0, sizeof(struct umesh_send_message));
    message.type = UMESH_FRAME_HEART_BEAT;
    memcpy(message.da, BCAST_ADDR, IEEE80211_MAC_ADDR_LEN);
    ret = umesh_init_full_frame(send_buf, state, &message);
    if (ret < 0) {
        log_e("init full frame failed,ret = %d", ret);
        buf_free(send_buf);
    }
    return ret;
}

int umesh_init_frame_data(umesh_state_t *state, const uint8_t *ra, const uint8_t *da,
                          const uint8_t *data, int data_len, umesh_buf_t **buf)
{
    int ret = 0;
    struct buf *send_buf;
    uint8_t *encrypt_data = NULL;
    uint8_t iv_data[UMESH_AES_KEY_LEN] = {0};
    uint8_t bcast = 0;
    p_Aes128_t aes = NULL;
    int request_len = IEEE80211_MAC_HEADER_LEN + sizeof(struct umesh_header) + sizeof(struct umesh_heart_beat_tlv) +
                      sizeof(struct umesh_data_fixed_tlv) + (ra == NULL ? 0 : IEEE80211_MAC_ADDR_LEN * 2) + data_len + UMESH_FCS_LEN;
    struct umesh_send_message message;
    if (da == NULL || state == NULL || buf == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }

    log_d("init data frame len = %d", request_len);
    encrypt_data = umesh_malloc(data_len);
    if (encrypt_data == NULL) {
        return UMESH_ERR_MALLOC_FAILED;
    }

    send_buf = buf_new_owned(request_len);

    if (send_buf == NULL) {
        umesh_free(encrypt_data);
        return UMESH_ERR_MALLOC_FAILED;
    }
    *buf = send_buf;
    memset(send_buf->data, 0, send_buf->len);
    memset(&message, 0, sizeof(struct umesh_send_message));
    message.type = UMESH_FRAME_DATA;
    if (ra != NULL) {

        if (memcmp(ra, ETHER_BROADCAST, IEEE80211_MAC_ADDR_LEN) == 0) { /* broadcast -> unicast*/
            bcast = 1;
        } else { /*unicast -> unicast*/
            bcast = 0;
        }

        memcpy(message.ra, ra, IEEE80211_MAC_ADDR_LEN);
        message.ra_enable = 1;
    } else { /*  no relay*/
        if (memcmp(da, ETHER_BROADCAST, IEEE80211_MAC_ADDR_LEN) == 0) { /* broadcast*/
            bcast = 1;
        } else { /* unicast*/
            bcast = 0;
        }
    }

    if (bcast == 0) { /* unicast encrypto */
        const uint8_t *to_addr = ra ? ra : da;
        struct umesh_peer *to_peer = NULL;
        ret = umesh_peer_get(state->peers_state.peers, to_addr, &to_peer);
        if (ret < 0) {
            buf_free(send_buf);
            umesh_free(encrypt_data);
            return UMESH_ERR_PEER_MISSING;
        }

        umesh_get_ucast_iv(to_peer->from_random, to_peer->to_random, iv_data);

        aes = utils_aes128_init(state->aes_key, iv_data, AES_ENCRYPTION);
        if (aes == NULL) {
            buf_free(send_buf);
            umesh_free(encrypt_data);
            return UMESH_ERR_AES_INIT;
        }

        ret = utils_aes128_cfb_encrypt(aes, data, data_len, encrypt_data);
        utils_aes128_destroy(aes);
        if (ret < 0) {
            buf_free(send_buf);
            umesh_free(encrypt_data);
            return UMESH_ERR_AES_ENCRYPT;
        }
    } else { /* broadcast encrypto */
        uint8_t session_array[4] = {0};
        union array_uint32 temp;
        temp.value32 = umesh_htole32(state->session_id);
        memcpy(session_array, temp.array, sizeof(session_array));
        umesh_get_bcast_iv(state->self_addr, session_array, iv_data);

        aes = utils_aes128_init(state->aes_key, iv_data, AES_ENCRYPTION);
        if (aes == NULL) {
            buf_free(send_buf);
            return UMESH_ERR_AES_INIT;
        }

        ret = utils_aes128_cfb_encrypt(aes, data, data_len, encrypt_data);
        utils_aes128_destroy(aes);
        if (ret < 0) {
            buf_free(send_buf);
            umesh_free(encrypt_data);
            return UMESH_ERR_AES_ENCRYPT;
        }
    }

    memcpy(message.da, da, IEEE80211_MAC_ADDR_LEN);
    message.data = encrypt_data;
    message.data_len = data_len;

    /*data seq num not used yet !!!*/

    ret = umesh_init_full_frame(send_buf, state, &message);
    if (ret < 0) {
        buf_free(send_buf);
    }
    umesh_free(encrypt_data);
    return ret;
}

int umesh_send_frame_discovery(umesh_state_t *state)
{
    int ret;
    umesh_buf_t *buf = NULL;
    if (state == NULL) {
        log_e("state == NULL");
        return UMESH_ERR_NULL_POINTER;
    }
    ret = umesh_init_frame_discovery(state, &buf);
    if (ret < 0) {
        return ret;
    }
    log_i("add to list ,buf len =%d", buf->len);
    umesh_mutex_lock(state->to_radio_list_lock);
    list_add_tail(&buf->linked_list, &state->send_to_radio_list);
    umesh_mutex_unlock(state->to_radio_list_lock);
    umesh_semaphore_post(state->generic_semap);
    return 0;
}

int umesh_send_frame_discovery_resp(umesh_state_t *state, const uint8_t *dst)
{
    int ret;
    umesh_buf_t *buf = NULL;
    if (state == NULL || dst == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }

    ret = umesh_init_frame_discovery_resp(state, dst, &buf);
    if (ret < 0) {
        return ret;
    }
    umesh_mutex_lock(state->to_radio_list_lock);
    list_add_tail(&buf->linked_list, &state->send_to_radio_list);
    umesh_mutex_unlock(state->to_radio_list_lock);
    umesh_semaphore_post(state->generic_semap);
    return 0;
}

int umesh_send_frame_join_request(umesh_state_t *state)
{
    int ret;
    umesh_buf_t *buf = NULL;
    if (state == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }

    ret = umesh_init_frame_join_request(state, &buf);
    if (ret < 0) {
        return ret;
    }
    umesh_mutex_lock(state->to_radio_list_lock);
    list_add_tail(&buf->linked_list, &state->send_to_radio_list);
    umesh_mutex_unlock(state->to_radio_list_lock);
    umesh_semaphore_post(state->generic_semap);
    return 0;
}

int umesh_send_frame_join_resp(umesh_state_t *state, const uint8_t *dst)
{
    int ret;
    umesh_buf_t *buf = NULL;
    if (state == NULL || dst == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }

    ret = umesh_init_frame_join_resp(state, dst, &buf);
    if (ret < 0) {
        return ret;
    }
    umesh_mutex_lock(state->to_radio_list_lock);
    list_add_tail(&buf->linked_list, &state->send_to_radio_list);
    umesh_mutex_unlock(state->to_radio_list_lock);
    umesh_semaphore_post(state->generic_semap);
    return 0;
}

int umesh_send_frame_join_finish(umesh_state_t *state, const uint8_t *dst)
{
    int ret;
    umesh_buf_t *buf = NULL;
    if (state == NULL || dst == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }

    ret = umesh_init_frame_join_finish(state, dst, &buf);
    if (ret < 0) {
        return ret;
    }
    umesh_mutex_lock(state->to_radio_list_lock);
    list_add_tail(&buf->linked_list, &state->send_to_radio_list);
    umesh_mutex_unlock(state->to_radio_list_lock);
    umesh_semaphore_post(state->generic_semap);
    return 0;
}

int umesh_send_frame_heart_beat(umesh_state_t *state)
{
    int ret;
    umesh_buf_t *buf = NULL;
    log_d("umesh_send_frame_heart_beat in ");
    if (state == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    ret = umesh_init_frame_heart_beat(state, &buf);
    if (ret < 0) {
        log_e("init heart beat frame failed,ret = %d", ret);
        return ret;
    }
    log_d("add heart beat frame to send list");
    umesh_mutex_lock(state->to_radio_list_lock);
    list_add_tail(&buf->linked_list, &state->send_to_radio_list);
    umesh_mutex_unlock(state->to_radio_list_lock);
    umesh_semaphore_post(state->generic_semap);
    return 0;
}

int umesh_send_frame_data(umesh_state_t *state, const uint8_t *ra, const uint8_t *da,
                          const uint8_t *data, int data_len)
{
    int ret;
    umesh_buf_t *buf = NULL;
    if (state == NULL || da == NULL || data == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }

    ret = umesh_init_frame_data(state, ra, da, data, data_len,  &buf);
    if (ret < 0) {
        log_e("init data frame failed,ret = %d", ret);
        return ret;
    }
    umesh_mutex_lock(state->to_radio_list_lock);
    list_add_tail(&buf->linked_list, &state->send_to_radio_list);
    umesh_mutex_unlock(state->to_radio_list_lock);
    umesh_semaphore_post(state->generic_semap);
    return 0;
}