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

#include <alibabacloud/live/model/ResumeLiveStreamRequest.h>

using AlibabaCloud::Live::Model::ResumeLiveStreamRequest;

ResumeLiveStreamRequest::ResumeLiveStreamRequest() :
	RpcServiceRequest("live", "2016-11-01", "ResumeLiveStream")
{
	setMethod(HttpRequest::Method::Post);
}

ResumeLiveStreamRequest::~ResumeLiveStreamRequest()
{}

std::string ResumeLiveStreamRequest::getAppName()const
{
	return appName_;
}

void ResumeLiveStreamRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string ResumeLiveStreamRequest::getSecurityToken()const
{
	return securityToken_;
}

void ResumeLiveStreamRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string ResumeLiveStreamRequest::getStreamName()const
{
	return streamName_;
}

void ResumeLiveStreamRequest::setStreamName(const std::string& streamName)
{
	streamName_ = streamName;
	setParameter("StreamName", streamName);
}

std::string ResumeLiveStreamRequest::getLiveStreamType()const
{
	return liveStreamType_;
}

void ResumeLiveStreamRequest::setLiveStreamType(const std::string& liveStreamType)
{
	liveStreamType_ = liveStreamType;
	setParameter("LiveStreamType", liveStreamType);
}

std::string ResumeLiveStreamRequest::getDomainName()const
{
	return domainName_;
}

void ResumeLiveStreamRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long ResumeLiveStreamRequest::getOwnerId()const
{
	return ownerId_;
}

void ResumeLiveStreamRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

