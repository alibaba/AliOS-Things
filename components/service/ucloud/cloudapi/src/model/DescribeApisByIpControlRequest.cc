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

#include <alibabacloud/cloudapi/model/DescribeApisByIpControlRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeApisByIpControlRequest;

DescribeApisByIpControlRequest::DescribeApisByIpControlRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeApisByIpControl")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeApisByIpControlRequest::~DescribeApisByIpControlRequest()
{}

int DescribeApisByIpControlRequest::getPageNumber()const
{
	return pageNumber_;
}

void DescribeApisByIpControlRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

std::string DescribeApisByIpControlRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeApisByIpControlRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeApisByIpControlRequest::getIpControlId()const
{
	return ipControlId_;
}

void DescribeApisByIpControlRequest::setIpControlId(const std::string& ipControlId)
{
	ipControlId_ = ipControlId;
	setParameter("IpControlId", ipControlId);
}

std::string DescribeApisByIpControlRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeApisByIpControlRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

int DescribeApisByIpControlRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeApisByIpControlRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

