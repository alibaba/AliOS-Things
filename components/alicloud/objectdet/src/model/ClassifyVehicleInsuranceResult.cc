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

#include <alibabacloud/objectdet/model/ClassifyVehicleInsuranceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Objectdet;
using namespace AlibabaCloud::Objectdet::Model;

ClassifyVehicleInsuranceResult::ClassifyVehicleInsuranceResult() :
	ServiceResult()
{}

ClassifyVehicleInsuranceResult::ClassifyVehicleInsuranceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ClassifyVehicleInsuranceResult::~ClassifyVehicleInsuranceResult()
{}

void ClassifyVehicleInsuranceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Threshold"].isNull())
		data_.threshold = std::stof(dataNode["Threshold"].asString());
	auto allLabelsNode = dataNode["Labels"]["Label"];
	for (auto dataNodeLabelsLabel : allLabelsNode)
	{
		Data::Label labelObject;
		if(!dataNodeLabelsLabel["Score"].isNull())
			labelObject.score = std::stof(dataNodeLabelsLabel["Score"].asString());
		if(!dataNodeLabelsLabel["Name"].isNull())
			labelObject.name = dataNodeLabelsLabel["Name"].asString();
		data_.labels.push_back(labelObject);
	}

}

ClassifyVehicleInsuranceResult::Data ClassifyVehicleInsuranceResult::getData()const
{
	return data_;
}

