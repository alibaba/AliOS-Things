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

#include <alibabacloud/imageprocess/model/ClassifyFNFResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imageprocess;
using namespace AlibabaCloud::Imageprocess::Model;

ClassifyFNFResult::ClassifyFNFResult() :
	ServiceResult()
{}

ClassifyFNFResult::ClassifyFNFResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ClassifyFNFResult::~ClassifyFNFResult()
{}

void ClassifyFNFResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["ImageUrl"].isNull())
		data_.imageUrl = dataNode["ImageUrl"].asString();
	if(!dataNode["OrgId"].isNull())
		data_.orgId = dataNode["OrgId"].asString();
	if(!dataNode["OrgName"].isNull())
		data_.orgName = dataNode["OrgName"].asString();
	auto allFracturesNode = dataNode["Fractures"]["FracturesItem"];
	for (auto dataNodeFracturesFracturesItem : allFracturesNode)
	{
		Data::FracturesItem fracturesItemObject;
		if(!dataNodeFracturesFracturesItem["Value"].isNull())
			fracturesItemObject.value = std::stof(dataNodeFracturesFracturesItem["Value"].asString());
		auto tagNode = value["Tag"];
		if(!tagNode["Label"].isNull())
			fracturesItemObject.tag.label = tagNode["Label"].asString();
		auto allBoxes = value["Boxes"]["Boxes"];
		for (auto value : allBoxes)
			fracturesItemObject.boxes.push_back(value.asString());
		data_.fractures.push_back(fracturesItemObject);
	}

}

ClassifyFNFResult::Data ClassifyFNFResult::getData()const
{
	return data_;
}

