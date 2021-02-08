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

#include <alibabacloud/cloudapi/model/DescribeApiTrafficDataRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeApiTrafficDataRequest;

DescribeApiTrafficDataRequest::DescribeApiTrafficDataRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeApiTrafficData")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeApiTrafficDataRequest::~DescribeApiTrafficDataRequest()
{}

std::string DescribeApiTrafficDataRequest::getGroupId()const
{
	return groupId_;
}

void DescribeApiTrafficDataRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string DescribeApiTrafficDataRequest::getEndTime()const
{
	return endTime_;
}

void DescribeApiTrafficDataRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

std::string DescribeApiTrafficDataRequest::getStartTime()const
{
	return startTime_;
}

void DescribeApiTrafficDataRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeApiTrafficDataRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeApiTrafficDataRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeApiTrafficDataRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeApiTrafficDataRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string DescribeApiTrafficDataRequest::getApiId()const
{
	return apiId_;
}

void DescribeApiTrafficDataRequest::setApiId(const std::string& apiId)
{
	apiId_ = apiId;
	setParameter("ApiId", apiId);
}

