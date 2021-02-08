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

#include <alibabacloud/cloudapi/model/DescribeApisByTrafficControlRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeApisByTrafficControlRequest;

DescribeApisByTrafficControlRequest::DescribeApisByTrafficControlRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeApisByTrafficControl")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeApisByTrafficControlRequest::~DescribeApisByTrafficControlRequest()
{}

std::string DescribeApisByTrafficControlRequest::getTrafficControlId()const
{
	return trafficControlId_;
}

void DescribeApisByTrafficControlRequest::setTrafficControlId(const std::string& trafficControlId)
{
	trafficControlId_ = trafficControlId;
	setParameter("TrafficControlId", trafficControlId);
}

int DescribeApisByTrafficControlRequest::getPageNumber()const
{
	return pageNumber_;
}

void DescribeApisByTrafficControlRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

std::string DescribeApisByTrafficControlRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeApisByTrafficControlRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeApisByTrafficControlRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeApisByTrafficControlRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

int DescribeApisByTrafficControlRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeApisByTrafficControlRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

