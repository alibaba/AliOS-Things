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

#include <alibabacloud/cloudapi/model/AbolishApiRequest.h>

using AlibabaCloud::CloudAPI::Model::AbolishApiRequest;

AbolishApiRequest::AbolishApiRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "AbolishApi")
{
	setMethod(HttpRequest::Method::Post);
}

AbolishApiRequest::~AbolishApiRequest()
{}

std::string AbolishApiRequest::getStageName()const
{
	return stageName_;
}

void AbolishApiRequest::setStageName(const std::string& stageName)
{
	stageName_ = stageName;
	setParameter("StageName", stageName);
}

std::string AbolishApiRequest::getGroupId()const
{
	return groupId_;
}

void AbolishApiRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string AbolishApiRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void AbolishApiRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string AbolishApiRequest::getSecurityToken()const
{
	return securityToken_;
}

void AbolishApiRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string AbolishApiRequest::getApiId()const
{
	return apiId_;
}

void AbolishApiRequest::setApiId(const std::string& apiId)
{
	apiId_ = apiId;
	setParameter("ApiId", apiId);
}

