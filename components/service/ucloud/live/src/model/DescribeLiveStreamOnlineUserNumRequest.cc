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

#include <alibabacloud/live/model/DescribeLiveStreamOnlineUserNumRequest.h>

using AlibabaCloud::Live::Model::DescribeLiveStreamOnlineUserNumRequest;

DescribeLiveStreamOnlineUserNumRequest::DescribeLiveStreamOnlineUserNumRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeLiveStreamOnlineUserNum")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeLiveStreamOnlineUserNumRequest::~DescribeLiveStreamOnlineUserNumRequest()
{}

std::string DescribeLiveStreamOnlineUserNumRequest::getStartTime()const
{
	return startTime_;
}

void DescribeLiveStreamOnlineUserNumRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeLiveStreamOnlineUserNumRequest::getAppName()const
{
	return appName_;
}

void DescribeLiveStreamOnlineUserNumRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string DescribeLiveStreamOnlineUserNumRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeLiveStreamOnlineUserNumRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string DescribeLiveStreamOnlineUserNumRequest::getStreamName()const
{
	return streamName_;
}

void DescribeLiveStreamOnlineUserNumRequest::setStreamName(const std::string& streamName)
{
	streamName_ = streamName;
	setParameter("StreamName", streamName);
}

std::string DescribeLiveStreamOnlineUserNumRequest::getDomainName()const
{
	return domainName_;
}

void DescribeLiveStreamOnlineUserNumRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string DescribeLiveStreamOnlineUserNumRequest::getEndTime()const
{
	return endTime_;
}

void DescribeLiveStreamOnlineUserNumRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeLiveStreamOnlineUserNumRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeLiveStreamOnlineUserNumRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

