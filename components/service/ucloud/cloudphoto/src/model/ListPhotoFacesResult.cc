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

#include <alibabacloud/cloudphoto/model/ListPhotoFacesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudPhoto;
using namespace AlibabaCloud::CloudPhoto::Model;

ListPhotoFacesResult::ListPhotoFacesResult() :
	ServiceResult()
{}

ListPhotoFacesResult::ListPhotoFacesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListPhotoFacesResult::~ListPhotoFacesResult()
{}

void ListPhotoFacesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allFacesNode = value["Faces"]["Face"];
	for (auto valueFacesFace : allFacesNode)
	{
		Face facesObject;
		if(!valueFacesFace["FaceId"].isNull())
			facesObject.faceId = std::stol(valueFacesFace["FaceId"].asString());
		if(!valueFacesFace["FaceIdStr"].isNull())
			facesObject.faceIdStr = valueFacesFace["FaceIdStr"].asString();
		if(!valueFacesFace["FaceName"].isNull())
			facesObject.faceName = valueFacesFace["FaceName"].asString();
		auto allAxis = value["Axis"]["Axis"];
		for (auto value : allAxis)
			facesObject.axis.push_back(value.asString());
		faces_.push_back(facesObject);
	}
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Action"].isNull())
		action_ = value["Action"].asString();

}

std::string ListPhotoFacesResult::getAction()const
{
	return action_;
}

std::string ListPhotoFacesResult::getMessage()const
{
	return message_;
}

std::string ListPhotoFacesResult::getCode()const
{
	return code_;
}

std::vector<ListPhotoFacesResult::Face> ListPhotoFacesResult::getFaces()const
{
	return faces_;
}

