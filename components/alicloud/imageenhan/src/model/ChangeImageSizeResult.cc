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

#include <alibabacloud/imageenhan/model/ChangeImageSizeResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imageenhan;
using namespace AlibabaCloud::Imageenhan::Model;

ChangeImageSizeResult::ChangeImageSizeResult() :
	ServiceResult()
{}

ChangeImageSizeResult::ChangeImageSizeResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ChangeImageSizeResult::~ChangeImageSizeResult()
{}

void ChangeImageSizeResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Url"].isNull())
		data_.url = dataNode["Url"].asString();
	auto retainLocationNode = dataNode["RetainLocation"];
	if(!retainLocationNode["X"].isNull())
		data_.retainLocation.x = std::stoi(retainLocationNode["X"].asString());
	if(!retainLocationNode["Y"].isNull())
		data_.retainLocation.y = std::stoi(retainLocationNode["Y"].asString());
	if(!retainLocationNode["Width"].isNull())
		data_.retainLocation.width = std::stoi(retainLocationNode["Width"].asString());
	if(!retainLocationNode["Height"].isNull())
		data_.retainLocation.height = std::stoi(retainLocationNode["Height"].asString());

}

ChangeImageSizeResult::Data ChangeImageSizeResult::getData()const
{
	return data_;
}

