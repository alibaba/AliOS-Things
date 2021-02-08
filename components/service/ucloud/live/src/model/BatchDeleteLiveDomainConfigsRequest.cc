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

#include <alibabacloud/live/model/BatchDeleteLiveDomainConfigsRequest.h>

using AlibabaCloud::Live::Model::BatchDeleteLiveDomainConfigsRequest;

BatchDeleteLiveDomainConfigsRequest::BatchDeleteLiveDomainConfigsRequest() :
	RpcServiceRequest("live", "2016-11-01", "BatchDeleteLiveDomainConfigs")
{
	setMethod(HttpRequest::Method::Post);
}

BatchDeleteLiveDomainConfigsRequest::~BatchDeleteLiveDomainConfigsRequest()
{}

std::string BatchDeleteLiveDomainConfigsRequest::getFunctionNames()const
{
	return functionNames_;
}

void BatchDeleteLiveDomainConfigsRequest::setFunctionNames(const std::string& functionNames)
{
	functionNames_ = functionNames;
	setParameter("FunctionNames", functionNames);
}

std::string BatchDeleteLiveDomainConfigsRequest::getDomainNames()const
{
	return domainNames_;
}

void BatchDeleteLiveDomainConfigsRequest::setDomainNames(const std::string& domainNames)
{
	domainNames_ = domainNames;
	setParameter("DomainNames", domainNames);
}

std::string BatchDeleteLiveDomainConfigsRequest::getOwnerAccount()const
{
	return ownerAccount_;
}

void BatchDeleteLiveDomainConfigsRequest::setOwnerAccount(const std::string& ownerAccount)
{
	ownerAccount_ = ownerAccount;
	setParameter("OwnerAccount", ownerAccount);
}

long BatchDeleteLiveDomainConfigsRequest::getOwnerId()const
{
	return ownerId_;
}

void BatchDeleteLiveDomainConfigsRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string BatchDeleteLiveDomainConfigsRequest::getSecurityToken()const
{
	return securityToken_;
}

void BatchDeleteLiveDomainConfigsRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

