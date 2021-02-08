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

#include <alibabacloud/live/model/DeleteLiveAppSnapshotConfigRequest.h>

using AlibabaCloud::Live::Model::DeleteLiveAppSnapshotConfigRequest;

DeleteLiveAppSnapshotConfigRequest::DeleteLiveAppSnapshotConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "DeleteLiveAppSnapshotConfig")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteLiveAppSnapshotConfigRequest::~DeleteLiveAppSnapshotConfigRequest()
{}

std::string DeleteLiveAppSnapshotConfigRequest::getAppName()const
{
	return appName_;
}

void DeleteLiveAppSnapshotConfigRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string DeleteLiveAppSnapshotConfigRequest::getSecurityToken()const
{
	return securityToken_;
}

void DeleteLiveAppSnapshotConfigRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string DeleteLiveAppSnapshotConfigRequest::getDomainName()const
{
	return domainName_;
}

void DeleteLiveAppSnapshotConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long DeleteLiveAppSnapshotConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void DeleteLiveAppSnapshotConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

