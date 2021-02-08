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

#include <alibabacloud/cloudapi/model/DescribeApiErrorDataRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeApiErrorDataRequest;

DescribeApiErrorDataRequest::DescribeApiErrorDataRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeApiErrorData")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeApiErrorDataRequest::~DescribeApiErrorDataRequest()
{}

std::string DescribeApiErrorDataRequest::getGroupId()const
{
	return groupId_;
}

void DescribeApiErrorDataRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string DescribeApiErrorDataRequest::getEndTime()const
{
	return endTime_;
}

void DescribeApiErrorDataRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

std::string DescribeApiErrorDataRequest::getStartTime()const
{
	return startTime_;
}

void DescribeApiErrorDataRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeApiErrorDataRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeApiErrorDataRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeApiErrorDataRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeApiErrorDataRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string DescribeApiErrorDataRequest::getApiId()const
{
	return apiId_;
}

void DescribeApiErrorDataRequest::setApiId(const std::string& apiId)
{
	apiId_ = apiId;
	setParameter("ApiId", apiId);
}

