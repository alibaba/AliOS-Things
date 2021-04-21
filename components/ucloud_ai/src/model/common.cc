#include <iostream>
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/core/CommonClient.h"
#include "ucloud_ai_common.h"
#include <ulog/ulog.h>

extern "C" {
using namespace std;
using namespace AlibabaCloud;

static string accessKey;
static string accessSecret;

#define USE_HTTPCLIENT
#define TAG "common"

#ifdef USE_HTTPCLIENT
int getResponseBodyByUrl(const char *url, const char **buffer)
{
    int ret;
    int recv_len;

    ret = ucloud_ai_connect((char *)url);
    if (ret < 0) {
        LOGE(TAG, "ucloud_ai_connect failed, ret: %d\n", ret);
        return -1;
    }

    recv_len = ucloud_ai_get_stream((char *)url, (char **)buffer);
    ucloud_ai_disconnect();
    if (!recv_len) {
        LOGE(TAG, "recv_len is %d\n", recv_len);
        return -1;
    }

    return recv_len;
}
#else
int getResponseBodyByUrl(const char *url, const char **buffer)
{
    CurlHttpClient client;
    HttpRequest request;

    Url tmpUrl;
    tmpUrl.setHost(url);
    tmpUrl.setScheme("http");

    request.setMethod(HttpRequest::Method::Get);
    request.setUrl(tmpUrl);
    request.setHeader("Accept", "*/*");
    HttpClient::HttpResponseOutcome out = client.makeRequest(request);
    *buffer = out.result().body();
    return out.result().bodySize();
}
#endif
void setAccesskeyAndSecret(char *key, char *secret)
{
    accessKey = key;
    accessSecret = secret;
}

const char *getAccessKey(void)
{
    return accessKey.c_str();
}

const char *getAccessSecret(void)
{
    return accessSecret.c_str();
}
}
