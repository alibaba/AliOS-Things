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

#include <alibabacloud/cloudapi/model/DescribeApisByAppRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeApisByAppRequest;

DescribeApisByAppRequest::DescribeApisByAppRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeApisByApp")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeApisByAppRequest::~DescribeApisByAppRequest()
{}

int DescribeApisByAppRequest::getPageNumber()const
{
	return pageNumber_;
}

void DescribeApisByAppRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

std::string DescribeApisByAppRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeApisByAppRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeApisByAppRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeApisByAppRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

long DescribeApisByAppRequest::getAppId()const
{
	return appId_;
}

void DescribeApisByAppRequest::setAppId(long appId)
{
	appId_ = appId;
	setParameter("AppId", std::to_string(appId));
}

int DescribeApisByAppRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeApisByAppRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

