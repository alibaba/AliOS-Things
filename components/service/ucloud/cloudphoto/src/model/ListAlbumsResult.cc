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

#include <alibabacloud/cloudphoto/model/ListAlbumsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudPhoto;
using namespace AlibabaCloud::CloudPhoto::Model;

ListAlbumsResult::ListAlbumsResult() :
	ServiceResult()
{}

ListAlbumsResult::ListAlbumsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListAlbumsResult::~ListAlbumsResult()
{}

void ListAlbumsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allAlbumsNode = value["Albums"]["Album"];
	for (auto valueAlbumsAlbum : allAlbumsNode)
	{
		Album albumsObject;
		if(!valueAlbumsAlbum["Id"].isNull())
			albumsObject.id = std::stol(valueAlbumsAlbum["Id"].asString());
		if(!valueAlbumsAlbum["IdStr"].isNull())
			albumsObject.idStr = valueAlbumsAlbum["IdStr"].asString();
		if(!valueAlbumsAlbum["Name"].isNull())
			albumsObject.name = valueAlbumsAlbum["Name"].asString();
		if(!valueAlbumsAlbum["State"].isNull())
			albumsObject.state = valueAlbumsAlbum["State"].asString();
		if(!valueAlbumsAlbum["Remark"].isNull())
			albumsObject.remark = valueAlbumsAlbum["Remark"].asString();
		if(!valueAlbumsAlbum["PhotosCount"].isNull())
			albumsObject.photosCount = std::stol(valueAlbumsAlbum["PhotosCount"].asString());
		if(!valueAlbumsAlbum["Ctime"].isNull())
			albumsObject.ctime = std::stol(valueAlbumsAlbum["Ctime"].asString());
		if(!valueAlbumsAlbum["Mtime"].isNull())
			albumsObject.mtime = std::stol(valueAlbumsAlbum["Mtime"].asString());
		auto coverNode = value["Cover"];
		if(!coverNode["Id"].isNull())
			albumsObject.cover.id = std::stol(coverNode["Id"].asString());
		if(!coverNode["IdStr"].isNull())
			albumsObject.cover.idStr = coverNode["IdStr"].asString();
		if(!coverNode["Title"].isNull())
			albumsObject.cover.title = coverNode["Title"].asString();
		if(!coverNode["FileId"].isNull())
			albumsObject.cover.fileId = coverNode["FileId"].asString();
		if(!coverNode["State"].isNull())
			albumsObject.cover.state = coverNode["State"].asString();
		if(!coverNode["Md5"].isNull())
			albumsObject.cover.md5 = coverNode["Md5"].asString();
		if(!coverNode["IsVideo"].isNull())
			albumsObject.cover.isVideo = coverNode["IsVideo"].asString() == "true";
		if(!coverNode["Remark"].isNull())
			albumsObject.cover.remark = coverNode["Remark"].asString();
		if(!coverNode["Width"].isNull())
			albumsObject.cover.width = std::stol(coverNode["Width"].asString());
		if(!coverNode["Height"].isNull())
			albumsObject.cover.height = std::stol(coverNode["Height"].asString());
		if(!coverNode["Ctime"].isNull())
			albumsObject.cover.ctime = std::stol(coverNode["Ctime"].asString());
		if(!coverNode["Mtime"].isNull())
			albumsObject.cover.mtime = std::stol(coverNode["Mtime"].asString());
		albums_.push_back(albumsObject);
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

std::string ListAlbumsResult::getNextCursor()const
{
	return nextCursor_;
}

int ListAlbumsResult::getTotalCount()const
{
	return totalCount_;
}

std::string ListAlbumsResult::getAction()const
{
	return action_;
}

std::string ListAlbumsResult::getMessage()const
{
	return message_;
}

std::string ListAlbumsResult::getCode()const
{
	return code_;
}

std::vector<ListAlbumsResult::Album> ListAlbumsResult::getAlbums()const
{
	return albums_;
}

