#include <iostream>
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/core/CommonClient.h"
#include "CurlHttpClient.h"
#include "alibabacloud/core/Url.h"
#include "alibabacloud/core/HttpRequest.h"

extern "C" {
using namespace std;
using namespace AlibabaCloud;

static string accessKey;
static string accessSecret;

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
