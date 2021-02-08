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

#include <alibabacloud/live/model/DescribeLiveStreamsFrameRateAndBitRateDataRequest.h>

using AlibabaCloud::Live::Model::DescribeLiveStreamsFrameRateAndBitRateDataRequest;

DescribeLiveStreamsFrameRateAndBitRateDataRequest::DescribeLiveStreamsFrameRateAndBitRateDataRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeLiveStreamsFrameRateAndBitRateData")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeLiveStreamsFrameRateAndBitRateDataRequest::~DescribeLiveStreamsFrameRateAndBitRateDataRequest()
{}

std::string DescribeLiveStreamsFrameRateAndBitRateDataRequest::getStartTime()const
{
	return startTime_;
}

void DescribeLiveStreamsFrameRateAndBitRateDataRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeLiveStreamsFrameRateAndBitRateDataRequest::getAppName()const
{
	return appName_;
}

void DescribeLiveStreamsFrameRateAndBitRateDataRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string DescribeLiveStreamsFrameRateAndBitRateDataRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeLiveStreamsFrameRateAndBitRateDataRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string DescribeLiveStreamsFrameRateAndBitRateDataRequest::getStreamName()const
{
	return streamName_;
}

void DescribeLiveStreamsFrameRateAndBitRateDataRequest::setStreamName(const std::string& streamName)
{
	streamName_ = streamName;
	setParameter("StreamName", streamName);
}

std::string DescribeLiveStreamsFrameRateAndBitRateDataRequest::getDomainName()const
{
	return domainName_;
}

void DescribeLiveStreamsFrameRateAndBitRateDataRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string DescribeLiveStreamsFrameRateAndBitRateDataRequest::getEndTime()const
{
	return endTime_;
}

void DescribeLiveStreamsFrameRateAndBitRateDataRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeLiveStreamsFrameRateAndBitRateDataRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeLiveStreamsFrameRateAndBitRateDataRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

