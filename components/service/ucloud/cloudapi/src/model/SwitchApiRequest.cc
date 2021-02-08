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

#include <alibabacloud/cloudapi/model/SwitchApiRequest.h>

using AlibabaCloud::CloudAPI::Model::SwitchApiRequest;

SwitchApiRequest::SwitchApiRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "SwitchApi")
{
	setMethod(HttpRequest::Method::Post);
}

SwitchApiRequest::~SwitchApiRequest()
{}

std::string SwitchApiRequest::getStageName()const
{
	return stageName_;
}

void SwitchApiRequest::setStageName(const std::string& stageName)
{
	stageName_ = stageName;
	setParameter("StageName", stageName);
}

std::string SwitchApiRequest::getGroupId()const
{
	return groupId_;
}

void SwitchApiRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string SwitchApiRequest::getDescription()const
{
	return description_;
}

void SwitchApiRequest::setDescription(const std::string& description)
{
	description_ = description;
	setParameter("Description", description);
}

std::string SwitchApiRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void SwitchApiRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string SwitchApiRequest::getSecurityToken()const
{
	return securityToken_;
}

void SwitchApiRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string SwitchApiRequest::getApiId()const
{
	return apiId_;
}

void SwitchApiRequest::setApiId(const std::string& apiId)
{
	apiId_ = apiId;
	setParameter("ApiId", apiId);
}

std::string SwitchApiRequest::getHistoryVersion()const
{
	return historyVersion_;
}

void SwitchApiRequest::setHistoryVersion(const std::string& historyVersion)
{
	historyVersion_ = historyVersion;
	setParameter("HistoryVersion", historyVersion);
}

