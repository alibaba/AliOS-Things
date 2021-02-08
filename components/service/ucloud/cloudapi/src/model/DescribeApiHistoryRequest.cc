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

#include <alibabacloud/cloudapi/model/DescribeApiHistoryRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeApiHistoryRequest;

DescribeApiHistoryRequest::DescribeApiHistoryRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeApiHistory")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeApiHistoryRequest::~DescribeApiHistoryRequest()
{}

std::string DescribeApiHistoryRequest::getStageName()const
{
	return stageName_;
}

void DescribeApiHistoryRequest::setStageName(const std::string& stageName)
{
	stageName_ = stageName;
	setParameter("StageName", stageName);
}

std::string DescribeApiHistoryRequest::getGroupId()const
{
	return groupId_;
}

void DescribeApiHistoryRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string DescribeApiHistoryRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeApiHistoryRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeApiHistoryRequest::getResourceOwnerToken()const
{
	return resourceOwnerToken_;
}

void DescribeApiHistoryRequest::setResourceOwnerToken(const std::string& resourceOwnerToken)
{
	resourceOwnerToken_ = resourceOwnerToken;
	setParameter("ResourceOwnerToken", resourceOwnerToken);
}

std::string DescribeApiHistoryRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeApiHistoryRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string DescribeApiHistoryRequest::getApiId()const
{
	return apiId_;
}

void DescribeApiHistoryRequest::setApiId(const std::string& apiId)
{
	apiId_ = apiId;
	setParameter("ApiId", apiId);
}

std::string DescribeApiHistoryRequest::getHistoryVersion()const
{
	return historyVersion_;
}

void DescribeApiHistoryRequest::setHistoryVersion(const std::string& historyVersion)
{
	historyVersion_ = historyVersion;
	setParameter("HistoryVersion", historyVersion);
}

