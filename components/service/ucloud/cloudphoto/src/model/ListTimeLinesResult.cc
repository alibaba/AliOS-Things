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

#include <alibabacloud/cloudphoto/model/ListTimeLinesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudPhoto;
using namespace AlibabaCloud::CloudPhoto::Model;

ListTimeLinesResult::ListTimeLinesResult() :
	ServiceResult()
{}

ListTimeLinesResult::ListTimeLinesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListTimeLinesResult::~ListTimeLinesResult()
{}

void ListTimeLinesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allTimeLinesNode = value["TimeLines"]["TimeLine"];
	for (auto valueTimeLinesTimeLine : allTimeLinesNode)
	{
		TimeLine timeLinesObject;
		if(!valueTimeLinesTimeLine["StartTime"].isNull())
			timeLinesObject.startTime = std::stol(valueTimeLinesTimeLine["StartTime"].asString());
		if(!valueTimeLinesTimeLine["EndTime"].isNull())
			timeLinesObject.endTime = std::stol(valueTimeLinesTimeLine["EndTime"].asString());
		if(!valueTimeLinesTimeLine["TotalCount"].isNull())
			timeLinesObject.totalCount = std::stoi(valueTimeLinesTimeLine["TotalCount"].asString());
		if(!valueTimeLinesTimeLine["PhotosCount"].isNull())
			timeLinesObject.photosCount = std::stoi(valueTimeLinesTimeLine["PhotosCount"].asString());
		auto allPhotosNode = allTimeLinesNode["Photos"]["Photo"];
		for (auto allTimeLinesNodePhotosPhoto : allPhotosNode)
		{
			TimeLine::Photo photosObject;
			if(!allTimeLinesNodePhotosPhoto["Id"].isNull())
				photosObject.id = std::stol(allTimeLinesNodePhotosPhoto["Id"].asString());
			if(!allTimeLinesNodePhotosPhoto["IdStr"].isNull())
				photosObject.idStr = allTimeLinesNodePhotosPhoto["IdStr"].asString();
			if(!allTimeLinesNodePhotosPhoto["Title"].isNull())
				photosObject.title = allTimeLinesNodePhotosPhoto["Title"].asString();
			if(!allTimeLinesNodePhotosPhoto["Location"].isNull())
				photosObject.location = allTimeLinesNodePhotosPhoto["Location"].asString();
			if(!allTimeLinesNodePhotosPhoto["FileId"].isNull())
				photosObject.fileId = allTimeLinesNodePhotosPhoto["FileId"].asString();
			if(!allTimeLinesNodePhotosPhoto["State"].isNull())
				photosObject.state = allTimeLinesNodePhotosPhoto["State"].asString();
			if(!allTimeLinesNodePhotosPhoto["Md5"].isNull())
				photosObject.md5 = allTimeLinesNodePhotosPhoto["Md5"].asString();
			if(!allTimeLinesNodePhotosPhoto["IsVideo"].isNull())
				photosObject.isVideo = allTimeLinesNodePhotosPhoto["IsVideo"].asString() == "true";
			if(!allTimeLinesNodePhotosPhoto["Remark"].isNull())
				photosObject.remark = allTimeLinesNodePhotosPhoto["Remark"].asString();
			if(!allTimeLinesNodePhotosPhoto["Size"].isNull())
				photosObject.size = std::stol(allTimeLinesNodePhotosPhoto["Size"].asString());
			if(!allTimeLinesNodePhotosPhoto["Width"].isNull())
				photosObject.width = std::stol(allTimeLinesNodePhotosPhoto["Width"].asString());
			if(!allTimeLinesNodePhotosPhoto["Height"].isNull())
				photosObject.height = std::stol(allTimeLinesNodePhotosPhoto["Height"].asString());
			if(!allTimeLinesNodePhotosPhoto["Ctime"].isNull())
				photosObject.ctime = std::stol(allTimeLinesNodePhotosPhoto["Ctime"].asString());
			if(!allTimeLinesNodePhotosPhoto["Mtime"].isNull())
				photosObject.mtime = std::stol(allTimeLinesNodePhotosPhoto["Mtime"].asString());
			if(!allTimeLinesNodePhotosPhoto["TakenAt"].isNull())
				photosObject.takenAt = std::stol(allTimeLinesNodePhotosPhoto["TakenAt"].asString());
			if(!allTimeLinesNodePhotosPhoto["ShareExpireTime"].isNull())
				photosObject.shareExpireTime = std::stol(allTimeLinesNodePhotosPhoto["ShareExpireTime"].asString());
			if(!allTimeLinesNodePhotosPhoto["Like"].isNull())
				photosObject.like = std::stol(allTimeLinesNodePhotosPhoto["Like"].asString());
			timeLinesObject.photos.push_back(photosObject);
		}
		timeLines_.push_back(timeLinesObject);
	}
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["NextCursor"].isNull())
		nextCursor_ = std::stoi(value["NextCursor"].asString());
	if(!value["Action"].isNull())
		action_ = value["Action"].asString();

}

std::vector<ListTimeLinesResult::TimeLine> ListTimeLinesResult::getTimeLines()const
{
	return timeLines_;
}

int ListTimeLinesResult::getNextCursor()const
{
	return nextCursor_;
}

std::string ListTimeLinesResult::getAction()const
{
	return action_;
}

std::string ListTimeLinesResult::getMessage()const
{
	return message_;
}

std::string ListTimeLinesResult::getCode()const
{
	return code_;
}

