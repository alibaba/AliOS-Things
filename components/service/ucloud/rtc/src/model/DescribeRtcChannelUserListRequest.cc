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

#include <alibabacloud/rtc/model/DescribeRtcChannelUserListRequest.h>

using AlibabaCloud::Rtc::Model::DescribeRtcChannelUserListRequest;

DescribeRtcChannelUserListRequest::DescribeRtcChannelUserListRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DescribeRtcChannelUserList")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeRtcChannelUserListRequest::~DescribeRtcChannelUserListRequest()
{}

long DescribeRtcChannelUserListRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeRtcChannelUserListRequest::setPageSize(long pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeRtcChannelUserListRequest::getShowLog()const
{
	return showLog_;
}

void DescribeRtcChannelUserListRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long DescribeRtcChannelUserListRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeRtcChannelUserListRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

long DescribeRtcChannelUserListRequest::getPageNo()const
{
	return pageNo_;
}

void DescribeRtcChannelUserListRequest::setPageNo(long pageNo)
{
	pageNo_ = pageNo;
	setParameter("PageNo", std::to_string(pageNo));
}

std::string DescribeRtcChannelUserListRequest::getAppId()const
{
	return appId_;
}

void DescribeRtcChannelUserListRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string DescribeRtcChannelUserListRequest::getChannelId()const
{
	return channelId_;
}

void DescribeRtcChannelUserListRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

std::string DescribeRtcChannelUserListRequest::getTimePoint()const
{
	return timePoint_;
}

void DescribeRtcChannelUserListRequest::setTimePoint(const std::string& timePoint)
{
	timePoint_ = timePoint;
	setParameter("TimePoint", timePoint);
}

