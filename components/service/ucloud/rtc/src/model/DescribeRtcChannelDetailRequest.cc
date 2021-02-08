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

#include <alibabacloud/rtc/model/DescribeRtcChannelDetailRequest.h>

using AlibabaCloud::Rtc::Model::DescribeRtcChannelDetailRequest;

DescribeRtcChannelDetailRequest::DescribeRtcChannelDetailRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DescribeRtcChannelDetail")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeRtcChannelDetailRequest::~DescribeRtcChannelDetailRequest()
{}

std::string DescribeRtcChannelDetailRequest::getStartTime()const
{
	return startTime_;
}

void DescribeRtcChannelDetailRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

int DescribeRtcChannelDetailRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeRtcChannelDetailRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeRtcChannelDetailRequest::getShowLog()const
{
	return showLog_;
}

void DescribeRtcChannelDetailRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string DescribeRtcChannelDetailRequest::getEndTime()const
{
	return endTime_;
}

void DescribeRtcChannelDetailRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeRtcChannelDetailRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeRtcChannelDetailRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

int DescribeRtcChannelDetailRequest::getPageNo()const
{
	return pageNo_;
}

void DescribeRtcChannelDetailRequest::setPageNo(int pageNo)
{
	pageNo_ = pageNo;
	setParameter("PageNo", std::to_string(pageNo));
}

std::string DescribeRtcChannelDetailRequest::getAppId()const
{
	return appId_;
}

void DescribeRtcChannelDetailRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string DescribeRtcChannelDetailRequest::getChannelId()const
{
	return channelId_;
}

void DescribeRtcChannelDetailRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

