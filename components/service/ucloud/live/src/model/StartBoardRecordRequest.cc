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

#include <alibabacloud/live/model/StartBoardRecordRequest.h>

using AlibabaCloud::Live::Model::StartBoardRecordRequest;

StartBoardRecordRequest::StartBoardRecordRequest() :
	RpcServiceRequest("live", "2016-11-01", "StartBoardRecord")
{
	setMethod(HttpRequest::Method::Post);
}

StartBoardRecordRequest::~StartBoardRecordRequest()
{}

std::string StartBoardRecordRequest::getStartTime()const
{
	return startTime_;
}

void StartBoardRecordRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string StartBoardRecordRequest::getBoardId()const
{
	return boardId_;
}

void StartBoardRecordRequest::setBoardId(const std::string& boardId)
{
	boardId_ = boardId;
	setParameter("BoardId", boardId);
}

long StartBoardRecordRequest::getOwnerId()const
{
	return ownerId_;
}

void StartBoardRecordRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string StartBoardRecordRequest::getAppId()const
{
	return appId_;
}

void StartBoardRecordRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

