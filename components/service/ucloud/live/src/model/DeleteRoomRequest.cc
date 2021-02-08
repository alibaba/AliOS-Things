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

#include <alibabacloud/live/model/DeleteRoomRequest.h>

using AlibabaCloud::Live::Model::DeleteRoomRequest;

DeleteRoomRequest::DeleteRoomRequest() :
	RpcServiceRequest("live", "2016-11-01", "DeleteRoom")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteRoomRequest::~DeleteRoomRequest()
{}

long DeleteRoomRequest::getOwnerId()const
{
	return ownerId_;
}

void DeleteRoomRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DeleteRoomRequest::getRoomId()const
{
	return roomId_;
}

void DeleteRoomRequest::setRoomId(const std::string& roomId)
{
	roomId_ = roomId;
	setParameter("RoomId", roomId);
}

std::string DeleteRoomRequest::getAppId()const
{
	return appId_;
}

void DeleteRoomRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

