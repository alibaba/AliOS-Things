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

#include <alibabacloud/rtc/model/DescribeRtcScaleDetailRequest.h>

using AlibabaCloud::Rtc::Model::DescribeRtcScaleDetailRequest;

DescribeRtcScaleDetailRequest::DescribeRtcScaleDetailRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DescribeRtcScaleDetail")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeRtcScaleDetailRequest::~DescribeRtcScaleDetailRequest()
{}

std::string DescribeRtcScaleDetailRequest::getStartTime()const
{
	return startTime_;
}

void DescribeRtcScaleDetailRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeRtcScaleDetailRequest::getShowLog()const
{
	return showLog_;
}

void DescribeRtcScaleDetailRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string DescribeRtcScaleDetailRequest::getEndTime()const
{
	return endTime_;
}

void DescribeRtcScaleDetailRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeRtcScaleDetailRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeRtcScaleDetailRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeRtcScaleDetailRequest::getAppId()const
{
	return appId_;
}

void DescribeRtcScaleDetailRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

