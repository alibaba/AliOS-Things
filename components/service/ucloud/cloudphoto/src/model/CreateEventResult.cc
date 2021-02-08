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

#include <alibabacloud/cloudphoto/model/CreateEventResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudPhoto;
using namespace AlibabaCloud::CloudPhoto::Model;

CreateEventResult::CreateEventResult() :
	ServiceResult()
{}

CreateEventResult::CreateEventResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

CreateEventResult::~CreateEventResult()
{}

void CreateEventResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto eventNode = value["Event"];
	if(!eventNode["Id"].isNull())
		event_.id = std::stol(eventNode["Id"].asString());
	if(!eventNode["IdStr"].isNull())
		event_.idStr = eventNode["IdStr"].asString();
	if(!eventNode["Title"].isNull())
		event_.title = eventNode["Title"].asString();
	if(!eventNode["BannerPhotoId"].isNull())
		event_.bannerPhotoId = eventNode["BannerPhotoId"].asString();
	if(!eventNode["Identity"].isNull())
		event_.identity = eventNode["Identity"].asString();
	if(!eventNode["SplashPhotoId"].isNull())
		event_.splashPhotoId = eventNode["SplashPhotoId"].asString();
	if(!eventNode["State"].isNull())
		event_.state = eventNode["State"].asString();
	if(!eventNode["WeixinTitle"].isNull())
		event_.weixinTitle = eventNode["WeixinTitle"].asString();
	if(!eventNode["WatermarkPhotoId"].isNull())
		event_.watermarkPhotoId = eventNode["WatermarkPhotoId"].asString();
	if(!eventNode["StartAt"].isNull())
		event_.startAt = std::stol(eventNode["StartAt"].asString());
	if(!eventNode["EndAt"].isNull())
		event_.endAt = std::stol(eventNode["EndAt"].asString());
	if(!eventNode["Ctime"].isNull())
		event_.ctime = std::stol(eventNode["Ctime"].asString());
	if(!eventNode["Mtime"].isNull())
		event_.mtime = std::stol(eventNode["Mtime"].asString());
	if(!eventNode["ViewsCount"].isNull())
		event_.viewsCount = std::stol(eventNode["ViewsCount"].asString());
	if(!eventNode["LibraryId"].isNull())
		event_.libraryId = eventNode["LibraryId"].asString();
	if(!eventNode["IdStr"].isNull())
		event_.idStr1 = eventNode["IdStr"].asString();
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Action"].isNull())
		action_ = value["Action"].asString();

}

std::string CreateEventResult::getAction()const
{
	return action_;
}

std::string CreateEventResult::getMessage()const
{
	return message_;
}

CreateEventResult::Event CreateEventResult::getEvent()const
{
	return event_;
}

std::string CreateEventResult::getCode()const
{
	return code_;
}

