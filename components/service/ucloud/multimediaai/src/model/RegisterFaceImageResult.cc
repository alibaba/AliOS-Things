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

#include <alibabacloud/multimediaai/model/RegisterFaceImageResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Multimediaai;
using namespace AlibabaCloud::Multimediaai::Model;

RegisterFaceImageResult::RegisterFaceImageResult() :
	ServiceResult()
{}

RegisterFaceImageResult::RegisterFaceImageResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RegisterFaceImageResult::~RegisterFaceImageResult()
{}

void RegisterFaceImageResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allFaceImagesNode = value["FaceImages"]["FaceImage"];
	for (auto valueFaceImagesFaceImage : allFaceImagesNode)
	{
		FaceImage faceImagesObject;
		if(!valueFaceImagesFaceImage["FaceImageId"].isNull())
			faceImagesObject.faceImageId = std::stol(valueFaceImagesFaceImage["FaceImageId"].asString());
		faceImages_.push_back(faceImagesObject);
	}

}

std::vector<RegisterFaceImageResult::FaceImage> RegisterFaceImageResult::getFaceImages()const
{
	return faceImages_;
}

