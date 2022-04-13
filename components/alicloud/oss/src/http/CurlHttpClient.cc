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

#include "CurlHttpClient.h"
#include <curl/curl.h>
#include <cassert>
#include <sstream>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <algorithm>
#include <utils/Crc64.h>
#include <alibabacloud/oss/client/Error.h>
#include <alibabacloud/oss/client/RateLimiter.h>
#include "../utils/LogUtils.h"
#include "utils/Utils.h"

#ifdef TAG
#undef TAG
#endif
using namespace AlibabaCloud::OSS;

namespace AlibabaCloud
{
namespace OSS
{
    const char * TAG = "CurlHttpClient";
    ////////////////////////////////////////////////////////////////////////////////////////////
    template< typename RESOURCE_TYPE>
    class ResourceManager_
    {
    public:
        ResourceManager_() : m_shutdown(false) {}
        RESOURCE_TYPE Acquire()
        {
            std::unique_lock<std::mutex> locker(m_queueLock);
            while(!m_shutdown.load() && m_resources.size() == 0)
            {
                m_semaphore.wait(locker, [&](){ return m_shutdown.load() || m_resources.size() > 0; });                    
            }
    
            assert(!m_shutdown.load());
    
            RESOURCE_TYPE resource = m_resources.back();
            m_resources.pop_back();
    
            return resource;
        }
    
        bool HasResourcesAvailable()
        {
            std::lock_guard<std::mutex> locker(m_queueLock);
            return m_resources.size() > 0 && !m_shutdown.load();
        }
    
        void Release(RESOURCE_TYPE resource)
        {
            std::unique_lock<std::mutex> locker(m_queueLock);
            m_resources.push_back(resource);
            locker.unlock();
            m_semaphore.notify_one();
        }
    
        void PutResource(RESOURCE_TYPE resource)
        {
            m_resources.push_back(resource);
        }
    
        std::vector<RESOURCE_TYPE> ShutdownAndWait(size_t resourceCount)
        {
            std::vector<RESOURCE_TYPE> resources;
            std::unique_lock<std::mutex> locker(m_queueLock);
            m_shutdown = true;
            while (m_resources.size() < resourceCount)
            {
                m_semaphore.wait(locker, [&]() { return m_resources.size() == resourceCount; });
            }
            resources = m_resources;
            m_resources.clear();
            return resources;
        }
    
    private:
        std::vector<RESOURCE_TYPE> m_resources;
        std::mutex m_queueLock;
        std::condition_variable m_semaphore;
        std::atomic<bool> m_shutdown;
    };
    
    
    class CurlContainer
    {
    public:
        CurlContainer(unsigned maxSize = 16, long requestTimeout = 10000, long connectTimeout = 5000):
              maxPoolSize_(maxSize), 
              requestTimeout_(requestTimeout), 
              connectTimeout_(connectTimeout),
              poolSize_(0)
        {
        }
    
        ~CurlContainer()
        {
            for (CURL* handle : handleContainer_.ShutdownAndWait(poolSize_)) {
                curl_easy_cleanup(handle);
            }
        }
    
        CURL* Acquire()
        {
            if(!handleContainer_.HasResourcesAvailable()) {
                growPool();
            }
            CURL* handle = handleContainer_.Acquire();
            return handle;
        }    
    
        void Release(CURL* handle, bool force)
        {
            if (handle) {
                curl_easy_reset(handle);
                if (force) {
                    CURL* newhandle = curl_easy_init();
                    if (newhandle) {
                        curl_easy_cleanup(handle);
                        handle = newhandle;
                    }
                }
                setDefaultOptions(handle);
                handleContainer_.Release(handle);
            }
        }
    
    private:
        CurlContainer(const CurlContainer&) = delete;
        const CurlContainer& operator = (const CurlContainer&) = delete;
        CurlContainer(const CurlContainer&&) = delete;
        const CurlContainer& operator = (const CurlContainer&&) = delete;
    
        bool growPool()
        {
            std::lock_guard<std::mutex> locker(containerLock_);
            if (poolSize_ < maxPoolSize_) {
                unsigned multiplier = poolSize_ > 0 ? poolSize_ : 1;
                unsigned amountToAdd = (std::min)(multiplier * 2, maxPoolSize_ - poolSize_);
        
                unsigned actuallyAdded = 0;
                for (unsigned i = 0; i < amountToAdd; ++i) {
                    CURL* curlHandle = curl_easy_init();
                    if (curlHandle) {
                        setDefaultOptions(curlHandle);
                        handleContainer_.Release(curlHandle);
                        ++actuallyAdded;
                    } else {
                        break;
                    }
                }
                poolSize_ += actuallyAdded;
                return actuallyAdded > 0;
            }
            return false;
        }
    
        void setDefaultOptions(CURL* handle)
        {
            curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1L);
            curl_easy_setopt(handle, CURLOPT_TCP_NODELAY, 1);
            curl_easy_setopt(handle, CURLOPT_NETRC, CURL_NETRC_IGNORED);

            curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, 0L);
            curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT_MS, connectTimeout_);
            curl_easy_setopt(handle, CURLOPT_LOW_SPEED_LIMIT, 1L);
            curl_easy_setopt(handle, CURLOPT_LOW_SPEED_TIME, requestTimeout_ / 1000);

            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0L);
        }
    
    private:
        ResourceManager_<CURL*> handleContainer_;
        unsigned maxPoolSize_;
        unsigned long requestTimeout_;
        unsigned long connectTimeout_;
        unsigned poolSize_;
        std::mutex containerLock_;
    };
    
    /////////////////////////////////////////////////////////////////////////////////////////////
    struct TransferState {
        CurlHttpClient *owner;
        CURL * curl;
        HttpRequest  *request;
        HttpResponse *response;
        int64_t transferred;
        int64_t total;
        bool firstRecvData;
        std::iostream::pos_type recvBodyPos;
        TransferProgressHandler progress;
        void *userData;
        bool enableCrc64;
        uint64_t sendCrc64Value;
        uint64_t recvCrc64Value;
        int sendSpeed;
        int recvSpeed;
    };

    static size_t sendBody(char *ptr, size_t size, size_t nmemb, void *userdata)
    {
        TransferState *state = static_cast<TransferState*>(userdata);

        if (state == nullptr || 
            state->request == nullptr) {
            return 0;
        }
        
        std::shared_ptr<std::iostream> &content = state->request->Body();
        const size_t wanted = size * nmemb;
        size_t got = 0;
        if (content != nullptr && wanted > 0) {
            size_t read = wanted;
            if (state->total > 0) {
                int64_t remains = state->total - state->transferred;
                if (remains < static_cast<int64_t>(wanted)) {
                    read = static_cast<size_t>(remains);
                }
            }
            // TODO: ethan
            printf("read\r\n");
            content->read(ptr, read);
            got = static_cast<size_t>(content->gcount());
        }

        state->transferred += got;
        if (state->progress) {
            state->progress(got, state->transferred, state->total, state->userData);
        }

        if (state->enableCrc64) {
            state->sendCrc64Value = CRC64::CalcCRC(state->sendCrc64Value, (void *)ptr, got);
        }

        return got;
    }

    static size_t recvBody(char *ptr, size_t size, size_t nmemb, void *userdata)
    {
        TransferState *state = static_cast<TransferState*>(userdata);
        const size_t wanted = size * nmemb;

        if (state == nullptr ||
            state->response == nullptr || 
            wanted == 0) {
            return -1;
        }

        if (state->firstRecvData) {
            long response_code = 0;
            curl_easy_getinfo(state->curl, CURLINFO_RESPONSE_CODE, &response_code);
            if (response_code / 100 == 2) {
                state->response->addBody(state->request->ResponseStreamFactory()());
                if (state->response->Body() != nullptr) {
                    state->recvBodyPos = state->response->Body()->tellp();
                }
                OSS_LOG(LogLevel::LogDebug, TAG, "request(%p) setResponseBody, recvBodyPos:%lld",
                    state->request, state->recvBodyPos);
            }
            else {
                state->response->addBody(std::make_shared<std::stringstream>());
            }
            state->firstRecvData = false;
        }

        std::shared_ptr<std::iostream> &content = state->response->Body();
        if (content == nullptr || content->fail()) {
            printf("error!\r\n");
            return -2;
        }
        // TODO: ethan
        printf("write\r\n");
        content->write(ptr, static_cast<std::streamsize>(wanted));
        if (content->bad()) {
            return -3;
        }

        state->transferred += wanted;
        if (state->progress) {
            state->progress(wanted, state->transferred, state->total, state->userData);
        }

        if (state->enableCrc64) {
            state->recvCrc64Value = CRC64::CalcCRC(state->recvCrc64Value, (void *)ptr, wanted);
        }

        return wanted;
    }

    static size_t recvHeaders(char *buffer, size_t size, size_t nitems, void *userdata)
    {
        TransferState *state = static_cast<TransferState*>(userdata);
        const size_t length = nitems * size;

        std::string line(buffer);
        auto pos = line.find(':');
        if (pos != line.npos)
        {
            size_t posEnd = line.rfind('\r');
            if (posEnd != line.npos) {
                posEnd = posEnd - pos - 2;
            }
            std::string name  = line.substr(0, pos);
            std::string value = line.substr(pos + 2, posEnd);
            state->response->setHeader(name, value);
        }

        if (length == 2 && (buffer[0] == 0x0D) && (buffer[1] == 0x0A)) {
            if (state->response->hasHeader(Http::CONTENT_LENGTH)) {
                double dval;
                curl_easy_getinfo(state->curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &dval);
                state->total = (int64_t)dval;
            }
        }
        return length;
    }

    static int debugCallback(void *handle, curl_infotype type, char *data, size_t size, void *userp)
    {
        UNUSED_PARAM(userp);
        switch (type) {
        default: 
            break;
        case CURLINFO_TEXT:
            OSS_LOG(LogLevel::LogInfo, TAG, "handle(%p)=> Info: %.*s", handle, size, data);
            break;
        case CURLINFO_HEADER_OUT:
            OSS_LOG(LogLevel::LogDebug, TAG, "handle(%p)=> Send header: %.*s", handle, size, data);
            break;
        case CURLINFO_HEADER_IN:
            OSS_LOG(LogLevel::LogDebug, TAG, "handle(%p)=> Recv header: %.*s", handle, size, data);
            break;
        }
        return 0;
    }

    static int progressCallback(void *userdata, double dltotal, double dlnow,  double ultotal, double ulnow)
    {
        UNUSED_PARAM(dltotal);
        UNUSED_PARAM(dlnow);
        UNUSED_PARAM(ultotal);
        UNUSED_PARAM(ulnow);
        TransferState *state = static_cast<TransferState*>(userdata);
        if (state == nullptr || state->owner == nullptr) {
            return 0;
        }

        CurlHttpClient *thiz = static_cast<CurlHttpClient *>(state->owner);

        //stop by upper caller
        if (!thiz->isEnable()) {
            return 1;
        }

        //for speed update
        if (thiz->sendRateLimiter_ != nullptr) {
            auto rate = thiz->sendRateLimiter_->Rate();
            if (rate != state->sendSpeed) {
                state->sendSpeed = rate;
                auto speed = static_cast<curl_off_t>(rate);
                speed = speed * 1024;
                curl_easy_setopt(state->curl, CURLOPT_MAX_SEND_SPEED_LARGE, speed);
            }
        }

        if (thiz->recvRateLimiter_ != nullptr) {
            auto rate = thiz->recvRateLimiter_->Rate();
            if (rate != state->recvSpeed) {
                state->recvSpeed = rate;
                auto speed = static_cast<curl_off_t>(rate);
                speed = speed * 1024;
                curl_easy_setopt(state->curl, CURLOPT_MAX_RECV_SPEED_LARGE, speed);
            }
        }

        return 0;
    }
}
}

void CurlHttpClient::initGlobalState()
{
    curl_global_init(CURL_GLOBAL_ALL);
}

void CurlHttpClient::cleanupGlobalState()
{
    curl_global_cleanup();
}

CurlHttpClient::CurlHttpClient(const ClientConfiguration &configuration) :
    HttpClient(),
    curlContainer_(new CurlContainer(configuration.maxConnections, 
                                                       configuration.requestTimeoutMs,
                                                       configuration.connectTimeoutMs)),
    userAgent_(configuration.userAgent),
    proxyScheme_(configuration.proxyScheme),
    proxyHost_(configuration.proxyHost),
    proxyPort_(configuration.proxyPort),
    proxyUserName_(configuration.proxyUserName),
    proxyPassword_(configuration.proxyPassword),
    verifySSL_(configuration.verifySSL),
    caPath_(configuration.caPath),
    caFile_(configuration.caFile),
    networkInterface_(configuration.networkInterface),
    sendRateLimiter_(configuration.sendRateLimiter),
    recvRateLimiter_(configuration.recvRateLimiter)
{
}

CurlHttpClient::~CurlHttpClient()
{
    if (curlContainer_) {
        delete curlContainer_;
    }
}

std::shared_ptr<HttpResponse> CurlHttpClient::makeRequest(const std::shared_ptr<HttpRequest> &request)
{
    OSS_LOG(LogLevel::LogDebug, TAG, "request(%p) enter makeRequest", request.get());
    curl_slist *list = nullptr;
    auto& headers = request->Headers();
    for (const auto &p : headers) {
        if (p.second.empty())
            continue;
        std::string str = p.first;
        str.append(": ").append(p.second);
        list = curl_slist_append(list, str.c_str());
    }
    // Disable Expect: 100-continue
    list = curl_slist_append(list, "Expect:");

    auto response = std::make_shared<HttpResponse>(request);
    
    std::iostream::pos_type requestBodyPos = -1;
    if (request->Body() != nullptr) {
        requestBodyPos = request->Body()->tellg();
    }

    CURL * curl = curlContainer_->Acquire();

    OSS_LOG(LogLevel::LogDebug, TAG, "request(%p) acquire curl handle:%p", request.get(), curl);

    uint64_t initCRC64 = 0;
#ifdef ENABLE_OSS_TEST
    if (headers.find("oss-test-crc64") != headers.end()) {
        initCRC64 = std::strtoull(headers.at("oss-test-crc64").c_str(), nullptr, 10);
    }
#endif
    TransferState transferState = {
        this,
        curl,
        request.get(),
        response.get(),
        0, -1, 
        true, -1, 
        request->TransferProgress().Handler,
        request->TransferProgress().UserData,
        request->hasCheckCrc64(), initCRC64, initCRC64, 
        0, 0
    };

    if (request->hasHeader(Http::CONTENT_LENGTH)) {
        transferState.total = std::atoll(request->Header(Http::CONTENT_LENGTH).c_str());
    }

    std::string url = request->url().toString();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    switch (request->method())
    {
    case Http::Method::Head:
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
        break;
    case Http::Method::Put:
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        break;
    case Http::Method::Post:
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        break;
    case Http::Method::Delete:
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        break;
    case Http::Method::Get:
    default:
        break;
    }
    
    curl_easy_setopt(curl, CURLOPT_USERAGENT,userAgent_.c_str());

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &transferState);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, recvHeaders);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &transferState);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, recvBody);

    curl_easy_setopt(curl, CURLOPT_READDATA, &transferState);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, sendBody);

    if (verifySSL_) {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    }
    if(!caPath_.empty()) {
        curl_easy_setopt(curl, CURLOPT_CAPATH, caPath_.c_str());
    }
    if(!caFile_.empty()){
        curl_easy_setopt(curl, CURLOPT_CAINFO, caFile_.c_str());
    }

    if (!proxyHost_.empty()) {
        std::stringstream ss;
        ss << Http::SchemeToString(proxyScheme_) << "://" << proxyHost_;
        curl_easy_setopt(curl, CURLOPT_PROXY, ss.str().c_str());
        curl_easy_setopt(curl, CURLOPT_PROXYPORT, (long) proxyPort_);
        curl_easy_setopt(curl, CURLOPT_PROXYUSERNAME, proxyUserName_.c_str());
        curl_easy_setopt(curl, CURLOPT_PROXYPASSWORD, proxyPassword_.c_str());
    }

    if (!networkInterface_.empty()) {
        curl_easy_setopt(curl, CURLOPT_INTERFACE, networkInterface_.c_str());
    }

    //debug
    if (GetLogLevelInner() >= LogLevel::LogInfo) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, debugCallback);
    }

    //Error Buffer
    char errbuf[CURL_ERROR_SIZE];
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
    errbuf[0] = 0;

    //progress Callback
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progressCallback);
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &transferState);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    //Send bytes/sec 
    if (sendRateLimiter_ != nullptr) {
        transferState.sendSpeed = sendRateLimiter_->Rate();
        auto speed = static_cast<curl_off_t>(transferState.sendSpeed);
        speed = speed * 1024;
        curl_easy_setopt(curl, CURLOPT_MAX_SEND_SPEED_LARGE, speed);
    }

    //Recv bytes/sec 
    if (recvRateLimiter_ != nullptr) {
        transferState.recvSpeed = recvRateLimiter_->Rate();
        auto speed = static_cast<curl_off_t>(transferState.recvSpeed);
        speed = speed * 1024;
        curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, speed);
    }

    CURLcode res = curl_easy_perform(curl);
    long response_code= 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    printf("[%s]response_code: %d\n", __func__, response_code);
    if (res == CURLE_OK) {
        response->setStatusCode(response_code);
    } else {
        response->setStatusCode(res + ERROR_CURL_BASE);
        switch (res) {
        case 23: //CURLE_WRITE_ERROR
        {
            std::string msg(curl_easy_strerror(res));
            if (response->Body() == nullptr) {
                msg.append(". Caused by content is null.");
            }
            else if (response->Body()->bad()) {
                msg.append(". Caused by content is in bad state(Read/writing error on i/o operation).");
            }
            else if (response->Body()->fail()) {
                msg.append(". Caused by content is in fail state(Logical error on i/o operation).");
            }
            response->setStatusMsg(msg);
        }
            break;
        default:
        {
            std::string msg(curl_easy_strerror(res));
            msg.append(".").append(errbuf);
            response->setStatusMsg(msg);
        }
            break;
        };
    }
   
    switch (request->method())
    {
    case Http::Method::Put:
    case Http::Method::Post:
        request->setCrc64Result(transferState.sendCrc64Value);
        break;
    default:
        request->setCrc64Result(transferState.recvCrc64Value);
        break;
    }
    request->setTransferedBytes(transferState.transferred);

    curlContainer_->Release(curl, (res != CURLE_OK));

    curl_slist_free_all(list);

    auto & body = response->Body();
    if (body != nullptr) {
        body->flush();
        if (res != CURLE_OK && transferState.recvBodyPos != static_cast<std::streampos>(-1)) {
            OSS_LOG(LogLevel::LogDebug, TAG, "request(%p) setResponseBody, tellp:%lld, recvBodyPos:%lld",
                request.get(), body->tellp(), transferState.recvBodyPos);
            body->clear();
            body->seekp(transferState.recvBodyPos);
        }
    }
    else {
        response->addBody(std::make_shared<std::stringstream>());
    }

    if (requestBodyPos != static_cast<std::streampos>(-1)) {
        request->Body()->clear();
        request->Body()->seekg(requestBodyPos);
    }

    OSS_LOG(LogLevel::LogDebug, TAG, "request(%p) leave makeRequest, CURLcode:%d, ResponseCode:%d", 
        request.get(), res, response_code);

    return response;
}
