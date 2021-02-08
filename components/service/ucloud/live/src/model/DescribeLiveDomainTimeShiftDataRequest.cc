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

#include <alibabacloud/live/model/DescribeLiveDomainTimeShiftDataRequest.h>

using AlibabaCloud::Live::Model::DescribeLiveDomainTimeShiftDataRequest;

DescribeLiveDomainTimeShiftDataRequest::DescribeLiveDomainTimeShiftDataRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeLiveDomainTimeShiftData")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeLiveDomainTimeShiftDataRequest::~DescribeLiveDomainTimeShiftDataRequest()
{}

std::string DescribeLiveDomainTimeShiftDataRequest::getStartTime()const
{
	return startTime_;
}

void DescribeLiveDomainTimeShiftDataRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeLiveDomainTimeShiftDataRequest::getDomainName()const
{
	return domainName_;
}

void DescribeLiveDomainTimeShiftDataRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string DescribeLiveDomainTimeShiftDataRequest::getEndTime()const
{
	return endTime_;
}

void DescribeLiveDomainTimeShiftDataRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeLiveDomainTimeShiftDataRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeLiveDomainTimeShiftDataRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeLiveDomainTimeShiftDataRequest::getInterval()const
{
	return interval_;
}

void DescribeLiveDomainTimeShiftDataRequest::setInterval(const std::string& interval)
{
	interval_ = interval;
	setParameter("Interval", interval);
}

