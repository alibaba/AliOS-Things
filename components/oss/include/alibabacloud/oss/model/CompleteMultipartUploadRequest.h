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
#include <alibabacloud/oss/model/Part.h>
#include <alibabacloud/oss/model/ObjectMetaData.h>
#include <sstream>
#include <iostream>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT CompleteMultipartUploadRequest: public OssObjectRequest
    {
    public:
        CompleteMultipartUploadRequest(const std::string& bucket, const std::string& key);
        CompleteMultipartUploadRequest(const std::string& bucket, const std::string& key,
            const PartList& partList);
        CompleteMultipartUploadRequest(const std::string& bucket, const std::string& key, 
            const PartList& partList, 
            const std::string& uploadId);
        void setEncodingType(const std::string& encodingType);
        void setPartList(const AlibabaCloud::OSS::PartList& partList);
        void setUploadId(const std::string& uploadId);
        void setAcl(CannedAccessControlList acl);
        void setCallback(const std::string& callback, const std::string& callbackVar = "");
        ObjectMetaData& MetaData();
    protected:
        virtual std::string payload() const;
        virtual ParameterCollection specialParameters() const;
        virtual HeaderCollection specialHeaders() const;
        virtual int validate() const;
    private:
        AlibabaCloud::OSS::PartList partList_;
        std::string uploadId_;
        std::string encodingType_;
        bool encodingTypeIsSet_;
        ObjectMetaData metaData_;
    };
} 
}