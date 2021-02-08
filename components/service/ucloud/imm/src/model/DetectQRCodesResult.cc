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

#include <alibabacloud/imm/model/DetectQRCodesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

DetectQRCodesResult::DetectQRCodesResult() :
	ServiceResult()
{}

DetectQRCodesResult::DetectQRCodesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DetectQRCodesResult::~DetectQRCodesResult()
{}

void DetectQRCodesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allSuccessDetailsNode = value["SuccessDetails"]["SuccessDetailsItem"];
	for (auto valueSuccessDetailsSuccessDetailsItem : allSuccessDetailsNode)
	{
		SuccessDetailsItem successDetailsObject;
		if(!valueSuccessDetailsSuccessDetailsItem["SrcUri"].isNull())
			successDetailsObject.srcUri = valueSuccessDetailsSuccessDetailsItem["SrcUri"].asString();
		auto allQRCodesNode = valueSuccessDetailsSuccessDetailsItem["QRCodes"]["QRCodesItem"];
		for (auto valueSuccessDetailsSuccessDetailsItemQRCodesQRCodesItem : allQRCodesNode)
		{
			SuccessDetailsItem::QRCodesItem qRCodesObject;
			if(!valueSuccessDetailsSuccessDetailsItemQRCodesQRCodesItem["Content"].isNull())
				qRCodesObject.content = valueSuccessDetailsSuccessDetailsItemQRCodesQRCodesItem["Content"].asString();
			auto qRCodesRectangleNode = value["QRCodesRectangle"];
			if(!qRCodesRectangleNode["Left"].isNull())
				qRCodesObject.qRCodesRectangle.left = qRCodesRectangleNode["Left"].asString();
			if(!qRCodesRectangleNode["Top"].isNull())
				qRCodesObject.qRCodesRectangle.top = qRCodesRectangleNode["Top"].asString();
			if(!qRCodesRectangleNode["Width"].isNull())
				qRCodesObject.qRCodesRectangle.width = qRCodesRectangleNode["Width"].asString();
			if(!qRCodesRectangleNode["Height"].isNull())
				qRCodesObject.qRCodesRectangle.height = qRCodesRectangleNode["Height"].asString();
			successDetailsObject.qRCodes.push_back(qRCodesObject);
		}
		successDetails_.push_back(successDetailsObject);
	}
	auto allFailDetailsNode = value["FailDetails"]["FailDetailsItem"];
	for (auto valueFailDetailsFailDetailsItem : allFailDetailsNode)
	{
		FailDetailsItem failDetailsObject;
		if(!valueFailDetailsFailDetailsItem["SrcUri"].isNull())
			failDetailsObject.srcUri = valueFailDetailsFailDetailsItem["SrcUri"].asString();
		if(!valueFailDetailsFailDetailsItem["ErrorCode"].isNull())
			failDetailsObject.errorCode = valueFailDetailsFailDetailsItem["ErrorCode"].asString();
		if(!valueFailDetailsFailDetailsItem["ErrorMessage"].isNull())
			failDetailsObject.errorMessage = valueFailDetailsFailDetailsItem["ErrorMessage"].asString();
		failDetails_.push_back(failDetailsObject);
	}

}

std::vector<DetectQRCodesResult::SuccessDetailsItem> DetectQRCodesResult::getSuccessDetails()const
{
	return successDetails_;
}

std::vector<DetectQRCodesResult::FailDetailsItem> DetectQRCodesResult::getFailDetails()const
{
	return failDetails_;
}

