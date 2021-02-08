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

#include <alibabacloud/cloudphoto/model/ListMomentsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudPhoto;
using namespace AlibabaCloud::CloudPhoto::Model;

ListMomentsResult::ListMomentsResult() :
	ServiceResult()
{}

ListMomentsResult::ListMomentsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListMomentsResult::~ListMomentsResult()
{}

void ListMomentsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allMomentsNode = value["Moments"]["Moment"];
	for (auto valueMomentsMoment : allMomentsNode)
	{
		Moment momentsObject;
		if(!valueMomentsMoment["Id"].isNull())
			momentsObject.id = std::stol(valueMomentsMoment["Id"].asString());
		if(!valueMomentsMoment["IdStr"].isNull())
			momentsObject.idStr = valueMomentsMoment["IdStr"].asString();
		if(!valueMomentsMoment["LocationName"].isNull())
			momentsObject.locationName = valueMomentsMoment["LocationName"].asString();
		if(!valueMomentsMoment["PhotosCount"].isNull())
			momentsObject.photosCount = std::stoi(valueMomentsMoment["PhotosCount"].asString());
		if(!valueMomentsMoment["State"].isNull())
			momentsObject.state = valueMomentsMoment["State"].asString();
		if(!valueMomentsMoment["TakenAt"].isNull())
			momentsObject.takenAt = std::stol(valueMomentsMoment["TakenAt"].asString());
		if(!valueMomentsMoment["Ctime"].isNull())
			momentsObject.ctime = std::stol(valueMomentsMoment["Ctime"].asString());
		if(!valueMomentsMoment["Mtime"].isNull())
			momentsObject.mtime = std::stol(valueMomentsMoment["Mtime"].asString());
		moments_.push_back(momentsObject);
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

std::string ListMomentsResult::getNextCursor()const
{
	return nextCursor_;
}

int ListMomentsResult::getTotalCount()const
{
	return totalCount_;
}

std::string ListMomentsResult::getAction()const
{
	return action_;
}

std::string ListMomentsResult::getMessage()const
{
	return message_;
}

std::vector<ListMomentsResult::Moment> ListMomentsResult::getMoments()const
{
	return moments_;
}

std::string ListMomentsResult::getCode()const
{
	return code_;
}

