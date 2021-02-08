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

#include <alibabacloud/imm/model/GetImageQualityResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

GetImageQualityResult::GetImageQualityResult() :
	ServiceResult()
{}

GetImageQualityResult::GetImageQualityResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetImageQualityResult::~GetImageQualityResult()
{}

void GetImageQualityResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto imageQualityNode = value["ImageQuality"];
	if(!imageQualityNode["OverallScore"].isNull())
		imageQuality_.overallScore = std::stof(imageQualityNode["OverallScore"].asString());
	if(!imageQualityNode["ClarityScore"].isNull())
		imageQuality_.clarityScore = std::stof(imageQualityNode["ClarityScore"].asString());
	if(!imageQualityNode["Clarity"].isNull())
		imageQuality_.clarity = std::stof(imageQualityNode["Clarity"].asString());
	if(!imageQualityNode["ExposureScore"].isNull())
		imageQuality_.exposureScore = std::stof(imageQualityNode["ExposureScore"].asString());
	if(!imageQualityNode["Exposure"].isNull())
		imageQuality_.exposure = std::stof(imageQualityNode["Exposure"].asString());
	if(!imageQualityNode["ContrastScore"].isNull())
		imageQuality_.contrastScore = std::stof(imageQualityNode["ContrastScore"].asString());
	if(!imageQualityNode["Contrast"].isNull())
		imageQuality_.contrast = std::stof(imageQualityNode["Contrast"].asString());
	if(!imageQualityNode["ColorScore"].isNull())
		imageQuality_.colorScore = std::stof(imageQualityNode["ColorScore"].asString());
	if(!imageQualityNode["Color"].isNull())
		imageQuality_.color = std::stof(imageQualityNode["Color"].asString());
	if(!imageQualityNode["CompositionScore"].isNull())
		imageQuality_.compositionScore = std::stof(imageQualityNode["CompositionScore"].asString());
	if(!value["ImageUri"].isNull())
		imageUri_ = value["ImageUri"].asString();

}

GetImageQualityResult::ImageQuality GetImageQualityResult::getImageQuality()const
{
	return imageQuality_;
}

std::string GetImageQualityResult::getImageUri()const
{
	return imageUri_;
}

