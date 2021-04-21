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

#include <alibabacloud/ocr/model/RecognizeLicensePlateResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

RecognizeLicensePlateResult::RecognizeLicensePlateResult() :
	ServiceResult()
{}

RecognizeLicensePlateResult::RecognizeLicensePlateResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeLicensePlateResult::~RecognizeLicensePlateResult()
{}

void RecognizeLicensePlateResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allPlatesNode = dataNode["Plates"]["Plate"];
	for (auto dataNodePlatesPlate : allPlatesNode)
	{
		Data::Plate plateObject;
		if(!dataNodePlatesPlate["Confidence"].isNull())
			plateObject.confidence = std::stof(dataNodePlatesPlate["Confidence"].asString());
		if(!dataNodePlatesPlate["PlateNumber"].isNull())
			plateObject.plateNumber = dataNodePlatesPlate["PlateNumber"].asString();
		if(!dataNodePlatesPlate["PlateType"].isNull())
			plateObject.plateType = dataNodePlatesPlate["PlateType"].asString();
		if(!dataNodePlatesPlate["PlateTypeConfidence"].isNull())
			plateObject.plateTypeConfidence = std::stof(dataNodePlatesPlate["PlateTypeConfidence"].asString());
		auto roiNode = value["Roi"];
		if(!roiNode["H"].isNull())
			plateObject.roi.h = std::stoi(roiNode["H"].asString());
		if(!roiNode["W"].isNull())
			plateObject.roi.w = std::stoi(roiNode["W"].asString());
		if(!roiNode["X"].isNull())
			plateObject.roi.x = std::stoi(roiNode["X"].asString());
		if(!roiNode["Y"].isNull())
			plateObject.roi.y = std::stoi(roiNode["Y"].asString());
		data_.plates.push_back(plateObject);
	}

}

RecognizeLicensePlateResult::Data RecognizeLicensePlateResult::getData()const
{
	return data_;
}

