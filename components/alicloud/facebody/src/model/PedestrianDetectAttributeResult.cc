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

#include <alibabacloud/facebody/model/PedestrianDetectAttributeResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Facebody;
using namespace AlibabaCloud::Facebody::Model;

PedestrianDetectAttributeResult::PedestrianDetectAttributeResult() :
	ServiceResult()
{}

PedestrianDetectAttributeResult::PedestrianDetectAttributeResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

PedestrianDetectAttributeResult::~PedestrianDetectAttributeResult()
{}

void PedestrianDetectAttributeResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["PersonNumber"].isNull())
		data_.personNumber = std::stoi(dataNode["PersonNumber"].asString());
	auto allAttributesNode = dataNode["Attributes"]["AttributesItem"];
	for (auto dataNodeAttributesAttributesItem : allAttributesNode)
	{
		Data::AttributesItem attributesItemObject;
		auto ageNode = value["Age"];
		if(!ageNode["Name"].isNull())
			attributesItemObject.age.name = ageNode["Name"].asString();
		if(!ageNode["Score"].isNull())
			attributesItemObject.age.score = std::stof(ageNode["Score"].asString());
		auto backpackNode = value["Backpack"];
		if(!backpackNode["Name"].isNull())
			attributesItemObject.backpack.name = backpackNode["Name"].asString();
		if(!backpackNode["Score"].isNull())
			attributesItemObject.backpack.score = std::stof(backpackNode["Score"].asString());
		auto genderNode = value["Gender"];
		if(!genderNode["Name"].isNull())
			attributesItemObject.gender.name = genderNode["Name"].asString();
		if(!genderNode["Score"].isNull())
			attributesItemObject.gender.score = std::stof(genderNode["Score"].asString());
		auto glassesNode = value["Glasses"];
		if(!glassesNode["Name"].isNull())
			attributesItemObject.glasses.name = glassesNode["Name"].asString();
		if(!glassesNode["Score"].isNull())
			attributesItemObject.glasses.score = std::stof(glassesNode["Score"].asString());
		auto handbagNode = value["Handbag"];
		if(!handbagNode["Name"].isNull())
			attributesItemObject.handbag.name = handbagNode["Name"].asString();
		if(!handbagNode["Score"].isNull())
			attributesItemObject.handbag.score = std::stof(handbagNode["Score"].asString());
		auto hatNode = value["Hat"];
		if(!hatNode["Name"].isNull())
			attributesItemObject.hat.name = hatNode["Name"].asString();
		if(!hatNode["Score"].isNull())
			attributesItemObject.hat.score = std::stof(hatNode["Score"].asString());
		auto lowerColorNode = value["LowerColor"];
		if(!lowerColorNode["Name"].isNull())
			attributesItemObject.lowerColor.name = lowerColorNode["Name"].asString();
		if(!lowerColorNode["Score"].isNull())
			attributesItemObject.lowerColor.score = std::stof(lowerColorNode["Score"].asString());
		auto lowerWearNode = value["LowerWear"];
		if(!lowerWearNode["Name"].isNull())
			attributesItemObject.lowerWear.name = lowerWearNode["Name"].asString();
		if(!lowerWearNode["Score"].isNull())
			attributesItemObject.lowerWear.score = std::stof(lowerWearNode["Score"].asString());
		auto orientNode = value["Orient"];
		if(!orientNode["Name"].isNull())
			attributesItemObject.orient.name = orientNode["Name"].asString();
		if(!orientNode["Score"].isNull())
			attributesItemObject.orient.score = std::stof(orientNode["Score"].asString());
		auto shoulderBagNode = value["ShoulderBag"];
		if(!shoulderBagNode["Name"].isNull())
			attributesItemObject.shoulderBag.name = shoulderBagNode["Name"].asString();
		if(!shoulderBagNode["Score"].isNull())
			attributesItemObject.shoulderBag.score = std::stof(shoulderBagNode["Score"].asString());
		auto upperColorNode = value["UpperColor"];
		if(!upperColorNode["Name"].isNull())
			attributesItemObject.upperColor.name = upperColorNode["Name"].asString();
		if(!upperColorNode["Score"].isNull())
			attributesItemObject.upperColor.score = std::stof(upperColorNode["Score"].asString());
		auto upperWearNode = value["UpperWear"];
		if(!upperWearNode["Name"].isNull())
			attributesItemObject.upperWear.name = upperWearNode["Name"].asString();
		if(!upperWearNode["Score"].isNull())
			attributesItemObject.upperWear.score = std::stof(upperWearNode["Score"].asString());
		data_.attributes.push_back(attributesItemObject);
	}
	auto allBoxesNode = dataNode["Boxes"]["BoxesItem"];
	for (auto dataNodeBoxesBoxesItem : allBoxesNode)
	{
		Data::BoxesItem boxesItemObject;
		if(!dataNodeBoxesBoxesItem["Score"].isNull())
			boxesItemObject.score = std::stof(dataNodeBoxesBoxesItem["Score"].asString());
		if(!dataNodeBoxesBoxesItem["TopLeftX"].isNull())
			boxesItemObject.topLeftX = std::stof(dataNodeBoxesBoxesItem["TopLeftX"].asString());
		if(!dataNodeBoxesBoxesItem["TopLeftY"].isNull())
			boxesItemObject.topLeftY = std::stof(dataNodeBoxesBoxesItem["TopLeftY"].asString());
		if(!dataNodeBoxesBoxesItem["BottomRightX"].isNull())
			boxesItemObject.bottomRightX = std::stof(dataNodeBoxesBoxesItem["BottomRightX"].asString());
		if(!dataNodeBoxesBoxesItem["BottomRightY"].isNull())
			boxesItemObject.bottomRightY = std::stof(dataNodeBoxesBoxesItem["BottomRightY"].asString());
		data_.boxes.push_back(boxesItemObject);
	}

}

PedestrianDetectAttributeResult::Data PedestrianDetectAttributeResult::getData()const
{
	return data_;
}

