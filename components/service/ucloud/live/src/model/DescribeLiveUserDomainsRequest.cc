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

#include <alibabacloud/live/model/DescribeLiveUserDomainsRequest.h>

using AlibabaCloud::Live::Model::DescribeLiveUserDomainsRequest;

DescribeLiveUserDomainsRequest::DescribeLiveUserDomainsRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeLiveUserDomains")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeLiveUserDomainsRequest::~DescribeLiveUserDomainsRequest()
{}

int DescribeLiveUserDomainsRequest::getPageNumber()const
{
	return pageNumber_;
}

void DescribeLiveUserDomainsRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

std::string DescribeLiveUserDomainsRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeLiveUserDomainsRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

int DescribeLiveUserDomainsRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeLiveUserDomainsRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeLiveUserDomainsRequest::getRegionName()const
{
	return regionName_;
}

void DescribeLiveUserDomainsRequest::setRegionName(const std::string& regionName)
{
	regionName_ = regionName;
	setParameter("RegionName", regionName);
}

std::vector<DescribeLiveUserDomainsRequest::Tag> DescribeLiveUserDomainsRequest::getTag()const
{
	return tag_;
}

void DescribeLiveUserDomainsRequest::setTag(const std::vector<Tag>& tag)
{
	tag_ = tag;
	for(int dep1 = 0; dep1!= tag.size(); dep1++) {
		auto tagObj = tag.at(dep1);
		std::string tagObjStr = "Tag." + std::to_string(dep1 + 1);
		setParameter(tagObjStr + ".Value", tagObj.value);
		setParameter(tagObjStr + ".Key", tagObj.key);
	}
}

std::string DescribeLiveUserDomainsRequest::getDomainName()const
{
	return domainName_;
}

void DescribeLiveUserDomainsRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long DescribeLiveUserDomainsRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeLiveUserDomainsRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeLiveUserDomainsRequest::getDomainStatus()const
{
	return domainStatus_;
}

void DescribeLiveUserDomainsRequest::setDomainStatus(const std::string& domainStatus)
{
	domainStatus_ = domainStatus;
	setParameter("DomainStatus", domainStatus);
}

std::string DescribeLiveUserDomainsRequest::getDomainSearchType()const
{
	return domainSearchType_;
}

void DescribeLiveUserDomainsRequest::setDomainSearchType(const std::string& domainSearchType)
{
	domainSearchType_ = domainSearchType;
	setParameter("DomainSearchType", domainSearchType);
}

std::string DescribeLiveUserDomainsRequest::getLiveDomainType()const
{
	return liveDomainType_;
}

void DescribeLiveUserDomainsRequest::setLiveDomainType(const std::string& liveDomainType)
{
	liveDomainType_ = liveDomainType;
	setParameter("LiveDomainType", liveDomainType);
}

