
#ifndef UMESH_FRAME_H_
#define UMESH_FRAME_H_

#include <stdint.h>
#include "define.h"
#include "state.h"

typedef enum {
	UMESH_FRAME_DISCOVERY_REQ = 0,
	UMESH_FRAME_DISCOVERY_RESP,
	UMESH_FRAME_JOIN_REQ,
	UMESH_FRAME_JOIN_PERMIT,
	UMESH_FRAME_JOIN_FINISH,
	UMESH_FRAME_HEART_BEAT,
	UMESH_FRAME_DATA,
}umesh_frame_type_t;

/* umesh TLV type values */
typedef enum umesh_tlvs {
	UMESH_VENDOR_TLV = 0x81, 
	UMESH_JOIN_TLV = 0x91, 
	UMESH_HEART_BEAT_TLV = 0xa1, 	
	UMESH_DATA_TLV = 0xa2, 	
	UMESH_ZERO_REQ_TLV = 0xb1, 	
	UMESH_ZERO_RESP_TLV = 0xb2, 	
	UMESH_ZERO_FINISH_TLV = 0xb3, 
}umesh_tlvs_t;


struct umesh_send_message {
	umesh_frame_type_t type;
	uint8_t ra_enable;
	uint8_t ra[IEEE80211_MAC_ADDR_LEN]; /* recieve address */
	uint8_t da[IEEE80211_MAC_ADDR_LEN]; /* destination address */
	uint8_t *data;
	uint16_t data_len;
    uint16_t data_seq; 
	
};

struct umesh_ieee80211_hdr {
	uint16_t frame_control;
	uint16_t duration_id;
	uint8_t addr1[IEEE80211_MAC_ADDR_LEN]; /* dst */
	uint8_t addr2[IEEE80211_MAC_ADDR_LEN]; /* src */
	uint8_t addr3[IEEE80211_MAC_ADDR_LEN]; /* bssid */
	uint16_t seq_ctrl;
} __attribute__((__packed__));

struct umesh_header {
	uint8_t category; /* 127 - vendor specific */
	uint8_t oui[3];
	uint16_t frame_seq;
	uint8_t flag;
    uint8_t reserved;
} __attribute__((__packed__));

struct tl {
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__));

struct umesh_join_paras_tlv {
	uint8_t type;
	uint16_t length;
	uint32_t session_id;
	uint8_t  random[UMESH_RANDOM_LEN];
} __attribute__((__packed__));

struct umesh_heart_beat_tlv {
	uint8_t type;
	uint16_t length;
	uint16_t internal;
} __attribute__((__packed__));

struct umesh_data_fixed_tlv {
	uint8_t type;
	uint16_t length;
    uint8_t flag; 
	uint16_t seq_num;
} __attribute__((__packed__));

const char * umesh_tlv_as_str(umesh_tlvs_t type);

#endif /* UMESH_FRAME_H_ */
