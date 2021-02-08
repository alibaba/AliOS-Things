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

#include <alibabacloud/cloudapi/model/SetWildcardDomainPatternsRequest.h>

using AlibabaCloud::CloudAPI::Model::SetWildcardDomainPatternsRequest;

SetWildcardDomainPatternsRequest::SetWildcardDomainPatternsRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "SetWildcardDomainPatterns")
{
	setMethod(HttpRequest::Method::Post);
}

SetWildcardDomainPatternsRequest::~SetWildcardDomainPatternsRequest()
{}

std::string SetWildcardDomainPatternsRequest::getGroupId()const
{
	return groupId_;
}

void SetWildcardDomainPatternsRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string SetWildcardDomainPatternsRequest::getDomainName()const
{
	return domainName_;
}

void SetWildcardDomainPatternsRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string SetWildcardDomainPatternsRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void SetWildcardDomainPatternsRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string SetWildcardDomainPatternsRequest::getSecurityToken()const
{
	return securityToken_;
}

void SetWildcardDomainPatternsRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string SetWildcardDomainPatternsRequest::getWildcardDomainPatterns()const
{
	return wildcardDomainPatterns_;
}

void SetWildcardDomainPatternsRequest::setWildcardDomainPatterns(const std::string& wildcardDomainPatterns)
{
	wildcardDomainPatterns_ = wildcardDomainPatterns;
	setParameter("WildcardDomainPatterns", wildcardDomainPatterns);
}

