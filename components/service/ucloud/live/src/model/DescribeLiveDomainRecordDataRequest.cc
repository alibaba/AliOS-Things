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

#include <alibabacloud/live/model/DescribeLiveDomainRecordDataRequest.h>

using AlibabaCloud::Live::Model::DescribeLiveDomainRecordDataRequest;

DescribeLiveDomainRecordDataRequest::DescribeLiveDomainRecordDataRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeLiveDomainRecordData")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeLiveDomainRecordDataRequest::~DescribeLiveDomainRecordDataRequest()
{}

std::string DescribeLiveDomainRecordDataRequest::getStartTime()const
{
	return startTime_;
}

void DescribeLiveDomainRecordDataRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeLiveDomainRecordDataRequest::getRecordType()const
{
	return recordType_;
}

void DescribeLiveDomainRecordDataRequest::setRecordType(const std::string& recordType)
{
	recordType_ = recordType;
	setParameter("RecordType", recordType);
}

std::string DescribeLiveDomainRecordDataRequest::getDomainName()const
{
	return domainName_;
}

void DescribeLiveDomainRecordDataRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string DescribeLiveDomainRecordDataRequest::getEndTime()const
{
	return endTime_;
}

void DescribeLiveDomainRecordDataRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeLiveDomainRecordDataRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeLiveDomainRecordDataRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

