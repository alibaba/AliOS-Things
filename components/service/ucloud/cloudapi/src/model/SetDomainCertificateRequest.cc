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

#include <alibabacloud/cloudapi/model/SetDomainCertificateRequest.h>

using AlibabaCloud::CloudAPI::Model::SetDomainCertificateRequest;

SetDomainCertificateRequest::SetDomainCertificateRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "SetDomainCertificate")
{
	setMethod(HttpRequest::Method::Post);
}

SetDomainCertificateRequest::~SetDomainCertificateRequest()
{}

std::string SetDomainCertificateRequest::getCertificatePrivateKey()const
{
	return certificatePrivateKey_;
}

void SetDomainCertificateRequest::setCertificatePrivateKey(const std::string& certificatePrivateKey)
{
	certificatePrivateKey_ = certificatePrivateKey;
	setParameter("CertificatePrivateKey", certificatePrivateKey);
}

std::string SetDomainCertificateRequest::getGroupId()const
{
	return groupId_;
}

void SetDomainCertificateRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string SetDomainCertificateRequest::getDomainName()const
{
	return domainName_;
}

void SetDomainCertificateRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string SetDomainCertificateRequest::getCertificateBody()const
{
	return certificateBody_;
}

void SetDomainCertificateRequest::setCertificateBody(const std::string& certificateBody)
{
	certificateBody_ = certificateBody;
	setParameter("CertificateBody", certificateBody);
}

std::string SetDomainCertificateRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void SetDomainCertificateRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string SetDomainCertificateRequest::getCaCertificateBody()const
{
	return caCertificateBody_;
}

void SetDomainCertificateRequest::setCaCertificateBody(const std::string& caCertificateBody)
{
	caCertificateBody_ = caCertificateBody;
	setParameter("CaCertificateBody", caCertificateBody);
}

std::string SetDomainCertificateRequest::getSecurityToken()const
{
	return securityToken_;
}

void SetDomainCertificateRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string SetDomainCertificateRequest::getCertificateName()const
{
	return certificateName_;
}

void SetDomainCertificateRequest::setCertificateName(const std::string& certificateName)
{
	certificateName_ = certificateName;
	setParameter("CertificateName", certificateName);
}

