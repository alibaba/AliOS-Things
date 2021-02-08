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

#include <alibabacloud/live/model/AddLivePullStreamInfoConfigRequest.h>

using AlibabaCloud::Live::Model::AddLivePullStreamInfoConfigRequest;

AddLivePullStreamInfoConfigRequest::AddLivePullStreamInfoConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "AddLivePullStreamInfoConfig")
{
	setMethod(HttpRequest::Method::Post);
}

AddLivePullStreamInfoConfigRequest::~AddLivePullStreamInfoConfigRequest()
{}

std::string AddLivePullStreamInfoConfigRequest::getStartTime()const
{
	return startTime_;
}

void AddLivePullStreamInfoConfigRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string AddLivePullStreamInfoConfigRequest::getAppName()const
{
	return appName_;
}

void AddLivePullStreamInfoConfigRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string AddLivePullStreamInfoConfigRequest::getStreamName()const
{
	return streamName_;
}

void AddLivePullStreamInfoConfigRequest::setStreamName(const std::string& streamName)
{
	streamName_ = streamName;
	setParameter("StreamName", streamName);
}

std::string AddLivePullStreamInfoConfigRequest::getPullAlways()const
{
	return pullAlways_;
}

void AddLivePullStreamInfoConfigRequest::setPullAlways(const std::string& pullAlways)
{
	pullAlways_ = pullAlways;
	setParameter("PullAlways", pullAlways);
}

std::string AddLivePullStreamInfoConfigRequest::getDomainName()const
{
	return domainName_;
}

void AddLivePullStreamInfoConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string AddLivePullStreamInfoConfigRequest::getEndTime()const
{
	return endTime_;
}

void AddLivePullStreamInfoConfigRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long AddLivePullStreamInfoConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void AddLivePullStreamInfoConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string AddLivePullStreamInfoConfigRequest::getSourceUrl()const
{
	return sourceUrl_;
}

void AddLivePullStreamInfoConfigRequest::setSourceUrl(const std::string& sourceUrl)
{
	sourceUrl_ = sourceUrl;
	setParameter("SourceUrl", sourceUrl);
}

