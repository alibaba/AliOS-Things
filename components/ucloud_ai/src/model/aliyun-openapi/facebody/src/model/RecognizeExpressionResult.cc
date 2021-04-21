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

#include <alibabacloud/facebody/model/RecognizeExpressionResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Facebody;
using namespace AlibabaCloud::Facebody::Model;

RecognizeExpressionResult::RecognizeExpressionResult() :
	ServiceResult()
{}

RecognizeExpressionResult::RecognizeExpressionResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeExpressionResult::~RecognizeExpressionResult()
{}

void RecognizeExpressionResult::parse(const std::string &payload)
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
		if(!dataNodeElementsElement["Expression"].isNull())
			elementObject.expression = dataNodeElementsElement["Expression"].asString();
		if(!dataNodeElementsElement["FaceProbability"].isNull())
			elementObject.faceProbability = std::stof(dataNodeElementsElement["FaceProbability"].asString());
		auto faceRectangleNode = value["FaceRectangle"];
		if(!faceRectangleNode["Height"].isNull())
			elementObject.faceRectangle.height = std::stoi(faceRectangleNode["Height"].asString());
		if(!faceRectangleNode["Left"].isNull())
			elementObject.faceRectangle.left = std::stoi(faceRectangleNode["Left"].asString());
		if(!faceRectangleNode["Top"].isNull())
			elementObject.faceRectangle.top = std::stoi(faceRectangleNode["Top"].asString());
		if(!faceRectangleNode["Width"].isNull())
			elementObject.faceRectangle.width = std::stoi(faceRectangleNode["Width"].asString());
		data_.elements.push_back(elementObject);
	}

}

RecognizeExpressionResult::Data RecognizeExpressionResult::getData()const
{
	return data_;
}

