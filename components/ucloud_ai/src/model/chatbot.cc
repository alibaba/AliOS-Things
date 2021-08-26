/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#include <iostream>
#include <string.h>
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/core/CommonClient.h"
#include "model/common.h"

extern "C" {
using namespace std;
using namespace AlibabaCloud;

/*recognizing identity card has issue to be resolved*/
char *chatbot(char *pInstanceId, char *pSessionId, char *text)
{
    InitializeSdk();
    string key = getAccessKey();
    string secret = getAccessSecret();
    string instanceId = pInstanceId;
    string utterance = text;
    ClientConfiguration configuration;
    configuration.setRegionId(CLOUD_AI_REGION_ID);
    configuration.setEndpoint(CLOUD_AI_CHATBOT_ENDPOINT);
    CommonClient client(key, secret, configuration);
    CommonRequest request(CommonRequest::RpcPattern);
    char *response = NULL;

    request.setScheme("http");
    request.setDomain(CLOUD_AI_CHATBOT_ENDPOINT);
    request.setHttpMethod(HttpRequest::Get);
    request.setQueryParameter("Action", "Chat");
    request.setVersion("2017-10-11");
    request.setQueryParameter("InstanceId", instanceId);
    request.setQueryParameter("Utterance", utterance);
    request.setQueryParameter("AccessKeyId", key);
    if (pSessionId) {
        string sessionId = pSessionId;
        request.setQueryParameter("SessionId", sessionId);
    }

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

}