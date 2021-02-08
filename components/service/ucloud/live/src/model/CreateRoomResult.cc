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

#include <alibabacloud/live/model/CreateRoomResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

CreateRoomResult::CreateRoomResult() :
	ServiceResult()
{}

CreateRoomResult::CreateRoomResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

CreateRoomResult::~CreateRoomResult()
{}

void CreateRoomResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["AppId"].isNull())
		appId_ = value["AppId"].asString();
	if(!value["RoomId"].isNull())
		roomId_ = value["RoomId"].asString();
	if(!value["AnchorId"].isNull())
		anchorId_ = value["AnchorId"].asString();

}

std::string CreateRoomResult::getAppId()const
{
	return appId_;
}

std::string CreateRoomResult::getAnchorId()const
{
	return anchorId_;
}

std::string CreateRoomResult::getRoomId()const
{
	return roomId_;
}

