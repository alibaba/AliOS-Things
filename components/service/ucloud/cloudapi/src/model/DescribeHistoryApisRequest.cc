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

#include <alibabacloud/cloudapi/model/DescribeHistoryApisRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeHistoryApisRequest;

DescribeHistoryApisRequest::DescribeHistoryApisRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeHistoryApis")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeHistoryApisRequest::~DescribeHistoryApisRequest()
{}

std::string DescribeHistoryApisRequest::getStageName()const
{
	return stageName_;
}

void DescribeHistoryApisRequest::setStageName(const std::string& stageName)
{
	stageName_ = stageName;
	setParameter("StageName", stageName);
}

std::string DescribeHistoryApisRequest::getGroupId()const
{
	return groupId_;
}

void DescribeHistoryApisRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string DescribeHistoryApisRequest::getPageNumber()const
{
	return pageNumber_;
}

void DescribeHistoryApisRequest::setPageNumber(const std::string& pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", pageNumber);
}

std::string DescribeHistoryApisRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeHistoryApisRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeHistoryApisRequest::getApiName()const
{
	return apiName_;
}

void DescribeHistoryApisRequest::setApiName(const std::string& apiName)
{
	apiName_ = apiName;
	setParameter("ApiName", apiName);
}

std::string DescribeHistoryApisRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeHistoryApisRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string DescribeHistoryApisRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeHistoryApisRequest::setPageSize(const std::string& pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", pageSize);
}

std::string DescribeHistoryApisRequest::getApiId()const
{
	return apiId_;
}

void DescribeHistoryApisRequest::setApiId(const std::string& apiId)
{
	apiId_ = apiId;
	setParameter("ApiId", apiId);
}

