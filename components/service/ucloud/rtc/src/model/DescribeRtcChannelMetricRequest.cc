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

#include <alibabacloud/rtc/model/DescribeRtcChannelMetricRequest.h>

using AlibabaCloud::Rtc::Model::DescribeRtcChannelMetricRequest;

DescribeRtcChannelMetricRequest::DescribeRtcChannelMetricRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DescribeRtcChannelMetric")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeRtcChannelMetricRequest::~DescribeRtcChannelMetricRequest()
{}

std::string DescribeRtcChannelMetricRequest::getShowLog()const
{
	return showLog_;
}

void DescribeRtcChannelMetricRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long DescribeRtcChannelMetricRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeRtcChannelMetricRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeRtcChannelMetricRequest::getAppId()const
{
	return appId_;
}

void DescribeRtcChannelMetricRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string DescribeRtcChannelMetricRequest::getChannelId()const
{
	return channelId_;
}

void DescribeRtcChannelMetricRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

std::string DescribeRtcChannelMetricRequest::getTimePoint()const
{
	return timePoint_;
}

void DescribeRtcChannelMetricRequest::setTimePoint(const std::string& timePoint)
{
	timePoint_ = timePoint;
	setParameter("TimePoint", timePoint);
}

