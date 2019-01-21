
#ifndef _EAPOL_H_
#define _EAPOL_H_

#include "soc_types.h"
#include "ieee80211.h"
#include "wpa.h"
//#include "soft_ap_def.h"

#define IP_FRAME			0x0800
#define EAPOL_FRAME		0x888e      /* 802.2 frames */

#define EAPOL_HDR_LEN		4

// Offsets in to frames where data is located.
#define OFFSET_PAST_MAC	12
#define OFFSET_TO_EAPOL	14
#define OFFSET_TO_EAP		18
#define OFFSET_TO_DATA		23

/* EAPOL Packet type */
#define EAPOL_EAP		0
#define EAPOL_START		1
#define EAPOL_LOGOFF	2
#define EAPOL_KEY		3
#define EAPOL_ASF		4

/* EAP Packet Code */
#define EAP_CODE_REQUEST		1
#define EAP_CODE_REQPONSE		2
#define EAP_CODE_SUCCESS		3
#define EAP_CODE_FAILURE		4

/* EAPOL for 802.3/Ethernet */
struct eapolkey_header
{
	u8	desc_type;
	u16	key_len;
	u8	replay_counter[8];
	char	key_IV[16];
	u8	key_index;
	char	key_signature[16];    
} __attribute__ ((packed));

struct eap_header {
	u8	eap_code;
	u8	eap_identifier;
	u16	eap_length;
	u8	eap_type;
} __attribute__ ((packed));

struct snap_header {
	u8	dsap;
	u8	ssap;
	u8	ctrl;
	u8	rfc1042[3];
	u16	ip_type;
} __attribute__ ((packed));

struct ieee80211_eapol_request {
	struct ieee80211_hdr_3addr ieee80211_hdr;
	struct snap_header snap_hdr;
//	struct eapol_frame eapol_hdr;
} __attribute__ ((packed));

//struct wpa_common_ctx;
//void tx_eapol_start_req ();
int rx_process_wpa (u8 id, struct eapol_frame *eap_frame);
int rx_process_eapol (u8 id, u8 *data, int datalen);

#endif
