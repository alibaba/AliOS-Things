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

#include <alibabacloud/live/model/SendRoomUserNotificationRequest.h>

using AlibabaCloud::Live::Model::SendRoomUserNotificationRequest;

SendRoomUserNotificationRequest::SendRoomUserNotificationRequest() :
	RpcServiceRequest("live", "2016-11-01", "SendRoomUserNotification")
{
	setMethod(HttpRequest::Method::Post);
}

SendRoomUserNotificationRequest::~SendRoomUserNotificationRequest()
{}

std::string SendRoomUserNotificationRequest::getData()const
{
	return data_;
}

void SendRoomUserNotificationRequest::setData(const std::string& data)
{
	data_ = data;
	setParameter("Data", data);
}

std::string SendRoomUserNotificationRequest::getToAppUid()const
{
	return toAppUid_;
}

void SendRoomUserNotificationRequest::setToAppUid(const std::string& toAppUid)
{
	toAppUid_ = toAppUid;
	setParameter("ToAppUid", toAppUid);
}

std::string SendRoomUserNotificationRequest::getAppUid()const
{
	return appUid_;
}

void SendRoomUserNotificationRequest::setAppUid(const std::string& appUid)
{
	appUid_ = appUid;
	setParameter("AppUid", appUid);
}

long SendRoomUserNotificationRequest::getOwnerId()const
{
	return ownerId_;
}

void SendRoomUserNotificationRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

int SendRoomUserNotificationRequest::getPriority()const
{
	return priority_;
}

void SendRoomUserNotificationRequest::setPriority(int priority)
{
	priority_ = priority;
	setParameter("Priority", std::to_string(priority));
}

std::string SendRoomUserNotificationRequest::getRoomId()const
{
	return roomId_;
}

void SendRoomUserNotificationRequest::setRoomId(const std::string& roomId)
{
	roomId_ = roomId;
	setParameter("RoomId", roomId);
}

std::string SendRoomUserNotificationRequest::getAppId()const
{
	return appId_;
}

void SendRoomUserNotificationRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

