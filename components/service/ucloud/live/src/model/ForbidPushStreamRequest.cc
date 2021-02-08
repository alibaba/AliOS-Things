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

#include <alibabacloud/live/model/ForbidPushStreamRequest.h>

using AlibabaCloud::Live::Model::ForbidPushStreamRequest;

ForbidPushStreamRequest::ForbidPushStreamRequest() :
	RpcServiceRequest("live", "2016-11-01", "ForbidPushStream")
{
	setMethod(HttpRequest::Method::Post);
}

ForbidPushStreamRequest::~ForbidPushStreamRequest()
{}

std::string ForbidPushStreamRequest::getUserData()const
{
	return userData_;
}

void ForbidPushStreamRequest::setUserData(const std::string& userData)
{
	userData_ = userData;
	setParameter("UserData", userData);
}

std::string ForbidPushStreamRequest::getEndTime()const
{
	return endTime_;
}

void ForbidPushStreamRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long ForbidPushStreamRequest::getOwnerId()const
{
	return ownerId_;
}

void ForbidPushStreamRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string ForbidPushStreamRequest::getRoomId()const
{
	return roomId_;
}

void ForbidPushStreamRequest::setRoomId(const std::string& roomId)
{
	roomId_ = roomId;
	setParameter("RoomId", roomId);
}

std::string ForbidPushStreamRequest::getAppId()const
{
	return appId_;
}

void ForbidPushStreamRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

