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

#include <alibabacloud/facebody/model/DetectPedestrianResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Facebody;
using namespace AlibabaCloud::Facebody::Model;

DetectPedestrianResult::DetectPedestrianResult() :
	ServiceResult()
{}

DetectPedestrianResult::DetectPedestrianResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DetectPedestrianResult::~DetectPedestrianResult()
{}

void DetectPedestrianResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Height"].isNull())
		data_.height = std::stoi(dataNode["Height"].asString());
	if(!dataNode["Width"].isNull())
		data_.width = std::stoi(dataNode["Width"].asString());
	auto allElementsNode = dataNode["Elements"]["Element"];
	for (auto dataNodeElementsElement : allElementsNode)
	{
		Data::Element elementObject;
		if(!dataNodeElementsElement["Score"].isNull())
			elementObject.score = std::stof(dataNodeElementsElement["Score"].asString());
		if(!dataNodeElementsElement["Type"].isNull())
			elementObject.type = dataNodeElementsElement["Type"].asString();
		auto allBoxes = value["Boxes"]["Box"];
		for (auto value : allBoxes)
			elementObject.boxes.push_back(value.asString());
		data_.elements.push_back(elementObject);
	}

}

DetectPedestrianResult::Data DetectPedestrianResult::getData()const
{
	return data_;
}

