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

#include <alibabacloud/live/model/CreateRoomRequest.h>

using AlibabaCloud::Live::Model::CreateRoomRequest;

CreateRoomRequest::CreateRoomRequest() :
	RpcServiceRequest("live", "2016-11-01", "CreateRoom")
{
	setMethod(HttpRequest::Method::Post);
}

CreateRoomRequest::~CreateRoomRequest()
{}

std::string CreateRoomRequest::getTemplateIds()const
{
	return templateIds_;
}

void CreateRoomRequest::setTemplateIds(const std::string& templateIds)
{
	templateIds_ = templateIds;
	setParameter("TemplateIds", templateIds);
}

std::string CreateRoomRequest::getAnchorId()const
{
	return anchorId_;
}

void CreateRoomRequest::setAnchorId(const std::string& anchorId)
{
	anchorId_ = anchorId;
	setParameter("AnchorId", anchorId);
}

bool CreateRoomRequest::getUseAppTranscode()const
{
	return useAppTranscode_;
}

void CreateRoomRequest::setUseAppTranscode(bool useAppTranscode)
{
	useAppTranscode_ = useAppTranscode;
	setParameter("UseAppTranscode", useAppTranscode ? "true" : "false");
}

long CreateRoomRequest::getOwnerId()const
{
	return ownerId_;
}

void CreateRoomRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string CreateRoomRequest::getRoomId()const
{
	return roomId_;
}

void CreateRoomRequest::setRoomId(const std::string& roomId)
{
	roomId_ = roomId;
	setParameter("RoomId", roomId);
}

std::string CreateRoomRequest::getAppId()const
{
	return appId_;
}

void CreateRoomRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

