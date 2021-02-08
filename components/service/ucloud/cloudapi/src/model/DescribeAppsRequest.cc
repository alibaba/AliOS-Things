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

#include <alibabacloud/cloudapi/model/DescribeAppsRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeAppsRequest;

DescribeAppsRequest::DescribeAppsRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeApps")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeAppsRequest::~DescribeAppsRequest()
{}

std::string DescribeAppsRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeAppsRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

long DescribeAppsRequest::getAppId()const
{
	return appId_;
}

void DescribeAppsRequest::setAppId(long appId)
{
	appId_ = appId;
	setParameter("AppId", std::to_string(appId));
}

int DescribeAppsRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeAppsRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

long DescribeAppsRequest::getAppOwner()const
{
	return appOwner_;
}

void DescribeAppsRequest::setAppOwner(long appOwner)
{
	appOwner_ = appOwner;
	setParameter("AppOwner", std::to_string(appOwner));
}

int DescribeAppsRequest::getPageNumber()const
{
	return pageNumber_;
}

void DescribeAppsRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

std::string DescribeAppsRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeAppsRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

