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

#include <alibabacloud/config/model/DescribeDiscoveredResourceRequest.h>

using AlibabaCloud::Config::Model::DescribeDiscoveredResourceRequest;

DescribeDiscoveredResourceRequest::DescribeDiscoveredResourceRequest() :
	RpcServiceRequest("config", "2019-01-08", "DescribeDiscoveredResource")
{
	setMethod(HttpRequest::Method::Get);
}

DescribeDiscoveredResourceRequest::~DescribeDiscoveredResourceRequest()
{}

std::string DescribeDiscoveredResourceRequest::getResourceId()const
{
	return resourceId_;
}

void DescribeDiscoveredResourceRequest::setResourceId(const std::string& resourceId)
{
	resourceId_ = resourceId;
	setParameter("ResourceId", resourceId);
}

bool DescribeDiscoveredResourceRequest::getMultiAccount()const
{
	return multiAccount_;
}

void DescribeDiscoveredResourceRequest::setMultiAccount(bool multiAccount)
{
	multiAccount_ = multiAccount;
	setParameter("MultiAccount", multiAccount ? "true" : "false");
}

std::string DescribeDiscoveredResourceRequest::getResourceType()const
{
	return resourceType_;
}

void DescribeDiscoveredResourceRequest::setResourceType(const std::string& resourceType)
{
	resourceType_ = resourceType;
	setParameter("ResourceType", resourceType);
}

std::string DescribeDiscoveredResourceRequest::getRegion()const
{
	return region_;
}

void DescribeDiscoveredResourceRequest::setRegion(const std::string& region)
{
	region_ = region;
	setParameter("Region", region);
}

long DescribeDiscoveredResourceRequest::getMemberId()const
{
	return memberId_;
}

void DescribeDiscoveredResourceRequest::setMemberId(long memberId)
{
	memberId_ = memberId;
	setParameter("MemberId", std::to_string(memberId));
}

