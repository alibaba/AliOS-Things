/*
 * Copyright 2009-2017 Alibaba Cloud All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <alibabacloud/oss/client/RetryStrategy.h>
#include <alibabacloud/oss/utils/Executor.h>
#include <external/tinyxml2/tinyxml2.h>
#include "Client.h"
#include "../http/CurlHttpClient.h"
#include "utils/Utils.h"
#include "../auth/Signer.h"
#include <sstream>
#include <ctime>
#ifdef USE_AOS_TIME_POSIX_API
#include <posix/timer.h>
#endif

using namespace AlibabaCloud::OSS;
using namespace tinyxml2;

Client::Client(const std::string & servicename, const ClientConfiguration &configuration) :
    requestDateOffset_(0),
    serviceName_(servicename),
    configuration_(configuration),
    httpClient_(configuration.httpClient? configuration.httpClient:std::make_shared<CurlHttpClient>(configuration))
{
}

Client::~Client()
{
}

const ClientConfiguration& Client::configuration()const
{
    return configuration_;
}

std::string Client::serviceName()const
{
    return serviceName_;
}

Client::ClientOutcome Client::AttemptRequest(const std::string & endpoint, const ServiceRequest & request, Http::Method method) const
{
    for (int retry =0; ;retry++) {
        auto outcome = AttemptOnceRequest(endpoint, request, method);
        if (outcome.isSuccess()) {
            return outcome;
        } 
        else if (!httpClient_->isEnable()) {
            return outcome;
        }
        else {
            if (configuration_.enableDateSkewAdjustment &&
                outcome.error().Status() == 403 &&
                outcome.error().Message().find("RequestTimeTooSkewed")) {
                auto serverTimeStr = analyzeServerTime(outcome.error().Message());
                auto serverTime = UtcToUnixTime(serverTimeStr);
                if (serverTime != -1) {
#ifdef USE_AOS_TIME_POSIX_API
                    struct timespec currentTime;
                    time_t localTime;
                    clock_gettime(CLOCK_REALTIME, &currentTime);
                    localTime = currentTime.tv_nsec/1000000000 + currentTime.tv_sec;
#else
                    std::time_t localTime = std::time(nullptr);
#endif
                    setRequestDateOffset(serverTime - localTime);
                }
            }
            RetryStrategy *retryStrategy = configuration().retryStrategy.get();
            if (retryStrategy == nullptr || !retryStrategy->shouldRetry(outcome.error(), retry)) {
                return outcome;
            }
            long sleepTmeMs = retryStrategy->calcDelayTimeMs(outcome.error(), retry);
            httpClient_->waitForRetry(sleepTmeMs);
        }
    }
}

Client::ClientOutcome Client::AttemptOnceRequest(const std::string & endpoint, const ServiceRequest & request, Http::Method method) const
{
    if (!httpClient_->isEnable()) {
        return ClientOutcome(Error("ClientError:100002", "Disable all requests by upper."));
    }

    auto r = buildHttpRequest(endpoint, request, method);
    auto response = httpClient_->makeRequest(r); 
    
    if(hasResponseError(response)) {
        return ClientOutcome(buildError(response));
    } else {
        return ClientOutcome(response);
    }
}

std::string Client::analyzeServerTime(const std::string &message) const
{
    XMLDocument doc;
    if (doc.Parse(message.c_str(), message.size()) == XML_SUCCESS) {
        XMLElement* root = doc.RootElement();
        if (root && !std::strncmp("Error", root->Name(), 5)) {
            XMLElement *node;
            node = root->FirstChildElement("ServerTime");
            return (node ? node->GetText() : "");
        }
    }
    return "";
}

Error Client::buildError(const std::shared_ptr<HttpResponse> &response) const
{
    Error error;
    if (response == nullptr) {
        error.setCode("NullptrError");
        error.setMessage("HttpResponse is nullptr, should not be here.");
        return error;
    }
    
    long responseCode = response->statusCode();
    error.setStatus(responseCode);
    std::stringstream ss;
    if ((responseCode == 203) || 
        (responseCode > 299 && responseCode < 600)) {
        ss << "ServerError:" << responseCode;
        error.setCode(ss.str());
        if (response->Body() != nullptr) {
            std::istreambuf_iterator<char> isb(*response->Body().get()), end;
            error.setMessage(std::string(isb, end));
        }
    } else {
        ss << "ClientError:" << responseCode;
        error.setCode(ss.str());
        error.setMessage(response->statusMsg());
    }
    error.setHeaders(response->Headers());
    return error;
}

bool Client::hasResponseError(const std::shared_ptr<HttpResponse>&response)const
{
    if (!response) {
        return true;
    }
    return (response->statusCode()/100 != 2);
}

void Client::disableRequest()
{
    httpClient_->disable();
}

void Client::enableRequest()
{
    httpClient_->enable();
}

bool Client::isEnableRequest() const
{
    return httpClient_->isEnable();
}
   
void Client::setRequestDateOffset(uint64_t offset) const
{
    requestDateOffset_ = offset;
}

uint64_t Client::getRequestDateOffset() const
{
    return requestDateOffset_;
}