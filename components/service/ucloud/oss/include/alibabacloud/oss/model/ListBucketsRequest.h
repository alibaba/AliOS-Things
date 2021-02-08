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
#include <alibabacloud/oss/model/Tagging.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT ListBucketsRequest: public OssRequest
    {
    public:
        ListBucketsRequest();
        ListBucketsRequest(const std::string& prefix, const std::string& marker, int maxKeys = 100);

        void setPrefix(const std::string& prefix) {prefix_ = prefix; prefixIsSet_ = true;}    
        void setMarker(const std::string& marker) {marker_ = marker; markerIsSet_ = true;}    
        void setMaxKeys(int maxKeys) {maxKeys_ = maxKeys; maxKeysIsSet_ = true;} 
        void setTag(const Tag& tag) { tag_ = tag; tagIsSet = true; }
    protected:
        virtual ParameterCollection specialParameters() const;

    private:
        std::string prefix_;
        bool prefixIsSet_;
        std::string marker_;
        bool markerIsSet_;
        int maxKeys_;
        bool maxKeysIsSet_;
        Tag tag_;
        bool tagIsSet;
    };
} 
}
