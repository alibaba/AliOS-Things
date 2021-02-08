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

#include <alibabacloud/cloudphoto/model/GetPhotosByMd5sResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudPhoto;
using namespace AlibabaCloud::CloudPhoto::Model;

GetPhotosByMd5sResult::GetPhotosByMd5sResult() :
	ServiceResult()
{}

GetPhotosByMd5sResult::GetPhotosByMd5sResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetPhotosByMd5sResult::~GetPhotosByMd5sResult()
{}

void GetPhotosByMd5sResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allPhotosNode = value["Photos"]["Photo"];
	for (auto valuePhotosPhoto : allPhotosNode)
	{
		Photo photosObject;
		if(!valuePhotosPhoto["Id"].isNull())
			photosObject.id = std::stol(valuePhotosPhoto["Id"].asString());
		if(!valuePhotosPhoto["IdStr"].isNull())
			photosObject.idStr = valuePhotosPhoto["IdStr"].asString();
		if(!valuePhotosPhoto["Title"].isNull())
			photosObject.title = valuePhotosPhoto["Title"].asString();
		if(!valuePhotosPhoto["FileId"].isNull())
			photosObject.fileId = valuePhotosPhoto["FileId"].asString();
		if(!valuePhotosPhoto["Location"].isNull())
			photosObject.location = valuePhotosPhoto["Location"].asString();
		if(!valuePhotosPhoto["State"].isNull())
			photosObject.state = valuePhotosPhoto["State"].asString();
		if(!valuePhotosPhoto["Md5"].isNull())
			photosObject.md5 = valuePhotosPhoto["Md5"].asString();
		if(!valuePhotosPhoto["IsVideo"].isNull())
			photosObject.isVideo = valuePhotosPhoto["IsVideo"].asString() == "true";
		if(!valuePhotosPhoto["Remark"].isNull())
			photosObject.remark = valuePhotosPhoto["Remark"].asString();
		if(!valuePhotosPhoto["Size"].isNull())
			photosObject.size = std::stol(valuePhotosPhoto["Size"].asString());
		if(!valuePhotosPhoto["Width"].isNull())
			photosObject.width = std::stol(valuePhotosPhoto["Width"].asString());
		if(!valuePhotosPhoto["Height"].isNull())
			photosObject.height = std::stol(valuePhotosPhoto["Height"].asString());
		if(!valuePhotosPhoto["Ctime"].isNull())
			photosObject.ctime = std::stol(valuePhotosPhoto["Ctime"].asString());
		if(!valuePhotosPhoto["Mtime"].isNull())
			photosObject.mtime = std::stol(valuePhotosPhoto["Mtime"].asString());
		if(!valuePhotosPhoto["TakenAt"].isNull())
			photosObject.takenAt = std::stol(valuePhotosPhoto["TakenAt"].asString());
		if(!valuePhotosPhoto["ShareExpireTime"].isNull())
			photosObject.shareExpireTime = std::stol(valuePhotosPhoto["ShareExpireTime"].asString());
		photos_.push_back(photosObject);
	}
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Action"].isNull())
		action_ = value["Action"].asString();

}

std::vector<GetPhotosByMd5sResult::Photo> GetPhotosByMd5sResult::getPhotos()const
{
	return photos_;
}

std::string GetPhotosByMd5sResult::getAction()const
{
	return action_;
}

std::string GetPhotosByMd5sResult::getMessage()const
{
	return message_;
}

std::string GetPhotosByMd5sResult::getCode()const
{
	return code_;
}

