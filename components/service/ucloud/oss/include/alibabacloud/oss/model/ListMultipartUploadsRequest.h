/*
 * Copyright 2009-2018 Alibaba Cloud All rights reserved.
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
#include <alibabacloud/oss/Export.h>
#include <alibabacloud/oss/OssRequest.h>
#include <sstream>
#include <iostream>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT ListMultipartUploadsRequest: public OssBucketRequest
    {
    public:
        ListMultipartUploadsRequest(const std::string& bucket);
        void setDelimiter(const std::string& delimiter);
        void setMaxUploads(uint32_t maxUploads);
        void setKeyMarker(const std::string& keyMarker);
        void setPrefix(const std::string& prefix);
        void setUploadIdMarker(const std::string& uploadIdMarker);
        void setEncodingType(const std::string& encodingType);
        void setRequestPayer(RequestPayer value);
    protected:
        virtual ParameterCollection specialParameters() const;
        virtual HeaderCollection specialHeaders() const;
    private:
        std::string delimiter_;
        bool delimiterIsSet_;
        std::string keyMarker_;
        bool keyMarkerIsSet_;
        std::string prefix_;
        bool prefixIsSet_;
        std::string uploadIdMarker_;
        bool uploadIdMarkerIsSet_;
        std::string encodingType_;
        bool encodingTypeIsSet_;
        int maxUploads_;
        bool maxUploadsIsSet_;
        RequestPayer requestPayer_;
    };
} 
}