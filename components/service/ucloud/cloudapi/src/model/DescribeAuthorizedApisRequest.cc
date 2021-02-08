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

#include <alibabacloud/cloudapi/model/DescribeAuthorizedApisRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeAuthorizedApisRequest;

DescribeAuthorizedApisRequest::DescribeAuthorizedApisRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeAuthorizedApis")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeAuthorizedApisRequest::~DescribeAuthorizedApisRequest()
{}

int DescribeAuthorizedApisRequest::getPageNumber()const
{
	return pageNumber_;
}

void DescribeAuthorizedApisRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

std::string DescribeAuthorizedApisRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeAuthorizedApisRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeAuthorizedApisRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeAuthorizedApisRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

long DescribeAuthorizedApisRequest::getAppId()const
{
	return appId_;
}

void DescribeAuthorizedApisRequest::setAppId(long appId)
{
	appId_ = appId;
	setParameter("AppId", std::to_string(appId));
}

int DescribeAuthorizedApisRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeAuthorizedApisRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

