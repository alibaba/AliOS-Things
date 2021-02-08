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

#include <alibabacloud/live/model/AddLiveAudioAuditConfigRequest.h>

using AlibabaCloud::Live::Model::AddLiveAudioAuditConfigRequest;

AddLiveAudioAuditConfigRequest::AddLiveAudioAuditConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "AddLiveAudioAuditConfig")
{
	setMethod(HttpRequest::Method::Post);
}

AddLiveAudioAuditConfigRequest::~AddLiveAudioAuditConfigRequest()
{}

std::string AddLiveAudioAuditConfigRequest::getOssEndpoint()const
{
	return ossEndpoint_;
}

void AddLiveAudioAuditConfigRequest::setOssEndpoint(const std::string& ossEndpoint)
{
	ossEndpoint_ = ossEndpoint;
	setParameter("OssEndpoint", ossEndpoint);
}

std::string AddLiveAudioAuditConfigRequest::getOssObject()const
{
	return ossObject_;
}

void AddLiveAudioAuditConfigRequest::setOssObject(const std::string& ossObject)
{
	ossObject_ = ossObject;
	setParameter("OssObject", ossObject);
}

std::string AddLiveAudioAuditConfigRequest::getAppName()const
{
	return appName_;
}

void AddLiveAudioAuditConfigRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string AddLiveAudioAuditConfigRequest::getStreamName()const
{
	return streamName_;
}

void AddLiveAudioAuditConfigRequest::setStreamName(const std::string& streamName)
{
	streamName_ = streamName;
	setParameter("StreamName", streamName);
}

std::string AddLiveAudioAuditConfigRequest::getOssBucket()const
{
	return ossBucket_;
}

void AddLiveAudioAuditConfigRequest::setOssBucket(const std::string& ossBucket)
{
	ossBucket_ = ossBucket;
	setParameter("OssBucket", ossBucket);
}

std::string AddLiveAudioAuditConfigRequest::getDomainName()const
{
	return domainName_;
}

void AddLiveAudioAuditConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long AddLiveAudioAuditConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void AddLiveAudioAuditConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string AddLiveAudioAuditConfigRequest::getBizType()const
{
	return bizType_;
}

void AddLiveAudioAuditConfigRequest::setBizType(const std::string& bizType)
{
	bizType_ = bizType;
	setParameter("BizType", bizType);
}

