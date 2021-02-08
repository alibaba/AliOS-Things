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

#include <alibabacloud/config/model/ListDiscoveredResourcesRequest.h>

using AlibabaCloud::Config::Model::ListDiscoveredResourcesRequest;

ListDiscoveredResourcesRequest::ListDiscoveredResourcesRequest() :
	RpcServiceRequest("config", "2019-01-08", "ListDiscoveredResources")
{
	setMethod(HttpRequest::Method::Get);
}

ListDiscoveredResourcesRequest::~ListDiscoveredResourcesRequest()
{}

int ListDiscoveredResourcesRequest::getResourceDeleted()const
{
	return resourceDeleted_;
}

void ListDiscoveredResourcesRequest::setResourceDeleted(int resourceDeleted)
{
	resourceDeleted_ = resourceDeleted;
	setParameter("ResourceDeleted", std::to_string(resourceDeleted));
}

bool ListDiscoveredResourcesRequest::getMultiAccount()const
{
	return multiAccount_;
}

void ListDiscoveredResourcesRequest::setMultiAccount(bool multiAccount)
{
	multiAccount_ = multiAccount;
	setParameter("MultiAccount", multiAccount ? "true" : "false");
}

std::string ListDiscoveredResourcesRequest::getRegions()const
{
	return regions_;
}

void ListDiscoveredResourcesRequest::setRegions(const std::string& regions)
{
	regions_ = regions;
	setParameter("Regions", regions);
}

int ListDiscoveredResourcesRequest::getPageNumber()const
{
	return pageNumber_;
}

void ListDiscoveredResourcesRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

int ListDiscoveredResourcesRequest::getPageSize()const
{
	return pageSize_;
}

void ListDiscoveredResourcesRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string ListDiscoveredResourcesRequest::getComplianceType()const
{
	return complianceType_;
}

void ListDiscoveredResourcesRequest::setComplianceType(const std::string& complianceType)
{
	complianceType_ = complianceType;
	setParameter("ComplianceType", complianceType);
}

std::string ListDiscoveredResourcesRequest::getResourceId()const
{
	return resourceId_;
}

void ListDiscoveredResourcesRequest::setResourceId(const std::string& resourceId)
{
	resourceId_ = resourceId;
	setParameter("ResourceId", resourceId);
}

std::string ListDiscoveredResourcesRequest::getResourceTypes()const
{
	return resourceTypes_;
}

void ListDiscoveredResourcesRequest::setResourceTypes(const std::string& resourceTypes)
{
	resourceTypes_ = resourceTypes;
	setParameter("ResourceTypes", resourceTypes);
}

long ListDiscoveredResourcesRequest::getMemberId()const
{
	return memberId_;
}

void ListDiscoveredResourcesRequest::setMemberId(long memberId)
{
	memberId_ = memberId;
	setParameter("MemberId", std::to_string(memberId));
}

