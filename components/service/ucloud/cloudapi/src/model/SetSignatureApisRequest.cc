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

#include <alibabacloud/cloudapi/model/SetSignatureApisRequest.h>

using AlibabaCloud::CloudAPI::Model::SetSignatureApisRequest;

SetSignatureApisRequest::SetSignatureApisRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "SetSignatureApis")
{
	setMethod(HttpRequest::Method::Post);
}

SetSignatureApisRequest::~SetSignatureApisRequest()
{}

std::string SetSignatureApisRequest::getStageName()const
{
	return stageName_;
}

void SetSignatureApisRequest::setStageName(const std::string& stageName)
{
	stageName_ = stageName;
	setParameter("StageName", stageName);
}

std::string SetSignatureApisRequest::getGroupId()const
{
	return groupId_;
}

void SetSignatureApisRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string SetSignatureApisRequest::getSignatureId()const
{
	return signatureId_;
}

void SetSignatureApisRequest::setSignatureId(const std::string& signatureId)
{
	signatureId_ = signatureId;
	setParameter("SignatureId", signatureId);
}

std::string SetSignatureApisRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void SetSignatureApisRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string SetSignatureApisRequest::getSecurityToken()const
{
	return securityToken_;
}

void SetSignatureApisRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string SetSignatureApisRequest::getApiIds()const
{
	return apiIds_;
}

void SetSignatureApisRequest::setApiIds(const std::string& apiIds)
{
	apiIds_ = apiIds;
	setParameter("ApiIds", apiIds);
}

