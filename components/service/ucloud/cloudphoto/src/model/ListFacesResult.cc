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

#include <alibabacloud/cloudphoto/model/ListFacesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudPhoto;
using namespace AlibabaCloud::CloudPhoto::Model;

ListFacesResult::ListFacesResult() :
	ServiceResult()
{}

ListFacesResult::ListFacesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListFacesResult::~ListFacesResult()
{}

void ListFacesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allFacesNode = value["Faces"]["Face"];
	for (auto valueFacesFace : allFacesNode)
	{
		Face facesObject;
		if(!valueFacesFace["Id"].isNull())
			facesObject.id = std::stol(valueFacesFace["Id"].asString());
		if(!valueFacesFace["IdStr"].isNull())
			facesObject.idStr = valueFacesFace["IdStr"].asString();
		if(!valueFacesFace["Name"].isNull())
			facesObject.name = valueFacesFace["Name"].asString();
		if(!valueFacesFace["PhotosCount"].isNull())
			facesObject.photosCount = std::stoi(valueFacesFace["PhotosCount"].asString());
		if(!valueFacesFace["State"].isNull())
			facesObject.state = valueFacesFace["State"].asString();
		if(!valueFacesFace["IsMe"].isNull())
			facesObject.isMe = valueFacesFace["IsMe"].asString() == "true";
		if(!valueFacesFace["Ctime"].isNull())
			facesObject.ctime = std::stol(valueFacesFace["Ctime"].asString());
		if(!valueFacesFace["Mtime"].isNull())
			facesObject.mtime = std::stol(valueFacesFace["Mtime"].asString());
		auto coverNode = value["Cover"];
		if(!coverNode["Id"].isNull())
			facesObject.cover.id = std::stol(coverNode["Id"].asString());
		if(!coverNode["IdStr"].isNull())
			facesObject.cover.idStr = coverNode["IdStr"].asString();
		if(!coverNode["Title"].isNull())
			facesObject.cover.title = coverNode["Title"].asString();
		if(!coverNode["FileId"].isNull())
			facesObject.cover.fileId = coverNode["FileId"].asString();
		if(!coverNode["State"].isNull())
			facesObject.cover.state = coverNode["State"].asString();
		if(!coverNode["Md5"].isNull())
			facesObject.cover.md5 = coverNode["Md5"].asString();
		if(!coverNode["IsVideo"].isNull())
			facesObject.cover.isVideo = coverNode["IsVideo"].asString() == "true";
		if(!coverNode["Width"].isNull())
			facesObject.cover.width = std::stol(coverNode["Width"].asString());
		if(!coverNode["Height"].isNull())
			facesObject.cover.height = std::stol(coverNode["Height"].asString());
		if(!coverNode["Ctime"].isNull())
			facesObject.cover.ctime = std::stol(coverNode["Ctime"].asString());
		if(!coverNode["Mtime"].isNull())
			facesObject.cover.mtime = std::stol(coverNode["Mtime"].asString());
		if(!coverNode["Remark"].isNull())
			facesObject.cover.remark = coverNode["Remark"].asString();
		auto allAxis = value["Axis"]["Axis"];
		for (auto value : allAxis)
			facesObject.axis.push_back(value.asString());
		faces_.push_back(facesObject);
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

std::string ListFacesResult::getNextCursor()const
{
	return nextCursor_;
}

int ListFacesResult::getTotalCount()const
{
	return totalCount_;
}

std::string ListFacesResult::getAction()const
{
	return action_;
}

std::string ListFacesResult::getMessage()const
{
	return message_;
}

std::string ListFacesResult::getCode()const
{
	return code_;
}

std::vector<ListFacesResult::Face> ListFacesResult::getFaces()const
{
	return faces_;
}

