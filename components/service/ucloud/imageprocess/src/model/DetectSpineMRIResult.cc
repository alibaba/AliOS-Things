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

#include <alibabacloud/imageprocess/model/DetectSpineMRIResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imageprocess;
using namespace AlibabaCloud::Imageprocess::Model;

DetectSpineMRIResult::DetectSpineMRIResult() :
	ServiceResult()
{}

DetectSpineMRIResult::DetectSpineMRIResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DetectSpineMRIResult::~DetectSpineMRIResult()
{}

void DetectSpineMRIResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allDiscsNode = dataNode["Discs"]["Disc"];
	for (auto dataNodeDiscsDisc : allDiscsNode)
	{
		Data::Disc discObject;
		if(!dataNodeDiscsDisc["Disease"].isNull())
			discObject.disease = dataNodeDiscsDisc["Disease"].asString();
		if(!dataNodeDiscsDisc["Identification"].isNull())
			discObject.identification = dataNodeDiscsDisc["Identification"].asString();
		auto allLocation = value["Location"]["Location"];
		for (auto value : allLocation)
			discObject.location.push_back(value.asString());
		data_.discs.push_back(discObject);
	}
	auto allVertebrasNode = dataNode["Vertebras"]["Vertebra"];
	for (auto dataNodeVertebrasVertebra : allVertebrasNode)
	{
		Data::Vertebra vertebraObject;
		if(!dataNodeVertebrasVertebra["Disease"].isNull())
			vertebraObject.disease = dataNodeVertebrasVertebra["Disease"].asString();
		if(!dataNodeVertebrasVertebra["Identification"].isNull())
			vertebraObject.identification = dataNodeVertebrasVertebra["Identification"].asString();
		auto allLocation1 = value["Location"]["Location"];
		for (auto value : allLocation1)
			vertebraObject.location1.push_back(value.asString());
		data_.vertebras.push_back(vertebraObject);
	}

}

DetectSpineMRIResult::Data DetectSpineMRIResult::getData()const
{
	return data_;
}

