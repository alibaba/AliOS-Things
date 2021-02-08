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

#include <alibabacloud/rtc/model/DescribeRtcUserCntDataRequest.h>

using AlibabaCloud::Rtc::Model::DescribeRtcUserCntDataRequest;

DescribeRtcUserCntDataRequest::DescribeRtcUserCntDataRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DescribeRtcUserCntData")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeRtcUserCntDataRequest::~DescribeRtcUserCntDataRequest()
{}

std::string DescribeRtcUserCntDataRequest::getStartTime()const
{
	return startTime_;
}

void DescribeRtcUserCntDataRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeRtcUserCntDataRequest::getServiceArea()const
{
	return serviceArea_;
}

void DescribeRtcUserCntDataRequest::setServiceArea(const std::string& serviceArea)
{
	serviceArea_ = serviceArea;
	setParameter("ServiceArea", serviceArea);
}

std::string DescribeRtcUserCntDataRequest::getShowLog()const
{
	return showLog_;
}

void DescribeRtcUserCntDataRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string DescribeRtcUserCntDataRequest::getEndTime()const
{
	return endTime_;
}

void DescribeRtcUserCntDataRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeRtcUserCntDataRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeRtcUserCntDataRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeRtcUserCntDataRequest::getAppId()const
{
	return appId_;
}

void DescribeRtcUserCntDataRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string DescribeRtcUserCntDataRequest::getInterval()const
{
	return interval_;
}

void DescribeRtcUserCntDataRequest::setInterval(const std::string& interval)
{
	interval_ = interval;
	setParameter("Interval", interval);
}

