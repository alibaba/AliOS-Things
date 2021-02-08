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

#include <alibabacloud/live/model/DescribeLiveTopDomainsByFlowRequest.h>

using AlibabaCloud::Live::Model::DescribeLiveTopDomainsByFlowRequest;

DescribeLiveTopDomainsByFlowRequest::DescribeLiveTopDomainsByFlowRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeLiveTopDomainsByFlow")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeLiveTopDomainsByFlowRequest::~DescribeLiveTopDomainsByFlowRequest()
{}

std::string DescribeLiveTopDomainsByFlowRequest::getStartTime()const
{
	return startTime_;
}

void DescribeLiveTopDomainsByFlowRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

long DescribeLiveTopDomainsByFlowRequest::getLimit()const
{
	return limit_;
}

void DescribeLiveTopDomainsByFlowRequest::setLimit(long limit)
{
	limit_ = limit;
	setParameter("Limit", std::to_string(limit));
}

std::string DescribeLiveTopDomainsByFlowRequest::getEndTime()const
{
	return endTime_;
}

void DescribeLiveTopDomainsByFlowRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeLiveTopDomainsByFlowRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeLiveTopDomainsByFlowRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

