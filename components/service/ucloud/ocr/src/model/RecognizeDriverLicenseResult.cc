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

#include <alibabacloud/ocr/model/RecognizeDriverLicenseResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

RecognizeDriverLicenseResult::RecognizeDriverLicenseResult() :
	ServiceResult()
{}

RecognizeDriverLicenseResult::RecognizeDriverLicenseResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeDriverLicenseResult::~RecognizeDriverLicenseResult()
{}

void RecognizeDriverLicenseResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto faceResultNode = dataNode["FaceResult"];
	if(!faceResultNode["Name"].isNull())
		data_.faceResult.name = faceResultNode["Name"].asString();
	if(!faceResultNode["LicenseNumber"].isNull())
		data_.faceResult.licenseNumber = faceResultNode["LicenseNumber"].asString();
	if(!faceResultNode["VehicleType"].isNull())
		data_.faceResult.vehicleType = faceResultNode["VehicleType"].asString();
	if(!faceResultNode["StartDate"].isNull())
		data_.faceResult.startDate = faceResultNode["StartDate"].asString();
	if(!faceResultNode["EndDate"].isNull())
		data_.faceResult.endDate = faceResultNode["EndDate"].asString();
	if(!faceResultNode["IssueDate"].isNull())
		data_.faceResult.issueDate = faceResultNode["IssueDate"].asString();
	if(!faceResultNode["Address"].isNull())
		data_.faceResult.address = faceResultNode["Address"].asString();
	if(!faceResultNode["Gender"].isNull())
		data_.faceResult.gender = faceResultNode["Gender"].asString();
	auto backResultNode = dataNode["BackResult"];
	if(!backResultNode["ArchiveNumber"].isNull())
		data_.backResult.archiveNumber = backResultNode["ArchiveNumber"].asString();

}

RecognizeDriverLicenseResult::Data RecognizeDriverLicenseResult::getData()const
{
	return data_;
}

