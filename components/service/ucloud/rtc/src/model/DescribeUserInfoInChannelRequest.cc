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

#include <alibabacloud/rtc/model/DescribeUserInfoInChannelRequest.h>

using AlibabaCloud::Rtc::Model::DescribeUserInfoInChannelRequest;

DescribeUserInfoInChannelRequest::DescribeUserInfoInChannelRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DescribeUserInfoInChannel")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeUserInfoInChannelRequest::~DescribeUserInfoInChannelRequest()
{}

std::string DescribeUserInfoInChannelRequest::getUserId()const
{
	return userId_;
}

void DescribeUserInfoInChannelRequest::setUserId(const std::string& userId)
{
	userId_ = userId;
	setParameter("UserId", userId);
}

std::string DescribeUserInfoInChannelRequest::getShowLog()const
{
	return showLog_;
}

void DescribeUserInfoInChannelRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long DescribeUserInfoInChannelRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeUserInfoInChannelRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeUserInfoInChannelRequest::getAppId()const
{
	return appId_;
}

void DescribeUserInfoInChannelRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string DescribeUserInfoInChannelRequest::getChannelId()const
{
	return channelId_;
}

void DescribeUserInfoInChannelRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

