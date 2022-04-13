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
    class ALIBABACLOUD_OSS_EXPORT PutLiveChannelRequest : public LiveChannelRequest
    {
    public:
        PutLiveChannelRequest(const std::string& bucket, const std::string& channelName, 
            const std::string& type);

        void setChannelType(const std::string& type);
        void setStatus(LiveChannelStatus status);
        void setDescripition(const std::string& description);
        void setPlayListName(const std::string& playListName);
        void setRoleName(const std::string& roleName);
        void setDestBucket(const std::string& destBucket);
        void setNotifyTopic(const std::string& notifyTopic);
        void setFragDuration(uint64_t fragDuration);
        void setFragCount(uint64_t fragCount);
        void setInterval(uint64_t interval);
    protected:
        virtual ParameterCollection specialParameters() const;
        virtual std::string payload() const;
        virtual int validate() const;
    private:
        std::string channelType_;
        std::string description_;
        std::string playListName_;
        std::string roleName_;
        std::string destBucket_;
        std::string notifyTopic_;
        LiveChannelStatus status_;
        uint64_t fragDuration_;
        uint64_t fragCount_;
        uint64_t interval_;
        bool snapshot_;
    };
} 
}
