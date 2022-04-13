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
#include <vector>
#include <memory>
#include <iostream>
#include <alibabacloud/oss/OssResult.h>

namespace AlibabaCloud
{
namespace OSS
{
    struct LiveChannelInfo
    {
        std::string name;
        std::string description;
        std::string status;
        std::string lastModified;
        std::string publishUrl;
        std::string playUrl;
    };

    using LiveChannelListInfo = std::vector<LiveChannelInfo>;

    class ALIBABACLOUD_OSS_EXPORT ListLiveChannelResult: public OssResult
    {
    public:
        ListLiveChannelResult();
        ListLiveChannelResult(const std::string& data);
        ListLiveChannelResult(const std::shared_ptr<std::iostream>& data);
        ListLiveChannelResult& operator=(const std::string& data);

        const std::string& Prefix() const;
        const std::string& Marker() const;
        const std::string& NextMarker() const;
        bool IsTruncated() const;
        const LiveChannelListInfo& LiveChannelList() const;
        uint32_t MaxKeys() const;
    private:
        std::string prefix_;
        std::string marker_;
        std::string nextMarker_;
        LiveChannelListInfo liveChannelList_;
        uint32_t maxKeys_;
        bool     isTruncated_;
        
    };
} 
}
