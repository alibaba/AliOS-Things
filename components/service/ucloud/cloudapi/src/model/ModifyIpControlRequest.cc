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

#include <alibabacloud/cloudapi/model/ModifyIpControlRequest.h>

using AlibabaCloud::CloudAPI::Model::ModifyIpControlRequest;

ModifyIpControlRequest::ModifyIpControlRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "ModifyIpControl")
{
	setMethod(HttpRequest::Method::Post);
}

ModifyIpControlRequest::~ModifyIpControlRequest()
{}

std::string ModifyIpControlRequest::getIpControlName()const
{
	return ipControlName_;
}

void ModifyIpControlRequest::setIpControlName(const std::string& ipControlName)
{
	ipControlName_ = ipControlName;
	setParameter("IpControlName", ipControlName);
}

std::string ModifyIpControlRequest::getDescription()const
{
	return description_;
}

void ModifyIpControlRequest::setDescription(const std::string& description)
{
	description_ = description;
	setParameter("Description", description);
}

std::string ModifyIpControlRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ModifyIpControlRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ModifyIpControlRequest::getIpControlId()const
{
	return ipControlId_;
}

void ModifyIpControlRequest::setIpControlId(const std::string& ipControlId)
{
	ipControlId_ = ipControlId;
	setParameter("IpControlId", ipControlId);
}

std::string ModifyIpControlRequest::getSecurityToken()const
{
	return securityToken_;
}

void ModifyIpControlRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

