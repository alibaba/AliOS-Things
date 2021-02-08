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
#include <alibabacloud/oss/OssResult.h>
#include <memory>
#include <iostream>
#include <list>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT DeletedObject
    {
    public:
        DeletedObject():deleteMarker_(false) {}
        const std::string& Key() const { return key_; }
        const std::string& VersionId() const { return versionId_; }
        const std::string& DeleteMarkerVersionId() const { return deleteMarkerVersionId_; }
        bool DeleteMarker() const { return deleteMarker_; }
        void setKey(const std::string& value) { key_ = value; }
        void setVersionId(const std::string& value) { versionId_ = value; }
        void setDeleteMarkerVersionId(const std::string& value) { deleteMarkerVersionId_ = value; }
        void setDeleteMarker(bool value) { deleteMarker_ = value; }
    private:
        std::string key_;
        std::string versionId_;
        std::string deleteMarkerVersionId_;
        bool deleteMarker_;
    };
    using DeletedObjectList = std::vector<DeletedObject>;
    
    class ALIBABACLOUD_OSS_EXPORT DeleteObjectVersionsResult : public OssResult
    {
    public:
        DeleteObjectVersionsResult();
        DeleteObjectVersionsResult(const std::string& data);
        DeleteObjectVersionsResult(const std::shared_ptr<std::iostream>& data);
        DeleteObjectVersionsResult& operator=(const std::string& data);
        bool Quiet() const;
        const DeletedObjectList& DeletedObjects() const;
    private:
        bool quiet_;
        DeletedObjectList deletedObjects_;
    };
} 
}
