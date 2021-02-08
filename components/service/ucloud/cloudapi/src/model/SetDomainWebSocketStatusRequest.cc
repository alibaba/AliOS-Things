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

#include <alibabacloud/cloudapi/model/SetDomainWebSocketStatusRequest.h>

using AlibabaCloud::CloudAPI::Model::SetDomainWebSocketStatusRequest;

SetDomainWebSocketStatusRequest::SetDomainWebSocketStatusRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "SetDomainWebSocketStatus")
{
	setMethod(HttpRequest::Method::Post);
}

SetDomainWebSocketStatusRequest::~SetDomainWebSocketStatusRequest()
{}

std::string SetDomainWebSocketStatusRequest::getGroupId()const
{
	return groupId_;
}

void SetDomainWebSocketStatusRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string SetDomainWebSocketStatusRequest::getDomainName()const
{
	return domainName_;
}

void SetDomainWebSocketStatusRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string SetDomainWebSocketStatusRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void SetDomainWebSocketStatusRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string SetDomainWebSocketStatusRequest::getSecurityToken()const
{
	return securityToken_;
}

void SetDomainWebSocketStatusRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string SetDomainWebSocketStatusRequest::getActionValue()const
{
	return actionValue_;
}

void SetDomainWebSocketStatusRequest::setActionValue(const std::string& actionValue)
{
	actionValue_ = actionValue;
	setParameter("ActionValue", actionValue);
}

