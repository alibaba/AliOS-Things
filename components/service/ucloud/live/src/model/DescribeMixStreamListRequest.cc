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

#include <alibabacloud/live/model/DescribeMixStreamListRequest.h>

using AlibabaCloud::Live::Model::DescribeMixStreamListRequest;

DescribeMixStreamListRequest::DescribeMixStreamListRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeMixStreamList")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeMixStreamListRequest::~DescribeMixStreamListRequest()
{}

std::string DescribeMixStreamListRequest::getStartTime()const
{
	return startTime_;
}

void DescribeMixStreamListRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeMixStreamListRequest::getAppName()const
{
	return appName_;
}

void DescribeMixStreamListRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

int DescribeMixStreamListRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeMixStreamListRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeMixStreamListRequest::getStreamName()const
{
	return streamName_;
}

void DescribeMixStreamListRequest::setStreamName(const std::string& streamName)
{
	streamName_ = streamName;
	setParameter("StreamName", streamName);
}

std::string DescribeMixStreamListRequest::getMixStreamId()const
{
	return mixStreamId_;
}

void DescribeMixStreamListRequest::setMixStreamId(const std::string& mixStreamId)
{
	mixStreamId_ = mixStreamId;
	setParameter("MixStreamId", mixStreamId);
}

std::string DescribeMixStreamListRequest::getDomainName()const
{
	return domainName_;
}

void DescribeMixStreamListRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string DescribeMixStreamListRequest::getEndTime()const
{
	return endTime_;
}

void DescribeMixStreamListRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeMixStreamListRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeMixStreamListRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

int DescribeMixStreamListRequest::getPageNo()const
{
	return pageNo_;
}

void DescribeMixStreamListRequest::setPageNo(int pageNo)
{
	pageNo_ = pageNo;
	setParameter("PageNo", std::to_string(pageNo));
}

