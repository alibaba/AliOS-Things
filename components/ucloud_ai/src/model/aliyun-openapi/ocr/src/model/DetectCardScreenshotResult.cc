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

#include <alibabacloud/ocr/model/DetectCardScreenshotResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

DetectCardScreenshotResult::DetectCardScreenshotResult() :
	ServiceResult()
{}

DetectCardScreenshotResult::DetectCardScreenshotResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DetectCardScreenshotResult::~DetectCardScreenshotResult()
{}

void DetectCardScreenshotResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["IsCard"].isNull())
		data_.isCard = dataNode["IsCard"].asString() == "true";
	if(!dataNode["IsBlur"].isNull())
		data_.isBlur = dataNode["IsBlur"].asString() == "true";
	auto spoofResultNode = dataNode["SpoofResult"];
	if(!spoofResultNode["IsSpoof"].isNull())
		data_.spoofResult.isSpoof = spoofResultNode["IsSpoof"].asString() == "true";
	auto resultMapNode = spoofResultNode["ResultMap"];
	if(!resultMapNode["ScreenScore"].isNull())
		data_.spoofResult.resultMap.screenScore = std::stof(resultMapNode["ScreenScore"].asString());
	if(!resultMapNode["ScreenThreshold"].isNull())
		data_.spoofResult.resultMap.screenThreshold = std::stof(resultMapNode["ScreenThreshold"].asString());

}

DetectCardScreenshotResult::Data DetectCardScreenshotResult::getData()const
{
	return data_;
}

