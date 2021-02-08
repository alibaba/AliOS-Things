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

#include <alibabacloud/ivision/model/SearchFaceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ivision;
using namespace AlibabaCloud::Ivision::Model;

SearchFaceResult::SearchFaceResult() :
	ServiceResult()
{}

SearchFaceResult::SearchFaceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

SearchFaceResult::~SearchFaceResult()
{}

void SearchFaceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allFaceResultsNode = value["FaceResults"]["FaceResult"];
	for (auto valueFaceResultsFaceResult : allFaceResultsNode)
	{
		FaceResult faceResultsObject;
		if(!valueFaceResultsFaceResult["FaceToken"].isNull())
			faceResultsObject.faceToken = valueFaceResultsFaceResult["FaceToken"].asString();
		if(!valueFaceResultsFaceResult["Probability"].isNull())
			faceResultsObject.probability = std::stof(valueFaceResultsFaceResult["Probability"].asString());
		faceResults_.push_back(faceResultsObject);
	}
	auto rectNode = value["Rect"];
	if(!rectNode["Left"].isNull())
		rect_.left = std::stoi(rectNode["Left"].asString());
	if(!rectNode["Top"].isNull())
		rect_.top = std::stoi(rectNode["Top"].asString());
	if(!rectNode["Width"].isNull())
		rect_.width = std::stoi(rectNode["Width"].asString());
	if(!rectNode["Height"].isNull())
		rect_.height = std::stoi(rectNode["Height"].asString());
	if(!value["GroupId"].isNull())
		groupId_ = value["GroupId"].asString();

}

SearchFaceResult::Rect SearchFaceResult::getRect()const
{
	return rect_;
}

std::vector<SearchFaceResult::FaceResult> SearchFaceResult::getFaceResults()const
{
	return faceResults_;
}

std::string SearchFaceResult::getGroupId()const
{
	return groupId_;
}

