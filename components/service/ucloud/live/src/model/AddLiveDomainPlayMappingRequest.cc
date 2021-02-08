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

#include <alibabacloud/live/model/AddLiveDomainPlayMappingRequest.h>

using AlibabaCloud::Live::Model::AddLiveDomainPlayMappingRequest;

AddLiveDomainPlayMappingRequest::AddLiveDomainPlayMappingRequest() :
	RpcServiceRequest("live", "2016-11-01", "AddLiveDomainPlayMapping")
{
	setMethod(HttpRequest::Method::Post);
}

AddLiveDomainPlayMappingRequest::~AddLiveDomainPlayMappingRequest()
{}

std::string AddLiveDomainPlayMappingRequest::getPlayDomain()const
{
	return playDomain_;
}

void AddLiveDomainPlayMappingRequest::setPlayDomain(const std::string& playDomain)
{
	playDomain_ = playDomain;
	setParameter("PlayDomain", playDomain);
}

long AddLiveDomainPlayMappingRequest::getOwnerId()const
{
	return ownerId_;
}

void AddLiveDomainPlayMappingRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string AddLiveDomainPlayMappingRequest::getPullDomain()const
{
	return pullDomain_;
}

void AddLiveDomainPlayMappingRequest::setPullDomain(const std::string& pullDomain)
{
	pullDomain_ = pullDomain;
	setParameter("PullDomain", pullDomain);
}

