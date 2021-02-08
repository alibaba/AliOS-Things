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

#include <alibabacloud/live/model/DescribeUpPeakPublishStreamDataRequest.h>

using AlibabaCloud::Live::Model::DescribeUpPeakPublishStreamDataRequest;

DescribeUpPeakPublishStreamDataRequest::DescribeUpPeakPublishStreamDataRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeUpPeakPublishStreamData")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeUpPeakPublishStreamDataRequest::~DescribeUpPeakPublishStreamDataRequest()
{}

std::string DescribeUpPeakPublishStreamDataRequest::getStartTime()const
{
	return startTime_;
}

void DescribeUpPeakPublishStreamDataRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeUpPeakPublishStreamDataRequest::getDomainName()const
{
	return domainName_;
}

void DescribeUpPeakPublishStreamDataRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string DescribeUpPeakPublishStreamDataRequest::getEndTime()const
{
	return endTime_;
}

void DescribeUpPeakPublishStreamDataRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeUpPeakPublishStreamDataRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeUpPeakPublishStreamDataRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeUpPeakPublishStreamDataRequest::getDomainSwitch()const
{
	return domainSwitch_;
}

void DescribeUpPeakPublishStreamDataRequest::setDomainSwitch(const std::string& domainSwitch)
{
	domainSwitch_ = domainSwitch;
	setParameter("DomainSwitch", domainSwitch);
}

