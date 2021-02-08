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

#include <alibabacloud/live/model/UpdateLiveSnapshotDetectPornConfigRequest.h>

using AlibabaCloud::Live::Model::UpdateLiveSnapshotDetectPornConfigRequest;

UpdateLiveSnapshotDetectPornConfigRequest::UpdateLiveSnapshotDetectPornConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "UpdateLiveSnapshotDetectPornConfig")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateLiveSnapshotDetectPornConfigRequest::~UpdateLiveSnapshotDetectPornConfigRequest()
{}

std::string UpdateLiveSnapshotDetectPornConfigRequest::getOssEndpoint()const
{
	return ossEndpoint_;
}

void UpdateLiveSnapshotDetectPornConfigRequest::setOssEndpoint(const std::string& ossEndpoint)
{
	ossEndpoint_ = ossEndpoint;
	setParameter("OssEndpoint", ossEndpoint);
}

std::string UpdateLiveSnapshotDetectPornConfigRequest::getOssObject()const
{
	return ossObject_;
}

void UpdateLiveSnapshotDetectPornConfigRequest::setOssObject(const std::string& ossObject)
{
	ossObject_ = ossObject;
	setParameter("OssObject", ossObject);
}

std::vector<std::string> UpdateLiveSnapshotDetectPornConfigRequest::getScene()const
{
	return scene_;
}

void UpdateLiveSnapshotDetectPornConfigRequest::setScene(const std::vector<std::string>& scene)
{
	scene_ = scene;
	for(int dep1 = 0; dep1!= scene.size(); dep1++) {
		setParameter("Scene."+ std::to_string(dep1), scene.at(dep1));
	}
}

std::string UpdateLiveSnapshotDetectPornConfigRequest::getAppName()const
{
	return appName_;
}

void UpdateLiveSnapshotDetectPornConfigRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string UpdateLiveSnapshotDetectPornConfigRequest::getSecurityToken()const
{
	return securityToken_;
}

void UpdateLiveSnapshotDetectPornConfigRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string UpdateLiveSnapshotDetectPornConfigRequest::getOssBucket()const
{
	return ossBucket_;
}

void UpdateLiveSnapshotDetectPornConfigRequest::setOssBucket(const std::string& ossBucket)
{
	ossBucket_ = ossBucket;
	setParameter("OssBucket", ossBucket);
}

std::string UpdateLiveSnapshotDetectPornConfigRequest::getDomainName()const
{
	return domainName_;
}

void UpdateLiveSnapshotDetectPornConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long UpdateLiveSnapshotDetectPornConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void UpdateLiveSnapshotDetectPornConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

int UpdateLiveSnapshotDetectPornConfigRequest::getInterval()const
{
	return interval_;
}

void UpdateLiveSnapshotDetectPornConfigRequest::setInterval(int interval)
{
	interval_ = interval;
	setParameter("Interval", std::to_string(interval));
}

