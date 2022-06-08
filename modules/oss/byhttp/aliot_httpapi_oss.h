#ifndef __ALIOT_HTTPAPI_OSS_H__
#define __ALIOT_HTTPAPI_OSS_H__

#include "aliot_base64.h"
#include "aliot_hmac.h"
#include "aliot_sha1.h"
#include "httpclient.h"
#include "ulog/ulog.h"

#define MAX_HTTP_OUTPUT_BUFFER (1024)

typedef struct {
    char gmtweek[8];
    char gmtmon[8];
} GmtTime_t;

typedef struct {
    int32_t year;
    int32_t month;
    int32_t day;
    int32_t hour;
    int32_t minute;
    int32_t second;
    int32_t timezone;  // one digit expresses a quarter of an hour, for example: 22 indicates "+5:30"
} ZYF_Time_t;

int32_t oss_http_get_object(const char *key, const char *secret, const char *endpoint, const char *bucketname,
                            const char *objectname, const char *localpath, char *range, char *resbuf, uint32_t reslen);

int32_t oss_http_put_object(const char *key, const char *secret, const char *endpoint, const char *bucketname,
                            const char *objectname, const char *localpath, char *resbuf, uint32_t reslen);

int32_t oss_http_put_content(const char *key, const char *secret, const char *endpoint, const char *bucketname,
                             const char *objectname, const char *ContentData, char *resbuf, uint32_t reslen);

#endif
