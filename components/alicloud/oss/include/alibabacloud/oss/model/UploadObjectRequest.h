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
#include <alibabacloud/oss/Types.h>
#include <alibabacloud/oss/model/ObjectMetaData.h>
#include <alibabacloud/oss/http/HttpType.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT UploadObjectRequest : public OssResumableBaseRequest
    {
    public:
        UploadObjectRequest(const std::string& bucket, const std::string& key, 
            const std::string& filePath, const std::string& checkpointDir,
            const uint64_t partSize, const uint32_t threadNum);
        UploadObjectRequest(const std::string& bucket, const std::string& key, 
            const std::string& filePath, const std::string &checkpointDir,
            const uint64_t partSize, const uint32_t threadNum, const ObjectMetaData& meta);
        UploadObjectRequest(const std::string& bucket, const std::string& key, 
            const std::string& filePath, const std::string& checkpointDir,
            const ObjectMetaData& meta);
        UploadObjectRequest(const std::string& bucket, const std::string& key, 
            const std::string& filePath, const std::string& checkpointDir);
        UploadObjectRequest(const std::string& bucket, const std::string& key, 
            const std::string& filePath);

        std::shared_ptr<std::iostream> Content(){ return content_; }
        const std::string& EncodingType() const{return encodingType_;}
        const std::string& FilePath() const{return filePath_;}
        const ObjectMetaData& MetaData() const { return metaData_; }
        ObjectMetaData& MetaData() { return metaData_; }

        UploadObjectRequest(const std::string& bucket, const std::string& key,
            const std::wstring& filePath, const std::wstring& checkpointDir,
            const uint64_t partSize, const uint32_t threadNum);
        UploadObjectRequest(const std::string& bucket, const std::string& key,
            const std::wstring& filePath, const std::wstring &checkpointDir,
            const uint64_t partSize, const uint32_t threadNum, const ObjectMetaData& meta);
        UploadObjectRequest(const std::string& bucket, const std::string& key,
            const std::wstring& filePath, const std::wstring& checkpointDir,
            const ObjectMetaData& meta);
        UploadObjectRequest(const std::string& bucket, const std::string& key,
            const std::wstring& filePath, const std::wstring& checkpointDir);
        UploadObjectRequest(const std::string& bucket, const std::string& key,
            const std::wstring& filePath);
        const std::wstring& FilePathW() const { return filePathW_; }

        void setCacheControl(const std::string& value){metaData_.addHeader(Http::CACHE_CONTROL, value);}
        void setContentDisposition(const std::string& value){metaData_.addHeader(Http::CONTENT_DISPOSITION, value);}
        void setContentEncoding(const std::string& value){metaData_.addHeader(Http::CONTENT_ENCODING, value);}
        void setExpires(const std::string& value){metaData_.addHeader(Http::EXPIRES, value);}
        void setAcl(CannedAccessControlList& acl);
        void setCallback(const std::string& callback, const std::string& callbackVar = "");
        void setEncodingType(const std::string& type) {encodingType_ = type; }       
        void setTagging(const std::string& value);

    protected:
        virtual int validate() const;
    private:
        std::string filePath_;
        std::shared_ptr<std::iostream> content_;
        ObjectMetaData metaData_;
        std::string encodingType_;
        std::wstring filePathW_;
        bool isFileExist_;
    };
} 
}