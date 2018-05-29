#ifndef __ALCS_API_INTERNAL_H__
#define __ALCS_API_INTERNAL_H__ 
#include "CoAPExport.h"
#include "alcs_api.h"
#include "lite-list.h"
                       
#define KEY_MAXCOUNT 10
#define RANDOMKEY_LEN 16
#define KEYSEQ_LEN 3
#define COAP_OPTION_SESSIONID 71

#ifdef ALCSCLIENT
typedef struct
{
    char*             accessKey;
    char*             accessToken;
    char*             deviceName;
    char*             productKey;
    struct list_head  lst;
} ctl_key_item;
#endif

#ifdef ALCSSERVER
typedef struct
{
    char              keyprefix[KEYPREFIX_LEN + 1];
    char*             secret;
    bool              groupKey;
    struct list_head  lst;
} svr_key_item;
#endif

typedef struct
{
    void                    *list_mutex;
#ifdef ALCSCLIENT
    struct list_head         lst_ctl;
    unsigned char            ctl_count;
#endif
#ifdef ALCSSERVER
    struct list_head         lst_svr;
    unsigned char            svr_count;
    char                    *revocation;
#endif

} auth_list;

typedef struct
{
    char randomKey[RANDOMKEY_LEN + 1];
    int sessionId;
    char sessionKey[32];
    int authed_time;
    int heart_time;
    int interval;
    NetworkAddr addr;
    struct list_head  lst;
} session_item;

#define ROLE_SERVER 2
#define ROLE_CLIENT 1

typedef struct
{
    CoAPContext* context;
//    char deviceName[30];
//    char productKey[30];
    int seq;
    auth_list lst_auth;
#ifdef ALCSSERVER
    struct list_head lst_svr_sessions;
#endif
#ifdef ALCSCLIENT
    struct list_head lst_ctl_sessions;
#endif
    char role;
    struct list_head lst;
} device_auth_list;

#ifdef SUPPORT_MULTI_DEVICES
extern struct list_head device_list;

device_auth_list* get_device(CoAPContext *context);

auth_list* get_list(CoAPContext *context);

#ifdef ALCSCLIENT
struct list_head* get_ctl_session_list (CoAPContext *context);
#endif

#ifdef ALCSSERVER
struct list_head* get_svr_session_list (CoAPContext *context);
#endif

#else
extern device_auth_list _device;
#define get_device(v) (&_device)

#ifdef ALCSSERVER
#define get_svr_session_list(v) (_device.role&ROLE_SERVER? &_device.lst_svr_sessions : NULL)
#endif
#ifdef ALCSCLIENT
#define get_ctl_session_list(v) (_device.role&ROLE_CLIENT? &_device.lst_ctl_sessions : NULL)
#endif

#define get_list(v) (&_device.lst_auth)
#endif

void remove_session (CoAPContext *ctx, session_item* session);
#ifdef ALCSCLIENT
session_item* get_ctl_session (CoAPContext *ctx, NetworkAddr* from);
#endif

#ifdef ALCSSERVER
session_item* get_svr_session (CoAPContext *ctx, NetworkAddr* from);

#define MAX_PATH_CHECKSUM_LEN (5)
typedef struct
{
    char              path[MAX_PATH_CHECKSUM_LEN];
    CoAPRecvMsgHandler cb;
    struct list_head   lst;
} secure_resource_cb_item;

extern struct list_head secure_resource_cb_head;
#endif

int alcs_encrypt (const char* src, int len, const char* key, void* out);
int alcs_decrypt (const char* src, int len, const char* key, void* out);
int observe_data_encrypt(CoAPContext *ctx, NetworkAddr* addr, CoAPMessage* message, CoAPLenString *src, CoAPLenString *dest);
bool is_networkadd_same (NetworkAddr* addr1, NetworkAddr* addr2);
void gen_random_key(unsigned char random[], int len);
bool req_payload_parser (const char* payload, int len, char** seq, int* seqlen, char** data, int* datalen);


int alcs_resource_register_secure (CoAPContext *context, const char *path, unsigned short permission,
            unsigned int ctype, unsigned int maxage, CoAPRecvMsgHandler callback);
void alcs_resource_cb_deinit(void);

#endif
