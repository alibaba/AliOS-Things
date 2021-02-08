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
#include <alibabacloud/oss/Export.h>
#include <alibabacloud/oss/OssRequest.h>

namespace AlibabaCloud
{
namespace OSS
{

    class ALIBABACLOUD_OSS_EXPORT ListObjectVersionsRequest : public OssBucketRequest
    {
    public:
        ListObjectVersionsRequest(const std::string& bucket):
            OssBucketRequest(bucket),
            delimiterIsSet_(false),
            keyMarkerIsSet_(false),
            maxKeysIsSet_(false),
            prefixIsSet_(false),
            encodingTypeIsSet_(false),
            versionIdMarkerIsSet_(false)
        {
        }
        void setDelimiter(const std::string& delimiter) { delimiter_ = delimiter; delimiterIsSet_ = true; }
        void setKeyMarker(const std::string& marker) { keyMarker_ = marker; keyMarkerIsSet_ = true;}
        void setMaxKeys(int maxKeys) {maxKeys_ = maxKeys; maxKeysIsSet_ = true;} 
        void setPrefix(const std::string& prefix) { prefix_ = prefix; prefixIsSet_ = true; }
        void setEncodingType(const std::string& type) { encodingType_ = type; encodingTypeIsSet_ = true; }
        void setVersionIdMarker(const std::string& marker) { versionIdMarker_ = marker; versionIdMarkerIsSet_ = true; }

    protected:
        virtual ParameterCollection specialParameters() const 
        {
            ParameterCollection params;
            params["versions"] = "";
            if (delimiterIsSet_) params["delimiter"] = delimiter_;
            if (keyMarkerIsSet_) params["key-marker"] = keyMarker_;
            if (maxKeysIsSet_) params["max-keys"] = std::to_string(maxKeys_);
            if (prefixIsSet_) params["prefix"] = prefix_;
            if (encodingTypeIsSet_) params["encoding-type"] = encodingType_;
            if (versionIdMarkerIsSet_) params["version-id-marker"] = versionIdMarker_;
            return params;
        }
    private:
        std::string delimiter_;
        bool delimiterIsSet_;
        std::string keyMarker_;
        bool keyMarkerIsSet_;
        int maxKeys_;
        bool maxKeysIsSet_;
        std::string prefix_;
        bool prefixIsSet_;
        std::string encodingType_;
        bool encodingTypeIsSet_;
        std::string versionIdMarker_;
        bool versionIdMarkerIsSet_;
    };
} 
}
