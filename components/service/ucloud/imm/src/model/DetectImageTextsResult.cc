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

#include <alibabacloud/imm/model/DetectImageTextsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

DetectImageTextsResult::DetectImageTextsResult() :
	ServiceResult()
{}

DetectImageTextsResult::DetectImageTextsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DetectImageTextsResult::~DetectImageTextsResult()
{}

void DetectImageTextsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allOCRNode = value["OCR"]["OCRItem"];
	for (auto valueOCROCRItem : allOCRNode)
	{
		OCRItem oCRObject;
		if(!valueOCROCRItem["OCRContents"].isNull())
			oCRObject.oCRContents = valueOCROCRItem["OCRContents"].asString();
		if(!valueOCROCRItem["OCRConfidence"].isNull())
			oCRObject.oCRConfidence = std::stof(valueOCROCRItem["OCRConfidence"].asString());
		auto oCRBoundaryNode = value["OCRBoundary"];
		if(!oCRBoundaryNode["Left"].isNull())
			oCRObject.oCRBoundary.left = std::stoi(oCRBoundaryNode["Left"].asString());
		if(!oCRBoundaryNode["Top"].isNull())
			oCRObject.oCRBoundary.top = std::stoi(oCRBoundaryNode["Top"].asString());
		if(!oCRBoundaryNode["Width"].isNull())
			oCRObject.oCRBoundary.width = std::stoi(oCRBoundaryNode["Width"].asString());
		if(!oCRBoundaryNode["Height"].isNull())
			oCRObject.oCRBoundary.height = std::stoi(oCRBoundaryNode["Height"].asString());
		oCR_.push_back(oCRObject);
	}
	if(!value["ImageUri"].isNull())
		imageUri_ = value["ImageUri"].asString();

}

std::string DetectImageTextsResult::getImageUri()const
{
	return imageUri_;
}

std::vector<DetectImageTextsResult::OCRItem> DetectImageTextsResult::getOCR()const
{
	return oCR_;
}

