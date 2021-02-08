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

#include <alibabacloud/cloudapi/model/DescribeDeployedApiRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeDeployedApiRequest;

DescribeDeployedApiRequest::DescribeDeployedApiRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeDeployedApi")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeDeployedApiRequest::~DescribeDeployedApiRequest()
{}

std::string DescribeDeployedApiRequest::getStageName()const
{
	return stageName_;
}

void DescribeDeployedApiRequest::setStageName(const std::string& stageName)
{
	stageName_ = stageName;
	setParameter("StageName", stageName);
}

std::string DescribeDeployedApiRequest::getGroupId()const
{
	return groupId_;
}

void DescribeDeployedApiRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string DescribeDeployedApiRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeDeployedApiRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeDeployedApiRequest::getResourceOwnerToken()const
{
	return resourceOwnerToken_;
}

void DescribeDeployedApiRequest::setResourceOwnerToken(const std::string& resourceOwnerToken)
{
	resourceOwnerToken_ = resourceOwnerToken;
	setParameter("ResourceOwnerToken", resourceOwnerToken);
}

std::string DescribeDeployedApiRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeDeployedApiRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string DescribeDeployedApiRequest::getApiId()const
{
	return apiId_;
}

void DescribeDeployedApiRequest::setApiId(const std::string& apiId)
{
	apiId_ = apiId;
	setParameter("ApiId", apiId);
}

