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


#include <alibabacloud/oss/model/PutLiveChannelStatusRequest.h>
#include <sstream>
#include "../utils/Utils.h"
#include "ModelError.h"

using namespace AlibabaCloud::OSS;

PutLiveChannelStatusRequest::PutLiveChannelStatusRequest(const std::string& bucket, const std::string& channelName)
    :LiveChannelRequest(bucket, channelName),
    status_(LiveChannelStatus::EnabledStatus)
{

}

PutLiveChannelStatusRequest::PutLiveChannelStatusRequest(const std::string& bucket, 
    const std::string& channelName, LiveChannelStatus status)
    :LiveChannelRequest(bucket, channelName),
    status_(status)
{

}

ParameterCollection PutLiveChannelStatusRequest::specialParameters() const
{
    ParameterCollection collection;
    collection["live"] = "";
    collection["status"] = ToLiveChannelStatusName(status_);
    return collection;
}

void PutLiveChannelStatusRequest::setStatus(LiveChannelStatus status)
{
    status_ = status;
}

int PutLiveChannelStatusRequest::validate() const
{
    int ret = LiveChannelRequest::validate();
    
    if (ret)
    {
        return ret;
    }

    if(status_ != LiveChannelStatus::EnabledStatus &&
        status_ != LiveChannelStatus::DisabledStatus)
    {
        return ARG_ERROR_LIVECHANNEL_BAD_STATUS_PARAM;
    }

    return 0;
}
