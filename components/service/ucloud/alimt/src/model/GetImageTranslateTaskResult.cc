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

#include <alibabacloud/alimt/model/GetImageTranslateTaskResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Alimt;
using namespace AlibabaCloud::Alimt::Model;

GetImageTranslateTaskResult::GetImageTranslateTaskResult() :
	ServiceResult()
{}

GetImageTranslateTaskResult::GetImageTranslateTaskResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetImageTranslateTaskResult::~GetImageTranslateTaskResult()
{}

void GetImageTranslateTaskResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["ImageData"].isNull())
		data_.imageData = dataNode["ImageData"].asString();
	if(!value["Code"].isNull())
		code_ = std::stoi(value["Code"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();

}

std::string GetImageTranslateTaskResult::getMessage()const
{
	return message_;
}

GetImageTranslateTaskResult::Data GetImageTranslateTaskResult::getData()const
{
	return data_;
}

int GetImageTranslateTaskResult::getCode()const
{
	return code_;
}

