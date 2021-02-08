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

#include <alibabacloud/live/model/DescribeCastersRequest.h>

using AlibabaCloud::Live::Model::DescribeCastersRequest;

DescribeCastersRequest::DescribeCastersRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeCasters")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeCastersRequest::~DescribeCastersRequest()
{}

std::string DescribeCastersRequest::getStartTime()const
{
	return startTime_;
}

void DescribeCastersRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

int DescribeCastersRequest::getPageNum()const
{
	return pageNum_;
}

void DescribeCastersRequest::setPageNum(int pageNum)
{
	pageNum_ = pageNum;
	setParameter("PageNum", std::to_string(pageNum));
}

std::string DescribeCastersRequest::getCasterName()const
{
	return casterName_;
}

void DescribeCastersRequest::setCasterName(const std::string& casterName)
{
	casterName_ = casterName;
	setParameter("CasterName", casterName);
}

int DescribeCastersRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeCastersRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeCastersRequest::getCasterId()const
{
	return casterId_;
}

void DescribeCastersRequest::setCasterId(const std::string& casterId)
{
	casterId_ = casterId;
	setParameter("CasterId", casterId);
}

std::string DescribeCastersRequest::getEndTime()const
{
	return endTime_;
}

void DescribeCastersRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeCastersRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeCastersRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

int DescribeCastersRequest::getStatus()const
{
	return status_;
}

void DescribeCastersRequest::setStatus(int status)
{
	status_ = status;
	setParameter("Status", std::to_string(status));
}

