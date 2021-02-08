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

#include <alibabacloud/cloudapi/model/DescribeDomainsResolutionRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeDomainsResolutionRequest;

DescribeDomainsResolutionRequest::DescribeDomainsResolutionRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeDomainsResolution")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeDomainsResolutionRequest::~DescribeDomainsResolutionRequest()
{}

std::string DescribeDomainsResolutionRequest::getDomainNames()const
{
	return domainNames_;
}

void DescribeDomainsResolutionRequest::setDomainNames(const std::string& domainNames)
{
	domainNames_ = domainNames;
	setParameter("DomainNames", domainNames);
}

std::string DescribeDomainsResolutionRequest::getGroupId()const
{
	return groupId_;
}

void DescribeDomainsResolutionRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string DescribeDomainsResolutionRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeDomainsResolutionRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeDomainsResolutionRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeDomainsResolutionRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

