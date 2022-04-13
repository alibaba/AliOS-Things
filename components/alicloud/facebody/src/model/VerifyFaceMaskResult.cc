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

#include <alibabacloud/facebody/model/VerifyFaceMaskResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Facebody;
using namespace AlibabaCloud::Facebody::Model;

VerifyFaceMaskResult::VerifyFaceMaskResult() :
	ServiceResult()
{}

VerifyFaceMaskResult::VerifyFaceMaskResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

VerifyFaceMaskResult::~VerifyFaceMaskResult()
{}

void VerifyFaceMaskResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Confidence"].isNull())
		data_.confidence = std::stof(dataNode["Confidence"].asString());
	if(!dataNode["Mask"].isNull())
		data_.mask = std::stoi(dataNode["Mask"].asString());
	if(!dataNode["MaskRef"].isNull())
		data_.maskRef = std::stoi(dataNode["MaskRef"].asString());
		auto allRectangle = dataNode["Rectangle"]["Rectangle"];
		for (auto value : allRectangle)
			data_.rectangle.push_back(value.asString());
		auto allRectangleRef = dataNode["RectangleRef"]["RectangleRef"];
		for (auto value : allRectangleRef)
			data_.rectangleRef.push_back(value.asString());
		auto allThresholds = dataNode["Thresholds"]["Thresholds"];
		for (auto value : allThresholds)
			data_.thresholds.push_back(value.asString());

}

VerifyFaceMaskResult::Data VerifyFaceMaskResult::getData()const
{
	return data_;
}

