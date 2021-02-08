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

#include <alibabacloud/live/model/DescribeLiveStreamsPublishListRequest.h>

using AlibabaCloud::Live::Model::DescribeLiveStreamsPublishListRequest;

DescribeLiveStreamsPublishListRequest::DescribeLiveStreamsPublishListRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeLiveStreamsPublishList")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeLiveStreamsPublishListRequest::~DescribeLiveStreamsPublishListRequest()
{}

std::string DescribeLiveStreamsPublishListRequest::getStartTime()const
{
	return startTime_;
}

void DescribeLiveStreamsPublishListRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

int DescribeLiveStreamsPublishListRequest::getPageNumber()const
{
	return pageNumber_;
}

void DescribeLiveStreamsPublishListRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

std::string DescribeLiveStreamsPublishListRequest::getAppName()const
{
	return appName_;
}

void DescribeLiveStreamsPublishListRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

int DescribeLiveStreamsPublishListRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeLiveStreamsPublishListRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeLiveStreamsPublishListRequest::getStreamName()const
{
	return streamName_;
}

void DescribeLiveStreamsPublishListRequest::setStreamName(const std::string& streamName)
{
	streamName_ = streamName;
	setParameter("StreamName", streamName);
}

std::string DescribeLiveStreamsPublishListRequest::getQueryType()const
{
	return queryType_;
}

void DescribeLiveStreamsPublishListRequest::setQueryType(const std::string& queryType)
{
	queryType_ = queryType;
	setParameter("QueryType", queryType);
}

std::string DescribeLiveStreamsPublishListRequest::getStreamType()const
{
	return streamType_;
}

void DescribeLiveStreamsPublishListRequest::setStreamType(const std::string& streamType)
{
	streamType_ = streamType;
	setParameter("StreamType", streamType);
}

std::string DescribeLiveStreamsPublishListRequest::getDomainName()const
{
	return domainName_;
}

void DescribeLiveStreamsPublishListRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string DescribeLiveStreamsPublishListRequest::getEndTime()const
{
	return endTime_;
}

void DescribeLiveStreamsPublishListRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

std::string DescribeLiveStreamsPublishListRequest::getOrderBy()const
{
	return orderBy_;
}

void DescribeLiveStreamsPublishListRequest::setOrderBy(const std::string& orderBy)
{
	orderBy_ = orderBy;
	setParameter("OrderBy", orderBy);
}

long DescribeLiveStreamsPublishListRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeLiveStreamsPublishListRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

