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

#include <alibabacloud/ocr/model/RecognizePassportMRZResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

RecognizePassportMRZResult::RecognizePassportMRZResult() :
	ServiceResult()
{}

RecognizePassportMRZResult::RecognizePassportMRZResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizePassportMRZResult::~RecognizePassportMRZResult()
{}

void RecognizePassportMRZResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allRegionsNode = dataNode["Regions"]["Region"];
	for (auto dataNodeRegionsRegion : allRegionsNode)
	{
		Data::Region regionObject;
		if(!dataNodeRegionsRegion["Name"].isNull())
			regionObject.name = dataNodeRegionsRegion["Name"].asString();
		if(!dataNodeRegionsRegion["RecognitionScore"].isNull())
			regionObject.recognitionScore = std::stof(dataNodeRegionsRegion["RecognitionScore"].asString());
		if(!dataNodeRegionsRegion["Content"].isNull())
			regionObject.content = dataNodeRegionsRegion["Content"].asString();
		if(!dataNodeRegionsRegion["DetectionScore"].isNull())
			regionObject.detectionScore = std::stof(dataNodeRegionsRegion["DetectionScore"].asString());
		auto allBandBoxes = value["BandBoxes"]["BandBox"];
		for (auto value : allBandBoxes)
			regionObject.bandBoxes.push_back(value.asString());
		data_.regions.push_back(regionObject);
	}

}

RecognizePassportMRZResult::Data RecognizePassportMRZResult::getData()const
{
	return data_;
}

