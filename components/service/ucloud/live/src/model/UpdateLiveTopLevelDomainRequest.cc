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

#include <alibabacloud/live/model/UpdateLiveTopLevelDomainRequest.h>

using AlibabaCloud::Live::Model::UpdateLiveTopLevelDomainRequest;

UpdateLiveTopLevelDomainRequest::UpdateLiveTopLevelDomainRequest() :
	RpcServiceRequest("live", "2016-11-01", "UpdateLiveTopLevelDomain")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateLiveTopLevelDomainRequest::~UpdateLiveTopLevelDomainRequest()
{}

std::string UpdateLiveTopLevelDomainRequest::getTopLevelDomain()const
{
	return topLevelDomain_;
}

void UpdateLiveTopLevelDomainRequest::setTopLevelDomain(const std::string& topLevelDomain)
{
	topLevelDomain_ = topLevelDomain;
	setParameter("TopLevelDomain", topLevelDomain);
}

std::string UpdateLiveTopLevelDomainRequest::getDomainName()const
{
	return domainName_;
}

void UpdateLiveTopLevelDomainRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long UpdateLiveTopLevelDomainRequest::getOwnerId()const
{
	return ownerId_;
}

void UpdateLiveTopLevelDomainRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string UpdateLiveTopLevelDomainRequest::getSecurityToken()const
{
	return securityToken_;
}

void UpdateLiveTopLevelDomainRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

