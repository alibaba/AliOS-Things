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

#include <alibabacloud/cloudapi/model/DescribePurchasedApiGroupsRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribePurchasedApiGroupsRequest;

DescribePurchasedApiGroupsRequest::DescribePurchasedApiGroupsRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribePurchasedApiGroups")
{
	setMethod(HttpRequest::Method::Post);
}

DescribePurchasedApiGroupsRequest::~DescribePurchasedApiGroupsRequest()
{}

int DescribePurchasedApiGroupsRequest::getPageNumber()const
{
	return pageNumber_;
}

void DescribePurchasedApiGroupsRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

std::string DescribePurchasedApiGroupsRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribePurchasedApiGroupsRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribePurchasedApiGroupsRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribePurchasedApiGroupsRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

int DescribePurchasedApiGroupsRequest::getPageSize()const
{
	return pageSize_;
}

void DescribePurchasedApiGroupsRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

