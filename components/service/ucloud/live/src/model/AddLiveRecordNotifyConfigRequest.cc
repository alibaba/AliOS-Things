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

#include <alibabacloud/live/model/AddLiveRecordNotifyConfigRequest.h>

using AlibabaCloud::Live::Model::AddLiveRecordNotifyConfigRequest;

AddLiveRecordNotifyConfigRequest::AddLiveRecordNotifyConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "AddLiveRecordNotifyConfig")
{
	setMethod(HttpRequest::Method::Post);
}

AddLiveRecordNotifyConfigRequest::~AddLiveRecordNotifyConfigRequest()
{}

std::string AddLiveRecordNotifyConfigRequest::getOnDemandUrl()const
{
	return onDemandUrl_;
}

void AddLiveRecordNotifyConfigRequest::setOnDemandUrl(const std::string& onDemandUrl)
{
	onDemandUrl_ = onDemandUrl;
	setParameter("OnDemandUrl", onDemandUrl);
}

std::string AddLiveRecordNotifyConfigRequest::getSecurityToken()const
{
	return securityToken_;
}

void AddLiveRecordNotifyConfigRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string AddLiveRecordNotifyConfigRequest::getNotifyUrl()const
{
	return notifyUrl_;
}

void AddLiveRecordNotifyConfigRequest::setNotifyUrl(const std::string& notifyUrl)
{
	notifyUrl_ = notifyUrl;
	setParameter("NotifyUrl", notifyUrl);
}

bool AddLiveRecordNotifyConfigRequest::getNeedStatusNotify()const
{
	return needStatusNotify_;
}

void AddLiveRecordNotifyConfigRequest::setNeedStatusNotify(bool needStatusNotify)
{
	needStatusNotify_ = needStatusNotify;
	setParameter("NeedStatusNotify", needStatusNotify ? "true" : "false");
}

std::string AddLiveRecordNotifyConfigRequest::getDomainName()const
{
	return domainName_;
}

void AddLiveRecordNotifyConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long AddLiveRecordNotifyConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void AddLiveRecordNotifyConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

