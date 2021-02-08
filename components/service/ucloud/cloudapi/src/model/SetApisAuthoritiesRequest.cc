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

#include <alibabacloud/cloudapi/model/SetApisAuthoritiesRequest.h>

using AlibabaCloud::CloudAPI::Model::SetApisAuthoritiesRequest;

SetApisAuthoritiesRequest::SetApisAuthoritiesRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "SetApisAuthorities")
{
	setMethod(HttpRequest::Method::Post);
}

SetApisAuthoritiesRequest::~SetApisAuthoritiesRequest()
{}

std::string SetApisAuthoritiesRequest::getAuthVaildTime()const
{
	return authVaildTime_;
}

void SetApisAuthoritiesRequest::setAuthVaildTime(const std::string& authVaildTime)
{
	authVaildTime_ = authVaildTime;
	setParameter("AuthVaildTime", authVaildTime);
}

std::string SetApisAuthoritiesRequest::getStageName()const
{
	return stageName_;
}

void SetApisAuthoritiesRequest::setStageName(const std::string& stageName)
{
	stageName_ = stageName;
	setParameter("StageName", stageName);
}

std::string SetApisAuthoritiesRequest::getGroupId()const
{
	return groupId_;
}

void SetApisAuthoritiesRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string SetApisAuthoritiesRequest::getDescription()const
{
	return description_;
}

void SetApisAuthoritiesRequest::setDescription(const std::string& description)
{
	description_ = description;
	setParameter("Description", description);
}

std::string SetApisAuthoritiesRequest::getAuthValidTime()const
{
	return authValidTime_;
}

void SetApisAuthoritiesRequest::setAuthValidTime(const std::string& authValidTime)
{
	authValidTime_ = authValidTime;
	setParameter("AuthValidTime", authValidTime);
}

std::string SetApisAuthoritiesRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void SetApisAuthoritiesRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string SetApisAuthoritiesRequest::getSecurityToken()const
{
	return securityToken_;
}

void SetApisAuthoritiesRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

long SetApisAuthoritiesRequest::getAppId()const
{
	return appId_;
}

void SetApisAuthoritiesRequest::setAppId(long appId)
{
	appId_ = appId;
	setParameter("AppId", std::to_string(appId));
}

std::string SetApisAuthoritiesRequest::getApiIds()const
{
	return apiIds_;
}

void SetApisAuthoritiesRequest::setApiIds(const std::string& apiIds)
{
	apiIds_ = apiIds;
	setParameter("ApiIds", apiIds);
}

