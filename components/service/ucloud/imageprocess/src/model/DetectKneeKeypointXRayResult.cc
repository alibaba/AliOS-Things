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

#include <alibabacloud/imageprocess/model/DetectKneeKeypointXRayResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imageprocess;
using namespace AlibabaCloud::Imageprocess::Model;

DetectKneeKeypointXRayResult::DetectKneeKeypointXRayResult() :
	ServiceResult()
{}

DetectKneeKeypointXRayResult::DetectKneeKeypointXRayResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DetectKneeKeypointXRayResult::~DetectKneeKeypointXRayResult()
{}

void DetectKneeKeypointXRayResult::parse(const std::string &payload)
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
	auto allKeyPointsNode = dataNode["KeyPoints"]["KeyPointsItem"];
	for (auto dataNodeKeyPointsKeyPointsItem : allKeyPointsNode)
	{
		Data::KeyPointsItem keyPointsItemObject;
		if(!dataNodeKeyPointsKeyPointsItem["Value"].isNull())
			keyPointsItemObject.value = std::stof(dataNodeKeyPointsKeyPointsItem["Value"].asString());
		auto tagNode = value["Tag"];
		if(!tagNode["Direction"].isNull())
			keyPointsItemObject.tag.direction = tagNode["Direction"].asString();
		if(!tagNode["Label"].isNull())
			keyPointsItemObject.tag.label = tagNode["Label"].asString();
		auto allCoordinates = value["Coordinates"]["Coordinate"];
		for (auto value : allCoordinates)
			keyPointsItemObject.coordinates.push_back(value.asString());
		data_.keyPoints.push_back(keyPointsItemObject);
	}

}

DetectKneeKeypointXRayResult::Data DetectKneeKeypointXRayResult::getData()const
{
	return data_;
}

