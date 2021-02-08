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

#include <alibabacloud/rtc/model/DescribeChannelUsersRequest.h>

using AlibabaCloud::Rtc::Model::DescribeChannelUsersRequest;

DescribeChannelUsersRequest::DescribeChannelUsersRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DescribeChannelUsers")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeChannelUsersRequest::~DescribeChannelUsersRequest()
{}

std::string DescribeChannelUsersRequest::getShowLog()const
{
	return showLog_;
}

void DescribeChannelUsersRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long DescribeChannelUsersRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeChannelUsersRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeChannelUsersRequest::getAppId()const
{
	return appId_;
}

void DescribeChannelUsersRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string DescribeChannelUsersRequest::getChannelId()const
{
	return channelId_;
}

void DescribeChannelUsersRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

