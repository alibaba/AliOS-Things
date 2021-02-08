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

#include <alibabacloud/imm/model/DetectImageQRCodesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

DetectImageQRCodesResult::DetectImageQRCodesResult() :
	ServiceResult()
{}

DetectImageQRCodesResult::DetectImageQRCodesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DetectImageQRCodesResult::~DetectImageQRCodesResult()
{}

void DetectImageQRCodesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allQRCodesNode = value["QRCodes"]["QRCodesItem"];
	for (auto valueQRCodesQRCodesItem : allQRCodesNode)
	{
		QRCodesItem qRCodesObject;
		if(!valueQRCodesQRCodesItem["Content"].isNull())
			qRCodesObject.content = valueQRCodesQRCodesItem["Content"].asString();
		auto qRCodeBoundaryNode = value["QRCodeBoundary"];
		if(!qRCodeBoundaryNode["Top"].isNull())
			qRCodesObject.qRCodeBoundary.top = std::stoi(qRCodeBoundaryNode["Top"].asString());
		if(!qRCodeBoundaryNode["Left"].isNull())
			qRCodesObject.qRCodeBoundary.left = std::stoi(qRCodeBoundaryNode["Left"].asString());
		if(!qRCodeBoundaryNode["Width"].isNull())
			qRCodesObject.qRCodeBoundary.width = std::stoi(qRCodeBoundaryNode["Width"].asString());
		if(!qRCodeBoundaryNode["Height"].isNull())
			qRCodesObject.qRCodeBoundary.height = std::stoi(qRCodeBoundaryNode["Height"].asString());
		qRCodes_.push_back(qRCodesObject);
	}
	if(!value["ImageUri"].isNull())
		imageUri_ = value["ImageUri"].asString();

}

std::string DetectImageQRCodesResult::getImageUri()const
{
	return imageUri_;
}

std::vector<DetectImageQRCodesResult::QRCodesItem> DetectImageQRCodesResult::getQRCodes()const
{
	return qRCodes_;
}

