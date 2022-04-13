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
#include <alibabacloud/oss/Export.h>
#include <alibabacloud/oss/OssResult.h>
#include <alibabacloud/oss/model/Owner.h>
#include <alibabacloud/oss/ServiceRequest.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT GetLiveChannelInfoResult : public OssResult
    {
    public:
        GetLiveChannelInfoResult();
        GetLiveChannelInfoResult(const std::string& data);
        GetLiveChannelInfoResult(const std::shared_ptr<std::iostream>& data);
        GetLiveChannelInfoResult& operator=(const std::string& data);

        const std::string& Description() const;
        LiveChannelStatus Status() const;

        const std::string& Type() const;
        uint64_t FragDuration() const;
        uint64_t FragCount() const;
        const std::string& PlaylistName() const;
    private:
        std::string channelType_;
        LiveChannelStatus status_;
        std::string description_;
        std::string playListName_;
        uint64_t fragDuration_;
        uint64_t fragCount_;
    };
} 
}

