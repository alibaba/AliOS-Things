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


#include <alibabacloud/oss/model/PutLiveChannelRequest.h>
#include <sstream>
#include "utils/Utils.h"
#include "ModelError.h"
#include "Const.h"


using namespace AlibabaCloud::OSS;

PutLiveChannelRequest::PutLiveChannelRequest(const std::string& bucket, 
    const std::string& channelName, const std::string& type)
    :LiveChannelRequest(bucket, channelName), 
    channelType_(type),
    playListName_("playlist.m3u8"),
    status_(LiveChannelStatus::EnabledStatus),
    fragDuration_(5),
    fragCount_(3),
    snapshot_(false)
{
}

void PutLiveChannelRequest::setChannelType(const std::string& channelType)
{
    channelType_ = channelType;
}

void PutLiveChannelRequest::setDescripition(const std::string& description)
{
    description_ = description;
}

void PutLiveChannelRequest::setDestBucket(const std::string& destBucket)
{
    destBucket_ = destBucket;
    snapshot_ = true;
}

void PutLiveChannelRequest::setFragCount(uint64_t fragCount)
{
    fragCount_ = fragCount;
}

void PutLiveChannelRequest::setFragDuration(uint64_t fragDuration)
{
    fragDuration_ = fragDuration;
}

void PutLiveChannelRequest::setStatus(LiveChannelStatus status)
{
    status_ = status;
}

void PutLiveChannelRequest::setInterval(uint64_t interval)
{
    interval_ = interval;
    snapshot_ = true;
}

void PutLiveChannelRequest::setNotifyTopic(const std::string& notifyTopic)
{
    notifyTopic_ = notifyTopic;
    snapshot_ = true;
}

void PutLiveChannelRequest::setRoleName(const std::string& roleName)
{
    roleName_ = roleName;
    snapshot_ = true;
}

void PutLiveChannelRequest::setPlayListName(const std::string& playListName)
{
    playListName_ = playListName;
    std::size_t pos = playListName_.find_last_of('.');
    if(pos != std::string::npos)
    {
        std::string suffixStr = playListName_.substr(pos);
        if(ToLower(suffixStr.c_str()) == ".m3u8")
        {
            std::string prefixStr;
            if(pos > 0)
            {
                prefixStr = playListName_.substr(0, pos);
                playListName_ = prefixStr + ".m3u8";
            }
        }
    }
}

int PutLiveChannelRequest::validate() const
{
    int ret = LiveChannelRequest::validate();

    if(ret)
    {
        return ret;
    }

    if(!description_.empty() && 
        description_.size() > MaxLiveChannelDescriptionLength)
    {
        return ARG_ERROR_LIVECHANNEL_BAD_DESCRIPTION_PARAM;
    }

    if(status_ != LiveChannelStatus::EnabledStatus && 
        status_ != LiveChannelStatus::DisabledStatus)
    {
        return ARG_ERROR_LIVECHANNEL_BAD_STATUS_PARAM;
    }

    if(channelType_ != "HLS")
    {
        return ARG_ERROR_LIVECHANNEL_BAD_CHANNEL_TYPE_PARAM;
    }

    if(fragDuration_ < MinLiveChannelFragDuration || 
        fragDuration_ > MaxLiveChannelFragDuration)
    {
        return ARG_ERROR_LIVECHANNEL_BAD_FRAGDURATION_PARAM;
    }

    if(fragCount_ < MinLiveChannelFragCount || 
        fragCount_ > MaxLiveChannelFragCount)
    {
        return ARG_ERROR_LIVECHANNEL_BAD_FRAGCOUNT_PARAM;
    }

    if(!IsValidPlayListName(playListName_))
    {
        return ARG_ERROR_LIVECHANNEL_BAD_PALYLIST_PARAM;
    }

    if(snapshot_)
    {
        if(roleName_.empty() || notifyTopic_.empty() || 
            destBucket_.empty() || !IsValidBucketName(bucket_) || 
            interval_ < MinLiveChannelInterval ||
            interval_ > MaxLiveChannelInterval)
        {
            return ARG_ERROR_LIVECHANNEL_BAD_SNAPSHOT_PARAM;
        }
    }
    return 0;
}

std::string PutLiveChannelRequest::payload() const
{
    std::stringstream ss;
    ss << "<LiveChannelConfiguration>" << std::endl;
    ss << "  <Description>" << description_ << "</Description>" << std::endl;
    ss << "  <Status>" << ToLiveChannelStatusName(status_) << "</Status>" << std::endl;
    ss << "  <Target>" << std::endl;
    ss << "    <Type>" << channelType_ << "</Type>" << std::endl;
    ss << "    <FragDuration>" << std::to_string(fragDuration_) << "</FragDuration>" << std::endl;
    ss << "    <FragCount>" << std::to_string(fragCount_) << "</FragCount>" << std::endl;
    ss << "    <PlaylistName>" << playListName_ << "</PlaylistName>" << std::endl;
    ss << "  </Target>" << std::endl;
    if(snapshot_)
    {
        ss << "  <Snapshot>" << std::endl;
        ss << "    <RoleName>" << roleName_ << "</RoleName>" << std::endl;
        ss << "    <DestBucket>" << destBucket_ << "</DestBucket>" << std::endl;
        ss << "    <NotifyTopic>" << notifyTopic_ << "</NotifyTopic>" << std::endl;
        ss << "    <Interval>" << std::to_string(interval_) << "</Interval>" << std::endl;
        ss << "  </Snapshot>" << std::endl;
    }
    ss << "</LiveChannelConfiguration>" << std::endl;
    return ss.str();
}

ParameterCollection PutLiveChannelRequest::specialParameters() const
{
    ParameterCollection collection;
    collection["live"] = "";
    return collection;
}
