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

#include <alibabacloud/live/model/DeleteLiveDomainMappingRequest.h>

using AlibabaCloud::Live::Model::DeleteLiveDomainMappingRequest;

DeleteLiveDomainMappingRequest::DeleteLiveDomainMappingRequest() :
	RpcServiceRequest("live", "2016-11-01", "DeleteLiveDomainMapping")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteLiveDomainMappingRequest::~DeleteLiveDomainMappingRequest()
{}

long DeleteLiveDomainMappingRequest::getOwnerId()const
{
	return ownerId_;
}

void DeleteLiveDomainMappingRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DeleteLiveDomainMappingRequest::getPullDomain()const
{
	return pullDomain_;
}

void DeleteLiveDomainMappingRequest::setPullDomain(const std::string& pullDomain)
{
	pullDomain_ = pullDomain;
	setParameter("PullDomain", pullDomain);
}

std::string DeleteLiveDomainMappingRequest::getSecurityToken()const
{
	return securityToken_;
}

void DeleteLiveDomainMappingRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string DeleteLiveDomainMappingRequest::getPushDomain()const
{
	return pushDomain_;
}

void DeleteLiveDomainMappingRequest::setPushDomain(const std::string& pushDomain)
{
	pushDomain_ = pushDomain;
	setParameter("PushDomain", pushDomain);
}

