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

#include <alibabacloud/cloudapi/model/DeleteDomainCertificateRequest.h>

using AlibabaCloud::CloudAPI::Model::DeleteDomainCertificateRequest;

DeleteDomainCertificateRequest::DeleteDomainCertificateRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DeleteDomainCertificate")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteDomainCertificateRequest::~DeleteDomainCertificateRequest()
{}

std::string DeleteDomainCertificateRequest::getGroupId()const
{
	return groupId_;
}

void DeleteDomainCertificateRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string DeleteDomainCertificateRequest::getCertificateId()const
{
	return certificateId_;
}

void DeleteDomainCertificateRequest::setCertificateId(const std::string& certificateId)
{
	certificateId_ = certificateId;
	setParameter("CertificateId", certificateId);
}

std::string DeleteDomainCertificateRequest::getDomainName()const
{
	return domainName_;
}

void DeleteDomainCertificateRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string DeleteDomainCertificateRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DeleteDomainCertificateRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DeleteDomainCertificateRequest::getSecurityToken()const
{
	return securityToken_;
}

void DeleteDomainCertificateRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

