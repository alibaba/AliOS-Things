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


#include <alibabacloud/oss/model/GetVodPlaylistRequest.h>
#include <sstream>
#include "../utils/Utils.h"
#include "ModelError.h"
#include "Const.h"


using namespace AlibabaCloud::OSS;

GetVodPlaylistRequest::GetVodPlaylistRequest(const std::string& bucket, 
    const std::string& channelName)
    :GetVodPlaylistRequest(bucket, channelName, 0, 0)
{
}

GetVodPlaylistRequest::GetVodPlaylistRequest(const std::string& bucket, 
    const std::string& channelName, uint64_t startTime, 
    uint64_t endTime)
    :LiveChannelRequest(bucket, channelName), 
    startTime_(startTime),
    endTime_(endTime)
{
}

void GetVodPlaylistRequest::setStartTime(uint64_t startTime)
{
    startTime_ = startTime;
}

void GetVodPlaylistRequest::setEndTime(uint64_t endTime)
{
    endTime_ = endTime;
}

int GetVodPlaylistRequest::validate() const
{
    int ret = LiveChannelRequest::validate();

    if(ret)
    {
        return ret;
    }
    
    if(startTime_ == 0 || endTime_ == 0 || 
        endTime_ < startTime_ || 
        endTime_ > (startTime_ + SecondsOfDay))
    {
        return ARG_ERROR_LIVECHANNEL_BAD_TIME_PARAM;
    }

    return 0;
}

ParameterCollection GetVodPlaylistRequest::specialParameters() const
{
    ParameterCollection collection;
    collection["startTime"] = std::to_string(startTime_);
    collection["endTime"] = std::to_string(endTime_);
    collection["vod"] = "";
    return collection;
}
