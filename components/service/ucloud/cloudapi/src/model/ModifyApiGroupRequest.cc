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

#include <alibabacloud/cloudapi/model/ModifyApiGroupRequest.h>

using AlibabaCloud::CloudAPI::Model::ModifyApiGroupRequest;

ModifyApiGroupRequest::ModifyApiGroupRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "ModifyApiGroup")
{
	setMethod(HttpRequest::Method::Post);
}

ModifyApiGroupRequest::~ModifyApiGroupRequest()
{}

std::string ModifyApiGroupRequest::getGroupId()const
{
	return groupId_;
}

void ModifyApiGroupRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string ModifyApiGroupRequest::getDescription()const
{
	return description_;
}

void ModifyApiGroupRequest::setDescription(const std::string& description)
{
	description_ = description;
	setParameter("Description", description);
}

std::string ModifyApiGroupRequest::getGroupName()const
{
	return groupName_;
}

void ModifyApiGroupRequest::setGroupName(const std::string& groupName)
{
	groupName_ = groupName;
	setParameter("GroupName", groupName);
}

std::string ModifyApiGroupRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ModifyApiGroupRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ModifyApiGroupRequest::getSecurityToken()const
{
	return securityToken_;
}

void ModifyApiGroupRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string ModifyApiGroupRequest::getCompatibleFlags()const
{
	return compatibleFlags_;
}

void ModifyApiGroupRequest::setCompatibleFlags(const std::string& compatibleFlags)
{
	compatibleFlags_ = compatibleFlags;
	setParameter("CompatibleFlags", compatibleFlags);
}

std::string ModifyApiGroupRequest::getRpcPattern()const
{
	return rpcPattern_;
}

void ModifyApiGroupRequest::setRpcPattern(const std::string& rpcPattern)
{
	rpcPattern_ = rpcPattern;
	setParameter("RpcPattern", rpcPattern);
}

std::string ModifyApiGroupRequest::getUserLogConfig()const
{
	return userLogConfig_;
}

void ModifyApiGroupRequest::setUserLogConfig(const std::string& userLogConfig)
{
	userLogConfig_ = userLogConfig;
	setParameter("UserLogConfig", userLogConfig);
}

std::vector<ModifyApiGroupRequest::Tag> ModifyApiGroupRequest::getTag()const
{
	return tag_;
}

void ModifyApiGroupRequest::setTag(const std::vector<Tag>& tag)
{
	tag_ = tag;
	for(int dep1 = 0; dep1!= tag.size(); dep1++) {
		auto tagObj = tag.at(dep1);
		std::string tagObjStr = "Tag." + std::to_string(dep1 + 1);
		setParameter(tagObjStr + ".Value", tagObj.value);
		setParameter(tagObjStr + ".Key", tagObj.key);
	}
}

std::string ModifyApiGroupRequest::getCustomTraceConfig()const
{
	return customTraceConfig_;
}

void ModifyApiGroupRequest::setCustomTraceConfig(const std::string& customTraceConfig)
{
	customTraceConfig_ = customTraceConfig;
	setParameter("CustomTraceConfig", customTraceConfig);
}

