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

#include <alibabacloud/rtc/model/UpdateChannelRequest.h>

using AlibabaCloud::Rtc::Model::UpdateChannelRequest;

UpdateChannelRequest::UpdateChannelRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "UpdateChannel")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateChannelRequest::~UpdateChannelRequest()
{}

std::string UpdateChannelRequest::getShowLog()const
{
	return showLog_;
}

void UpdateChannelRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long UpdateChannelRequest::getOwnerId()const
{
	return ownerId_;
}

void UpdateChannelRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string UpdateChannelRequest::getNonce()const
{
	return nonce_;
}

void UpdateChannelRequest::setNonce(const std::string& nonce)
{
	nonce_ = nonce;
	setParameter("Nonce", nonce);
}

std::string UpdateChannelRequest::getAppId()const
{
	return appId_;
}

void UpdateChannelRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string UpdateChannelRequest::getChannelId()const
{
	return channelId_;
}

void UpdateChannelRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

