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

#include <alibabacloud/imagerecog/model/DetectFruitsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imagerecog;
using namespace AlibabaCloud::Imagerecog::Model;

DetectFruitsResult::DetectFruitsResult() :
	ServiceResult()
{}

DetectFruitsResult::DetectFruitsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DetectFruitsResult::~DetectFruitsResult()
{}

void DetectFruitsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allElementsNode = dataNode["Elements"];
	for (auto dataNodeElementsElement : allElementsNode)
	{
		Data::Element elementObject;
		if(!dataNodeElementsElement["Name"].isNull())
			elementObject.name = dataNodeElementsElement["Name"].asString();
		if(!dataNodeElementsElement["Score"].isNull())
			elementObject.score = std::stof(dataNodeElementsElement["Score"].asString());
		auto allBox = dataNodeElementsElement["Box"];
		for (auto value : allBox)
			elementObject.box.push_back(value.asString());
		data_.elements.push_back(elementObject);
	}

}

DetectFruitsResult::Data DetectFruitsResult::getData()const
{
	return data_;
}

