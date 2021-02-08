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

#include <alibabacloud/cloudapi/model/ReactivateDomainRequest.h>

using AlibabaCloud::CloudAPI::Model::ReactivateDomainRequest;

ReactivateDomainRequest::ReactivateDomainRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "ReactivateDomain")
{
	setMethod(HttpRequest::Method::Post);
}

ReactivateDomainRequest::~ReactivateDomainRequest()
{}

std::string ReactivateDomainRequest::getGroupId()const
{
	return groupId_;
}

void ReactivateDomainRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string ReactivateDomainRequest::getDomainName()const
{
	return domainName_;
}

void ReactivateDomainRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string ReactivateDomainRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ReactivateDomainRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ReactivateDomainRequest::getSecurityToken()const
{
	return securityToken_;
}

void ReactivateDomainRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

