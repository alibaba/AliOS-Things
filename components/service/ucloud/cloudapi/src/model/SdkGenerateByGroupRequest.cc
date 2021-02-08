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

#include <alibabacloud/cloudapi/model/SdkGenerateByGroupRequest.h>

using AlibabaCloud::CloudAPI::Model::SdkGenerateByGroupRequest;

SdkGenerateByGroupRequest::SdkGenerateByGroupRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "SdkGenerateByGroup")
{
	setMethod(HttpRequest::Method::Post);
}

SdkGenerateByGroupRequest::~SdkGenerateByGroupRequest()
{}

std::string SdkGenerateByGroupRequest::getGroupId()const
{
	return groupId_;
}

void SdkGenerateByGroupRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string SdkGenerateByGroupRequest::getLanguage()const
{
	return language_;
}

void SdkGenerateByGroupRequest::setLanguage(const std::string& language)
{
	language_ = language;
	setParameter("Language", language);
}

std::string SdkGenerateByGroupRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void SdkGenerateByGroupRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string SdkGenerateByGroupRequest::getSecurityToken()const
{
	return securityToken_;
}

void SdkGenerateByGroupRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

