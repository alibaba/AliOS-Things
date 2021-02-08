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

#include <alibabacloud/cloudapi/model/DeployApiRequest.h>

using AlibabaCloud::CloudAPI::Model::DeployApiRequest;

DeployApiRequest::DeployApiRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DeployApi")
{
	setMethod(HttpRequest::Method::Post);
}

DeployApiRequest::~DeployApiRequest()
{}

std::string DeployApiRequest::getStageName()const
{
	return stageName_;
}

void DeployApiRequest::setStageName(const std::string& stageName)
{
	stageName_ = stageName;
	setParameter("StageName", stageName);
}

std::string DeployApiRequest::getGroupId()const
{
	return groupId_;
}

void DeployApiRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string DeployApiRequest::getDescription()const
{
	return description_;
}

void DeployApiRequest::setDescription(const std::string& description)
{
	description_ = description;
	setParameter("Description", description);
}

std::string DeployApiRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DeployApiRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DeployApiRequest::getResourceOwnerToken()const
{
	return resourceOwnerToken_;
}

void DeployApiRequest::setResourceOwnerToken(const std::string& resourceOwnerToken)
{
	resourceOwnerToken_ = resourceOwnerToken;
	setParameter("ResourceOwnerToken", resourceOwnerToken);
}

std::string DeployApiRequest::getSecurityToken()const
{
	return securityToken_;
}

void DeployApiRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string DeployApiRequest::getApiId()const
{
	return apiId_;
}

void DeployApiRequest::setApiId(const std::string& apiId)
{
	apiId_ = apiId;
	setParameter("ApiId", apiId);
}

