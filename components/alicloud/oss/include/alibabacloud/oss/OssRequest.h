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
#include <alibabacloud/oss/ServiceRequest.h>

namespace AlibabaCloud
{
namespace OSS
{
    class OssClientImpl;
    class OssEncryptionClient;
    class  ALIBABACLOUD_OSS_EXPORT OssRequest : public ServiceRequest
    {
    public:
        OssRequest();
        virtual ~ OssRequest() = default;
        virtual HeaderCollection Headers() const;
        virtual ParameterCollection Parameters() const;
        virtual std::shared_ptr<std::iostream> Body() const;
    protected:
        OssRequest(const std::string& bucket, const std::string& key);
        friend class OssClientImpl;
        friend class OssEncryptionClient;

        virtual int validate() const;
        const char *validateMessage(int code) const;
        
        virtual std::string payload() const;
        virtual HeaderCollection specialHeaders() const;
        virtual ParameterCollection specialParameters() const;
        
        const std::string& bucket() const;
        const std::string& key()  const;
    protected:
        std::string bucket_;
        std::string key_;
    };

    class ALIBABACLOUD_OSS_EXPORT OssBucketRequest : public OssRequest
    {
    public:
        OssBucketRequest(const std::string& bucket):
            OssRequest(bucket, "")
        {}
        void setBucket(const std::string& bucket);
        const std::string& Bucket() const;
    protected:
        virtual int validate() const;
    };

    class ALIBABACLOUD_OSS_EXPORT OssObjectRequest : public OssRequest
    {
    public:
        OssObjectRequest(const std::string& bucket, const std::string& key) :
            OssRequest(bucket, key),
            requestPayer_(AlibabaCloud::OSS::RequestPayer::NotSet),
            versionId_()
        {}
        void setBucket(const std::string& bucket);
        const std::string& Bucket() const;

        void setKey(const std::string& key);
        const std::string& Key() const;

        void setRequestPayer(AlibabaCloud::OSS::RequestPayer value);
        AlibabaCloud::OSS::RequestPayer RequestPayer() const;
        void setVersionId(const std::string& versionId);
        const std::string& VersionId() const;
    protected:
        virtual int validate() const;
        virtual HeaderCollection specialHeaders() const;
        virtual ParameterCollection specialParameters() const;
        AlibabaCloud::OSS::RequestPayer requestPayer_;
        std::string versionId_;
    };

    class ALIBABACLOUD_OSS_EXPORT OssResumableBaseRequest : public OssRequest
    {
    public:
        OssResumableBaseRequest(const std::string& bucket, const std::string& key,
            const std::string& checkpointDir, const uint64_t partSize, const uint32_t threadNum) :
            OssRequest(bucket, key), 
            partSize_(partSize),
            checkpointDir_(checkpointDir),
            requestPayer_(AlibabaCloud::OSS::RequestPayer::NotSet),
            trafficLimit_(0),
            versionId_()
        {
            threadNum_ = threadNum == 0 ? 1 : threadNum;
        }

        OssResumableBaseRequest(const std::string& bucket, const std::string& key,
            const std::wstring& checkpointDir, const uint64_t partSize, const uint32_t threadNum) :
            OssRequest(bucket, key),
            partSize_(partSize),
            checkpointDirW_(checkpointDir),
            requestPayer_(AlibabaCloud::OSS::RequestPayer::NotSet),
            trafficLimit_(0),
            versionId_()
        {
            threadNum_ = threadNum == 0 ? 1 : threadNum;
        }

        void setBucket(const std::string& bucket);
        const std::string& Bucket() const;

        void setKey(const std::string& key);
        const std::string& Key() const;
        
        void setPartSize(uint64_t partSize);
        uint64_t PartSize() const;

        void setObjectSize(uint64_t objectSize);
        uint64_t ObjectSize() const;
        
        void setThreadNum(uint32_t threadNum);
        uint32_t ThreadNum() const;

        void setCheckpointDir(const std::string& checkpointDir);
        const std::string& CheckpointDir() const;

        void setCheckpointDir(const std::wstring& checkpointDir);
        const std::wstring& CheckpointDirW() const;

        bool hasCheckpointDir() const;

        void setObjectMtime(const std::string& mtime);
        const std::string& ObjectMtime() const;

        void setRequestPayer(RequestPayer value);
        AlibabaCloud::OSS::RequestPayer RequestPayer() const;
		
        void setTrafficLimit(uint64_t value);
        uint64_t TrafficLimit() const;

        void setVersionId(const std::string& versionId);
        const std::string& VersionId() const;

    protected:
        friend class OssClientImpl;
        friend class OssEncryptionClient;
        virtual int validate() const;
        const char *validateMessage(int code) const;

    protected:
        uint64_t partSize_;
        uint64_t objectSize_;
        uint32_t threadNum_;
        std::string checkpointDir_;
        std::wstring checkpointDirW_;
        std::string mtime_;
        AlibabaCloud::OSS::RequestPayer requestPayer_;
        uint64_t trafficLimit_;
        std::string versionId_;
    };

    class ALIBABACLOUD_OSS_EXPORT LiveChannelRequest : public OssRequest
    {
    public:
        LiveChannelRequest(const std::string &bucket, const std::string &channelName) :
            OssRequest(bucket, channelName),
            channelName_(channelName)
        {}
        void setBucket(const std::string &bucket);
        const std::string& Bucket() const;

        void setChannelName(const std::string &channelName);
        const std::string& ChannelName() const;
    protected:
        virtual int validate() const;
    protected:
        std::string channelName_;
    };
}
}
