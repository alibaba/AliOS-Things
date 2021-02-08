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

#include <alibabacloud/live/model/DescribeLiveSnapshotDetectPornConfigRequest.h>

using AlibabaCloud::Live::Model::DescribeLiveSnapshotDetectPornConfigRequest;

DescribeLiveSnapshotDetectPornConfigRequest::DescribeLiveSnapshotDetectPornConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeLiveSnapshotDetectPornConfig")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeLiveSnapshotDetectPornConfigRequest::~DescribeLiveSnapshotDetectPornConfigRequest()
{}

int DescribeLiveSnapshotDetectPornConfigRequest::getPageNum()const
{
	return pageNum_;
}

void DescribeLiveSnapshotDetectPornConfigRequest::setPageNum(int pageNum)
{
	pageNum_ = pageNum;
	setParameter("PageNum", std::to_string(pageNum));
}

std::string DescribeLiveSnapshotDetectPornConfigRequest::getAppName()const
{
	return appName_;
}

void DescribeLiveSnapshotDetectPornConfigRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string DescribeLiveSnapshotDetectPornConfigRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeLiveSnapshotDetectPornConfigRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

int DescribeLiveSnapshotDetectPornConfigRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeLiveSnapshotDetectPornConfigRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeLiveSnapshotDetectPornConfigRequest::getOrder()const
{
	return order_;
}

void DescribeLiveSnapshotDetectPornConfigRequest::setOrder(const std::string& order)
{
	order_ = order;
	setParameter("Order", order);
}

std::string DescribeLiveSnapshotDetectPornConfigRequest::getDomainName()const
{
	return domainName_;
}

void DescribeLiveSnapshotDetectPornConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long DescribeLiveSnapshotDetectPornConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeLiveSnapshotDetectPornConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

