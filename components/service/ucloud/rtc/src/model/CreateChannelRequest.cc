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

#include <alibabacloud/rtc/model/CreateChannelRequest.h>

using AlibabaCloud::Rtc::Model::CreateChannelRequest;

CreateChannelRequest::CreateChannelRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "CreateChannel")
{
	setMethod(HttpRequest::Method::Post);
}

CreateChannelRequest::~CreateChannelRequest()
{}

std::string CreateChannelRequest::getShowLog()const
{
	return showLog_;
}

void CreateChannelRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long CreateChannelRequest::getOwnerId()const
{
	return ownerId_;
}

void CreateChannelRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string CreateChannelRequest::getAppId()const
{
	return appId_;
}

void CreateChannelRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string CreateChannelRequest::getChannelId()const
{
	return channelId_;
}

void CreateChannelRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

