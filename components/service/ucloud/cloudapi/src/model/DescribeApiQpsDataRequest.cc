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

#include <alibabacloud/cloudapi/model/DescribeApiQpsDataRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeApiQpsDataRequest;

DescribeApiQpsDataRequest::DescribeApiQpsDataRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeApiQpsData")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeApiQpsDataRequest::~DescribeApiQpsDataRequest()
{}

std::string DescribeApiQpsDataRequest::getGroupId()const
{
	return groupId_;
}

void DescribeApiQpsDataRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string DescribeApiQpsDataRequest::getEndTime()const
{
	return endTime_;
}

void DescribeApiQpsDataRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

std::string DescribeApiQpsDataRequest::getStartTime()const
{
	return startTime_;
}

void DescribeApiQpsDataRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeApiQpsDataRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeApiQpsDataRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeApiQpsDataRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeApiQpsDataRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string DescribeApiQpsDataRequest::getApiId()const
{
	return apiId_;
}

void DescribeApiQpsDataRequest::setApiId(const std::string& apiId)
{
	apiId_ = apiId;
	setParameter("ApiId", apiId);
}

