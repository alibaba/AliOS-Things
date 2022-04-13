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

#include <alibabacloud/imagerecog/model/ClassifyingRubbishResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imagerecog;
using namespace AlibabaCloud::Imagerecog::Model;

ClassifyingRubbishResult::ClassifyingRubbishResult() :
	ServiceResult()
{}

ClassifyingRubbishResult::ClassifyingRubbishResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ClassifyingRubbishResult::~ClassifyingRubbishResult()
{}

void ClassifyingRubbishResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Sensitive"].isNull())
		data_.sensitive = dataNode["Sensitive"].asString() == "true";
	auto allElementsNode = dataNode["Elements"];
	for (auto dataNodeElementsElement : allElementsNode)
	{
		Data::Element elementObject;
		if(!dataNodeElementsElement["Category"].isNull())
			elementObject.category = dataNodeElementsElement["Category"].asString();
		if(!dataNodeElementsElement["CategoryScore"].isNull())
			elementObject.categoryScore = std::stof(dataNodeElementsElement["CategoryScore"].asString());
		if(!dataNodeElementsElement["Rubbish"].isNull())
			elementObject.rubbish = dataNodeElementsElement["Rubbish"].asString();
		if(!dataNodeElementsElement["RubbishScore"].isNull())
			elementObject.rubbishScore = std::stof(dataNodeElementsElement["RubbishScore"].asString());
		data_.elements.push_back(elementObject);
	}

}

ClassifyingRubbishResult::Data ClassifyingRubbishResult::getData()const
{
	return data_;
}

