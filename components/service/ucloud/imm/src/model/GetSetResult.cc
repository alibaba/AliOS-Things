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

#include <alibabacloud/imm/model/GetSetResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

GetSetResult::GetSetResult() :
	ServiceResult()
{}

GetSetResult::GetSetResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetSetResult::~GetSetResult()
{}

void GetSetResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["SetId"].isNull())
		setId_ = value["SetId"].asString();
	if(!value["SetName"].isNull())
		setName_ = value["SetName"].asString();
	if(!value["CreateTime"].isNull())
		createTime_ = value["CreateTime"].asString();
	if(!value["ModifyTime"].isNull())
		modifyTime_ = value["ModifyTime"].asString();
	if(!value["FaceCount"].isNull())
		faceCount_ = std::stoi(value["FaceCount"].asString());
	if(!value["ImageCount"].isNull())
		imageCount_ = std::stoi(value["ImageCount"].asString());
	if(!value["VideoCount"].isNull())
		videoCount_ = std::stoi(value["VideoCount"].asString());
	if(!value["VideoLength"].isNull())
		videoLength_ = std::stoi(value["VideoLength"].asString());

}

std::string GetSetResult::getModifyTime()const
{
	return modifyTime_;
}

int GetSetResult::getImageCount()const
{
	return imageCount_;
}

int GetSetResult::getVideoCount()const
{
	return videoCount_;
}

std::string GetSetResult::getCreateTime()const
{
	return createTime_;
}

std::string GetSetResult::getSetName()const
{
	return setName_;
}

std::string GetSetResult::getSetId()const
{
	return setId_;
}

int GetSetResult::getFaceCount()const
{
	return faceCount_;
}

int GetSetResult::getVideoLength()const
{
	return videoLength_;
}

