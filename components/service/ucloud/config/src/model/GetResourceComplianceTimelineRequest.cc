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

#include <alibabacloud/config/model/GetResourceComplianceTimelineRequest.h>

using AlibabaCloud::Config::Model::GetResourceComplianceTimelineRequest;

GetResourceComplianceTimelineRequest::GetResourceComplianceTimelineRequest() :
	RpcServiceRequest("config", "2019-01-08", "GetResourceComplianceTimeline")
{
	setMethod(HttpRequest::Method::Get);
}

GetResourceComplianceTimelineRequest::~GetResourceComplianceTimelineRequest()
{}

bool GetResourceComplianceTimelineRequest::getMultiAccount()const
{
	return multiAccount_;
}

void GetResourceComplianceTimelineRequest::setMultiAccount(bool multiAccount)
{
	multiAccount_ = multiAccount;
	setParameter("MultiAccount", multiAccount ? "true" : "false");
}

long GetResourceComplianceTimelineRequest::getStartTime()const
{
	return startTime_;
}

void GetResourceComplianceTimelineRequest::setStartTime(long startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", std::to_string(startTime));
}

std::string GetResourceComplianceTimelineRequest::getNextToken()const
{
	return nextToken_;
}

void GetResourceComplianceTimelineRequest::setNextToken(const std::string& nextToken)
{
	nextToken_ = nextToken;
	setParameter("NextToken", nextToken);
}

int GetResourceComplianceTimelineRequest::getLimit()const
{
	return limit_;
}

void GetResourceComplianceTimelineRequest::setLimit(int limit)
{
	limit_ = limit;
	setParameter("Limit", std::to_string(limit));
}

std::string GetResourceComplianceTimelineRequest::getResourceId()const
{
	return resourceId_;
}

void GetResourceComplianceTimelineRequest::setResourceId(const std::string& resourceId)
{
	resourceId_ = resourceId;
	setParameter("ResourceId", resourceId);
}

long GetResourceComplianceTimelineRequest::getEndTime()const
{
	return endTime_;
}

void GetResourceComplianceTimelineRequest::setEndTime(long endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", std::to_string(endTime));
}

std::string GetResourceComplianceTimelineRequest::getResourceType()const
{
	return resourceType_;
}

void GetResourceComplianceTimelineRequest::setResourceType(const std::string& resourceType)
{
	resourceType_ = resourceType;
	setParameter("ResourceType", resourceType);
}

std::string GetResourceComplianceTimelineRequest::getRegion()const
{
	return region_;
}

void GetResourceComplianceTimelineRequest::setRegion(const std::string& region)
{
	region_ = region;
	setParameter("Region", region);
}

std::string GetResourceComplianceTimelineRequest::getMemberId()const
{
	return memberId_;
}

void GetResourceComplianceTimelineRequest::setMemberId(const std::string& memberId)
{
	memberId_ = memberId;
	setParameter("MemberId", memberId);
}

