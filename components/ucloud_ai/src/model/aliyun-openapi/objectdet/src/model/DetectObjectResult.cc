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

#include <alibabacloud/objectdet/model/DetectObjectResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Objectdet;
using namespace AlibabaCloud::Objectdet::Model;

DetectObjectResult::DetectObjectResult() :
	ServiceResult()
{}

DetectObjectResult::DetectObjectResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DetectObjectResult::~DetectObjectResult()
{}

void DetectObjectResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Width"].isNull())
		data_.width = std::stoi(dataNode["Width"].asString());
	if(!dataNode["Height"].isNull())
		data_.height = std::stoi(dataNode["Height"].asString());
	auto allElementsNode = dataNode["Elements"];
	for (auto dataNodeElementsElement : allElementsNode)
	{
		Data::Element elementObject;
		if(!dataNodeElementsElement["Score"].isNull())
			elementObject.score = std::stof(dataNodeElementsElement["Score"].asString());
		if(!dataNodeElementsElement["Type"].isNull())
			elementObject.type = dataNodeElementsElement["Type"].asString();
		auto allBoxes = dataNodeElementsElement["Boxes"];
		for (auto value : allBoxes)
			elementObject.boxes.push_back(value.asDouble());
		data_.elements.push_back(elementObject);
	}

}

DetectObjectResult::Data DetectObjectResult::getData()const
{
	return data_;
}

