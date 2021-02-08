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

#include <alibabacloud/live/model/DescribeLiveStreamsOnlineListRequest.h>

using AlibabaCloud::Live::Model::DescribeLiveStreamsOnlineListRequest;

DescribeLiveStreamsOnlineListRequest::DescribeLiveStreamsOnlineListRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeLiveStreamsOnlineList")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeLiveStreamsOnlineListRequest::~DescribeLiveStreamsOnlineListRequest()
{}

std::string DescribeLiveStreamsOnlineListRequest::getStartTime()const
{
	return startTime_;
}

void DescribeLiveStreamsOnlineListRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

int DescribeLiveStreamsOnlineListRequest::getPageNum()const
{
	return pageNum_;
}

void DescribeLiveStreamsOnlineListRequest::setPageNum(int pageNum)
{
	pageNum_ = pageNum;
	setParameter("PageNum", std::to_string(pageNum));
}

std::string DescribeLiveStreamsOnlineListRequest::getAppName()const
{
	return appName_;
}

void DescribeLiveStreamsOnlineListRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

int DescribeLiveStreamsOnlineListRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeLiveStreamsOnlineListRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeLiveStreamsOnlineListRequest::getStreamName()const
{
	return streamName_;
}

void DescribeLiveStreamsOnlineListRequest::setStreamName(const std::string& streamName)
{
	streamName_ = streamName;
	setParameter("StreamName", streamName);
}

std::string DescribeLiveStreamsOnlineListRequest::getQueryType()const
{
	return queryType_;
}

void DescribeLiveStreamsOnlineListRequest::setQueryType(const std::string& queryType)
{
	queryType_ = queryType;
	setParameter("QueryType", queryType);
}

std::string DescribeLiveStreamsOnlineListRequest::getStreamType()const
{
	return streamType_;
}

void DescribeLiveStreamsOnlineListRequest::setStreamType(const std::string& streamType)
{
	streamType_ = streamType;
	setParameter("StreamType", streamType);
}

std::string DescribeLiveStreamsOnlineListRequest::getDomainName()const
{
	return domainName_;
}

void DescribeLiveStreamsOnlineListRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string DescribeLiveStreamsOnlineListRequest::getEndTime()const
{
	return endTime_;
}

void DescribeLiveStreamsOnlineListRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

std::string DescribeLiveStreamsOnlineListRequest::getOrderBy()const
{
	return orderBy_;
}

void DescribeLiveStreamsOnlineListRequest::setOrderBy(const std::string& orderBy)
{
	orderBy_ = orderBy;
	setParameter("OrderBy", orderBy);
}

long DescribeLiveStreamsOnlineListRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeLiveStreamsOnlineListRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

