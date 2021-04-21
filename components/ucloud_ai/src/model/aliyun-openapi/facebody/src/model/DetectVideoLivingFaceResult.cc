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

#include <alibabacloud/facebody/model/DetectVideoLivingFaceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Facebody;
using namespace AlibabaCloud::Facebody::Model;

DetectVideoLivingFaceResult::DetectVideoLivingFaceResult() :
	ServiceResult()
{}

DetectVideoLivingFaceResult::DetectVideoLivingFaceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DetectVideoLivingFaceResult::~DetectVideoLivingFaceResult()
{}

void DetectVideoLivingFaceResult::parse(const std::string &payload)
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
		if(!dataNodeElementsElement["LiveConfidence"].isNull())
			elementObject.liveConfidence = std::stof(dataNodeElementsElement["LiveConfidence"].asString());
		if(!dataNodeElementsElement["FaceConfidence"].isNull())
			elementObject.faceConfidence = std::stof(dataNodeElementsElement["FaceConfidence"].asString());
		auto allRect = value["Rect"]["Result"];
		for (auto value : allRect)
			elementObject.rect.push_back(value.asString());
		data_.elements.push_back(elementObject);
	}

}

DetectVideoLivingFaceResult::Data DetectVideoLivingFaceResult::getData()const
{
	return data_;
}

