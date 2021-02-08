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

#include <alibabacloud/cloudapi/model/RemoveIpControlPolicyItemRequest.h>

using AlibabaCloud::CloudAPI::Model::RemoveIpControlPolicyItemRequest;

RemoveIpControlPolicyItemRequest::RemoveIpControlPolicyItemRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "RemoveIpControlPolicyItem")
{
	setMethod(HttpRequest::Method::Post);
}

RemoveIpControlPolicyItemRequest::~RemoveIpControlPolicyItemRequest()
{}

std::string RemoveIpControlPolicyItemRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void RemoveIpControlPolicyItemRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string RemoveIpControlPolicyItemRequest::getPolicyItemIds()const
{
	return policyItemIds_;
}

void RemoveIpControlPolicyItemRequest::setPolicyItemIds(const std::string& policyItemIds)
{
	policyItemIds_ = policyItemIds;
	setParameter("PolicyItemIds", policyItemIds);
}

std::string RemoveIpControlPolicyItemRequest::getIpControlId()const
{
	return ipControlId_;
}

void RemoveIpControlPolicyItemRequest::setIpControlId(const std::string& ipControlId)
{
	ipControlId_ = ipControlId;
	setParameter("IpControlId", ipControlId);
}

std::string RemoveIpControlPolicyItemRequest::getSecurityToken()const
{
	return securityToken_;
}

void RemoveIpControlPolicyItemRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

