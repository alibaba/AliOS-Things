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

#include <alibabacloud/cloudphoto/model/ListEventsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudPhoto;
using namespace AlibabaCloud::CloudPhoto::Model;

ListEventsResult::ListEventsResult() :
	ServiceResult()
{}

ListEventsResult::ListEventsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListEventsResult::~ListEventsResult()
{}

void ListEventsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allEventsNode = value["Events"]["Event"];
	for (auto valueEventsEvent : allEventsNode)
	{
		Event eventsObject;
		if(!valueEventsEvent["Id"].isNull())
			eventsObject.id = std::stol(valueEventsEvent["Id"].asString());
		if(!valueEventsEvent["IdStr"].isNull())
			eventsObject.idStr = valueEventsEvent["IdStr"].asString();
		if(!valueEventsEvent["Title"].isNull())
			eventsObject.title = valueEventsEvent["Title"].asString();
		if(!valueEventsEvent["BannerPhotoId"].isNull())
			eventsObject.bannerPhotoId = valueEventsEvent["BannerPhotoId"].asString();
		if(!valueEventsEvent["Identity"].isNull())
			eventsObject.identity = valueEventsEvent["Identity"].asString();
		if(!valueEventsEvent["SplashPhotoId"].isNull())
			eventsObject.splashPhotoId = valueEventsEvent["SplashPhotoId"].asString();
		if(!valueEventsEvent["State"].isNull())
			eventsObject.state = valueEventsEvent["State"].asString();
		if(!valueEventsEvent["WeixinTitle"].isNull())
			eventsObject.weixinTitle = valueEventsEvent["WeixinTitle"].asString();
		if(!valueEventsEvent["WatermarkPhotoId"].isNull())
			eventsObject.watermarkPhotoId = valueEventsEvent["WatermarkPhotoId"].asString();
		if(!valueEventsEvent["StartAt"].isNull())
			eventsObject.startAt = std::stol(valueEventsEvent["StartAt"].asString());
		if(!valueEventsEvent["EndAt"].isNull())
			eventsObject.endAt = std::stol(valueEventsEvent["EndAt"].asString());
		if(!valueEventsEvent["Ctime"].isNull())
			eventsObject.ctime = std::stol(valueEventsEvent["Ctime"].asString());
		if(!valueEventsEvent["Mtime"].isNull())
			eventsObject.mtime = std::stol(valueEventsEvent["Mtime"].asString());
		if(!valueEventsEvent["ViewsCount"].isNull())
			eventsObject.viewsCount = std::stol(valueEventsEvent["ViewsCount"].asString());
		if(!valueEventsEvent["LibraryId"].isNull())
			eventsObject.libraryId = valueEventsEvent["LibraryId"].asString();
		if(!valueEventsEvent["IdStr"].isNull())
			eventsObject.idStr1 = valueEventsEvent["IdStr"].asString();
		events_.push_back(eventsObject);
	}
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["NextCursor"].isNull())
		nextCursor_ = value["NextCursor"].asString();
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stoi(value["TotalCount"].asString());
	if(!value["Action"].isNull())
		action_ = value["Action"].asString();

}

std::string ListEventsResult::getNextCursor()const
{
	return nextCursor_;
}

int ListEventsResult::getTotalCount()const
{
	return totalCount_;
}

std::string ListEventsResult::getAction()const
{
	return action_;
}

std::string ListEventsResult::getMessage()const
{
	return message_;
}

std::vector<ListEventsResult::Event> ListEventsResult::getEvents()const
{
	return events_;
}

std::string ListEventsResult::getCode()const
{
	return code_;
}

