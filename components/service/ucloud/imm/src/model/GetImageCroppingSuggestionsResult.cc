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

#include <alibabacloud/imm/model/GetImageCroppingSuggestionsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

GetImageCroppingSuggestionsResult::GetImageCroppingSuggestionsResult() :
	ServiceResult()
{}

GetImageCroppingSuggestionsResult::GetImageCroppingSuggestionsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetImageCroppingSuggestionsResult::~GetImageCroppingSuggestionsResult()
{}

void GetImageCroppingSuggestionsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allCroppingSuggestionsNode = value["CroppingSuggestions"]["CroppingSuggestionsItem"];
	for (auto valueCroppingSuggestionsCroppingSuggestionsItem : allCroppingSuggestionsNode)
	{
		CroppingSuggestionsItem croppingSuggestionsObject;
		if(!valueCroppingSuggestionsCroppingSuggestionsItem["Score"].isNull())
			croppingSuggestionsObject.score = std::stof(valueCroppingSuggestionsCroppingSuggestionsItem["Score"].asString());
		if(!valueCroppingSuggestionsCroppingSuggestionsItem["AspectRatio"].isNull())
			croppingSuggestionsObject.aspectRatio = valueCroppingSuggestionsCroppingSuggestionsItem["AspectRatio"].asString();
		auto croppingBoundaryNode = value["CroppingBoundary"];
		if(!croppingBoundaryNode["Left"].isNull())
			croppingSuggestionsObject.croppingBoundary.left = std::stoi(croppingBoundaryNode["Left"].asString());
		if(!croppingBoundaryNode["Top"].isNull())
			croppingSuggestionsObject.croppingBoundary.top = std::stoi(croppingBoundaryNode["Top"].asString());
		if(!croppingBoundaryNode["Width"].isNull())
			croppingSuggestionsObject.croppingBoundary.width = std::stoi(croppingBoundaryNode["Width"].asString());
		if(!croppingBoundaryNode["Height"].isNull())
			croppingSuggestionsObject.croppingBoundary.height = std::stoi(croppingBoundaryNode["Height"].asString());
		croppingSuggestions_.push_back(croppingSuggestionsObject);
	}
	if(!value["ImageUri"].isNull())
		imageUri_ = value["ImageUri"].asString();

}

std::string GetImageCroppingSuggestionsResult::getImageUri()const
{
	return imageUri_;
}

std::vector<GetImageCroppingSuggestionsResult::CroppingSuggestionsItem> GetImageCroppingSuggestionsResult::getCroppingSuggestions()const
{
	return croppingSuggestions_;
}

