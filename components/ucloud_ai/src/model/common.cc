#include <iostream>
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/core/CommonClient.h"
#include "alibabacloud/core/Utils.h"
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

char *getTokenId(const char *domain, const char *regionId)
{
    InitializeSdk();
    string key = getAccessKey();
    string secret = getAccessSecret();
    string regionIdStr = regionId;
    string domainStr = domain;
    ClientConfiguration configuration;
    configuration.setRegionId(regionIdStr);
    configuration.setEndpoint(domainStr);
    CommonClient client(key, secret, configuration);
    CommonRequest request(CommonRequest::RpcPattern);
    char *response = NULL;

    request.setScheme("http");
    request.setDomain(domainStr);
    request.setHttpMethod(HttpRequest::Post);
    request.setQueryParameter("Action", "CreateToken");
    request.setVersion("2019-02-28");

    auto outcome = client.commonResponse(request);
    if (!outcome.isSuccess()) {
        cout << "error code:      " << outcome.error().errorCode() << endl;
        cout << "error message:   " << outcome.error().errorMessage() << endl;
        cout << "error host:      " << outcome.error().host() << endl;
        cout << "error requestId: " << outcome.error().requestId() << endl;
        cout << "error detail:    " << outcome.error().detail() << endl;
        ShutdownSdk();
        return NULL;
    }
    cout << endl << "add item returns: " << outcome.result().payload() << endl << endl;
    response = strdup(outcome.result().payload().c_str());
    ShutdownSdk();

    return response;
}

char *computeContentMD5(const char *data, size_t size)
{
    return (char *)ComputeContentMD5(data, size).c_str();
}

char *generateUuid()
{
    return (char *)GenerateUuid().c_str();
}

char *urlEncode(const char *src)
{
    return (char *)UrlEncode(src).c_str();
}

char *urlDecode(const char *src)
{
    return (char *)UrlDecode(src).c_str();
}

}
