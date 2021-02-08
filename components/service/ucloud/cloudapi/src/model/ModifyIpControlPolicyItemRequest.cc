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

#include <alibabacloud/cloudapi/model/ModifyIpControlPolicyItemRequest.h>

using AlibabaCloud::CloudAPI::Model::ModifyIpControlPolicyItemRequest;

ModifyIpControlPolicyItemRequest::ModifyIpControlPolicyItemRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "ModifyIpControlPolicyItem")
{
	setMethod(HttpRequest::Method::Post);
}

ModifyIpControlPolicyItemRequest::~ModifyIpControlPolicyItemRequest()
{}

std::string ModifyIpControlPolicyItemRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ModifyIpControlPolicyItemRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ModifyIpControlPolicyItemRequest::getIpControlId()const
{
	return ipControlId_;
}

void ModifyIpControlPolicyItemRequest::setIpControlId(const std::string& ipControlId)
{
	ipControlId_ = ipControlId;
	setParameter("IpControlId", ipControlId);
}

std::string ModifyIpControlPolicyItemRequest::getPolicyItemId()const
{
	return policyItemId_;
}

void ModifyIpControlPolicyItemRequest::setPolicyItemId(const std::string& policyItemId)
{
	policyItemId_ = policyItemId;
	setParameter("PolicyItemId", policyItemId);
}

std::string ModifyIpControlPolicyItemRequest::getSecurityToken()const
{
	return securityToken_;
}

void ModifyIpControlPolicyItemRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string ModifyIpControlPolicyItemRequest::getAppId()const
{
	return appId_;
}

void ModifyIpControlPolicyItemRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string ModifyIpControlPolicyItemRequest::getCidrIp()const
{
	return cidrIp_;
}

void ModifyIpControlPolicyItemRequest::setCidrIp(const std::string& cidrIp)
{
	cidrIp_ = cidrIp;
	setParameter("CidrIp", cidrIp);
}

