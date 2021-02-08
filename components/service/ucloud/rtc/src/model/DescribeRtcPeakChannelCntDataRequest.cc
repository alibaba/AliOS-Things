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

#include <alibabacloud/rtc/model/DescribeRtcPeakChannelCntDataRequest.h>

using AlibabaCloud::Rtc::Model::DescribeRtcPeakChannelCntDataRequest;

DescribeRtcPeakChannelCntDataRequest::DescribeRtcPeakChannelCntDataRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DescribeRtcPeakChannelCntData")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeRtcPeakChannelCntDataRequest::~DescribeRtcPeakChannelCntDataRequest()
{}

std::string DescribeRtcPeakChannelCntDataRequest::getStartTime()const
{
	return startTime_;
}

void DescribeRtcPeakChannelCntDataRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeRtcPeakChannelCntDataRequest::getServiceArea()const
{
	return serviceArea_;
}

void DescribeRtcPeakChannelCntDataRequest::setServiceArea(const std::string& serviceArea)
{
	serviceArea_ = serviceArea;
	setParameter("ServiceArea", serviceArea);
}

std::string DescribeRtcPeakChannelCntDataRequest::getShowLog()const
{
	return showLog_;
}

void DescribeRtcPeakChannelCntDataRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string DescribeRtcPeakChannelCntDataRequest::getEndTime()const
{
	return endTime_;
}

void DescribeRtcPeakChannelCntDataRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeRtcPeakChannelCntDataRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeRtcPeakChannelCntDataRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeRtcPeakChannelCntDataRequest::getAppId()const
{
	return appId_;
}

void DescribeRtcPeakChannelCntDataRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string DescribeRtcPeakChannelCntDataRequest::getInterval()const
{
	return interval_;
}

void DescribeRtcPeakChannelCntDataRequest::setInterval(const std::string& interval)
{
	interval_ = interval;
	setParameter("Interval", interval);
}

