#ifndef __ANT_HTTP_H__
#define __ANT_HTTP_H__
#include "ant_typedef.h"

#define HTTPS_READ_BLOCK_ENABLE  0
#define HTTPS_CA_IGNORE_ENABLE 
#define HTTP_RANGE_DOWNLOAD_ENABLE 
#define HTTP_RANGE_TEST_SIZE_DISC  1500000
#define HTTP_HDR_RANGE_VELUE_BYTES  "bytes"
#define HTTP_CB_NOTIFY_CONTENT_LENGTH     0
#define HTTP_CB_NOTIFY_DOWN_UNCOMPLETE   -1
#define HTTP_CB_NOTIFY_DOWN_FULL_RESP    -2

#define AUDIO_WELCOME   "welcome.mp3"
#define AUDIO_SCAN_BIND "scan_bind.mp3"
#define AUDIO_BIND_OK   "bind_ok.mp3"
#define AUDIO_UNBIND    "unbind.mp3"


#define RANGE_REQ_FMT   "GET %s  HTTP/1.1\r\n"  \
                        "Host: %s\r\n"   \
                        "User-Agent: alipay/1.0 alipaybox\r\n" \
                        "Connection: keep-alive\r\n"        \
                        "Range: bytes=%d-%d\r\n"          \
                        "Cache-Control: max-age=0\r\n"    \
                         "\r\n"

typedef enum {
    HTTP_RANGE_SUPPORT_NONE,
    HTTP_RANGE_SUPPORT_BYTES,
} http_range_ability_enum;

typedef struct {
	int start;
	int end;    
} http_range;

typedef struct {
    int content_len;
	int range_support;
	http_range_ability_enum range_ability;    
} http_header;

typedef struct {
	char      *filename;
	ant_s32   file;
	ant_char  *buf;
	ant_s32   buf_siz;
	ant_s32   buf_pos;
} download_context;

typedef int (* http_get_cb)(void *arg, void *buf, int size);
typedef int (* https_get_cb)(void *arg, void *buf, int size);
typedef ant_s32 (*http_read_fn)(ant_s32 sock, void *buf, int size);
typedef ant_u32 (*https_read_fn)(void *arg, void *buf, int size);
typedef void (* http_parse_header_field_func)(const char *key, unsigned int keylen,
                                        const char *value, unsigned int valuelen,
                                        http_header *header);

int http_get(char *web_url, http_get_cb cb, void *arg, int escape_header);
int https_get(char *web_url, https_get_cb cb, void *arg, int escape_header, http_range *range);
void http_parse_url(const char *url, char *host, int *port, char *file_name, char *path);
int http_parse_header(ant_s32 sock, http_header *h);
void parse_all_header_fields(const char* key, unsigned int klen,
                                    const char* value, unsigned int vlen,
                                    http_header* header);
void http_parse_header_raw(const char* data, unsigned int len,
                         http_parse_header_field_func parse_func,
                         http_header* header);
void https_cb(void *ap, void *buf, int size);

#endif
