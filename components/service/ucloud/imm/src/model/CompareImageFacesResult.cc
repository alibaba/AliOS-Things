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

#include <alibabacloud/imm/model/CompareImageFacesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

CompareImageFacesResult::CompareImageFacesResult() :
	ServiceResult()
{}

CompareImageFacesResult::CompareImageFacesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

CompareImageFacesResult::~CompareImageFacesResult()
{}

void CompareImageFacesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto faceANode = value["FaceA"];
	if(!faceANode["FaceId"].isNull())
		faceA_.faceId = faceANode["FaceId"].asString();
	auto faceAttributesNode = faceANode["FaceAttributes"];
	auto faceBoundaryNode = faceAttributesNode["FaceBoundary"];
	if(!faceBoundaryNode["Left"].isNull())
		faceA_.faceAttributes.faceBoundary.left = std::stoi(faceBoundaryNode["Left"].asString());
	if(!faceBoundaryNode["Top"].isNull())
		faceA_.faceAttributes.faceBoundary.top = std::stoi(faceBoundaryNode["Top"].asString());
	if(!faceBoundaryNode["Width"].isNull())
		faceA_.faceAttributes.faceBoundary.width = std::stoi(faceBoundaryNode["Width"].asString());
	if(!faceBoundaryNode["Height"].isNull())
		faceA_.faceAttributes.faceBoundary.height = std::stoi(faceBoundaryNode["Height"].asString());
	auto faceBNode = value["FaceB"];
	if(!faceBNode["FaceId"].isNull())
		faceB_.faceId = faceBNode["FaceId"].asString();
	auto faceAttributes1Node = faceBNode["FaceAttributes"];
	auto faceBoundary2Node = faceAttributes1Node["FaceBoundary"];
	if(!faceBoundary2Node["Left"].isNull())
		faceB_.faceAttributes1.faceBoundary2.left = std::stoi(faceBoundary2Node["Left"].asString());
	if(!faceBoundary2Node["Top"].isNull())
		faceB_.faceAttributes1.faceBoundary2.top = std::stoi(faceBoundary2Node["Top"].asString());
	if(!faceBoundary2Node["Width"].isNull())
		faceB_.faceAttributes1.faceBoundary2.width = std::stoi(faceBoundary2Node["Width"].asString());
	if(!faceBoundary2Node["Height"].isNull())
		faceB_.faceAttributes1.faceBoundary2.height = std::stoi(faceBoundary2Node["Height"].asString());
	if(!value["SetId"].isNull())
		setId_ = value["SetId"].asString();
	if(!value["Similarity"].isNull())
		similarity_ = std::stof(value["Similarity"].asString());

}

float CompareImageFacesResult::getSimilarity()const
{
	return similarity_;
}

CompareImageFacesResult::FaceA CompareImageFacesResult::getFaceA()const
{
	return faceA_;
}

std::string CompareImageFacesResult::getSetId()const
{
	return setId_;
}

CompareImageFacesResult::FaceB CompareImageFacesResult::getFaceB()const
{
	return faceB_;
}

