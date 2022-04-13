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

#include <ctime>
#include <algorithm>
#include <sstream>
#include <set>
#include <external/tinyxml2/tinyxml2.h>
#include <alibabacloud/oss/http/HttpType.h>
#include <alibabacloud/oss/Const.h>
#include <fstream>
#include "utils/Utils.h"
#include "utils/SignUtils.h"
#include "utils/ThreadExecutor.h"
#include "auth/HmacSha1Signer.h"
#include "OssClientImpl.h"
#include "utils/LogUtils.h"
#include "utils/FileSystemUtils.h"
#if !defined(OSS_DISABLE_RESUAMABLE)
#include "resumable/ResumableUploader.h"
#include "resumable/ResumableDownloader.h"
#include "resumable/ResumableCopier.h"
#endif
#include <posix/timer.h>
using namespace AlibabaCloud::OSS;
using namespace tinyxml2;

Url g_ags_url;

namespace
{
const std::string SERVICE_NAME = "OSS";
const char *TAG = "OssClientImpl";
}

OssClientImpl::OssClientImpl(const std::string &endpoint, const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
    Client(SERVICE_NAME, configuration),
    endpoint_(endpoint),
    credentialsProvider_(credentialsProvider),
    signer_(std::make_shared<HmacSha1Signer>()),
    executor_(configuration.executor ? configuration.executor :std::make_shared<ThreadExecutor>())
{
}

OssClientImpl::~OssClientImpl()
{
}

int OssClientImpl::asyncExecute(Runnable * r) const
{
    if (executor_ == nullptr)
        return 1;
    
    executor_->execute(r);
    return 0;
}

std::shared_ptr<HttpRequest> OssClientImpl::buildHttpRequest(const std::string & endpoint, const ServiceRequest & msg, Http::Method method) const
{
    auto httpRequest = std::make_shared<HttpRequest>(method);
    auto calcContentMD5 = !!(msg.Flags()&REQUEST_FLAG_CONTENTMD5);
    auto paramInPath = !!(msg.Flags()&REQUEST_FLAG_PARAM_IN_PATH);
    httpRequest->setResponseStreamFactory(msg.ResponseStreamFactory());
    addHeaders(httpRequest, msg.Headers());
    addBody(httpRequest, msg.Body(), calcContentMD5);
    if (paramInPath) {
        httpRequest->setUrl(Url(msg.Path()));
    }
    else {
        addSignInfo(httpRequest, msg);
        addUrl(httpRequest, endpoint, msg);
    }
    addOther(httpRequest, msg);
    return httpRequest;
}

bool OssClientImpl::hasResponseError(const std::shared_ptr<HttpResponse>&response) const
{
    if (BASE::hasResponseError(response)) {
        return true;
    }

    //check crc64
    if (response->request().hasCheckCrc64() && 
        !response->request().hasHeader(Http::RANGE) &&
        response->hasHeader("x-oss-hash-crc64ecma")) {
        uint64_t clientCrc64 = response->request().Crc64Result();
        uint64_t serverCrc64 = std::strtoull(response->Header("x-oss-hash-crc64ecma").c_str(), nullptr, 10);
        if (clientCrc64 != serverCrc64) {
            response->setStatusCode(ERROR_CRC_INCONSISTENT);
            std::stringstream ss;
            ss << "Crc64 validation failed. Expected hash:" << serverCrc64
                << " not equal to calculated hash:" << clientCrc64
                << ". Transferd bytes:" << response->request().TransferedBytes()
                << ". RequestId:" << response->Header("x-oss-request-id").c_str();
            response->setStatusMsg(ss.str().c_str());
            return true;
        }
    }

    //check Calback 
    if (response->statusCode() == 203 &&
        (response->request().hasHeader("x-oss-callback") ||
        (response->request().url().query().find("callback=") != std::string::npos))) {
        return true;
    }

    return false;
}

void OssClientImpl::addHeaders(const std::shared_ptr<HttpRequest> &httpRequest, const HeaderCollection &headers) const
{
    for (auto const& header : headers) {
        httpRequest->addHeader(header.first, header.second);
    }

    //common headers
    httpRequest->addHeader(Http::USER_AGENT, configuration().userAgent);

    //Date
    if (httpRequest->hasHeader("x-oss-date")) {
        httpRequest->addHeader(Http::DATE, httpRequest->Header("x-oss-date"));
    }
    if (!httpRequest->hasHeader(Http::DATE)) {
#ifdef USE_AOS_TIME_POSIX_API
    struct timespec currentTime;
    time_t t;
    clock_gettime(CLOCK_REALTIME, &currentTime);
    t = currentTime.tv_nsec/1000000000 + currentTime.tv_sec;
    printf("----- time %lld ---------\r\n",t);
#else
    std::time_t t = std::time(nullptr);
#endif
        t += getRequestDateOffset();
        httpRequest->addHeader(Http::DATE, ToGmtTime(t));
    }
}

void OssClientImpl::addBody(const std::shared_ptr<HttpRequest> &httpRequest, const std::shared_ptr<std::iostream>& body, bool contentMd5) const
{
    if (body == nullptr) {
        Http::Method methold = httpRequest->method();
        if (methold == Http::Method::Get || methold == Http::Method::Post) {
            httpRequest->setHeader(Http::CONTENT_LENGTH, "0");
        } else {
            httpRequest->removeHeader(Http::CONTENT_LENGTH);
        }
    }
    
    if ((body != nullptr) && !httpRequest->hasHeader(Http::CONTENT_LENGTH)) {
        auto streamSize = GetIOStreamLength(*body);
        httpRequest->setHeader(Http::CONTENT_LENGTH, std::to_string(streamSize));
    }

    if (contentMd5 && body && !httpRequest->hasHeader(Http::CONTENT_MD5)) {
        auto md5 = ComputeContentMD5(*body);
        httpRequest->setHeader(Http::CONTENT_MD5, md5);
    }

    httpRequest->addBody(body);
}

void OssClientImpl::addSignInfo(const std::shared_ptr<HttpRequest> &httpRequest, const ServiceRequest &request) const
{
    const Credentials credentials = credentialsProvider_->getCredentials();
    if (!credentials.SessionToken().empty()) {
        httpRequest->addHeader("x-oss-security-token", credentials.SessionToken());
    }

    //Sort the parameters
    ParameterCollection parameters;
    for (auto const&param : request.Parameters()) {
        parameters[param.first] = param.second;
    }

    std::string method = Http::MethodToString(httpRequest->method());

    const OssRequest& ossRequest = static_cast<const OssRequest&>(request);
    std::string resource;
    resource.append("/");
    if (!ossRequest.bucket().empty()) {
        resource.append(ossRequest.bucket());
        resource.append("/");
    }
    if (!ossRequest.key().empty()) {
        resource.append(ossRequest.key());
    }

    std::string date = httpRequest->Header(Http::DATE);

    SignUtils signUtils(signer_->version());
    signUtils.build(method, resource, date, httpRequest->Headers(), parameters);
    auto signature = signer_->generate(signUtils.CanonicalString(), credentials.AccessKeySecret());

    std::stringstream authValue;
    authValue
        << "OSS "
        << credentials.AccessKeyId()
        << ":"
        << signature;

    httpRequest->addHeader(Http::AUTHORIZATION, authValue.str());

    OSS_LOG(LogLevel::LogDebug, TAG, "client(%p) request(%p) CanonicalString:%s", this, httpRequest.get(), signUtils.CanonicalString().c_str());
    OSS_LOG(LogLevel::LogDebug, TAG, "client(%p) request(%p) Authorization:%s", this, httpRequest.get(), authValue.str().c_str());
}

void OssClientImpl::addUrl(const std::shared_ptr<HttpRequest> &httpRequest, const std::string &endpoint, const ServiceRequest &request) const
{
    const OssRequest& ossRequest = static_cast<const OssRequest&>(request);

    auto host = CombineHostString(endpoint, ossRequest.bucket(), configuration().isCname);
    auto path = CombinePathString(endpoint, ossRequest.bucket(), ossRequest.key());

    Url url(host);
    url.setPath(path);

    OSS_LOG(LogLevel::LogDebug, TAG, "client(%p) request(%p) host:%s, path:%s", this, httpRequest.get(), host.c_str(), path.c_str());
    
    auto parameters = request.Parameters();
    if (!parameters.empty()) {
        std::stringstream queryString;
        for (const auto &p : parameters)
        {
            if (p.second.empty())
                queryString << "&" << UrlEncode(p.first);
            else
                queryString << "&" << UrlEncode(p.first) << "=" << UrlEncode(p.second);
        }
        url.setQuery(queryString.str().substr(1));
    }
    httpRequest->setUrl(url);
    g_ags_url = httpRequest->url();
    //std::cout << "addUrl url:" << url.toString() <<std::endl;
    //std::cout << "g_ags_url url:" << g_ags_url.toString() <<std::endl;
}

void OssClientImpl::addOther(const std::shared_ptr<HttpRequest> &httpRequest, const ServiceRequest &request) const
{
    //progress
    httpRequest->setTransferProgress(request.TransferProgress());

    //crc64 check
    auto checkCRC64 = !!(request.Flags()&REQUEST_FLAG_CHECK_CRC64);
    if (configuration().enableCrc64 && checkCRC64 ) {
        httpRequest->setCheckCrc64(true);
#ifdef ENABLE_OSS_TEST
        if (!!(request.Flags()&0x80000000)) {
            httpRequest->addHeader("oss-test-crc64", "1");
        }
#endif
    }
}

OssError OssClientImpl::buildError(const Error &error) const
{
    OssError err;
    if (((error.Status() == 203) || (error.Status() > 299 && error.Status() < 600)) && 
        !error.Message().empty()) {
        XMLDocument doc;
        XMLError xml_err;
        if ((xml_err = doc.Parse(error.Message().c_str(), error.Message().size())) == XML_SUCCESS) {
            XMLElement* root =doc.RootElement();
            if (root && !std::strncmp("Error", root->Name(), 5)) {
                XMLElement *node;
                node = root->FirstChildElement("Code");
                err.setCode(node ? node->GetText(): "");
                node = root->FirstChildElement("Message");
                err.setMessage(node ? node->GetText(): "");
                node = root->FirstChildElement("RequestId");
                err.setRequestId(node ? node->GetText(): "");
                node = root->FirstChildElement("HostId");
                err.setHost(node ? node->GetText(): "");
            } else {
                err.setCode("ParseXMLError");
                err.setMessage("Xml format invalid, root node name is not Error. the content is:\n" + error.Message());
            }
        } else {
            std::stringstream ss;
            ss << "ParseXMLError:" << xml_err;
            err.setCode(ss.str());
            err.setMessage(XMLDocument::ErrorIDToName(xml_err));
        }
    } 
    else {
        err.setCode(error.Code());
        err.setMessage(error.Message());
    }

    //get from header if body has nothing
    if (err.RequestId().empty()) {
        auto it = error.Headers().find("x-oss-request-id");
        if (it != error.Headers().end()) {
            err.setRequestId(it->second);
        }
    }

    return err;
}

ServiceResult OssClientImpl::buildResult(const OssRequest &request, const std::shared_ptr<HttpResponse> &httpResponse) const
{
    ServiceResult result;
    auto flag = request.Flags();
    if ((flag & REQUEST_FLAG_CHECK_CRC64) &&
        (flag & REQUEST_FLAG_SAVE_CLIENT_CRC64)) {
        httpResponse->addHeader("x-oss-hash-crc64ecma-by-client", std::to_string(httpResponse->request().Crc64Result()));
    }
    result.setRequestId(httpResponse->Header("x-oss-request-id"));
    result.setPlayload(httpResponse->Body());
    result.setResponseCode(httpResponse->statusCode());
    result.setHeaderCollection(httpResponse->Headers());
    return result;
}

OssOutcome OssClientImpl::MakeRequest(const OssRequest &request, Http::Method method) const
{
    int ret = request.validate();
    if (ret != 0) {
        return OssOutcome(OssError("ValidateError", request.validateMessage(ret)));
    }

    auto outcome = BASE::AttemptRequest(endpoint_, request, method);
    if (outcome.isSuccess()) {
        return OssOutcome(buildResult(request, outcome.result()));
    } else {
        return OssOutcome(buildError(outcome.error()));
    }
}

#if !defined(OSS_DISABLE_BUCKET)

ListBucketsOutcome OssClientImpl::ListBuckets(const ListBucketsRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        ListBucketsResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? ListBucketsOutcome(std::move(result)) :
            ListBucketsOutcome(OssError("ParseXMLError", "Parsing ListBuckets result fail."));
    } else {
        return ListBucketsOutcome(outcome.error());
    }
}

CreateBucketOutcome OssClientImpl::CreateBucket(const CreateBucketRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        return  CreateBucketOutcome(Bucket());
    } else {
        return CreateBucketOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::SetBucketAcl(const SetBucketAclRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    } else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::SetBucketLogging(const SetBucketLoggingRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::SetBucketWebsite(const SetBucketWebsiteRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::SetBucketReferer(const SetBucketRefererRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::SetBucketLifecycle(const SetBucketLifecycleRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::SetBucketCors(const SetBucketCorsRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::SetBucketStorageCapacity(const SetBucketStorageCapacityRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::SetBucketPolicy(const SetBucketPolicyRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}
VoidOutcome OssClientImpl::SetBucketRequestPayment(const SetBucketRequestPaymentRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome AlibabaCloud::OSS::OssClientImpl::SetBucketEncryption(const SetBucketEncryptionRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::SetBucketTagging(const SetBucketTaggingRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::SetBucketQosInfo(const SetBucketQosInfoRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::SetBucketVersioning(const SetBucketVersioningRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::SetBucketInventoryConfiguration(const SetBucketInventoryConfigurationRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::DeleteBucket(const DeleteBucketRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Delete);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::DeleteBucketLogging(const DeleteBucketLoggingRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Delete);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::DeleteBucketWebsite(const DeleteBucketWebsiteRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Delete);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::DeleteBucketLifecycle(const DeleteBucketLifecycleRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Delete);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::DeleteBucketCors(const DeleteBucketCorsRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Delete);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::DeleteBucketPolicy(const DeleteBucketPolicyRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Delete);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::DeleteBucketEncryption(const DeleteBucketEncryptionRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Delete);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::DeleteBucketTagging(const DeleteBucketTaggingRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Delete);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::DeleteBucketQosInfo(const DeleteBucketQosInfoRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Delete);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::DeleteBucketInventoryConfiguration(const DeleteBucketInventoryConfigurationRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Delete);
    if (outcome.isSuccess()) {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

GetBucketAclOutcome OssClientImpl::GetBucketAcl(const GetBucketAclRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetBucketAclResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? GetBucketAclOutcome(std::move(result)) :
            GetBucketAclOutcome(OssError("ParseXMLError", "Parsing ListObject result fail."));
    }
    else {
        return GetBucketAclOutcome(outcome.error());
    }
}

GetBucketLocationOutcome OssClientImpl::GetBucketLocation(const GetBucketLocationRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetBucketLocationResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? GetBucketLocationOutcome(std::move(result)) :
            GetBucketLocationOutcome(OssError("ParseXMLError", "Parsing ListObject result fail."));
    }
    else {
        return GetBucketLocationOutcome(outcome.error());
    }
}

GetBucketInfoOutcome  OssClientImpl::GetBucketInfo(const  GetBucketInfoRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetBucketInfoResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? GetBucketInfoOutcome(std::move(result)) :
            GetBucketInfoOutcome(OssError("ParseXMLError", "Parsing ListObject result fail."));
    }
    else {
        return GetBucketInfoOutcome(outcome.error());
    }
}

GetBucketLoggingOutcome OssClientImpl::GetBucketLogging(const GetBucketLoggingRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetBucketLoggingResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? GetBucketLoggingOutcome(std::move(result)) :
            GetBucketLoggingOutcome(OssError("ParseXMLError", "Parsing ListObject result fail."));
    }
    else {
        return GetBucketLoggingOutcome(outcome.error());
    }
}

GetBucketWebsiteOutcome OssClientImpl::GetBucketWebsite(const GetBucketWebsiteRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetBucketWebsiteResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? GetBucketWebsiteOutcome(std::move(result)) :
            GetBucketWebsiteOutcome(OssError("ParseXMLError", "Parsing ListObject result fail."));
    }
    else {
        return GetBucketWebsiteOutcome(outcome.error());
    }
}

GetBucketRefererOutcome OssClientImpl::GetBucketReferer(const GetBucketRefererRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetBucketRefererResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? GetBucketRefererOutcome(std::move(result)) :
            GetBucketRefererOutcome(OssError("ParseXMLError", "Parsing ListObject result fail."));
    }
    else {
        return GetBucketRefererOutcome(outcome.error());
    }
}

GetBucketLifecycleOutcome OssClientImpl::GetBucketLifecycle(const GetBucketLifecycleRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetBucketLifecycleResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? GetBucketLifecycleOutcome(std::move(result)) :
            GetBucketLifecycleOutcome(OssError("ParseXMLError", "Parsing ListObject result fail."));
    }
    else {
        return GetBucketLifecycleOutcome(outcome.error());
    }
}

GetBucketStatOutcome OssClientImpl::GetBucketStat(const GetBucketStatRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetBucketStatResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? GetBucketStatOutcome(std::move(result)) :
            GetBucketStatOutcome(OssError("ParseXMLError", "Parsing ListObject result fail."));
    }
    else {
        return GetBucketStatOutcome(outcome.error());
    }
}

GetBucketCorsOutcome OssClientImpl::GetBucketCors(const GetBucketCorsRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetBucketCorsResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? GetBucketCorsOutcome(std::move(result)) :
            GetBucketCorsOutcome(OssError("ParseXMLError", "Parsing ListObject result fail."));
    }
    else {
        return GetBucketCorsOutcome(outcome.error());
    }
}

GetBucketStorageCapacityOutcome OssClientImpl::GetBucketStorageCapacity(const GetBucketStorageCapacityRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetBucketStorageCapacityResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? GetBucketStorageCapacityOutcome(std::move(result)) :
            GetBucketStorageCapacityOutcome(OssError("ParseXMLError", "Parsing ListObject result fail."));
    }
    else {
        return GetBucketStorageCapacityOutcome(outcome.error());
    }
}

GetBucketPolicyOutcome OssClientImpl::GetBucketPolicy(const GetBucketPolicyRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetBucketPolicyResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? GetBucketPolicyOutcome(std::move(result)) :
            GetBucketPolicyOutcome(OssError("ParseXMLError", "Parsing ListObject result fail."));
    }
    else {
        return GetBucketPolicyOutcome(outcome.error());
    }
}
GetBucketPaymentOutcome OssClientImpl::GetBucketRequestPayment(const GetBucketRequestPaymentRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetBucketPaymentResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? GetBucketPaymentOutcome(std::move(result)) :
            GetBucketPaymentOutcome(OssError("ParseXMLError", "Parsing GetBucketPayment result fail."));
    }
    else {
        return GetBucketPaymentOutcome(outcome.error());
    }
}

GetBucketEncryptionOutcome OssClientImpl::GetBucketEncryption(const GetBucketEncryptionRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetBucketEncryptionResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? GetBucketEncryptionOutcome(std::move(result)) :
            GetBucketEncryptionOutcome(OssError("ParseXMLError", "Parsing GetBucketEncryption result fail."));
    }
    else {
        return GetBucketEncryptionOutcome(outcome.error());
    }
}

GetBucketTaggingOutcome OssClientImpl::GetBucketTagging(const GetBucketTaggingRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetBucketTaggingResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? GetBucketTaggingOutcome(std::move(result)) :
            GetBucketTaggingOutcome(OssError("ParseXMLError", "Parsing GetBucketTagging result fail."));
    }
    else {
        return GetBucketTaggingOutcome(outcome.error());
    }
}

GetBucketQosInfoOutcome OssClientImpl::GetBucketQosInfo(const GetBucketQosInfoRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetBucketQosInfoResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? GetBucketQosInfoOutcome(std::move(result)) :
            GetBucketQosInfoOutcome(OssError("ParseXMLError", "Parsing GetBucketQosInfo result fail."));
    }
    else {
        return GetBucketQosInfoOutcome(outcome.error());
    }
}

GetUserQosInfoOutcome OssClientImpl::GetUserQosInfo(const GetUserQosInfoRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetUserQosInfoResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? GetUserQosInfoOutcome(std::move(result)) :
            GetUserQosInfoOutcome(OssError("ParseXMLError", "Parsing GetUserQosInfo result fail."));
    }
    else {
        return GetUserQosInfoOutcome(outcome.error());
    }
}

GetBucketVersioningOutcome OssClientImpl::GetBucketVersioning(const GetBucketVersioningRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetBucketVersioningResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? GetBucketVersioningOutcome(std::move(result)) :
            GetBucketVersioningOutcome(OssError("ParseXMLError", "Parsing GetBucketVersioning result fail."));
    }
    else {
        return GetBucketVersioningOutcome(outcome.error());
    }
}

GetBucketInventoryConfigurationOutcome OssClientImpl::GetBucketInventoryConfiguration(const GetBucketInventoryConfigurationRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetBucketInventoryConfigurationResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? GetBucketInventoryConfigurationOutcome(std::move(result)) :
            GetBucketInventoryConfigurationOutcome(OssError("ParseXMLError", "Parsing GetBucketInventoryConfiguration result fail."));
    }
    else {
        return GetBucketInventoryConfigurationOutcome(outcome.error());
    }
}

ListBucketInventoryConfigurationsOutcome OssClientImpl::ListBucketInventoryConfigurations(const ListBucketInventoryConfigurationsRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        ListBucketInventoryConfigurationsResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? ListBucketInventoryConfigurationsOutcome(std::move(result)) :
            ListBucketInventoryConfigurationsOutcome(OssError("ParseXMLError", "Parsing ListBucketInventoryConfigurations result fail."));
    }
    else {
        return ListBucketInventoryConfigurationsOutcome(outcome.error());
    }
}
#endif

ListObjectOutcome OssClientImpl::ListObjects(const ListObjectsRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        ListObjectsResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? ListObjectOutcome(std::move(result)) :
            ListObjectOutcome(OssError("ParseXMLError", "Parsing ListObject result fail."));
    }
    else {
        return ListObjectOutcome(outcome.error());
    }
}

ListObjectVersionsOutcome OssClientImpl::ListObjectVersions(const ListObjectVersionsRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        ListObjectVersionsResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? ListObjectVersionsOutcome(std::move(result)) :
            ListObjectVersionsOutcome(OssError("ParseXMLError", "Parsing ListObjectVersions result fail."));
    }
    else {
        return ListObjectVersionsOutcome(outcome.error());
    }
}

#undef GetObject
GetObjectOutcome OssClientImpl::GetObject(const GetObjectRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        return GetObjectOutcome(GetObjectResult(request.Bucket(), request.Key(),
            outcome.result().payload(),outcome.result().headerCollection()));
    }
    else {
        return GetObjectOutcome(outcome.error());
    }
}

PutObjectOutcome OssClientImpl::PutObject(const PutObjectRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        return PutObjectOutcome(PutObjectResult(outcome.result().headerCollection(), 
            outcome.result().payload()));
    }
    else {
        return PutObjectOutcome(outcome.error());
    }
}

DeleteObjectOutcome OssClientImpl::DeleteObject(const DeleteObjectRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Delete);
    if (outcome.isSuccess()) {
        return DeleteObjectOutcome(DeleteObjectResult(outcome.result().headerCollection()));
    }
    else {
        return DeleteObjectOutcome(outcome.error());
    }
}

DeleteObjecstOutcome OssClientImpl::DeleteObjects(const DeleteObjectsRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Post);
    if (outcome.isSuccess()) {
        DeleteObjectsResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? DeleteObjecstOutcome(std::move(result)) :
            DeleteObjecstOutcome(OssError("ParseXMLError", "Parsing DeleteObjects result fail."));
    }
    else {
        return DeleteObjecstOutcome(outcome.error());
    }
}

DeleteObjecVersionstOutcome OssClientImpl::DeleteObjectVersions(const DeleteObjectVersionsRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Post);
    if (outcome.isSuccess()) {
        DeleteObjectVersionsResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? DeleteObjecVersionstOutcome(std::move(result)) :
            DeleteObjecVersionstOutcome(OssError("ParseXMLError", "Parsing DeleteObjectVersions result fail."));
    }
    else {
        return DeleteObjecVersionstOutcome(outcome.error());
    }
}

ObjectMetaDataOutcome OssClientImpl::HeadObject(const HeadObjectRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Head);
    if (outcome.isSuccess()) {
        ObjectMetaData metaData = outcome.result().headerCollection();
        return ObjectMetaDataOutcome(std::move(metaData));
    }
    else {
        return ObjectMetaDataOutcome(outcome.error());
    }
}

ObjectMetaDataOutcome OssClientImpl::GetObjectMeta(const GetObjectMetaRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Head);
    if (outcome.isSuccess()) {
        ObjectMetaData metaData = outcome.result().headerCollection();
        return ObjectMetaDataOutcome(std::move(metaData));
    }
    else {
        return ObjectMetaDataOutcome(outcome.error());
    }
}

GetObjectAclOutcome OssClientImpl::GetObjectAcl(const GetObjectAclRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetObjectAclResult result(outcome.result().headerCollection(), outcome.result().payload());
        return result.ParseDone() ? GetObjectAclOutcome(std::move(result)) :
            GetObjectAclOutcome(OssError("ParseXMLError", "Parsing GetObjectAcl result fail."));
    }
    else {
        return GetObjectAclOutcome(outcome.error());
    }
}

AppendObjectOutcome OssClientImpl::AppendObject(const AppendObjectRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Post);
    if (outcome.isSuccess()) {
		AppendObjectResult result(outcome.result().headerCollection());
        return result.ParseDone() ? AppendObjectOutcome(std::move(result)) :
            AppendObjectOutcome(OssError("ParseXMLError", "no position or no crc64"));
    }
    else {
        return AppendObjectOutcome(outcome.error());
    }
}

CopyObjectOutcome OssClientImpl::CopyObject(const CopyObjectRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        return CopyObjectOutcome(CopyObjectResult(outcome.result().headerCollection(), outcome.result().payload()));
    }
    else {
        return CopyObjectOutcome(outcome.error());
    }
}

GetSymlinkOutcome OssClientImpl::GetSymlink(const GetSymlinkRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        return GetSymlinkOutcome(GetSymlinkResult(outcome.result().headerCollection()));
    }
    else {
        return GetSymlinkOutcome(outcome.error());
    }
}

RestoreObjectOutcome OssClientImpl::RestoreObject(const RestoreObjectRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Post);
    if (outcome.isSuccess()) {
        return RestoreObjectOutcome(RestoreObjectResult(outcome.result().headerCollection()));
    }
    else {
        return RestoreObjectOutcome(outcome.error());
    }
}

CreateSymlinkOutcome OssClientImpl::CreateSymlink(const CreateSymlinkRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        return CreateSymlinkOutcome(CreateSymlinkResult(outcome.result().headerCollection()));
    }
    else {
        return CreateSymlinkOutcome(outcome.error());
    }
}

SetObjectAclOutcome OssClientImpl::SetObjectAcl(const SetObjectAclRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        return SetObjectAclOutcome(SetObjectAclResult(outcome.result().headerCollection()));
    }
    else {
        return SetObjectAclOutcome(outcome.error());
    }
}

GetObjectOutcome OssClientImpl::ProcessObject(const ProcessObjectRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Post);
    if (outcome.isSuccess()) {
        return GetObjectOutcome(GetObjectResult(request.Bucket(), request.Key(),
            outcome.result().payload(), outcome.result().headerCollection()));
    }
    else {
        return GetObjectOutcome(outcome.error());
    }
}

GetObjectOutcome OssClientImpl::SelectObject(const SelectObjectRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Post);
    int ret = request.dispose();
    if (outcome.isSuccess()) {
        return GetObjectOutcome(GetObjectResult(request.Bucket(), request.Key(),
            outcome.result().payload(), outcome.result().headerCollection()));
    }
    else {
        if (ret != 0) {
            return GetObjectOutcome(OssError("SelectObjectError", request.validateMessage(ret)));
        }
        return GetObjectOutcome(outcome.error());
    }
}

CreateSelectObjectMetaOutcome OssClientImpl::CreateSelectObjectMeta(const CreateSelectObjectMetaRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Method::Post);
    if (outcome.isSuccess()) {
        CreateSelectObjectMetaResult result(request.Bucket(), request.Key(),
            outcome.result().RequestId(), outcome.result().payload());
        return result.ParseDone() ? CreateSelectObjectMetaOutcome(result) :
            CreateSelectObjectMetaOutcome(OssError("ParseIOStreamError", "Parse create select object meta IOStream fail."));
    }
    else {
        return CreateSelectObjectMetaOutcome(outcome.error());
    }
}

SetObjectTaggingOutcome OssClientImpl::SetObjectTagging(const SetObjectTaggingRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Put);
    if (outcome.isSuccess()) {
        return SetObjectTaggingOutcome(SetObjectTaggingResult(outcome.result().headerCollection()));
    }
    else {
        return SetObjectTaggingOutcome(outcome.error());
    }
}

DeleteObjectTaggingOutcome OssClientImpl::DeleteObjectTagging(const DeleteObjectTaggingRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Delete);
    if (outcome.isSuccess()) {
        return DeleteObjectTaggingOutcome(DeleteObjectTaggingResult(outcome.result().headerCollection()));
    }
    else {
        return DeleteObjectTaggingOutcome(outcome.error());
    }
}

GetObjectTaggingOutcome OssClientImpl::GetObjectTagging(const GetObjectTaggingRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Method::Get);
    if (outcome.isSuccess()) {
        GetObjectTaggingResult result(outcome.result().headerCollection(), outcome.result().payload());
        return result.ParseDone() ? GetObjectTaggingOutcome(std::move(result)) :
            GetObjectTaggingOutcome(OssError("ParseXMLError", "Parsing ObjectTagging result fail."));
    }
    else {
        return GetObjectTaggingOutcome(outcome.error());
    }
}

StringOutcome OssClientImpl::GeneratePresignedUrl(const GeneratePresignedUrlRequest &request) const
{
    if (!IsValidBucketName(request.bucket_) ||
        !IsValidObjectKey(request.key_)) {
        return StringOutcome(OssError("ValidateError", "The Bucket or Key is invalid."));
    }

    HeaderCollection headers = request.metaData_.toHeaderCollection();

    ParameterCollection parameters;
    const Credentials credentials = credentialsProvider_->getCredentials();
    if (!credentials.SessionToken().empty()) {
        parameters["security-token"] = credentials.SessionToken();
    }
    for (auto const&param : request.parameters_) {
        parameters[param.first] = param.second;
    }

    SignUtils signUtils(signer_->version());
    auto method = Http::MethodToString(request.method_);
    auto resource = std::string().append("/").append(request.bucket_).append("/").append(request.key_);
    auto date = headers[Http::EXPIRES];
    signUtils.build(method, resource, date, headers, parameters);
    auto signature = signer_->generate(signUtils.CanonicalString(), credentials.AccessKeySecret());
    parameters["Expires"] = date;
    parameters["OSSAccessKeyId"] = credentials.AccessKeyId();
    parameters["Signature"] = signature;

    std::stringstream ss;
    ss << CombineHostString(endpoint_, request.bucket_, configuration().isCname);
    ss << CombinePathString(endpoint_, request.bucket_, request.key_);
    ss << "?";
    ss << CombineQueryString(parameters);

    return StringOutcome(ss.str());
}

GetObjectOutcome OssClientImpl::GetObjectByUrl(const GetObjectByUrlRequest &request) const
{
    auto outcome = BASE::AttemptRequest(endpoint_, request, Http::Method::Get);
    if (outcome.isSuccess()) {
        return GetObjectOutcome(GetObjectResult("", "", 
            outcome.result()->Body(),
            outcome.result()->Headers()));
    }
    else {
        return GetObjectOutcome(buildError(outcome.error()));
    }
}

PutObjectOutcome OssClientImpl::PutObjectByUrl(const PutObjectByUrlRequest &request) const
{
    auto outcome = BASE::AttemptRequest(endpoint_, request, Http::Method::Put);
    if (outcome.isSuccess()) {
        return PutObjectOutcome(PutObjectResult(outcome.result()->Headers(), 
            outcome.result()->Body()));
    }
    else {
        return PutObjectOutcome(buildError(outcome.error()));
    }
}

InitiateMultipartUploadOutcome OssClientImpl::InitiateMultipartUpload(const InitiateMultipartUploadRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Post);
    if(outcome.isSuccess()){
        InitiateMultipartUploadResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? InitiateMultipartUploadOutcome(std::move(result)):
                InitiateMultipartUploadOutcome(
                    OssError("InitiateMultipartUploadError",
                    "Parsing InitiateMultipartUploadResult fail"));
    }
    else{
        return InitiateMultipartUploadOutcome(outcome.error());
    }
}

PutObjectOutcome OssClientImpl::UploadPart(const UploadPartRequest &request)const
{
    auto outcome = MakeRequest(request, Http::Put);
    if(outcome.isSuccess()){
        const HeaderCollection& header = outcome.result().headerCollection();
        return PutObjectOutcome(PutObjectResult(header));
    }else{
        return PutObjectOutcome(outcome.error());
    }
}

UploadPartCopyOutcome OssClientImpl::UploadPartCopy(const UploadPartCopyRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Put);
    if(outcome.isSuccess()){
        const HeaderCollection& header = outcome.result().headerCollection();
        return UploadPartCopyOutcome(
            UploadPartCopyResult(outcome.result().payload(), header));
    }
    else{
        return UploadPartCopyOutcome(outcome.error());
    }
}

CompleteMultipartUploadOutcome OssClientImpl::CompleteMultipartUpload(const CompleteMultipartUploadRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Post);
    if (outcome.isSuccess()){
        CompleteMultipartUploadResult result(outcome.result().payload(), outcome.result().headerCollection());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ?
            CompleteMultipartUploadOutcome(std::move(result)) : 
            CompleteMultipartUploadOutcome(OssError("CompleteMultipartUpload", ""));
    }
    else {
        return CompleteMultipartUploadOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::AbortMultipartUpload(const AbortMultipartUploadRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Delete);
    if(outcome.isSuccess()){
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }
    else {
        return VoidOutcome(outcome.error());
    }
}

ListMultipartUploadsOutcome OssClientImpl::ListMultipartUploads(
    const ListMultipartUploadsRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Get);
    if(outcome.isSuccess())
    {
        ListMultipartUploadsResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ?
            ListMultipartUploadsOutcome(std::move(result)) :
            ListMultipartUploadsOutcome(OssError("ListMultipartUploads", "Parse Error"));
    }
    else {
        return ListMultipartUploadsOutcome(outcome.error());
    }
}

ListPartsOutcome OssClientImpl::ListParts(const ListPartsRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Get);
    if(outcome.isSuccess())
    {
        ListPartsResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone() ? 
            ListPartsOutcome(std::move(result)) :
            ListPartsOutcome(OssError("ListParts", "Parse Error"));
    }else{
        return ListPartsOutcome(outcome.error());
    }
}

#if !defined(OSS_DISABLE_RESUAMABLE)
/*Resumable Operation*/
PutObjectOutcome OssClientImpl::ResumableUploadObject(const UploadObjectRequest& request) const 
{
    const auto& reqeustBase = static_cast<const OssResumableBaseRequest &>(request);
    int code = reqeustBase.validate();
    if (code != 0) {
        return PutObjectOutcome(OssError("ValidateError", reqeustBase.validateMessage(code)));
    }

    if (request.ObjectSize() <= request.PartSize())
    {
        auto content = GetFstreamByPath(request.FilePath(), request.FilePathW(),
            std::ios::in | std::ios::binary);
        PutObjectRequest putObjectReq(request.Bucket(), request.Key(), content, request.MetaData());
        if (request.TransferProgress().Handler) {
            putObjectReq.setTransferProgress(request.TransferProgress());
        }
        if (request.RequestPayer() == RequestPayer::Requester) {
            putObjectReq.setRequestPayer(request.RequestPayer());
        }
        if (request.TrafficLimit() != 0) {
            putObjectReq.setTrafficLimit(request.TrafficLimit());
        }
        return PutObject(putObjectReq);
    }
    else
    {
        ResumableUploader uploader(request, this);
        return uploader.Upload();
    }
}

CopyObjectOutcome OssClientImpl::ResumableCopyObject(const MultiCopyObjectRequest& request) const 
{
    const auto& reqeustBase = static_cast<const OssResumableBaseRequest &>(request);
    int code = reqeustBase.validate();
    if (code != 0) {
        return CopyObjectOutcome(OssError("ValidateError", reqeustBase.validateMessage(code)));
    }

    auto getObjectMetaReq = GetObjectMetaRequest(request.SrcBucket(), request.SrcKey());
    if (request.RequestPayer() == RequestPayer::Requester) {
        getObjectMetaReq.setRequestPayer(request.RequestPayer());
    }
    if (!request.VersionId().empty()) {
        getObjectMetaReq.setVersionId(request.VersionId());
    }
    auto outcome = GetObjectMeta(getObjectMetaReq);
    if (!outcome.isSuccess()) {
        return CopyObjectOutcome(outcome.error());
    }

    auto objectSize = outcome.result().ContentLength();
    if (objectSize < (int64_t)request.PartSize()) {
        auto copyObjectReq = CopyObjectRequest(request.Bucket(), request.Key(), request.MetaData());
        copyObjectReq.setCopySource(request.SrcBucket(), request.SrcKey());
        if (request.RequestPayer() == RequestPayer::Requester) {
            copyObjectReq.setRequestPayer(request.RequestPayer());
        }
        if (request.TrafficLimit() != 0) {
            copyObjectReq.setTrafficLimit(request.TrafficLimit());
        }
        if (!request.VersionId().empty()) {
            copyObjectReq.setVersionId(request.VersionId());
        }
        return CopyObject(copyObjectReq);
    }

    ResumableCopier copier(request, this, objectSize);
    return copier.Copy();
}

GetObjectOutcome OssClientImpl::ResumableDownloadObject(const DownloadObjectRequest &request) const 
{
    const auto& reqeustBase = static_cast<const OssResumableBaseRequest &>(request);
    int code = reqeustBase.validate();
    if (code != 0) {
        return GetObjectOutcome(OssError("ValidateError", reqeustBase.validateMessage(code)));
    }

    auto getObjectMetaReq = GetObjectMetaRequest(request.Bucket(), request.Key());
    if (request.RequestPayer() == RequestPayer::Requester) {
        getObjectMetaReq.setRequestPayer(request.RequestPayer());
    }
    if (!request.VersionId().empty()) {
        getObjectMetaReq.setVersionId(request.VersionId());
    }
    auto hOutcome = GetObjectMeta(getObjectMetaReq);
    if (!hOutcome.isSuccess()) {
        return GetObjectOutcome(hOutcome.error());
    }

    auto objectSize = hOutcome.result().ContentLength();
    if (objectSize < (int64_t)request.PartSize()) {
        auto getObjectReq = GetObjectRequest(request.Bucket(), request.Key(), request.ModifiedSinceConstraint(), request.UnmodifiedSinceConstraint(),request.MatchingETagsConstraint(), request.NonmatchingETagsConstraint(), request.ResponseHeaderParameters());
        if (request.RangeIsSet()) {
            getObjectReq.setRange(request.RangeStart(), request.RangeEnd());
        }
        if (request.TransferProgress().Handler) {
            getObjectReq.setTransferProgress(request.TransferProgress());
        }
        if (request.RequestPayer() == RequestPayer::Requester) {
            getObjectReq.setRequestPayer(request.RequestPayer());
        }
        if (request.TrafficLimit() != 0) {
            getObjectReq.setTrafficLimit(request.TrafficLimit());
        }
        if (!request.VersionId().empty()) {
            getObjectReq.setVersionId(request.VersionId());
        }
        getObjectReq.setResponseStreamFactory([=]() {
            return GetFstreamByPath(request.FilePath(), request.FilePathW(),
                std::ios_base::out | std::ios_base::in | std::ios_base::trunc | std::ios_base::binary);
        });
        auto outcome = this->GetObject(getObjectReq);
        std::shared_ptr<std::iostream> content = nullptr;
        outcome.result().setContent(content);
        if (IsFileExist(request.TempFilePath())) {
            RemoveFile(request.TempFilePath());
        }
#ifdef _WIN32
        else if (IsFileExist(request.TempFilePathW())) {
            RemoveFile(request.TempFilePathW());
        }
#endif
        return outcome;
    }

    ResumableDownloader downloader(request, this, objectSize);
    return downloader.Download();
}
#endif

#if !defined(OSS_DISABLE_LIVECHANNEL)
/*Live Channel*/
VoidOutcome OssClientImpl::PutLiveChannelStatus(const PutLiveChannelStatusRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Put);
    if(outcome.isSuccess())
    {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(result);
    }else{
        return VoidOutcome(outcome.error());
    }
}

PutLiveChannelOutcome OssClientImpl::PutLiveChannel(const PutLiveChannelRequest& request) const
{
    auto outcome = MakeRequest(request, Http::Put);
    if(outcome.isSuccess())
    {
        PutLiveChannelResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone()?
            PutLiveChannelOutcome(std::move(result)):
            PutLiveChannelOutcome(OssError("PutLiveChannelError", "Parse Error"));
    }else{
        return PutLiveChannelOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::PostVodPlaylist(const PostVodPlaylistRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Post);
    if(outcome.isSuccess())
    {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(std::move(result));
    }else{
        return VoidOutcome(outcome.error());
    }
}

GetVodPlaylistOutcome OssClientImpl::GetVodPlaylist(const GetVodPlaylistRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Get);
    if(outcome.isSuccess())
    {
        GetVodPlaylistResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return GetVodPlaylistOutcome(std::move(result));
    }else{
        return GetVodPlaylistOutcome(outcome.error());
    }
}

GetLiveChannelStatOutcome OssClientImpl::GetLiveChannelStat(const GetLiveChannelStatRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Get);
    if(outcome.isSuccess())
    {
        GetLiveChannelStatResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone()?
            GetLiveChannelStatOutcome(std::move(result)):
            GetLiveChannelStatOutcome(OssError("GetLiveChannelStatError", "Parse Error"));
    }else{
        return GetLiveChannelStatOutcome(outcome.error());
    }
}

GetLiveChannelInfoOutcome OssClientImpl::GetLiveChannelInfo(const GetLiveChannelInfoRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Get);
    if(outcome.isSuccess())
    {
        GetLiveChannelInfoResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone()?
            GetLiveChannelInfoOutcome(std::move(result)):
            GetLiveChannelInfoOutcome(OssError("GetLiveChannelStatError", "Parse Error"));
    }else{
        return GetLiveChannelInfoOutcome(outcome.error());
    }
}

GetLiveChannelHistoryOutcome OssClientImpl::GetLiveChannelHistory(const GetLiveChannelHistoryRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Get);
    if(outcome.isSuccess())
    {
        GetLiveChannelHistoryResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone()?
            GetLiveChannelHistoryOutcome(std::move(result)):
            GetLiveChannelHistoryOutcome(OssError("GetLiveChannelStatError", "Parse Error"));
    }else{
        return GetLiveChannelHistoryOutcome(outcome.error());
    }
}

ListLiveChannelOutcome OssClientImpl::ListLiveChannel(const ListLiveChannelRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Get);
    if(outcome.isSuccess())
    {
        ListLiveChannelResult result(outcome.result().payload());
        result.requestId_ = outcome.result().RequestId();
        return result.ParseDone()?
            ListLiveChannelOutcome(std::move(result)):
            ListLiveChannelOutcome(OssError("GetLiveChannelStatError", "Parse Error"));
    }else{
        return ListLiveChannelOutcome(outcome.error());
    }
}

VoidOutcome OssClientImpl::DeleteLiveChannel(const DeleteLiveChannelRequest &request) const
{
    auto outcome = MakeRequest(request, Http::Delete);
    if(outcome.isSuccess())
    {
        VoidResult result;
        result.requestId_ = outcome.result().RequestId();
        return VoidOutcome(std::move(result));
    }else{
        return VoidOutcome(outcome.error());
    }
}

StringOutcome OssClientImpl::GenerateRTMPSignedUrl(const GenerateRTMPSignedUrlRequest &request) const
{
    if (!IsValidBucketName(request.bucket_) ||
        !IsValidChannelName(request.ChannelName()) || 
        !IsValidPlayListName(request.PlayList()) ||
        0 == request.Expires()) {
        return StringOutcome(OssError("ValidateError", "The Bucket or ChannelName or "
            "PlayListName or Expires is invalid."));
    }

    ParameterCollection parameters;
    const Credentials credentials = credentialsProvider_->getCredentials();
    if (!credentials.SessionToken().empty()) {
        parameters["security-token"] = credentials.SessionToken();
    }
    
    parameters = request.Parameters();

    std::string expireStr;
    std::stringstream ss;
    ss << request.Expires();
    expireStr = ss.str();

    SignUtils signUtils(signer_->version());
    auto resource = std::string().append("/").append(request.Bucket()).append("/").append(request.ChannelName());
    signUtils.build(expireStr, resource, parameters);
    auto signature = signer_->generate(signUtils.CanonicalString(), credentials.AccessKeySecret());
    parameters["Expires"] = expireStr;
    parameters["OSSAccessKeyId"] = credentials.AccessKeyId();
    parameters["Signature"] = signature;

    ss.str("");
    ss << CombineRTMPString(endpoint_, request.bucket_, configuration().isCname);
    ss << "/live";
    ss << CombinePathString(endpoint_, request.bucket_, request.key_);
    ss << "?";
    ss << CombineQueryString(parameters);

    return StringOutcome(ss.str());
}
#endif

/*Requests control*/
void OssClientImpl::DisableRequest()
{
    BASE::disableRequest();
    OSS_LOG(LogLevel::LogDebug, TAG, "client(%p) DisableRequest", this);
}

void OssClientImpl::EnableRequest()
{
    BASE::enableRequest();
    OSS_LOG(LogLevel::LogDebug, TAG, "client(%p) EnableRequest", this);
}
