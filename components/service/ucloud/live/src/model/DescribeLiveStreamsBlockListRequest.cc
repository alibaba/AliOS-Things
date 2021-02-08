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

#include <alibabacloud/live/model/DescribeLiveStreamsBlockListRequest.h>

using AlibabaCloud::Live::Model::DescribeLiveStreamsBlockListRequest;

DescribeLiveStreamsBlockListRequest::DescribeLiveStreamsBlockListRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeLiveStreamsBlockList")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeLiveStreamsBlockListRequest::~DescribeLiveStreamsBlockListRequest()
{}

int DescribeLiveStreamsBlockListRequest::getPageNum()const
{
	return pageNum_;
}

void DescribeLiveStreamsBlockListRequest::setPageNum(int pageNum)
{
	pageNum_ = pageNum;
	setParameter("PageNum", std::to_string(pageNum));
}

std::string DescribeLiveStreamsBlockListRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeLiveStreamsBlockListRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

int DescribeLiveStreamsBlockListRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeLiveStreamsBlockListRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeLiveStreamsBlockListRequest::getDomainName()const
{
	return domainName_;
}

void DescribeLiveStreamsBlockListRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long DescribeLiveStreamsBlockListRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeLiveStreamsBlockListRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

