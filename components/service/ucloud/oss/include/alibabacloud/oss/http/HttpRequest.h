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

#pragma once

#include <string>
#include <alibabacloud/oss/Types.h>
#include <alibabacloud/oss/ServiceRequest.h>
#include <alibabacloud/oss/http/HttpMessage.h>
#include <alibabacloud/oss/http/Url.h>

namespace AlibabaCloud
{
namespace OSS
{

    class ALIBABACLOUD_OSS_EXPORT HttpRequest : public HttpMessage
    {
        public:
            HttpRequest(Http::Method method = Http::Method::Get);
            ~HttpRequest();

            Http::Method method() const;
            Url url() const;
            void setMethod(Http::Method method);
            void setUrl(const Url &url);
            
            const IOStreamFactory& ResponseStreamFactory() const { return responseStreamFactory_; }
            void setResponseStreamFactory(const IOStreamFactory& factory) { responseStreamFactory_ = factory; }

            const AlibabaCloud::OSS::TransferProgress & TransferProgress() const {  return transferProgress_; }
            void setTransferProgress(const AlibabaCloud::OSS::TransferProgress &arg) { transferProgress_ = arg;}

            void setCheckCrc64(bool enable) { hasCheckCrc64_ = enable; }
            bool hasCheckCrc64() const { return hasCheckCrc64_; }
            void setCrc64Result(uint64_t crc) { crc64Result_ = crc; }
            uint64_t Crc64Result() const { return crc64Result_; }

            void setTransferedBytes(int64_t value) { transferedBytes_ = value; }
            uint64_t TransferedBytes() const { return transferedBytes_;}

        private:
            Http::Method method_;
            Url url_;
            IOStreamFactory responseStreamFactory_;
            AlibabaCloud::OSS::TransferProgress transferProgress_;
            bool hasCheckCrc64_;
            uint64_t crc64Result_;
            int64_t transferedBytes_;
    };
}
}
