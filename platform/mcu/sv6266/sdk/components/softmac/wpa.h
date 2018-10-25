
#ifndef _WPA_H_
#define _WPA_H_

#include "soc_types.h"
//#include "ieee80211.h"
//#include "EAPOL.h"

#define  IEEE80211_RSN_VERSION          1
#define  UIP_LLH_LEN            14
/* EAPOL version */
#define WPA2_EAPOL_VERSION	2
#define WPA_EAPOL_VERSION		1
#define LEAP_EAPOL_VERSION		1
#define SES_EAPOL_VERSION		1

/* EAPOL-Key types */
#define EAPOL_RC4_KEY		1
#define EAPOL_WPA2_KEY		2	/* 802.11i/WPA2 */
#define EAPOL_WPA_KEY		254	/* WPA */

#define CTYPE_WEP40                    0x00000001
#define CTYPE_WEP104                   0x00000002
#define CTYPE_TKIP                     0x00000004
#define CTYPE_CCMP                     0x00000008

#define AKM_EAP		0x00000001
#define AKM_PSK		0x00000002

/* EAPOL - key information */
/** Key descriptor version, indicating WPA or WPA2 */
#define EAPOL_KEY_INFO_VERSION	0x0007

/** Key type bit, indicating pairwise or group */
#define EAPOL_KEY_INFO_TYPE		0x0008

/** Key install bit; set on message 3 except when legacy hacks are used */
#define EAPOL_KEY_INFO_INSTALL		0x0040

/** Key ACK bit; set when a response is required, on all messages except #4 */
#define EAPOL_KEY_INFO_KEY_ACK	0x0080

/** Key MIC bit; set when the MIC field is valid, on messages 3 and 4 */
#define EAPOL_KEY_INFO_KEY_MIC	0x0100

/** Secure bit; set when both sides have both keys, on messages 3 and 4 */
#define EAPOL_KEY_INFO_SECURE		0x0200

/** Error bit; set on a MIC failure for TKIP */
#define EAPOL_KEY_INFO_ERROR		0x0400

/** Request bit; set when authentication is initiated by the Peer (unusual) */
#define EAPOL_KEY_INFO_REQUEST	0x0800

/** Key Encrypted bit; set when the Key Data field is encrypted */
#define EAPOL_KEY_INFO_KEY_ENC	0x1000

/** SMC Message bit; set when this frame is part of an IBSS SMK handshake */
#define EAPOL_KEY_INFO_SMC_MESS	0x2000

/** Key descriptor version field value for WPA (TKIP) */
#define EAPOL_KEY_VERSION_WPA		1

/** Key descriptor version field value for WPA2 (CCMP) */
#define EAPOL_KEY_VERSION_WPA2	2

/** Key type field value for a PTK (pairwise) key handshake */
#define EAPOL_KEY_TYPE_PTK			0x0008

/** Key type field value for a GTK (group) key handshake */
#define EAPOL_KEY_TYPE_GTK			0x0000

/** Length of a nonce */
#define WPA_NONCE_LEN		32

/** Length of a TKIP main key */
#define WPA_TKIP_KEY_LEN	16

/** Length of a TKIP MIC key */
#define WPA_TKIP_MIC_KEY_LEN	8

/** Length of a CCMP key */
#define WPA_CCMP_KEY_LEN	16

/** Length of an EAPOL Key Confirmation Key */
#define WPA_KCK_LEN		16

/** Length of an EAPOL Key Encryption Key */
#define WPA_KEK_LEN		16

/** Usual length of a Pairwise Master Key */
#define WPA_PMK_LEN		32

/** Length of a PMKID */
#define WPA_PMKID_LEN		16

#define MKOUI( a, b, c, t ) ( ( ( t ) << 24 ) | ( ( c ) << 16 ) | ( ( b ) << 8 ) | ( a ) )
#define OUI_ORG_MASK		0x00FFFFFF
#define OUI_TYPE_MASK		0xFF000000

#define IEEE80211_RSN_OUI    			MKOUI (0x00, 0x0F, 0xAC, 0)
#define IEEE80211_WPA_OUI    			MKOUI (0x00, 0x50, 0xF2, 0)
#define IEEE80211_RSN_CTYPE_WEP40  	MKOUI (0, 0, 0, 0x01)
#define IEEE80211_RSN_CTYPE_WEP104 	MKOUI (0, 0, 0, 0x05)
#define IEEE80211_RSN_CTYPE_TKIP   		MKOUI (0, 0, 0, 0x02)
#define IEEE80211_RSN_CTYPE_CCMP   	MKOUI (0, 0, 0, 0x04)

#define IEEE80211_AKM_EAP   		MKOUI (0, 0, 0, 0x01)
#define IEEE80211_AKM_PSK   	    MKOUI (0, 0, 0, 0x02)

/** KDE type for an encapsulated Group Transient Key (requires encryption) */
#define WPA_KDE_GTK	MKOUI ( 0x00, 0x0F, 0xAC, 0x01 )

/** KDE type for a MAC address */
#define WPA_KDE_MAC	MKOUI ( 0x00, 0x0F, 0xAC, 0x03 )

/** KDE type for a PMKID */
#define WPA_KDE_PMKID	MKOUI ( 0x00, 0x0F, 0xAC, 0x04 )

/** KDE type for a nonce */
#define WPA_KDE_NONCE	MKOUI ( 0x00, 0x0F, 0xAC, 0x06 )

/** KDE type for a lifetime value */
#define WPA_KDE_LIFETIME MKOUI ( 0x00, 0x0F, 0xAC, 0x07 )

/** An 802.11 data encryption algorithm */
enum net80211_crypto_alg {
	NET80211_CRYPT_NONE = 0,
	NET80211_CRYPT_WEP = 1,
	NET80211_CRYPT_TKIP = 2,
	NET80211_CRYPT_CCMP = 3,
	NET80211_CRYPT_CCMP_TKIP = 4,
	NET80211_CRYPT_UNKNOWN = 5,
};

/** WPA handshaking state */
enum wpa_state {
	/** Waiting for PMK to be set */
	WPA_WAITING = 0,
	/** Ready for 4-Way Handshake */
	WPA_READY,
	/** Performing 4-Way Handshake */
	WPA_WORKING,
	/** 4-Way Handshake succeeded */
	WPA_SUCCESS,
	/** 4-Way Handshake failed */
	WPA_FAILURE,
};

enum key_type {
	KEY_NONE,
	KEY_PAIR,
	KEY_GROUP,
};

/*
 * EAP Method Types as allocated by IANA:
 * http://www.iana.org/assignments/eap-numbers
 */
typedef enum {
	EAP_TYPE_NONE = 0,
	EAP_TYPE_IDENTITY = 1 /* RFC 3748 */,
	EAP_TYPE_NOTIFICATION = 2 /* RFC 3748 */,
	EAP_TYPE_NAK = 3 /* Response only, RFC 3748 */,
	EAP_TYPE_MD5 = 4, /* RFC 3748 */
	EAP_TYPE_OTP = 5 /* RFC 3748 */,
	EAP_TYPE_GTC = 6, /* RFC 3748 */
	EAP_TYPE_TLS = 13 /* RFC 2716 */,
	EAP_TYPE_LEAP = 17 /* Cisco proprietary */,
	EAP_TYPE_SIM = 18 /* RFC 4186 */,
	EAP_TYPE_TTLS = 21 /* RFC 5281 */,
	EAP_TYPE_AKA = 23 /* RFC 4187 */,
	EAP_TYPE_PEAP = 25 /* draft-josefsson-pppext-eap-tls-eap-06.txt */,
	EAP_TYPE_MSCHAPV2 = 26 /* draft-kamath-pppext-eap-mschapv2-00.txt */,
	EAP_TYPE_TLV = 33 /* draft-josefsson-pppext-eap-tls-eap-07.txt */,
	EAP_TYPE_TNC = 38 /* TNC IF-T v1.0-r3; note: tentative assignment;
			   * type 38 has previously been allocated for
			   * EAP-HTTP Digest, (funk.com) */,
	EAP_TYPE_FAST = 43 /* RFC 4851 */,
	EAP_TYPE_PAX = 46 /* RFC 4746 */,
	EAP_TYPE_PSK = 47 /* RFC 4764 */,
	EAP_TYPE_SAKE = 48 /* RFC 4763 */,
	EAP_TYPE_IKEV2 = 49 /* RFC 5106 */,
	EAP_TYPE_AKA_PRIME = 50 /* RFC 5448 */,
	EAP_TYPE_GPSK = 51 /* RFC 5433 */,
	EAP_TYPE_PWD = 52 /* RFC 5931 */,
	EAP_TYPE_EKE = 53 /* RFC 6124 */,
	EAP_TYPE_EXPANDED = 254 /* RFC 3748 */
} EapType;

/** Structure of the Temporal Key for TKIP encryption */
struct tkip_tk
{
	/** Main key: input to TKIP Phase 1 and Phase 2 key mixing functions */
	u8 key[WPA_TKIP_KEY_LEN];

	/** Michael MIC keys */
	struct {
		/** MIC key for packets from the AP */
		u8 rx[WPA_TKIP_MIC_KEY_LEN];

		/** MIC key for packets to the AP */
		u8 tx[WPA_TKIP_MIC_KEY_LEN];
	} __attribute__ (( packed )) mic;
} __attribute__ (( packed ));

/** Structure of a generic Temporal Key */
union wpa_tk
{
	/** CCMP key */
	u8 ccmp[WPA_CCMP_KEY_LEN];

	/** TKIP keys */
	struct tkip_tk tkip;
};

/** Structure of the Pairwise Transient Key */
struct wpa_ptk
{
	/** EAPOL-Key Key Confirmation Key (KCK) */
	u8 kck[WPA_KCK_LEN];

	/** EAPOL-Key Key Encryption Key (KEK) */
	u8 kek[WPA_KEK_LEN];

	/** Temporal key */
	union wpa_tk tk;
} __attribute__ (( packed ));

/** Structure of the Group Transient Key */
struct wpa_gtk
{
	/** Temporal key */
	union wpa_tk tk;
} __attribute__ (( packed ));

struct ieee80211_ie_rsn {
	u8 		id;
	u8 		len;
	u16 	version;
	u32 	group_cipher;		/** Cipher ID for the cipher used in multicast/broadcast frames */
	u16 	pairwise_count;		/** Number of unicast ciphers supported */
	u32 	pairwise_cipher;	/** Number of unicast ciphers supported */
	u16 	akm_count;			/** Number of authentication types supported */
	u32 	akm_list;			/** List of authentication type IDs for supported types */
	u16 	capability;		/** Security capabilities field (RSN only) */
	//u16 		pmkid_count;		/** Number of PMKIDs included (present only in association frames) */
	//u8 		pmkid_list[0];		/** List of PMKIDs included, each a 16-byte SHA1 hash */
}__attribute__ ((packed));

/** 802.11 Vendor Specific information element
 *
 * One often sees the RSN IE masquerading as vendor-specific on
 * devices that were produced prior to 802.11i (the WPA amendment)
 * being finalized.
 */
struct ieee80211_ie_vendor {
	u8 id;			/**< Vendor-specific ID: 221 */
	u8 len;			/**< Vendor-specific length: variable */
	u32 oui;		/**< OUI and vendor-specific type byte */
	u8 data[0];		/**< Vendor-specific data */
} __attribute__ ((packed));

struct ieee80211_wpa {
	u8 id;			/**< Vendor-specific ID: 221 */
	u8 len;			/**< Vendor-specific length: variable */
	u32 oui;		/**< OUI and vendor-specific type byte */
	u16 ver;		/**< Vendor-specific data */
	u32 group_cipher;		/**< OUI and vendor-specific type byte */
	u16 pairwise_count;		/**< Vendor-specific data */
	u32 pairwise_cipher;		/**< OUI and vendor-specific type byte */
	u16 akm_count;		/**< Vendor-specific data */
	u32 akm_list;		/**< OUI and vendor-specific type byte */
} __attribute__ ((packed));

/** Common context for WPA security handshaking
 *
 * Any implementor of a particular handshaking type (e.g. PSK or EAP)
 * must include this structure at the very beginning of their private
 * data context structure, to allow the EAPOL-Key handling code to
 * work. When the preliminary authentication is done, it is necessary
 * to call wpa_start(), passing the PMK (derived from PSK or EAP MSK)
 * as an argument. The handshaker can use its @a step function to
 * monitor @a state in this wpa_ctx structure for success or
 * failure. On success, the keys will be available in @a ptk and @a
 * gtk according to the state of the @a valid bitmask.
 *
 * After an initial success, the parent handshaker does not need to
 * concern itself with rekeying; the WPA common code takes care of
 * that.
 */
struct wpa_common_ctx
{
	/** The Pairwise Master Key to use in handshaking
	 *
	 * This is set either by running the PBKDF2 algorithm on a
	 * passphrase with the SSID as salt to generate a pre-shared
	 * key, or by copying the first 32 bytes of the EAP Master
	 * Session Key in 802.1X-served authentication.
	 */
	u8 pmk[WPA_PMK_LEN];

	/** Length of the Pairwise Master Key
	 *
	 * This is always 32 except with one EAP method which only
	 * gives 16 bytes.
	 */
	int pmk_len;

	/** State of EAPOL-Key handshaking */
	enum wpa_state state;

	/** Replay counter for this association
	 *
	 * This stores the replay counter value for the most recent
	 * packet we've accepted. It is initially initialised to ~0 to
	 * show we'll accept anything.
	 */
	u64 replay;

    u32 rxgroup_pn_lo;
    u32 rxgroup_pn_hi;
    u16 keyinfo;
    
	/** The cipher to use for unicast RX and all TX */
	enum net80211_crypto_alg crypt;

	/** The cipher to use for broadcast and multicast RX */
	enum net80211_crypto_alg gcrypt;

	/** The Pairwise Transient Key derived from the handshake */
	struct wpa_ptk ptk;

	/** The Group Transient Key derived from the handshake */
	struct wpa_gtk gtk;

	/** Authenticator-provided nonce */
	u8 Anonce[WPA_NONCE_LEN];

	/** Supplicant-generated nonce (that's us) */
	u8 Snonce[WPA_NONCE_LEN];

	/** MAC address of the access point most recently associated */
	u8 bssid[6];

	/** Supplicant-generated nonce (that's us) */
	u8 lmac[6];
	
	struct ieee80211_ie_rsn rsn_ie;

	struct ieee80211_wpa wpa_ie;
	/** Whether @a ap_rsn_ie is an RSN IE (as opposed to old WPA) */
	int ap_rsn_is_rsn;
 	/** Whether @a ap_ie_is_wpa is an WPA IE (as opposed is not support WPA) */
	int ap_ie_is_wpa;
};

struct eapol_frame
{
	u8				version;      			/* Protocol version */
	u8				type;         			/* Packet Type (7.5.4) */
	u16				length;          		/* Packet Body Length (7.5.5)*/
	u8 				body[0];
} __attribute__ ((packed));

struct eapol_key_pkt
{
	u8	type;		/* One of the EAPOL_KEY_TYPE_* defines. */
	u16	key_info;		/* Bitfield of key characteristics, network byte order */
	u16 	key_length;	/* 16 for CCMP, 32 for TKIP, and 5 or 13 for WEP */
	u64	replay_counter;
	u8	key_nonce[32];
	u8	key_iv[16];	/* Initialization vector */
	u8	key_rsc[8];	/* Receive sequence counter for GTK */
	u8 	key_id[8];
	u8	key_mic[16];	/* Message integrity code over the entire EAPOL frame */
	u16	key_data_len;
	u8	data[0];
} __attribute__ (( packed ));

void init_wpa_ctx(struct wpa_common_ctx *ctx, u8 *pmk);
void init_AP_wpa_ctx(struct wpa_common_ctx *ctx, u8 *pmk, u8 *addr);
int pack_wpa2 (struct wpa_common_ctx *ctx, u8 *ptr);
int sec80211_detect(struct wpa_common_ctx *ctx, u8 *buf, u16 buf_len, u8 crypto);
void sec80211_detect_for_autoScan(u8 *buf, u16 buf_len, u8 *auth, u8 *crypt);
int rx_wpa_1_of_4 (u8 ifid, struct wpa_common_ctx *ctx, struct eapol_key_pkt *eapol_key);
void tx_wpa_2_of_4 (u8 ifid, struct wpa_common_ctx *ctx);
int rx_wpa_3_of_4 (u8 ifid, struct wpa_common_ctx *ctx, struct eapol_key_pkt *eapol_key);
void tx_wpa_4_of_4 (u8 ifid, struct wpa_common_ctx *ctx);
int rx_wpa_1_of_2 (u8 ifid, struct wpa_common_ctx *ctx, struct eapol_key_pkt *eapol_key);
void tx_wpa_1_of_4 (struct wpa_common_ctx *ctx);
int rx_wpa_2_of_4 (struct wpa_common_ctx *ctx, struct eapol_frame *eap_frame);
void tx_wpa_3_of_4 (struct wpa_common_ctx *ctx);
#endif
