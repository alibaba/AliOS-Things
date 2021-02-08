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

#include <alibabacloud/live/model/DescribeLiveAudioAuditConfigRequest.h>

using AlibabaCloud::Live::Model::DescribeLiveAudioAuditConfigRequest;

DescribeLiveAudioAuditConfigRequest::DescribeLiveAudioAuditConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeLiveAudioAuditConfig")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeLiveAudioAuditConfigRequest::~DescribeLiveAudioAuditConfigRequest()
{}

std::string DescribeLiveAudioAuditConfigRequest::getAppName()const
{
	return appName_;
}

void DescribeLiveAudioAuditConfigRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string DescribeLiveAudioAuditConfigRequest::getStreamName()const
{
	return streamName_;
}

void DescribeLiveAudioAuditConfigRequest::setStreamName(const std::string& streamName)
{
	streamName_ = streamName;
	setParameter("StreamName", streamName);
}

std::string DescribeLiveAudioAuditConfigRequest::getDomainName()const
{
	return domainName_;
}

void DescribeLiveAudioAuditConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long DescribeLiveAudioAuditConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeLiveAudioAuditConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

