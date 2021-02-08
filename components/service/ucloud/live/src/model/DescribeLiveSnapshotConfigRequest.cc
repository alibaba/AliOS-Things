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

#include <alibabacloud/live/model/DescribeLiveSnapshotConfigRequest.h>

using AlibabaCloud::Live::Model::DescribeLiveSnapshotConfigRequest;

DescribeLiveSnapshotConfigRequest::DescribeLiveSnapshotConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeLiveSnapshotConfig")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeLiveSnapshotConfigRequest::~DescribeLiveSnapshotConfigRequest()
{}

int DescribeLiveSnapshotConfigRequest::getPageNum()const
{
	return pageNum_;
}

void DescribeLiveSnapshotConfigRequest::setPageNum(int pageNum)
{
	pageNum_ = pageNum;
	setParameter("PageNum", std::to_string(pageNum));
}

std::string DescribeLiveSnapshotConfigRequest::getAppName()const
{
	return appName_;
}

void DescribeLiveSnapshotConfigRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string DescribeLiveSnapshotConfigRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeLiveSnapshotConfigRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

int DescribeLiveSnapshotConfigRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeLiveSnapshotConfigRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeLiveSnapshotConfigRequest::getOrder()const
{
	return order_;
}

void DescribeLiveSnapshotConfigRequest::setOrder(const std::string& order)
{
	order_ = order;
	setParameter("Order", order);
}

std::string DescribeLiveSnapshotConfigRequest::getDomainName()const
{
	return domainName_;
}

void DescribeLiveSnapshotConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long DescribeLiveSnapshotConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeLiveSnapshotConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

