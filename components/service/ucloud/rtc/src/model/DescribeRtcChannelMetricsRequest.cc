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

#include <alibabacloud/rtc/model/DescribeRtcChannelMetricsRequest.h>

using AlibabaCloud::Rtc::Model::DescribeRtcChannelMetricsRequest;

DescribeRtcChannelMetricsRequest::DescribeRtcChannelMetricsRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DescribeRtcChannelMetrics")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeRtcChannelMetricsRequest::~DescribeRtcChannelMetricsRequest()
{}

std::string DescribeRtcChannelMetricsRequest::getStartTime()const
{
	return startTime_;
}

void DescribeRtcChannelMetricsRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeRtcChannelMetricsRequest::getSubUid()const
{
	return subUid_;
}

void DescribeRtcChannelMetricsRequest::setSubUid(const std::string& subUid)
{
	subUid_ = subUid;
	setParameter("SubUid", subUid);
}

std::string DescribeRtcChannelMetricsRequest::getPubUid()const
{
	return pubUid_;
}

void DescribeRtcChannelMetricsRequest::setPubUid(const std::string& pubUid)
{
	pubUid_ = pubUid;
	setParameter("PubUid", pubUid);
}

std::string DescribeRtcChannelMetricsRequest::getShowLog()const
{
	return showLog_;
}

void DescribeRtcChannelMetricsRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string DescribeRtcChannelMetricsRequest::getEndTime()const
{
	return endTime_;
}

void DescribeRtcChannelMetricsRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeRtcChannelMetricsRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeRtcChannelMetricsRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeRtcChannelMetricsRequest::getAppId()const
{
	return appId_;
}

void DescribeRtcChannelMetricsRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string DescribeRtcChannelMetricsRequest::getChannelId()const
{
	return channelId_;
}

void DescribeRtcChannelMetricsRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

