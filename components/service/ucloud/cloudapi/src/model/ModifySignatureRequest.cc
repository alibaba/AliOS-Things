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

#include <alibabacloud/cloudapi/model/ModifySignatureRequest.h>

using AlibabaCloud::CloudAPI::Model::ModifySignatureRequest;

ModifySignatureRequest::ModifySignatureRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "ModifySignature")
{
	setMethod(HttpRequest::Method::Post);
}

ModifySignatureRequest::~ModifySignatureRequest()
{}

std::string ModifySignatureRequest::getSignatureName()const
{
	return signatureName_;
}

void ModifySignatureRequest::setSignatureName(const std::string& signatureName)
{
	signatureName_ = signatureName;
	setParameter("SignatureName", signatureName);
}

std::string ModifySignatureRequest::getSignatureId()const
{
	return signatureId_;
}

void ModifySignatureRequest::setSignatureId(const std::string& signatureId)
{
	signatureId_ = signatureId;
	setParameter("SignatureId", signatureId);
}

std::string ModifySignatureRequest::getSignatureSecret()const
{
	return signatureSecret_;
}

void ModifySignatureRequest::setSignatureSecret(const std::string& signatureSecret)
{
	signatureSecret_ = signatureSecret;
	setParameter("SignatureSecret", signatureSecret);
}

std::string ModifySignatureRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ModifySignatureRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ModifySignatureRequest::getSecurityToken()const
{
	return securityToken_;
}

void ModifySignatureRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string ModifySignatureRequest::getSignatureKey()const
{
	return signatureKey_;
}

void ModifySignatureRequest::setSignatureKey(const std::string& signatureKey)
{
	signatureKey_ = signatureKey;
	setParameter("SignatureKey", signatureKey);
}

