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

#include <alibabacloud/cloudphoto/model/CreatePhotoResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudPhoto;
using namespace AlibabaCloud::CloudPhoto::Model;

CreatePhotoResult::CreatePhotoResult() :
	ServiceResult()
{}

CreatePhotoResult::CreatePhotoResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

CreatePhotoResult::~CreatePhotoResult()
{}

void CreatePhotoResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto photoNode = value["Photo"];
	if(!photoNode["Id"].isNull())
		photo_.id = std::stol(photoNode["Id"].asString());
	if(!photoNode["IdStr"].isNull())
		photo_.idStr = photoNode["IdStr"].asString();
	if(!photoNode["Title"].isNull())
		photo_.title = photoNode["Title"].asString();
	if(!photoNode["FileId"].isNull())
		photo_.fileId = photoNode["FileId"].asString();
	if(!photoNode["Location"].isNull())
		photo_.location = photoNode["Location"].asString();
	if(!photoNode["State"].isNull())
		photo_.state = photoNode["State"].asString();
	if(!photoNode["Md5"].isNull())
		photo_.md5 = photoNode["Md5"].asString();
	if(!photoNode["IsVideo"].isNull())
		photo_.isVideo = photoNode["IsVideo"].asString() == "true";
	if(!photoNode["Size"].isNull())
		photo_.size = std::stol(photoNode["Size"].asString());
	if(!photoNode["Remark"].isNull())
		photo_.remark = photoNode["Remark"].asString();
	if(!photoNode["Width"].isNull())
		photo_.width = std::stol(photoNode["Width"].asString());
	if(!photoNode["Height"].isNull())
		photo_.height = std::stol(photoNode["Height"].asString());
	if(!photoNode["Ctime"].isNull())
		photo_.ctime = std::stol(photoNode["Ctime"].asString());
	if(!photoNode["Mtime"].isNull())
		photo_.mtime = std::stol(photoNode["Mtime"].asString());
	if(!photoNode["TakenAt"].isNull())
		photo_.takenAt = std::stol(photoNode["TakenAt"].asString());
	if(!photoNode["ShareExpireTime"].isNull())
		photo_.shareExpireTime = std::stol(photoNode["ShareExpireTime"].asString());
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Action"].isNull())
		action_ = value["Action"].asString();

}

std::string CreatePhotoResult::getAction()const
{
	return action_;
}

std::string CreatePhotoResult::getMessage()const
{
	return message_;
}

CreatePhotoResult::Photo CreatePhotoResult::getPhoto()const
{
	return photo_;
}

std::string CreatePhotoResult::getCode()const
{
	return code_;
}

