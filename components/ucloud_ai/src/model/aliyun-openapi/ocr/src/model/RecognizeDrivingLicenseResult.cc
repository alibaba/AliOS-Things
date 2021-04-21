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

#include <alibabacloud/ocr/model/RecognizeDrivingLicenseResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

RecognizeDrivingLicenseResult::RecognizeDrivingLicenseResult() :
	ServiceResult()
{}

RecognizeDrivingLicenseResult::RecognizeDrivingLicenseResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RecognizeDrivingLicenseResult::~RecognizeDrivingLicenseResult()
{}

void RecognizeDrivingLicenseResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto faceResultNode = dataNode["FaceResult"];
	if(!faceResultNode["PlateNumber"].isNull())
		data_.faceResult.plateNumber = faceResultNode["PlateNumber"].asString();
	if(!faceResultNode["VehicleType"].isNull())
		data_.faceResult.vehicleType = faceResultNode["VehicleType"].asString();
	if(!faceResultNode["Owner"].isNull())
		data_.faceResult.owner = faceResultNode["Owner"].asString();
	if(!faceResultNode["UseCharacter"].isNull())
		data_.faceResult.useCharacter = faceResultNode["UseCharacter"].asString();
	if(!faceResultNode["Address"].isNull())
		data_.faceResult.address = faceResultNode["Address"].asString();
	if(!faceResultNode["Model"].isNull())
		data_.faceResult.model = faceResultNode["Model"].asString();
	if(!faceResultNode["Vin"].isNull())
		data_.faceResult.vin = faceResultNode["Vin"].asString();
	if(!faceResultNode["EngineNumber"].isNull())
		data_.faceResult.engineNumber = faceResultNode["EngineNumber"].asString();
	if(!faceResultNode["RegisterDate"].isNull())
		data_.faceResult.registerDate = faceResultNode["RegisterDate"].asString();
	if(!faceResultNode["IssueDate"].isNull())
		data_.faceResult.issueDate = faceResultNode["IssueDate"].asString();
	auto backResultNode = dataNode["BackResult"];
	if(!backResultNode["ApprovedPassengerCapacity"].isNull())
		data_.backResult.approvedPassengerCapacity = backResultNode["ApprovedPassengerCapacity"].asString();
	if(!backResultNode["ApprovedLoad"].isNull())
		data_.backResult.approvedLoad = backResultNode["ApprovedLoad"].asString();
	if(!backResultNode["FileNumber"].isNull())
		data_.backResult.fileNumber = backResultNode["FileNumber"].asString();
	if(!backResultNode["GrossMass"].isNull())
		data_.backResult.grossMass = backResultNode["GrossMass"].asString();
	if(!backResultNode["EnergyType"].isNull())
		data_.backResult.energyType = backResultNode["EnergyType"].asString();
	if(!backResultNode["InspectionRecord"].isNull())
		data_.backResult.inspectionRecord = backResultNode["InspectionRecord"].asString();
	if(!backResultNode["OverallDimension"].isNull())
		data_.backResult.overallDimension = backResultNode["OverallDimension"].asString();
	if(!backResultNode["TractionMass"].isNull())
		data_.backResult.tractionMass = backResultNode["TractionMass"].asString();
	if(!backResultNode["UnladenMass"].isNull())
		data_.backResult.unladenMass = backResultNode["UnladenMass"].asString();
	if(!backResultNode["PlateNumber"].isNull())
		data_.backResult.plateNumber = backResultNode["PlateNumber"].asString();

}

RecognizeDrivingLicenseResult::Data RecognizeDrivingLicenseResult::getData()const
{
	return data_;
}

