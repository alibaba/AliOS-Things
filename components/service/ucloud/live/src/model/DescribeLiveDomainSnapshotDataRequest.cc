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

#include <alibabacloud/live/model/DescribeLiveDomainSnapshotDataRequest.h>

using AlibabaCloud::Live::Model::DescribeLiveDomainSnapshotDataRequest;

DescribeLiveDomainSnapshotDataRequest::DescribeLiveDomainSnapshotDataRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeLiveDomainSnapshotData")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeLiveDomainSnapshotDataRequest::~DescribeLiveDomainSnapshotDataRequest()
{}

std::string DescribeLiveDomainSnapshotDataRequest::getStartTime()const
{
	return startTime_;
}

void DescribeLiveDomainSnapshotDataRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeLiveDomainSnapshotDataRequest::getDomainName()const
{
	return domainName_;
}

void DescribeLiveDomainSnapshotDataRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string DescribeLiveDomainSnapshotDataRequest::getEndTime()const
{
	return endTime_;
}

void DescribeLiveDomainSnapshotDataRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeLiveDomainSnapshotDataRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeLiveDomainSnapshotDataRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

