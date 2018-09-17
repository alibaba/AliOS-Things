/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _ENROLLEE_H_
#define _ENROLLEE_H_
#include "os.h"
#include "digest_algorithm.h"

/* enrollee/registrar doc see following
 * http://docs.alibaba-inc.com/pages/viewpage.action?pageId=450855381
 */
/* ie oui def. */
#define WLAN_OUI_ALIBABA            (0xD896E0)
#define WLAN_OUI_TYPE_ENROLLEE          (0xAA)
#define WLAN_OUI_TYPE_REGISTRAR         (0xAB)

#define DEVICE_TYPE_VERSION         (0)
#define ENROLLEE_FRAME_TYPE         (0)
#define REGISTRAR_FRAME_TYPE        (1)

/* enrollee frame */
#if 0   //len = 46 + model(n) + devid(n)
struct ieee80211_alibaba_ie {
    uint8_t element_id;     /* 221 */
    uint8_t len;            /* len of this struct, exclude element id & len field */
    uint8_t oui[3];         /* D896E0 */
    uint8_t oui_type;       /* 0xAA, device request */

    uint8_t version: 4, bit7 - bit4;
    uint8_t dev_type: 4, bit3 - bit0; /* alink=0, alink_cloud=1, yoc=8 */
    uint8_t devid_len;
    uint8_t devid[n];       /* device id, unique id for device(mac or sn) */
    uint8_t frame_type;     /* frame_type = 0 */

    uint8_t model_len;
    uint8_t model[n];       /* requester device name */
    uint8_t random[4];      /* random salt */
    uint8_t sign[16];       /* requester device name, sign = Hmacmd5(secret, random+devid+model) */
};
#endif

#define ENROLLEE_IE_FIX_LEN     (6 + (1 + 1 + 1) + (1 + 4 + 16))
#define REGISTRAR_IE_FIX_LEN    (6 + (1 + 1 + 1) + (1 + 1 + 6 + 1))

#define MAX_DEVID_LEN           (64)
#define MAX_MODEL_LEN           (80)
#define MAX_KEY_LEN             (32)
#define MAX_TOKEN_LEN           (16)
#define ENROLLEE_SIGN_SIZE      (MD5_SIZE_BYTE)
#define ENROLLEE_INFO_HDR_SIZE  (1 + 1 + MAX_DEVID_LEN + 1 + 1 + MAX_MODEL_LEN + 4 + 16)
struct enrollee_info {
    uint8_t dev_type_ver;
    uint8_t devid_len;
    uint8_t devid[MAX_DEVID_LEN];
    uint8_t frame_type;

    uint8_t model_len;
    uint8_t model[MAX_MODEL_LEN];
    uint8_t random[4];
    uint8_t sign[ENROLLEE_SIGN_SIZE];

    int rssi;

    uint8_t key[MAX_KEY_LEN + 1];
    uint8_t token[MAX_TOKEN_LEN + 1];

    uint8_t state;  /* free or not */
    uint8_t checkin_priority; /* smaller means high pri */
    uint32_t checkin_timestamp;
    uint32_t timestamp;
    uint32_t report_timestamp;
};

/* registrar configuration */
#define MAX_ENROLLEE_NUM            (5)//Note: max enrollee num supported

/* enrollee config */
#define ENROLLEE_EVICT_PERIOD_MS    (30 * 1000)
#define ENROLLEE_CHECKIN_PERIOD_MS  (15 * 1000)


/*
 * ENR_FREE     --producer-->   ENR_IN_QUEUE
 * ENR_IN_QUEUE     --cloud----->   ENR_CHECKIN_ENABLE
 * ENR_CHECKIN_ENABLE   --consumer-->   ENR_CHECKIN_ONGOING --> ENR_CHECKIN_END/ENR_FREE
 * *any state*      --consumer-->   ENR_FREE
 */
enum enrollee_state {
    ENR_FREE = 0,
    ENR_IN_QUEUE,
    ENR_CHECKIN_ENABLE,
    ENR_CHECKIN_ONGOING,
    ENR_CHECKIN_END,
    //ENR_OUTOFDATE = 0
};

#define AES_KEY_LEN     (16)
/* return 0 for success, -1 devid not match, otherwise return -2 */
extern int (*vendor_decrypt_ssid_passwd)(
    uint8_t *ie, uint8_t ie_len,
    uint8_t out_ssid[OS_MAX_SSID_LEN],
    uint8_t out_passwd[OS_MAX_PASSWD_LEN],
    uint8_t out_bssid[ETH_ALEN]);

extern const uint8_t probe_req_frame[64];
#define SA_POS              (10) //source mac pos
#define FCS_SIZE            (4)
extern const uint8_t iv[32];

/* enrollee API */
#ifdef AWSS_DISABLE_ENROLLEE
static inline void awss_init_enrollee_info(void) { }
static inline void awss_broadcast_enrollee_info(void) { }
static inline void awss_destroy_enrollee_info(void) { }
#else
void awss_init_enrollee_info(void);
void awss_broadcast_enrollee_info(void);
void awss_destroy_enrollee_info(void);
#endif
#ifdef CONFIG_YWSS
char *awss_get_enrollee_token(void);
void awss_clear_enrollee_token(void);
#else
static inline char *awss_get_enrollee_token(void)
{
    return NULL;
}

static inline void awss_clear_enrollee_token(void)
{
}
#endif
int awss_set_enrollee_token(char *token, int tokenLen);
void awss_calc_sign(uint32_t rand,
                    char devid[OS_PRODUCT_SN_LEN],
                    char model[OS_PRODUCT_MODEL_LEN],
                    char secret[OS_PRODUCT_SECRET_LEN],
                    char sign[ENROLLEE_SIGN_SIZE]);

/* registrar API */
#ifdef AWSS_DISABLE_REGISTRAR
static inline void awss_registrar_exit(void) { }
static inline void awss_registrar_init(void) { }
#else
void awss_registrar_init(void);
void awss_registrar_exit(void);
#endif

#endif
