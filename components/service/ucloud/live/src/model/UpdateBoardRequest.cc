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

#include <alibabacloud/live/model/UpdateBoardRequest.h>

using AlibabaCloud::Live::Model::UpdateBoardRequest;

UpdateBoardRequest::UpdateBoardRequest() :
	RpcServiceRequest("live", "2016-11-01", "UpdateBoard")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateBoardRequest::~UpdateBoardRequest()
{}

long UpdateBoardRequest::getOwnerId()const
{
	return ownerId_;
}

void UpdateBoardRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string UpdateBoardRequest::getAppId()const
{
	return appId_;
}

void UpdateBoardRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string UpdateBoardRequest::getBoardData()const
{
	return boardData_;
}

void UpdateBoardRequest::setBoardData(const std::string& boardData)
{
	boardData_ = boardData;
	setParameter("BoardData", boardData);
}

