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

#include <alibabacloud/facebody/model/RecognizeActionResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Facebody;
using namespace AlibabaCloud::Facebody::Model;

RecognizeActionResult::RecognizeActionResult() :
	ServiceResult()
{}

RecognizeActionResult::RecognizeActionResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeActionResult::~RecognizeActionResult()
{}

void RecognizeActionResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allElementsNode = dataNode["Elements"]["Element"];
	for (auto dataNodeElementsElement : allElementsNode)
	{
		Data::Element elementObject;
		if(!dataNodeElementsElement["Timestamp"].isNull())
			elementObject.timestamp = std::stoi(dataNodeElementsElement["Timestamp"].asString());
		auto allBoxesNode = dataNodeElementsElement["Boxes"]["BoxesItem"];
		for (auto dataNodeElementsElementBoxesBoxesItem : allBoxesNode)
		{
			Data::Element::BoxesItem boxesObject;
			auto allBox = value["Box"]["Box"];
			for (auto value : allBox)
				boxesObject.box.push_back(value.asString());
			elementObject.boxes.push_back(boxesObject);
		}
		auto allScores = value["Scores"]["Score"];
		for (auto value : allScores)
			elementObject.scores.push_back(value.asString());
		auto allLabels = value["Labels"]["Label"];
		for (auto value : allLabels)
			elementObject.labels.push_back(value.asString());
		data_.elements.push_back(elementObject);
	}

}

RecognizeActionResult::Data RecognizeActionResult::getData()const
{
	return data_;
}

