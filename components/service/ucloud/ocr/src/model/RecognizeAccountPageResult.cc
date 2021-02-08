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

#include <alibabacloud/ocr/model/RecognizeAccountPageResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

RecognizeAccountPageResult::RecognizeAccountPageResult() :
	ServiceResult()
{}

RecognizeAccountPageResult::RecognizeAccountPageResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeAccountPageResult::~RecognizeAccountPageResult()
{}

void RecognizeAccountPageResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Angle"].isNull())
		data_.angle = std::stof(dataNode["Angle"].asString());
	if(!dataNode["Name"].isNull())
		data_.name = dataNode["Name"].asString();
	if(!dataNode["Gender"].isNull())
		data_.gender = dataNode["Gender"].asString();
	if(!dataNode["Relation"].isNull())
		data_.relation = dataNode["Relation"].asString();
	if(!dataNode["BirthPlace"].isNull())
		data_.birthPlace = dataNode["BirthPlace"].asString();
	if(!dataNode["Nationality"].isNull())
		data_.nationality = dataNode["Nationality"].asString();
	if(!dataNode["NativePlace"].isNull())
		data_.nativePlace = dataNode["NativePlace"].asString();
	if(!dataNode["BirthDate"].isNull())
		data_.birthDate = dataNode["BirthDate"].asString();
	if(!dataNode["IDNumber"].isNull())
		data_.iDNumber = dataNode["IDNumber"].asString();
	auto allInvalidStampAreasNode = dataNode["InvalidStampAreas"]["InvalidStampArea"];
	for (auto dataNodeInvalidStampAreasInvalidStampArea : allInvalidStampAreasNode)
	{
		Data::InvalidStampArea invalidStampAreaObject;
		if(!dataNodeInvalidStampAreasInvalidStampArea["Left"].isNull())
			invalidStampAreaObject.left = std::stoi(dataNodeInvalidStampAreasInvalidStampArea["Left"].asString());
		if(!dataNodeInvalidStampAreasInvalidStampArea["Top"].isNull())
			invalidStampAreaObject.top = std::stoi(dataNodeInvalidStampAreasInvalidStampArea["Top"].asString());
		if(!dataNodeInvalidStampAreasInvalidStampArea["Height"].isNull())
			invalidStampAreaObject.height = std::stoi(dataNodeInvalidStampAreasInvalidStampArea["Height"].asString());
		if(!dataNodeInvalidStampAreasInvalidStampArea["Width"].isNull())
			invalidStampAreaObject.width = std::stoi(dataNodeInvalidStampAreasInvalidStampArea["Width"].asString());
		data_.invalidStampAreas.push_back(invalidStampAreaObject);
	}
	auto allUndertakeStampAreasNode = dataNode["UndertakeStampAreas"]["UndertakeStampArea"];
	for (auto dataNodeUndertakeStampAreasUndertakeStampArea : allUndertakeStampAreasNode)
	{
		Data::UndertakeStampArea undertakeStampAreaObject;
		if(!dataNodeUndertakeStampAreasUndertakeStampArea["Left"].isNull())
			undertakeStampAreaObject.left = std::stoi(dataNodeUndertakeStampAreasUndertakeStampArea["Left"].asString());
		if(!dataNodeUndertakeStampAreasUndertakeStampArea["Top"].isNull())
			undertakeStampAreaObject.top = std::stoi(dataNodeUndertakeStampAreasUndertakeStampArea["Top"].asString());
		if(!dataNodeUndertakeStampAreasUndertakeStampArea["Height"].isNull())
			undertakeStampAreaObject.height = std::stoi(dataNodeUndertakeStampAreasUndertakeStampArea["Height"].asString());
		if(!dataNodeUndertakeStampAreasUndertakeStampArea["Width"].isNull())
			undertakeStampAreaObject.width = std::stoi(dataNodeUndertakeStampAreasUndertakeStampArea["Width"].asString());
		data_.undertakeStampAreas.push_back(undertakeStampAreaObject);
	}
	auto allRegisterStampAreasNode = dataNode["RegisterStampAreas"]["RegisterStampArea"];
	for (auto dataNodeRegisterStampAreasRegisterStampArea : allRegisterStampAreasNode)
	{
		Data::RegisterStampArea registerStampAreaObject;
		if(!dataNodeRegisterStampAreasRegisterStampArea["Left"].isNull())
			registerStampAreaObject.left = std::stoi(dataNodeRegisterStampAreasRegisterStampArea["Left"].asString());
		if(!dataNodeRegisterStampAreasRegisterStampArea["Top"].isNull())
			registerStampAreaObject.top = std::stoi(dataNodeRegisterStampAreasRegisterStampArea["Top"].asString());
		if(!dataNodeRegisterStampAreasRegisterStampArea["Height"].isNull())
			registerStampAreaObject.height = std::stoi(dataNodeRegisterStampAreasRegisterStampArea["Height"].asString());
		if(!dataNodeRegisterStampAreasRegisterStampArea["Width"].isNull())
			registerStampAreaObject.width = std::stoi(dataNodeRegisterStampAreasRegisterStampArea["Width"].asString());
		data_.registerStampAreas.push_back(registerStampAreaObject);
	}
	auto allOtherStampAreasNode = dataNode["OtherStampAreas"]["OtherStampArea"];
	for (auto dataNodeOtherStampAreasOtherStampArea : allOtherStampAreasNode)
	{
		Data::OtherStampArea otherStampAreaObject;
		if(!dataNodeOtherStampAreasOtherStampArea["Left"].isNull())
			otherStampAreaObject.left = std::stoi(dataNodeOtherStampAreasOtherStampArea["Left"].asString());
		if(!dataNodeOtherStampAreasOtherStampArea["Top"].isNull())
			otherStampAreaObject.top = std::stoi(dataNodeOtherStampAreasOtherStampArea["Top"].asString());
		if(!dataNodeOtherStampAreasOtherStampArea["Height"].isNull())
			otherStampAreaObject.height = std::stoi(dataNodeOtherStampAreasOtherStampArea["Height"].asString());
		if(!dataNodeOtherStampAreasOtherStampArea["Width"].isNull())
			otherStampAreaObject.width = std::stoi(dataNodeOtherStampAreasOtherStampArea["Width"].asString());
		data_.otherStampAreas.push_back(otherStampAreaObject);
	}
	auto titleAreaNode = dataNode["TitleArea"];
	if(!titleAreaNode["Left"].isNull())
		data_.titleArea.left = std::stoi(titleAreaNode["Left"].asString());
	if(!titleAreaNode["Top"].isNull())
		data_.titleArea.top = std::stoi(titleAreaNode["Top"].asString());
	if(!titleAreaNode["Height"].isNull())
		data_.titleArea.height = std::stoi(titleAreaNode["Height"].asString());
	if(!titleAreaNode["Width"].isNull())
		data_.titleArea.width = std::stoi(titleAreaNode["Width"].asString());

}

RecognizeAccountPageResult::Data RecognizeAccountPageResult::getData()const
{
	return data_;
}

