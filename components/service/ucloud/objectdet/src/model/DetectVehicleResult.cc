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

#include <alibabacloud/objectdet/model/DetectVehicleResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Objectdet;
using namespace AlibabaCloud::Objectdet::Model;

DetectVehicleResult::DetectVehicleResult() :
	ServiceResult()
{}

DetectVehicleResult::DetectVehicleResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DetectVehicleResult::~DetectVehicleResult()
{}

void DetectVehicleResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Width"].isNull())
		data_.width = std::stoi(dataNode["Width"].asString());
	if(!dataNode["Height"].isNull())
		data_.height = std::stoi(dataNode["Height"].asString());
	auto allDetectObjectInfoListNode = dataNode["DetectObjectInfoList"]["DetectObjectInfo"];
	for (auto dataNodeDetectObjectInfoListDetectObjectInfo : allDetectObjectInfoListNode)
	{
		Data::DetectObjectInfo detectObjectInfoObject;
		if(!dataNodeDetectObjectInfoListDetectObjectInfo["Score"].isNull())
			detectObjectInfoObject.score = std::stof(dataNodeDetectObjectInfoListDetectObjectInfo["Score"].asString());
		if(!dataNodeDetectObjectInfoListDetectObjectInfo["Type"].isNull())
			detectObjectInfoObject.type = dataNodeDetectObjectInfoListDetectObjectInfo["Type"].asString();
		if(!dataNodeDetectObjectInfoListDetectObjectInfo["Id"].isNull())
			detectObjectInfoObject.id = std::stoi(dataNodeDetectObjectInfoListDetectObjectInfo["Id"].asString());
		auto allBoxes = value["Boxes"]["Box"];
		for (auto value : allBoxes)
			detectObjectInfoObject.boxes.push_back(value.asString());
		data_.detectObjectInfoList.push_back(detectObjectInfoObject);
	}

}

DetectVehicleResult::Data DetectVehicleResult::getData()const
{
	return data_;
}

