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

#include <alibabacloud/cloudapi/model/CreateIntranetDomainRequest.h>

using AlibabaCloud::CloudAPI::Model::CreateIntranetDomainRequest;

CreateIntranetDomainRequest::CreateIntranetDomainRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "CreateIntranetDomain")
{
	setMethod(HttpRequest::Method::Post);
}

CreateIntranetDomainRequest::~CreateIntranetDomainRequest()
{}

std::string CreateIntranetDomainRequest::getGroupId()const
{
	return groupId_;
}

void CreateIntranetDomainRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string CreateIntranetDomainRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateIntranetDomainRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

bool CreateIntranetDomainRequest::getDeleteInternetDomain()const
{
	return deleteInternetDomain_;
}

void CreateIntranetDomainRequest::setDeleteInternetDomain(bool deleteInternetDomain)
{
	deleteInternetDomain_ = deleteInternetDomain;
	setParameter("DeleteInternetDomain", deleteInternetDomain ? "true" : "false");
}

std::string CreateIntranetDomainRequest::getSecurityToken()const
{
	return securityToken_;
}

void CreateIntranetDomainRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

