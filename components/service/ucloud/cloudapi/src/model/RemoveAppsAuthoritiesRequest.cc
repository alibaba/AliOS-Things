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

#include <alibabacloud/cloudapi/model/RemoveAppsAuthoritiesRequest.h>

using AlibabaCloud::CloudAPI::Model::RemoveAppsAuthoritiesRequest;

RemoveAppsAuthoritiesRequest::RemoveAppsAuthoritiesRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "RemoveAppsAuthorities")
{
	setMethod(HttpRequest::Method::Post);
}

RemoveAppsAuthoritiesRequest::~RemoveAppsAuthoritiesRequest()
{}

std::string RemoveAppsAuthoritiesRequest::getStageName()const
{
	return stageName_;
}

void RemoveAppsAuthoritiesRequest::setStageName(const std::string& stageName)
{
	stageName_ = stageName;
	setParameter("StageName", stageName);
}

std::string RemoveAppsAuthoritiesRequest::getGroupId()const
{
	return groupId_;
}

void RemoveAppsAuthoritiesRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string RemoveAppsAuthoritiesRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void RemoveAppsAuthoritiesRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string RemoveAppsAuthoritiesRequest::getAppIds()const
{
	return appIds_;
}

void RemoveAppsAuthoritiesRequest::setAppIds(const std::string& appIds)
{
	appIds_ = appIds;
	setParameter("AppIds", appIds);
}

std::string RemoveAppsAuthoritiesRequest::getSecurityToken()const
{
	return securityToken_;
}

void RemoveAppsAuthoritiesRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string RemoveAppsAuthoritiesRequest::getApiId()const
{
	return apiId_;
}

void RemoveAppsAuthoritiesRequest::setApiId(const std::string& apiId)
{
	apiId_ = apiId;
	setParameter("ApiId", apiId);
}

