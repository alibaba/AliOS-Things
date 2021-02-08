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

#include <alibabacloud/cloudapi/model/RemoveSignatureApisRequest.h>

using AlibabaCloud::CloudAPI::Model::RemoveSignatureApisRequest;

RemoveSignatureApisRequest::RemoveSignatureApisRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "RemoveSignatureApis")
{
	setMethod(HttpRequest::Method::Post);
}

RemoveSignatureApisRequest::~RemoveSignatureApisRequest()
{}

std::string RemoveSignatureApisRequest::getStageName()const
{
	return stageName_;
}

void RemoveSignatureApisRequest::setStageName(const std::string& stageName)
{
	stageName_ = stageName;
	setParameter("StageName", stageName);
}

std::string RemoveSignatureApisRequest::getGroupId()const
{
	return groupId_;
}

void RemoveSignatureApisRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string RemoveSignatureApisRequest::getSignatureId()const
{
	return signatureId_;
}

void RemoveSignatureApisRequest::setSignatureId(const std::string& signatureId)
{
	signatureId_ = signatureId;
	setParameter("SignatureId", signatureId);
}

std::string RemoveSignatureApisRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void RemoveSignatureApisRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string RemoveSignatureApisRequest::getSecurityToken()const
{
	return securityToken_;
}

void RemoveSignatureApisRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string RemoveSignatureApisRequest::getApiIds()const
{
	return apiIds_;
}

void RemoveSignatureApisRequest::setApiIds(const std::string& apiIds)
{
	apiIds_ = apiIds;
	setParameter("ApiIds", apiIds);
}

