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

#include <alibabacloud/cloudapi/model/CreateApiGroupRequest.h>

using AlibabaCloud::CloudAPI::Model::CreateApiGroupRequest;

CreateApiGroupRequest::CreateApiGroupRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "CreateApiGroup")
{
	setMethod(HttpRequest::Method::Post);
}

CreateApiGroupRequest::~CreateApiGroupRequest()
{}

std::string CreateApiGroupRequest::getDescription()const
{
	return description_;
}

void CreateApiGroupRequest::setDescription(const std::string& description)
{
	description_ = description;
	setParameter("Description", description);
}

std::string CreateApiGroupRequest::getSource()const
{
	return source_;
}

void CreateApiGroupRequest::setSource(const std::string& source)
{
	source_ = source;
	setParameter("Source", source);
}

std::string CreateApiGroupRequest::getGroupName()const
{
	return groupName_;
}

void CreateApiGroupRequest::setGroupName(const std::string& groupName)
{
	groupName_ = groupName;
	setParameter("GroupName", groupName);
}

std::string CreateApiGroupRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateApiGroupRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateApiGroupRequest::getResourceOwnerToken()const
{
	return resourceOwnerToken_;
}

void CreateApiGroupRequest::setResourceOwnerToken(const std::string& resourceOwnerToken)
{
	resourceOwnerToken_ = resourceOwnerToken;
	setParameter("ResourceOwnerToken", resourceOwnerToken);
}

std::string CreateApiGroupRequest::getInstanceId()const
{
	return instanceId_;
}

void CreateApiGroupRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string CreateApiGroupRequest::getSecurityToken()const
{
	return securityToken_;
}

void CreateApiGroupRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::vector<CreateApiGroupRequest::Tag> CreateApiGroupRequest::getTag()const
{
	return tag_;
}

void CreateApiGroupRequest::setTag(const std::vector<Tag>& tag)
{
	tag_ = tag;
	for(int dep1 = 0; dep1!= tag.size(); dep1++) {
		auto tagObj = tag.at(dep1);
		std::string tagObjStr = "Tag." + std::to_string(dep1 + 1);
		setParameter(tagObjStr + ".Value", tagObj.value);
		setParameter(tagObjStr + ".Key", tagObj.key);
	}
}

