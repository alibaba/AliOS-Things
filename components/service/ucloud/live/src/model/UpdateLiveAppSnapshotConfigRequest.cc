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

#include <alibabacloud/live/model/UpdateLiveAppSnapshotConfigRequest.h>

using AlibabaCloud::Live::Model::UpdateLiveAppSnapshotConfigRequest;

UpdateLiveAppSnapshotConfigRequest::UpdateLiveAppSnapshotConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "UpdateLiveAppSnapshotConfig")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateLiveAppSnapshotConfigRequest::~UpdateLiveAppSnapshotConfigRequest()
{}

int UpdateLiveAppSnapshotConfigRequest::getTimeInterval()const
{
	return timeInterval_;
}

void UpdateLiveAppSnapshotConfigRequest::setTimeInterval(int timeInterval)
{
	timeInterval_ = timeInterval;
	setParameter("TimeInterval", std::to_string(timeInterval));
}

std::string UpdateLiveAppSnapshotConfigRequest::getOssEndpoint()const
{
	return ossEndpoint_;
}

void UpdateLiveAppSnapshotConfigRequest::setOssEndpoint(const std::string& ossEndpoint)
{
	ossEndpoint_ = ossEndpoint;
	setParameter("OssEndpoint", ossEndpoint);
}

std::string UpdateLiveAppSnapshotConfigRequest::getAppName()const
{
	return appName_;
}

void UpdateLiveAppSnapshotConfigRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string UpdateLiveAppSnapshotConfigRequest::getSecurityToken()const
{
	return securityToken_;
}

void UpdateLiveAppSnapshotConfigRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string UpdateLiveAppSnapshotConfigRequest::getOverwriteOssObject()const
{
	return overwriteOssObject_;
}

void UpdateLiveAppSnapshotConfigRequest::setOverwriteOssObject(const std::string& overwriteOssObject)
{
	overwriteOssObject_ = overwriteOssObject;
	setParameter("OverwriteOssObject", overwriteOssObject);
}

std::string UpdateLiveAppSnapshotConfigRequest::getOssBucket()const
{
	return ossBucket_;
}

void UpdateLiveAppSnapshotConfigRequest::setOssBucket(const std::string& ossBucket)
{
	ossBucket_ = ossBucket;
	setParameter("OssBucket", ossBucket);
}

std::string UpdateLiveAppSnapshotConfigRequest::getDomainName()const
{
	return domainName_;
}

void UpdateLiveAppSnapshotConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string UpdateLiveAppSnapshotConfigRequest::getSequenceOssObject()const
{
	return sequenceOssObject_;
}

void UpdateLiveAppSnapshotConfigRequest::setSequenceOssObject(const std::string& sequenceOssObject)
{
	sequenceOssObject_ = sequenceOssObject;
	setParameter("SequenceOssObject", sequenceOssObject);
}

long UpdateLiveAppSnapshotConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void UpdateLiveAppSnapshotConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string UpdateLiveAppSnapshotConfigRequest::getCallback()const
{
	return callback_;
}

void UpdateLiveAppSnapshotConfigRequest::setCallback(const std::string& callback)
{
	callback_ = callback;
	setParameter("Callback", callback);
}

