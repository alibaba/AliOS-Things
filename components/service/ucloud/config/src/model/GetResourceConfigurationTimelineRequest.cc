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

#include <alibabacloud/config/model/GetResourceConfigurationTimelineRequest.h>

using AlibabaCloud::Config::Model::GetResourceConfigurationTimelineRequest;

GetResourceConfigurationTimelineRequest::GetResourceConfigurationTimelineRequest() :
	RpcServiceRequest("config", "2019-01-08", "GetResourceConfigurationTimeline")
{
	setMethod(HttpRequest::Method::Get);
}

GetResourceConfigurationTimelineRequest::~GetResourceConfigurationTimelineRequest()
{}

bool GetResourceConfigurationTimelineRequest::getMultiAccount()const
{
	return multiAccount_;
}

void GetResourceConfigurationTimelineRequest::setMultiAccount(bool multiAccount)
{
	multiAccount_ = multiAccount;
	setParameter("MultiAccount", multiAccount ? "true" : "false");
}

long GetResourceConfigurationTimelineRequest::getStartTime()const
{
	return startTime_;
}

void GetResourceConfigurationTimelineRequest::setStartTime(long startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", std::to_string(startTime));
}

std::string GetResourceConfigurationTimelineRequest::getNextToken()const
{
	return nextToken_;
}

void GetResourceConfigurationTimelineRequest::setNextToken(const std::string& nextToken)
{
	nextToken_ = nextToken;
	setParameter("NextToken", nextToken);
}

int GetResourceConfigurationTimelineRequest::getLimit()const
{
	return limit_;
}

void GetResourceConfigurationTimelineRequest::setLimit(int limit)
{
	limit_ = limit;
	setParameter("Limit", std::to_string(limit));
}

std::string GetResourceConfigurationTimelineRequest::getResourceId()const
{
	return resourceId_;
}

void GetResourceConfigurationTimelineRequest::setResourceId(const std::string& resourceId)
{
	resourceId_ = resourceId;
	setParameter("ResourceId", resourceId);
}

long GetResourceConfigurationTimelineRequest::getEndTime()const
{
	return endTime_;
}

void GetResourceConfigurationTimelineRequest::setEndTime(long endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", std::to_string(endTime));
}

std::string GetResourceConfigurationTimelineRequest::getResourceType()const
{
	return resourceType_;
}

void GetResourceConfigurationTimelineRequest::setResourceType(const std::string& resourceType)
{
	resourceType_ = resourceType;
	setParameter("ResourceType", resourceType);
}

std::string GetResourceConfigurationTimelineRequest::getRegion()const
{
	return region_;
}

void GetResourceConfigurationTimelineRequest::setRegion(const std::string& region)
{
	region_ = region;
	setParameter("Region", region);
}

long GetResourceConfigurationTimelineRequest::getMemberId()const
{
	return memberId_;
}

void GetResourceConfigurationTimelineRequest::setMemberId(long memberId)
{
	memberId_ = memberId;
	setParameter("MemberId", std::to_string(memberId));
}

