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

#include <alibabacloud/cloudapi/model/DeleteDomainRequest.h>

using AlibabaCloud::CloudAPI::Model::DeleteDomainRequest;

DeleteDomainRequest::DeleteDomainRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DeleteDomain")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteDomainRequest::~DeleteDomainRequest()
{}

std::string DeleteDomainRequest::getGroupId()const
{
	return groupId_;
}

void DeleteDomainRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string DeleteDomainRequest::getDomainName()const
{
	return domainName_;
}

void DeleteDomainRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string DeleteDomainRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DeleteDomainRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DeleteDomainRequest::getSecurityToken()const
{
	return securityToken_;
}

void DeleteDomainRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

