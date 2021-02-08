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

#include <alibabacloud/rtc/model/DescribeRtcUserListRequest.h>

using AlibabaCloud::Rtc::Model::DescribeRtcUserListRequest;

DescribeRtcUserListRequest::DescribeRtcUserListRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DescribeRtcUserList")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeRtcUserListRequest::~DescribeRtcUserListRequest()
{}

std::string DescribeRtcUserListRequest::getStartTime()const
{
	return startTime_;
}

void DescribeRtcUserListRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeRtcUserListRequest::getShowLog()const
{
	return showLog_;
}

void DescribeRtcUserListRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string DescribeRtcUserListRequest::getSubUser()const
{
	return subUser_;
}

void DescribeRtcUserListRequest::setSubUser(const std::string& subUser)
{
	subUser_ = subUser;
	setParameter("SubUser", subUser);
}

std::string DescribeRtcUserListRequest::getEndTime()const
{
	return endTime_;
}

void DescribeRtcUserListRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeRtcUserListRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeRtcUserListRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeRtcUserListRequest::getPubUser()const
{
	return pubUser_;
}

void DescribeRtcUserListRequest::setPubUser(const std::string& pubUser)
{
	pubUser_ = pubUser;
	setParameter("PubUser", pubUser);
}

std::string DescribeRtcUserListRequest::getAppId()const
{
	return appId_;
}

void DescribeRtcUserListRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string DescribeRtcUserListRequest::getChannelId()const
{
	return channelId_;
}

void DescribeRtcUserListRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

