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

#include <alibabacloud/rtc/model/DescribeRtcScaleRequest.h>

using AlibabaCloud::Rtc::Model::DescribeRtcScaleRequest;

DescribeRtcScaleRequest::DescribeRtcScaleRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DescribeRtcScale")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeRtcScaleRequest::~DescribeRtcScaleRequest()
{}

std::string DescribeRtcScaleRequest::getStartTime()const
{
	return startTime_;
}

void DescribeRtcScaleRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeRtcScaleRequest::getShowLog()const
{
	return showLog_;
}

void DescribeRtcScaleRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string DescribeRtcScaleRequest::getEndTime()const
{
	return endTime_;
}

void DescribeRtcScaleRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeRtcScaleRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeRtcScaleRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeRtcScaleRequest::getAppId()const
{
	return appId_;
}

void DescribeRtcScaleRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

