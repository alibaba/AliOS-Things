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

#include <alibabacloud/ocr/model/RecognizeBusinessCardResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

RecognizeBusinessCardResult::RecognizeBusinessCardResult() :
	ServiceResult()
{}

RecognizeBusinessCardResult::RecognizeBusinessCardResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeBusinessCardResult::~RecognizeBusinessCardResult()
{}

void RecognizeBusinessCardResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Name"].isNull())
		data_.name = dataNode["Name"].asString();
		auto allCompanies = dataNode["Companies"]["Company"];
		for (auto value : allCompanies)
			data_.companies.push_back(value.asString());
		auto allDepartments = dataNode["Departments"]["Department"];
		for (auto value : allDepartments)
			data_.departments.push_back(value.asString());
		auto allTitles = dataNode["Titles"]["Title"];
		for (auto value : allTitles)
			data_.titles.push_back(value.asString());
		auto allCellPhoneNumbers = dataNode["CellPhoneNumbers"]["CellPhoneNumber"];
		for (auto value : allCellPhoneNumbers)
			data_.cellPhoneNumbers.push_back(value.asString());
		auto allOfficePhoneNumbers = dataNode["OfficePhoneNumbers"]["OfficePhoneNumber"];
		for (auto value : allOfficePhoneNumbers)
			data_.officePhoneNumbers.push_back(value.asString());
		auto allAddresses = dataNode["Addresses"]["Address"];
		for (auto value : allAddresses)
			data_.addresses.push_back(value.asString());
		auto allEmails = dataNode["Emails"]["Email"];
		for (auto value : allEmails)
			data_.emails.push_back(value.asString());

}

RecognizeBusinessCardResult::Data RecognizeBusinessCardResult::getData()const
{
	return data_;
}

