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

#include <alibabacloud/live/model/DescribeLiveRealtimeDeliveryAccRequest.h>

using AlibabaCloud::Live::Model::DescribeLiveRealtimeDeliveryAccRequest;

DescribeLiveRealtimeDeliveryAccRequest::DescribeLiveRealtimeDeliveryAccRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeLiveRealtimeDeliveryAcc")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeLiveRealtimeDeliveryAccRequest::~DescribeLiveRealtimeDeliveryAccRequest()
{}

std::string DescribeLiveRealtimeDeliveryAccRequest::getProject()const
{
	return project_;
}

void DescribeLiveRealtimeDeliveryAccRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string DescribeLiveRealtimeDeliveryAccRequest::getStartTime()const
{
	return startTime_;
}

void DescribeLiveRealtimeDeliveryAccRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeLiveRealtimeDeliveryAccRequest::getEndTime()const
{
	return endTime_;
}

void DescribeLiveRealtimeDeliveryAccRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeLiveRealtimeDeliveryAccRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeLiveRealtimeDeliveryAccRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeLiveRealtimeDeliveryAccRequest::getInterval()const
{
	return interval_;
}

void DescribeLiveRealtimeDeliveryAccRequest::setInterval(const std::string& interval)
{
	interval_ = interval;
	setParameter("Interval", interval);
}

std::string DescribeLiveRealtimeDeliveryAccRequest::getLogStore()const
{
	return logStore_;
}

void DescribeLiveRealtimeDeliveryAccRequest::setLogStore(const std::string& logStore)
{
	logStore_ = logStore;
	setParameter("LogStore", logStore);
}

