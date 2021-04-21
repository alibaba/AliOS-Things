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

#include <alibabacloud/facebody/model/DetectChefCapResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Facebody;
using namespace AlibabaCloud::Facebody::Model;

DetectChefCapResult::DetectChefCapResult() :
	ServiceResult()
{}

DetectChefCapResult::DetectChefCapResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DetectChefCapResult::~DetectChefCapResult()
{}

void DetectChefCapResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allElementsNode = dataNode["Elements"]["ElementsItem"];
	for (auto dataNodeElementsElementsItem : allElementsNode)
	{
		Data::ElementsItem elementsItemObject;
		if(!dataNodeElementsElementsItem["Category"].isNull())
			elementsItemObject.category = dataNodeElementsElementsItem["Category"].asString();
		if(!dataNodeElementsElementsItem["Confidence"].isNull())
			elementsItemObject.confidence = std::stof(dataNodeElementsElementsItem["Confidence"].asString());
		auto allBox = value["Box"]["Box"];
		for (auto value : allBox)
			elementsItemObject.box.push_back(value.asString());
		data_.elements.push_back(elementsItemObject);
	}

}

DetectChefCapResult::Data DetectChefCapResult::getData()const
{
	return data_;
}

