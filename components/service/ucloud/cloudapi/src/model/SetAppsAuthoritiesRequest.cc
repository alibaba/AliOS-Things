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

#include <alibabacloud/cloudapi/model/SetAppsAuthoritiesRequest.h>

using AlibabaCloud::CloudAPI::Model::SetAppsAuthoritiesRequest;

SetAppsAuthoritiesRequest::SetAppsAuthoritiesRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "SetAppsAuthorities")
{
	setMethod(HttpRequest::Method::Post);
}

SetAppsAuthoritiesRequest::~SetAppsAuthoritiesRequest()
{}

std::string SetAppsAuthoritiesRequest::getAuthVaildTime()const
{
	return authVaildTime_;
}

void SetAppsAuthoritiesRequest::setAuthVaildTime(const std::string& authVaildTime)
{
	authVaildTime_ = authVaildTime;
	setParameter("AuthVaildTime", authVaildTime);
}

std::string SetAppsAuthoritiesRequest::getStageName()const
{
	return stageName_;
}

void SetAppsAuthoritiesRequest::setStageName(const std::string& stageName)
{
	stageName_ = stageName;
	setParameter("StageName", stageName);
}

std::string SetAppsAuthoritiesRequest::getGroupId()const
{
	return groupId_;
}

void SetAppsAuthoritiesRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string SetAppsAuthoritiesRequest::getDescription()const
{
	return description_;
}

void SetAppsAuthoritiesRequest::setDescription(const std::string& description)
{
	description_ = description;
	setParameter("Description", description);
}

std::string SetAppsAuthoritiesRequest::getAuthValidTime()const
{
	return authValidTime_;
}

void SetAppsAuthoritiesRequest::setAuthValidTime(const std::string& authValidTime)
{
	authValidTime_ = authValidTime;
	setParameter("AuthValidTime", authValidTime);
}

std::string SetAppsAuthoritiesRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void SetAppsAuthoritiesRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string SetAppsAuthoritiesRequest::getAppIds()const
{
	return appIds_;
}

void SetAppsAuthoritiesRequest::setAppIds(const std::string& appIds)
{
	appIds_ = appIds;
	setParameter("AppIds", appIds);
}

std::string SetAppsAuthoritiesRequest::getSecurityToken()const
{
	return securityToken_;
}

void SetAppsAuthoritiesRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string SetAppsAuthoritiesRequest::getApiId()const
{
	return apiId_;
}

void SetAppsAuthoritiesRequest::setApiId(const std::string& apiId)
{
	apiId_ = apiId;
	setParameter("ApiId", apiId);
}

