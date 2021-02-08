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
#include <vector>
#include <alibabacloud/oss/Export.h>
#include <alibabacloud/oss/OssResult.h>
#include <alibabacloud/oss/model/Owner.h>
#include <alibabacloud/oss/ServiceRequest.h>

using std::vector;

namespace AlibabaCloud
{
namespace OSS
{
    struct LiveRecord
    {
        std::string startTime;
        std::string endTime;
        std::string remoteAddr;
    };

    using LiveRecordVec = vector<LiveRecord>;

    class ALIBABACLOUD_OSS_EXPORT GetLiveChannelHistoryResult : public OssResult
    {
    public:
        GetLiveChannelHistoryResult();
        GetLiveChannelHistoryResult(const std::string& data);
        GetLiveChannelHistoryResult(const std::shared_ptr<std::iostream>& data);
        GetLiveChannelHistoryResult& operator=(const std::string& data);

        const LiveRecordVec& LiveRecordList() const;
    private:
        LiveRecordVec recordList_;
    };
} 
}

