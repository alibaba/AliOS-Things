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

#include <alibabacloud/cloudapi/model/DescribeDomainRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeDomainRequest;

DescribeDomainRequest::DescribeDomainRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeDomain")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeDomainRequest::~DescribeDomainRequest()
{}

std::string DescribeDomainRequest::getGroupId()const
{
	return groupId_;
}

void DescribeDomainRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string DescribeDomainRequest::getDomainName()const
{
	return domainName_;
}

void DescribeDomainRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string DescribeDomainRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeDomainRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeDomainRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeDomainRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

