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

#include <alibabacloud/cloudphoto/model/CreateAlbumResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudPhoto;
using namespace AlibabaCloud::CloudPhoto::Model;

CreateAlbumResult::CreateAlbumResult() :
	ServiceResult()
{}

CreateAlbumResult::CreateAlbumResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

CreateAlbumResult::~CreateAlbumResult()
{}

void CreateAlbumResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto albumNode = value["Album"];
	if(!albumNode["Id"].isNull())
		album_.id = std::stol(albumNode["Id"].asString());
	if(!albumNode["IdStr"].isNull())
		album_.idStr = albumNode["IdStr"].asString();
	if(!albumNode["Name"].isNull())
		album_.name = albumNode["Name"].asString();
	if(!albumNode["State"].isNull())
		album_.state = albumNode["State"].asString();
	if(!albumNode["Remark"].isNull())
		album_.remark = albumNode["Remark"].asString();
	if(!albumNode["PhotosCount"].isNull())
		album_.photosCount = std::stol(albumNode["PhotosCount"].asString());
	if(!albumNode["Ctime"].isNull())
		album_.ctime = std::stol(albumNode["Ctime"].asString());
	if(!albumNode["Mtime"].isNull())
		album_.mtime = std::stol(albumNode["Mtime"].asString());
	auto coverNode = albumNode["Cover"];
	if(!coverNode["Id"].isNull())
		album_.cover.id = std::stol(coverNode["Id"].asString());
	if(!coverNode["IdStr"].isNull())
		album_.cover.idStr = coverNode["IdStr"].asString();
	if(!coverNode["Title"].isNull())
		album_.cover.title = coverNode["Title"].asString();
	if(!coverNode["FileId"].isNull())
		album_.cover.fileId = coverNode["FileId"].asString();
	if(!coverNode["State"].isNull())
		album_.cover.state = coverNode["State"].asString();
	if(!coverNode["Md5"].isNull())
		album_.cover.md5 = coverNode["Md5"].asString();
	if(!coverNode["IsVideo"].isNull())
		album_.cover.isVideo = coverNode["IsVideo"].asString() == "true";
	if(!coverNode["Width"].isNull())
		album_.cover.width = std::stol(coverNode["Width"].asString());
	if(!coverNode["Height"].isNull())
		album_.cover.height = std::stol(coverNode["Height"].asString());
	if(!coverNode["Ctime"].isNull())
		album_.cover.ctime = std::stol(coverNode["Ctime"].asString());
	if(!coverNode["Mtime"].isNull())
		album_.cover.mtime = std::stol(coverNode["Mtime"].asString());
	if(!coverNode["Remark"].isNull())
		album_.cover.remark = coverNode["Remark"].asString();
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Action"].isNull())
		action_ = value["Action"].asString();

}

std::string CreateAlbumResult::getAction()const
{
	return action_;
}

std::string CreateAlbumResult::getMessage()const
{
	return message_;
}

CreateAlbumResult::Album CreateAlbumResult::getAlbum()const
{
	return album_;
}

std::string CreateAlbumResult::getCode()const
{
	return code_;
}

