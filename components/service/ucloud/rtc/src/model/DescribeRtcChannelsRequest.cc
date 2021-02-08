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

#include <alibabacloud/rtc/model/DescribeRtcChannelsRequest.h>

using AlibabaCloud::Rtc::Model::DescribeRtcChannelsRequest;

DescribeRtcChannelsRequest::DescribeRtcChannelsRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DescribeRtcChannels")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeRtcChannelsRequest::~DescribeRtcChannelsRequest()
{}

std::string DescribeRtcChannelsRequest::getStartTime()const
{
	return startTime_;
}

void DescribeRtcChannelsRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

int DescribeRtcChannelsRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeRtcChannelsRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeRtcChannelsRequest::getShowLog()const
{
	return showLog_;
}

void DescribeRtcChannelsRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string DescribeRtcChannelsRequest::getEndTime()const
{
	return endTime_;
}

void DescribeRtcChannelsRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeRtcChannelsRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeRtcChannelsRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

int DescribeRtcChannelsRequest::getPageNo()const
{
	return pageNo_;
}

void DescribeRtcChannelsRequest::setPageNo(int pageNo)
{
	pageNo_ = pageNo;
	setParameter("PageNo", std::to_string(pageNo));
}

std::string DescribeRtcChannelsRequest::getAppId()const
{
	return appId_;
}

void DescribeRtcChannelsRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string DescribeRtcChannelsRequest::getChannelId()const
{
	return channelId_;
}

void DescribeRtcChannelsRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

