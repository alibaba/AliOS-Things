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

#include <alibabacloud/imageseg/model/ParseFaceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imageseg;
using namespace AlibabaCloud::Imageseg::Model;

ParseFaceResult::ParseFaceResult() :
	ServiceResult()
{}

ParseFaceResult::ParseFaceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ParseFaceResult::~ParseFaceResult()
{}

void ParseFaceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["OriginImageURL"].isNull())
		data_.originImageURL = dataNode["OriginImageURL"].asString();
	auto allElementsNode = dataNode["Elements"]["Element"];
	for (auto dataNodeElementsElement : allElementsNode)
	{
		Data::Element elementObject;
		if(!dataNodeElementsElement["Name"].isNull())
			elementObject.name = dataNodeElementsElement["Name"].asString();
		if(!dataNodeElementsElement["ImageURL"].isNull())
			elementObject.imageURL = dataNodeElementsElement["ImageURL"].asString();
		data_.elements.push_back(elementObject);
	}

}

ParseFaceResult::Data ParseFaceResult::getData()const
{
	return data_;
}

