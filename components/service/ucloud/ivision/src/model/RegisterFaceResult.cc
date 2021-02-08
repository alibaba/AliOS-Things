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

#include <alibabacloud/ivision/model/RegisterFaceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ivision;
using namespace AlibabaCloud::Ivision::Model;

RegisterFaceResult::RegisterFaceResult() :
	ServiceResult()
{}

RegisterFaceResult::RegisterFaceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RegisterFaceResult::~RegisterFaceResult()
{}

void RegisterFaceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allFacesNode = value["Faces"]["Face"];
	for (auto valueFacesFace : allFacesNode)
	{
		Face facesObject;
		if(!valueFacesFace["FaceToken"].isNull())
			facesObject.faceToken = valueFacesFace["FaceToken"].asString();
		auto rectNode = value["Rect"];
		if(!rectNode["Left"].isNull())
			facesObject.rect.left = std::stoi(rectNode["Left"].asString());
		if(!rectNode["Top"].isNull())
			facesObject.rect.top = std::stoi(rectNode["Top"].asString());
		if(!rectNode["Width"].isNull())
			facesObject.rect.width = std::stoi(rectNode["Width"].asString());
		if(!rectNode["Height"].isNull())
			facesObject.rect.height = std::stoi(rectNode["Height"].asString());
		faces_.push_back(facesObject);
	}
	if(!value["GroupId"].isNull())
		groupId_ = value["GroupId"].asString();

}

std::vector<RegisterFaceResult::Face> RegisterFaceResult::getFaces()const
{
	return faces_;
}

std::string RegisterFaceResult::getGroupId()const
{
	return groupId_;
}

