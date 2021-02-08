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

#include <alibabacloud/imageprocess/model/DetectKneeXRayResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imageprocess;
using namespace AlibabaCloud::Imageprocess::Model;

DetectKneeXRayResult::DetectKneeXRayResult() :
	ServiceResult()
{}

DetectKneeXRayResult::DetectKneeXRayResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DetectKneeXRayResult::~DetectKneeXRayResult()
{}

void DetectKneeXRayResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allKLDetectionsNode = dataNode["KLDetections"]["KLDetectionsItem"];
	for (auto dataNodeKLDetectionsKLDetectionsItem : allKLDetectionsNode)
	{
		Data::KLDetectionsItem kLDetectionsItemObject;
		auto allDetections = value["Detections"]["Detections"];
		for (auto value : allDetections)
			kLDetectionsItemObject.detections.push_back(value.asString());
		data_.kLDetections.push_back(kLDetectionsItemObject);
	}

}

DetectKneeXRayResult::Data DetectKneeXRayResult::getData()const
{
	return data_;
}

