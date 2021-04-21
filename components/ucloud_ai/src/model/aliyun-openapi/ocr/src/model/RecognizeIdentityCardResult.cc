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

#include <alibabacloud/ocr/model/RecognizeIdentityCardResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

RecognizeIdentityCardResult::RecognizeIdentityCardResult() :
	ServiceResult()
{}

RecognizeIdentityCardResult::RecognizeIdentityCardResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeIdentityCardResult::~RecognizeIdentityCardResult()
{}

void RecognizeIdentityCardResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto frontResultNode = dataNode["FrontResult"];
	if(!frontResultNode["Address"].isNull())
		data_.frontResult.address = frontResultNode["Address"].asString();
	if(!frontResultNode["Name"].isNull())
		data_.frontResult.name = frontResultNode["Name"].asString();
	if(!frontResultNode["Nationality"].isNull())
		data_.frontResult.nationality = frontResultNode["Nationality"].asString();
	if(!frontResultNode["IDNumber"].isNull())
		data_.frontResult.iDNumber = frontResultNode["IDNumber"].asString();
	if(!frontResultNode["Gender"].isNull())
		data_.frontResult.gender = frontResultNode["Gender"].asString();
	if(!frontResultNode["BirthDate"].isNull())
		data_.frontResult.birthDate = frontResultNode["BirthDate"].asString();
	auto allCardAreasNode = frontResultNode["CardAreas"];
	for (auto frontResultNodeCardAreasCardArea : allCardAreasNode)
	{
		Data::FrontResult::CardArea cardAreaObject;
		if(!frontResultNodeCardAreasCardArea["X"].isNull())
			cardAreaObject.x = std::stof(frontResultNodeCardAreasCardArea["X"].asString());
		if(!frontResultNodeCardAreasCardArea["Y"].isNull())
			cardAreaObject.y = std::stof(frontResultNodeCardAreasCardArea["Y"].asString());
		data_.frontResult.cardAreas.push_back(cardAreaObject);
	}
	auto allFaceRectVerticesNode = frontResultNode["FaceRectVertices"];
	for (auto frontResultNodeFaceRectVerticesFaceRectVertice : allFaceRectVerticesNode)
	{
		Data::FrontResult::FaceRectVertice faceRectVerticeObject;
		if(!frontResultNodeFaceRectVerticesFaceRectVertice["X"].isNull())
			faceRectVerticeObject.x = std::stof(frontResultNodeFaceRectVerticesFaceRectVertice["X"].asString());
		if(!frontResultNodeFaceRectVerticesFaceRectVertice["Y"].isNull())
			faceRectVerticeObject.y = std::stof(frontResultNodeFaceRectVerticesFaceRectVertice["Y"].asString());
		data_.frontResult.faceRectVertices.push_back(faceRectVerticeObject);
	}
	auto faceRectangleNode = frontResultNode["FaceRectangle"];
	if(!faceRectangleNode["Angle"].isNull())
		data_.frontResult.faceRectangle.angle = std::stof(faceRectangleNode["Angle"].asString());
	auto centerNode = faceRectangleNode["Center"];
	if(!centerNode["X"].isNull())
		data_.frontResult.faceRectangle.center.x = std::stof(centerNode["X"].asString());
	if(!centerNode["Y"].isNull())
		data_.frontResult.faceRectangle.center.y = std::stof(centerNode["Y"].asString());
	auto sizeNode = faceRectangleNode["Size"];
	if(!sizeNode["Height"].isNull())
		data_.frontResult.faceRectangle.size.height = std::stof(sizeNode["Height"].asString());
	if(!sizeNode["Width"].isNull())
		data_.frontResult.faceRectangle.size.width = std::stof(sizeNode["Width"].asString());
	auto backResultNode = dataNode["BackResult"];
	if(!backResultNode["StartDate"].isNull())
		data_.backResult.startDate = backResultNode["StartDate"].asString();
	if(!backResultNode["EndDate"].isNull())
		data_.backResult.endDate = backResultNode["EndDate"].asString();
	if(!backResultNode["Issue"].isNull())
		data_.backResult.issue = backResultNode["Issue"].asString();

}

RecognizeIdentityCardResult::Data RecognizeIdentityCardResult::getData()const
{
	return data_;
}

