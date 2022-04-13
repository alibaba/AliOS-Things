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

#include <alibabacloud/facebody/model/DetectCelebrityResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Facebody;
using namespace AlibabaCloud::Facebody::Model;

DetectCelebrityResult::DetectCelebrityResult() :
	ServiceResult()
{}

DetectCelebrityResult::DetectCelebrityResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DetectCelebrityResult::~DetectCelebrityResult()
{}

void DetectCelebrityResult::parse(const std::string &payload)
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
	auto allFaceRecognizeResultsNode = dataNode["FaceRecognizeResults"]["FaceRecognizeResult"];
	for (auto dataNodeFaceRecognizeResultsFaceRecognizeResult : allFaceRecognizeResultsNode)
	{
		Data::FaceRecognizeResult faceRecognizeResultObject;
		if(!dataNodeFaceRecognizeResultsFaceRecognizeResult["Name"].isNull())
			faceRecognizeResultObject.name = dataNodeFaceRecognizeResultsFaceRecognizeResult["Name"].asString();
		auto allFaceBoxes = value["FaceBoxes"]["FaceBox"];
		for (auto value : allFaceBoxes)
			faceRecognizeResultObject.faceBoxes.push_back(value.asString());
		data_.faceRecognizeResults.push_back(faceRecognizeResultObject);
	}

}

DetectCelebrityResult::Data DetectCelebrityResult::getData()const
{
	return data_;
}

