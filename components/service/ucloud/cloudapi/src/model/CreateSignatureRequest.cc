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

#include <alibabacloud/cloudapi/model/CreateSignatureRequest.h>

using AlibabaCloud::CloudAPI::Model::CreateSignatureRequest;

CreateSignatureRequest::CreateSignatureRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "CreateSignature")
{
	setMethod(HttpRequest::Method::Post);
}

CreateSignatureRequest::~CreateSignatureRequest()
{}

std::string CreateSignatureRequest::getSignatureName()const
{
	return signatureName_;
}

void CreateSignatureRequest::setSignatureName(const std::string& signatureName)
{
	signatureName_ = signatureName;
	setParameter("SignatureName", signatureName);
}

std::string CreateSignatureRequest::getSignatureSecret()const
{
	return signatureSecret_;
}

void CreateSignatureRequest::setSignatureSecret(const std::string& signatureSecret)
{
	signatureSecret_ = signatureSecret;
	setParameter("SignatureSecret", signatureSecret);
}

std::string CreateSignatureRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateSignatureRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateSignatureRequest::getSecurityToken()const
{
	return securityToken_;
}

void CreateSignatureRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string CreateSignatureRequest::getSignatureKey()const
{
	return signatureKey_;
}

void CreateSignatureRequest::setSignatureKey(const std::string& signatureKey)
{
	signatureKey_ = signatureKey;
	setParameter("SignatureKey", signatureKey);
}

