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

#include <alibabacloud/imm/model/DetectImageCelebrityResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

DetectImageCelebrityResult::DetectImageCelebrityResult() :
	ServiceResult()
{}

DetectImageCelebrityResult::DetectImageCelebrityResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DetectImageCelebrityResult::~DetectImageCelebrityResult()
{}

void DetectImageCelebrityResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allCelebrityNode = value["Celebrity"]["CelebrityItem"];
	for (auto valueCelebrityCelebrityItem : allCelebrityNode)
	{
		CelebrityItem celebrityObject;
		if(!valueCelebrityCelebrityItem["CelebrityName"].isNull())
			celebrityObject.celebrityName = valueCelebrityCelebrityItem["CelebrityName"].asString();
		if(!valueCelebrityCelebrityItem["CelebrityGender"].isNull())
			celebrityObject.celebrityGender = valueCelebrityCelebrityItem["CelebrityGender"].asString();
		if(!valueCelebrityCelebrityItem["CelebrityConfidence"].isNull())
			celebrityObject.celebrityConfidence = std::stof(valueCelebrityCelebrityItem["CelebrityConfidence"].asString());
		if(!valueCelebrityCelebrityItem["CelebrityLibraryName"].isNull())
			celebrityObject.celebrityLibraryName = valueCelebrityCelebrityItem["CelebrityLibraryName"].asString();
		auto celebrityBoundaryNode = value["CelebrityBoundary"];
		if(!celebrityBoundaryNode["Left"].isNull())
			celebrityObject.celebrityBoundary.left = std::stoi(celebrityBoundaryNode["Left"].asString());
		if(!celebrityBoundaryNode["Top"].isNull())
			celebrityObject.celebrityBoundary.top = std::stoi(celebrityBoundaryNode["Top"].asString());
		if(!celebrityBoundaryNode["Width"].isNull())
			celebrityObject.celebrityBoundary.width = std::stoi(celebrityBoundaryNode["Width"].asString());
		if(!celebrityBoundaryNode["Height"].isNull())
			celebrityObject.celebrityBoundary.height = std::stoi(celebrityBoundaryNode["Height"].asString());
		celebrity_.push_back(celebrityObject);
	}
	if(!value["ImageUri"].isNull())
		imageUri_ = value["ImageUri"].asString();

}

std::vector<DetectImageCelebrityResult::CelebrityItem> DetectImageCelebrityResult::getCelebrity()const
{
	return celebrity_;
}

std::string DetectImageCelebrityResult::getImageUri()const
{
	return imageUri_;
}

