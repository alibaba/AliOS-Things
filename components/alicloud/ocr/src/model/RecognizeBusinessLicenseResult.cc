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

#include <alibabacloud/ocr/model/RecognizeBusinessLicenseResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

RecognizeBusinessLicenseResult::RecognizeBusinessLicenseResult() :
	ServiceResult()
{}

RecognizeBusinessLicenseResult::RecognizeBusinessLicenseResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeBusinessLicenseResult::~RecognizeBusinessLicenseResult()
{}

void RecognizeBusinessLicenseResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Angle"].isNull())
		data_.angle = std::stof(dataNode["Angle"].asString());
	if(!dataNode["RegisterNumber"].isNull())
		data_.registerNumber = dataNode["RegisterNumber"].asString();
	if(!dataNode["Name"].isNull())
		data_.name = dataNode["Name"].asString();
	if(!dataNode["Type"].isNull())
		data_.type = dataNode["Type"].asString();
	if(!dataNode["LegalPerson"].isNull())
		data_.legalPerson = dataNode["LegalPerson"].asString();
	if(!dataNode["EstablishDate"].isNull())
		data_.establishDate = dataNode["EstablishDate"].asString();
	if(!dataNode["ValidPeriod"].isNull())
		data_.validPeriod = dataNode["ValidPeriod"].asString();
	if(!dataNode["Address"].isNull())
		data_.address = dataNode["Address"].asString();
	if(!dataNode["Capital"].isNull())
		data_.capital = dataNode["Capital"].asString();
	if(!dataNode["Business"].isNull())
		data_.business = dataNode["Business"].asString();
	auto emblemNode = dataNode["Emblem"];
	if(!emblemNode["Top"].isNull())
		data_.emblem.top = std::stoi(emblemNode["Top"].asString());
	if(!emblemNode["Left"].isNull())
		data_.emblem.left = std::stoi(emblemNode["Left"].asString());
	if(!emblemNode["Height"].isNull())
		data_.emblem.height = std::stoi(emblemNode["Height"].asString());
	if(!emblemNode["Width"].isNull())
		data_.emblem.width = std::stoi(emblemNode["Width"].asString());
	auto titleNode = dataNode["Title"];
	if(!titleNode["Top"].isNull())
		data_.title.top = std::stoi(titleNode["Top"].asString());
	if(!titleNode["Left"].isNull())
		data_.title.left = std::stoi(titleNode["Left"].asString());
	if(!titleNode["Height"].isNull())
		data_.title.height = std::stoi(titleNode["Height"].asString());
	if(!titleNode["Width"].isNull())
		data_.title.width = std::stoi(titleNode["Width"].asString());
	auto stampNode = dataNode["Stamp"];
	if(!stampNode["Top"].isNull())
		data_.stamp.top = std::stoi(stampNode["Top"].asString());
	if(!stampNode["Left"].isNull())
		data_.stamp.left = std::stoi(stampNode["Left"].asString());
	if(!stampNode["Height"].isNull())
		data_.stamp.height = std::stoi(stampNode["Height"].asString());
	if(!stampNode["Width"].isNull())
		data_.stamp.width = std::stoi(stampNode["Width"].asString());
	auto qRCodeNode = dataNode["QRCode"];
	if(!qRCodeNode["Top"].isNull())
		data_.qRCode.top = std::stoi(qRCodeNode["Top"].asString());
	if(!qRCodeNode["Left"].isNull())
		data_.qRCode.left = std::stoi(qRCodeNode["Left"].asString());
	if(!qRCodeNode["Height"].isNull())
		data_.qRCode.height = std::stoi(qRCodeNode["Height"].asString());
	if(!qRCodeNode["Width"].isNull())
		data_.qRCode.width = std::stoi(qRCodeNode["Width"].asString());

}

RecognizeBusinessLicenseResult::Data RecognizeBusinessLicenseResult::getData()const
{
	return data_;
}

