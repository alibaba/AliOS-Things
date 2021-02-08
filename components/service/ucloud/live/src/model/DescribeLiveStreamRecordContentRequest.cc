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

#include <alibabacloud/live/model/DescribeLiveStreamRecordContentRequest.h>

using AlibabaCloud::Live::Model::DescribeLiveStreamRecordContentRequest;

DescribeLiveStreamRecordContentRequest::DescribeLiveStreamRecordContentRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeLiveStreamRecordContent")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeLiveStreamRecordContentRequest::~DescribeLiveStreamRecordContentRequest()
{}

std::string DescribeLiveStreamRecordContentRequest::getStartTime()const
{
	return startTime_;
}

void DescribeLiveStreamRecordContentRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeLiveStreamRecordContentRequest::getAppName()const
{
	return appName_;
}

void DescribeLiveStreamRecordContentRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string DescribeLiveStreamRecordContentRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeLiveStreamRecordContentRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string DescribeLiveStreamRecordContentRequest::getStreamName()const
{
	return streamName_;
}

void DescribeLiveStreamRecordContentRequest::setStreamName(const std::string& streamName)
{
	streamName_ = streamName;
	setParameter("StreamName", streamName);
}

std::string DescribeLiveStreamRecordContentRequest::getDomainName()const
{
	return domainName_;
}

void DescribeLiveStreamRecordContentRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string DescribeLiveStreamRecordContentRequest::getEndTime()const
{
	return endTime_;
}

void DescribeLiveStreamRecordContentRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeLiveStreamRecordContentRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeLiveStreamRecordContentRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

