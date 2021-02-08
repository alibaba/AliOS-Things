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

#include <alibabacloud/alimt/model/GetImageTranslateResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Alimt;
using namespace AlibabaCloud::Alimt::Model;

GetImageTranslateResult::GetImageTranslateResult() :
	ServiceResult()
{}

GetImageTranslateResult::GetImageTranslateResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetImageTranslateResult::~GetImageTranslateResult()
{}

void GetImageTranslateResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Url"].isNull())
		data_.url = dataNode["Url"].asString();
	if(!dataNode["Orc"].isNull())
		data_.orc = dataNode["Orc"].asString();
	if(!dataNode["PictureEditor"].isNull())
		data_.pictureEditor = dataNode["PictureEditor"].asString();
	if(!value["Code"].isNull())
		code_ = std::stoi(value["Code"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();

}

std::string GetImageTranslateResult::getMessage()const
{
	return message_;
}

GetImageTranslateResult::Data GetImageTranslateResult::getData()const
{
	return data_;
}

int GetImageTranslateResult::getCode()const
{
	return code_;
}

