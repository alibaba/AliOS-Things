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

#include <alibabacloud/objectdet/model/DetectMainBodyResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Objectdet;
using namespace AlibabaCloud::Objectdet::Model;

DetectMainBodyResult::DetectMainBodyResult() :
	ServiceResult()
{}

DetectMainBodyResult::DetectMainBodyResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DetectMainBodyResult::~DetectMainBodyResult()
{}

void DetectMainBodyResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto locationNode = dataNode["Location"];
	if(!locationNode["X"].isNull())
		data_.location.x = std::stoi(locationNode["X"].asString());
	if(!locationNode["Y"].isNull())
		data_.location.y = std::stoi(locationNode["Y"].asString());
	if(!locationNode["Width"].isNull())
		data_.location.width = std::stoi(locationNode["Width"].asString());
	if(!locationNode["Height"].isNull())
		data_.location.height = std::stoi(locationNode["Height"].asString());

}

DetectMainBodyResult::Data DetectMainBodyResult::getData()const
{
	return data_;
}

