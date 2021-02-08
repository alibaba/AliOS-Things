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

#include <alibabacloud/live/model/SendRoomNotificationRequest.h>

using AlibabaCloud::Live::Model::SendRoomNotificationRequest;

SendRoomNotificationRequest::SendRoomNotificationRequest() :
	RpcServiceRequest("live", "2016-11-01", "SendRoomNotification")
{
	setMethod(HttpRequest::Method::Post);
}

SendRoomNotificationRequest::~SendRoomNotificationRequest()
{}

std::string SendRoomNotificationRequest::getData()const
{
	return data_;
}

void SendRoomNotificationRequest::setData(const std::string& data)
{
	data_ = data;
	setParameter("Data", data);
}

std::string SendRoomNotificationRequest::getAppUid()const
{
	return appUid_;
}

void SendRoomNotificationRequest::setAppUid(const std::string& appUid)
{
	appUid_ = appUid;
	setParameter("AppUid", appUid);
}

long SendRoomNotificationRequest::getOwnerId()const
{
	return ownerId_;
}

void SendRoomNotificationRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

int SendRoomNotificationRequest::getPriority()const
{
	return priority_;
}

void SendRoomNotificationRequest::setPriority(int priority)
{
	priority_ = priority;
	setParameter("Priority", std::to_string(priority));
}

std::string SendRoomNotificationRequest::getRoomId()const
{
	return roomId_;
}

void SendRoomNotificationRequest::setRoomId(const std::string& roomId)
{
	roomId_ = roomId;
	setParameter("RoomId", roomId);
}

std::string SendRoomNotificationRequest::getAppId()const
{
	return appId_;
}

void SendRoomNotificationRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

