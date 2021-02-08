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

#include <alibabacloud/live/model/DescribeBoardEventsRequest.h>

using AlibabaCloud::Live::Model::DescribeBoardEventsRequest;

DescribeBoardEventsRequest::DescribeBoardEventsRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeBoardEvents")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeBoardEventsRequest::~DescribeBoardEventsRequest()
{}

std::string DescribeBoardEventsRequest::getStartTime()const
{
	return startTime_;
}

void DescribeBoardEventsRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string DescribeBoardEventsRequest::getBoardId()const
{
	return boardId_;
}

void DescribeBoardEventsRequest::setBoardId(const std::string& boardId)
{
	boardId_ = boardId;
	setParameter("BoardId", boardId);
}

std::string DescribeBoardEventsRequest::getEndTime()const
{
	return endTime_;
}

void DescribeBoardEventsRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeBoardEventsRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeBoardEventsRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeBoardEventsRequest::getAppId()const
{
	return appId_;
}

void DescribeBoardEventsRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

