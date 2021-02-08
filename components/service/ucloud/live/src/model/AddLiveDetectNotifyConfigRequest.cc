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

#include <alibabacloud/live/model/AddLiveDetectNotifyConfigRequest.h>

using AlibabaCloud::Live::Model::AddLiveDetectNotifyConfigRequest;

AddLiveDetectNotifyConfigRequest::AddLiveDetectNotifyConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "AddLiveDetectNotifyConfig")
{
	setMethod(HttpRequest::Method::Post);
}

AddLiveDetectNotifyConfigRequest::~AddLiveDetectNotifyConfigRequest()
{}

std::string AddLiveDetectNotifyConfigRequest::getDomainName()const
{
	return domainName_;
}

void AddLiveDetectNotifyConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long AddLiveDetectNotifyConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void AddLiveDetectNotifyConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string AddLiveDetectNotifyConfigRequest::getSecurityToken()const
{
	return securityToken_;
}

void AddLiveDetectNotifyConfigRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string AddLiveDetectNotifyConfigRequest::getNotifyUrl()const
{
	return notifyUrl_;
}

void AddLiveDetectNotifyConfigRequest::setNotifyUrl(const std::string& notifyUrl)
{
	notifyUrl_ = notifyUrl;
	setParameter("NotifyUrl", notifyUrl);
}

