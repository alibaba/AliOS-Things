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

#include <alibabacloud/ocr/model/RecognizeChinapassportResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

RecognizeChinapassportResult::RecognizeChinapassportResult() :
	ServiceResult()
{}

RecognizeChinapassportResult::RecognizeChinapassportResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeChinapassportResult::~RecognizeChinapassportResult()
{}

void RecognizeChinapassportResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Authority"].isNull())
		data_.authority = dataNode["Authority"].asString();
	if(!dataNode["BirthDate"].isNull())
		data_.birthDate = dataNode["BirthDate"].asString();
	if(!dataNode["BirthDay"].isNull())
		data_.birthDay = dataNode["BirthDay"].asString();
	if(!dataNode["BirthPlace"].isNull())
		data_.birthPlace = dataNode["BirthPlace"].asString();
	if(!dataNode["BirthPlaceRaw"].isNull())
		data_.birthPlaceRaw = dataNode["BirthPlaceRaw"].asString();
	if(!dataNode["Country"].isNull())
		data_.country = dataNode["Country"].asString();
	if(!dataNode["ExpiryDate"].isNull())
		data_.expiryDate = dataNode["ExpiryDate"].asString();
	if(!dataNode["ExpiryDay"].isNull())
		data_.expiryDay = dataNode["ExpiryDay"].asString();
	if(!dataNode["IssueDate"].isNull())
		data_.issueDate = dataNode["IssueDate"].asString();
	if(!dataNode["IssuePlace"].isNull())
		data_.issuePlace = dataNode["IssuePlace"].asString();
	if(!dataNode["IssuePlaceRaw"].isNull())
		data_.issuePlaceRaw = dataNode["IssuePlaceRaw"].asString();
	if(!dataNode["LineZero"].isNull())
		data_.lineZero = dataNode["LineZero"].asString();
	if(!dataNode["LineOne"].isNull())
		data_.lineOne = dataNode["LineOne"].asString();
	if(!dataNode["Name"].isNull())
		data_.name = dataNode["Name"].asString();
	if(!dataNode["NameChinese"].isNull())
		data_.nameChinese = dataNode["NameChinese"].asString();
	if(!dataNode["NameChineseRaw"].isNull())
		data_.nameChineseRaw = dataNode["NameChineseRaw"].asString();
	if(!dataNode["PassportNo"].isNull())
		data_.passportNo = dataNode["PassportNo"].asString();
	if(!dataNode["PersonId"].isNull())
		data_.personId = dataNode["PersonId"].asString();
	if(!dataNode["Sex"].isNull())
		data_.sex = dataNode["Sex"].asString();
	if(!dataNode["SourceCountry"].isNull())
		data_.sourceCountry = dataNode["SourceCountry"].asString();
	if(!dataNode["Success"].isNull())
		data_.success = dataNode["Success"].asString() == "true";
	if(!dataNode["Type"].isNull())
		data_.type = dataNode["Type"].asString();

}

RecognizeChinapassportResult::Data RecognizeChinapassportResult::getData()const
{
	return data_;
}

