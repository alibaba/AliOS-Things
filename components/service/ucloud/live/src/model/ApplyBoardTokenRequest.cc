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

#include <alibabacloud/live/model/ApplyBoardTokenRequest.h>

using AlibabaCloud::Live::Model::ApplyBoardTokenRequest;

ApplyBoardTokenRequest::ApplyBoardTokenRequest() :
	RpcServiceRequest("live", "2016-11-01", "ApplyBoardToken")
{
	setMethod(HttpRequest::Method::Post);
}

ApplyBoardTokenRequest::~ApplyBoardTokenRequest()
{}

std::string ApplyBoardTokenRequest::getBoardId()const
{
	return boardId_;
}

void ApplyBoardTokenRequest::setBoardId(const std::string& boardId)
{
	boardId_ = boardId;
	setParameter("BoardId", boardId);
}

std::string ApplyBoardTokenRequest::getAppUid()const
{
	return appUid_;
}

void ApplyBoardTokenRequest::setAppUid(const std::string& appUid)
{
	appUid_ = appUid;
	setParameter("AppUid", appUid);
}

long ApplyBoardTokenRequest::getOwnerId()const
{
	return ownerId_;
}

void ApplyBoardTokenRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string ApplyBoardTokenRequest::getAppId()const
{
	return appId_;
}

void ApplyBoardTokenRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

