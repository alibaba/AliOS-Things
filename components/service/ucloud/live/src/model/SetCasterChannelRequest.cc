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

#include <alibabacloud/live/model/SetCasterChannelRequest.h>

using AlibabaCloud::Live::Model::SetCasterChannelRequest;

SetCasterChannelRequest::SetCasterChannelRequest() :
	RpcServiceRequest("live", "2016-11-01", "SetCasterChannel")
{
	setMethod(HttpRequest::Method::Post);
}

SetCasterChannelRequest::~SetCasterChannelRequest()
{}

int SetCasterChannelRequest::getSeekOffset()const
{
	return seekOffset_;
}

void SetCasterChannelRequest::setSeekOffset(int seekOffset)
{
	seekOffset_ = seekOffset;
	setParameter("SeekOffset", std::to_string(seekOffset));
}

int SetCasterChannelRequest::getPlayStatus()const
{
	return playStatus_;
}

void SetCasterChannelRequest::setPlayStatus(int playStatus)
{
	playStatus_ = playStatus;
	setParameter("PlayStatus", std::to_string(playStatus));
}

std::string SetCasterChannelRequest::getResourceId()const
{
	return resourceId_;
}

void SetCasterChannelRequest::setResourceId(const std::string& resourceId)
{
	resourceId_ = resourceId;
	setParameter("ResourceId", resourceId);
}

std::string SetCasterChannelRequest::getCasterId()const
{
	return casterId_;
}

void SetCasterChannelRequest::setCasterId(const std::string& casterId)
{
	casterId_ = casterId;
	setParameter("CasterId", casterId);
}

long SetCasterChannelRequest::getOwnerId()const
{
	return ownerId_;
}

void SetCasterChannelRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

int SetCasterChannelRequest::getReloadFlag()const
{
	return reloadFlag_;
}

void SetCasterChannelRequest::setReloadFlag(int reloadFlag)
{
	reloadFlag_ = reloadFlag;
	setParameter("ReloadFlag", std::to_string(reloadFlag));
}

std::string SetCasterChannelRequest::getChannelId()const
{
	return channelId_;
}

void SetCasterChannelRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

