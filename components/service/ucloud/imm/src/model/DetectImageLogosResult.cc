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

#include <alibabacloud/imm/model/DetectImageLogosResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

DetectImageLogosResult::DetectImageLogosResult() :
	ServiceResult()
{}

DetectImageLogosResult::DetectImageLogosResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DetectImageLogosResult::~DetectImageLogosResult()
{}

void DetectImageLogosResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allLogosNode = value["Logos"]["LogosItem"];
	for (auto valueLogosLogosItem : allLogosNode)
	{
		LogosItem logosObject;
		if(!valueLogosLogosItem["LogoName"].isNull())
			logosObject.logoName = valueLogosLogosItem["LogoName"].asString();
		if(!valueLogosLogosItem["LogoConfidence"].isNull())
			logosObject.logoConfidence = std::stof(valueLogosLogosItem["LogoConfidence"].asString());
		auto logoBoundaryNode = value["LogoBoundary"];
		if(!logoBoundaryNode["Width"].isNull())
			logosObject.logoBoundary.width = std::stoi(logoBoundaryNode["Width"].asString());
		if(!logoBoundaryNode["Height"].isNull())
			logosObject.logoBoundary.height = std::stoi(logoBoundaryNode["Height"].asString());
		if(!logoBoundaryNode["Top"].isNull())
			logosObject.logoBoundary.top = std::stoi(logoBoundaryNode["Top"].asString());
		if(!logoBoundaryNode["Left"].isNull())
			logosObject.logoBoundary.left = std::stoi(logoBoundaryNode["Left"].asString());
		logos_.push_back(logosObject);
	}
	if(!value["ImageUri"].isNull())
		imageUri_ = value["ImageUri"].asString();

}

std::vector<DetectImageLogosResult::LogosItem> DetectImageLogosResult::getLogos()const
{
	return logos_;
}

std::string DetectImageLogosResult::getImageUri()const
{
	return imageUri_;
}

