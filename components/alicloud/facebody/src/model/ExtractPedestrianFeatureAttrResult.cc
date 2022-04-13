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

#include <alibabacloud/facebody/model/ExtractPedestrianFeatureAttrResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Facebody;
using namespace AlibabaCloud::Facebody::Model;

ExtractPedestrianFeatureAttrResult::ExtractPedestrianFeatureAttrResult() :
	ServiceResult()
{}

ExtractPedestrianFeatureAttrResult::ExtractPedestrianFeatureAttrResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ExtractPedestrianFeatureAttrResult::~ExtractPedestrianFeatureAttrResult()
{}

void ExtractPedestrianFeatureAttrResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["ObjType"].isNull())
		data_.objType = dataNode["ObjType"].asString();
	if(!dataNode["ObjTypeScore"].isNull())
		data_.objTypeScore = std::stof(dataNode["ObjTypeScore"].asString());
	if(!dataNode["Feature"].isNull())
		data_.feature = dataNode["Feature"].asString();
	if(!dataNode["QualityScore"].isNull())
		data_.qualityScore = std::stof(dataNode["QualityScore"].asString());
	if(!dataNode["UpperColor"].isNull())
		data_.upperColor = dataNode["UpperColor"].asString();
	if(!dataNode["UpperColorScore"].isNull())
		data_.upperColorScore = std::stof(dataNode["UpperColorScore"].asString());
	if(!dataNode["UpperType"].isNull())
		data_.upperType = dataNode["UpperType"].asString();
	if(!dataNode["UpperTypeScore"].isNull())
		data_.upperTypeScore = std::stof(dataNode["UpperTypeScore"].asString());
	if(!dataNode["LowerColor"].isNull())
		data_.lowerColor = dataNode["LowerColor"].asString();
	if(!dataNode["LowerColorScore"].isNull())
		data_.lowerColorScore = std::stof(dataNode["LowerColorScore"].asString());
	if(!dataNode["LowerType"].isNull())
		data_.lowerType = dataNode["LowerType"].asString();
	if(!dataNode["LowerTypeScore"].isNull())
		data_.lowerTypeScore = std::stof(dataNode["LowerTypeScore"].asString());
	if(!dataNode["Gender"].isNull())
		data_.gender = dataNode["Gender"].asString();
	if(!dataNode["GenderScore"].isNull())
		data_.genderScore = std::stof(dataNode["GenderScore"].asString());
	if(!dataNode["Hair"].isNull())
		data_.hair = dataNode["Hair"].asString();
	if(!dataNode["HairScore"].isNull())
		data_.hairScore = std::stof(dataNode["HairScore"].asString());
	if(!dataNode["Age"].isNull())
		data_.age = dataNode["Age"].asString();
	if(!dataNode["AgeScore"].isNull())
		data_.ageScore = std::stof(dataNode["AgeScore"].asString());

}

ExtractPedestrianFeatureAttrResult::Data ExtractPedestrianFeatureAttrResult::getData()const
{
	return data_;
}

