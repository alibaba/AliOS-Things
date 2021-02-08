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

#include <alibabacloud/live/model/DescribeLiveStreamsControlHistoryRequest.h>

using AlibabaCloud::Live::Model::DescribeLiveStreamsControlHistoryRequest;

DescribeLiveStreamsControlHistoryRequest::DescribeLiveStreamsControlHistoryRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeLiveStreamsControlHistory")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeLiveStreamsControlHistoryRequest::~DescribeLiveStreamsControlHistoryRequest()
{}

std::string DescribeLiveStreamsControlHistoryRequest::getStartTime()const
{
	return startTime_;
}

void DescribeLiveStreamsControlHistoryRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeLiveStreamsControlHistoryRequest::getAppName()const
{
	return appName_;
}

void DescribeLiveStreamsControlHistoryRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string DescribeLiveStreamsControlHistoryRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeLiveStreamsControlHistoryRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string DescribeLiveStreamsControlHistoryRequest::getDomainName()const
{
	return domainName_;
}

void DescribeLiveStreamsControlHistoryRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string DescribeLiveStreamsControlHistoryRequest::getEndTime()const
{
	return endTime_;
}

void DescribeLiveStreamsControlHistoryRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeLiveStreamsControlHistoryRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeLiveStreamsControlHistoryRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

