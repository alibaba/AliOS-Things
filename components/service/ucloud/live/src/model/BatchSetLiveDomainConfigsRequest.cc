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

#include <alibabacloud/live/model/BatchSetLiveDomainConfigsRequest.h>

using AlibabaCloud::Live::Model::BatchSetLiveDomainConfigsRequest;

BatchSetLiveDomainConfigsRequest::BatchSetLiveDomainConfigsRequest() :
	RpcServiceRequest("live", "2016-11-01", "BatchSetLiveDomainConfigs")
{
	setMethod(HttpRequest::Method::Post);
}

BatchSetLiveDomainConfigsRequest::~BatchSetLiveDomainConfigsRequest()
{}

std::string BatchSetLiveDomainConfigsRequest::getFunctions()const
{
	return functions_;
}

void BatchSetLiveDomainConfigsRequest::setFunctions(const std::string& functions)
{
	functions_ = functions;
	setParameter("Functions", functions);
}

std::string BatchSetLiveDomainConfigsRequest::getDomainNames()const
{
	return domainNames_;
}

void BatchSetLiveDomainConfigsRequest::setDomainNames(const std::string& domainNames)
{
	domainNames_ = domainNames;
	setParameter("DomainNames", domainNames);
}

std::string BatchSetLiveDomainConfigsRequest::getOwnerAccount()const
{
	return ownerAccount_;
}

void BatchSetLiveDomainConfigsRequest::setOwnerAccount(const std::string& ownerAccount)
{
	ownerAccount_ = ownerAccount;
	setParameter("OwnerAccount", ownerAccount);
}

long BatchSetLiveDomainConfigsRequest::getOwnerId()const
{
	return ownerId_;
}

void BatchSetLiveDomainConfigsRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string BatchSetLiveDomainConfigsRequest::getSecurityToken()const
{
	return securityToken_;
}

void BatchSetLiveDomainConfigsRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

