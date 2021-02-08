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

#include <alibabacloud/live/model/UpdateLiveAudioAuditConfigRequest.h>

using AlibabaCloud::Live::Model::UpdateLiveAudioAuditConfigRequest;

UpdateLiveAudioAuditConfigRequest::UpdateLiveAudioAuditConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "UpdateLiveAudioAuditConfig")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateLiveAudioAuditConfigRequest::~UpdateLiveAudioAuditConfigRequest()
{}

std::string UpdateLiveAudioAuditConfigRequest::getOssEndpoint()const
{
	return ossEndpoint_;
}

void UpdateLiveAudioAuditConfigRequest::setOssEndpoint(const std::string& ossEndpoint)
{
	ossEndpoint_ = ossEndpoint;
	setParameter("OssEndpoint", ossEndpoint);
}

std::string UpdateLiveAudioAuditConfigRequest::getOssObject()const
{
	return ossObject_;
}

void UpdateLiveAudioAuditConfigRequest::setOssObject(const std::string& ossObject)
{
	ossObject_ = ossObject;
	setParameter("OssObject", ossObject);
}

std::string UpdateLiveAudioAuditConfigRequest::getAppName()const
{
	return appName_;
}

void UpdateLiveAudioAuditConfigRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string UpdateLiveAudioAuditConfigRequest::getStreamName()const
{
	return streamName_;
}

void UpdateLiveAudioAuditConfigRequest::setStreamName(const std::string& streamName)
{
	streamName_ = streamName;
	setParameter("StreamName", streamName);
}

std::string UpdateLiveAudioAuditConfigRequest::getOssBucket()const
{
	return ossBucket_;
}

void UpdateLiveAudioAuditConfigRequest::setOssBucket(const std::string& ossBucket)
{
	ossBucket_ = ossBucket;
	setParameter("OssBucket", ossBucket);
}

std::string UpdateLiveAudioAuditConfigRequest::getDomainName()const
{
	return domainName_;
}

void UpdateLiveAudioAuditConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long UpdateLiveAudioAuditConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void UpdateLiveAudioAuditConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string UpdateLiveAudioAuditConfigRequest::getBizType()const
{
	return bizType_;
}

void UpdateLiveAudioAuditConfigRequest::setBizType(const std::string& bizType)
{
	bizType_ = bizType;
	setParameter("BizType", bizType);
}

