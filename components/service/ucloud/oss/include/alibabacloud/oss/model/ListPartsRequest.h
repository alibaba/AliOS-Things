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
#include <sstream>
#include <iostream>
#include <alibabacloud/oss/Export.h>
#include <alibabacloud/oss/OssRequest.h>
#include <alibabacloud/oss/Types.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT ListPartsRequest: public OssObjectRequest
    {
    public:
        ListPartsRequest(const std::string& bucket, const std::string& key);
        ListPartsRequest(const std::string& bucket, const std::string& key,
            const std::string& uploadId);
        void setUploadId(const std::string& uploadId);
        void setMaxParts(uint32_t maxParts);
        void setPartNumberMarker(uint32_t partNumberMarker);
        void setEncodingType(const std::string& encodingType);
    protected:
        virtual ParameterCollection specialParameters() const;
    private:
        std::string uploadId_;
        uint32_t maxParts_;
        bool maxPartsIsSet_;
        uint32_t partNumberMarker_;
        bool partNumberMarkerIsSet_;
        std::string encodingType_;
        bool encodingTypeIsSet_;
    };
} 
}