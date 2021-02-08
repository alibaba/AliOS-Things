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

#include <alibabacloud/multimediaai/model/ListFacePersonsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Multimediaai;
using namespace AlibabaCloud::Multimediaai::Model;

ListFacePersonsResult::ListFacePersonsResult() :
	ServiceResult()
{}

ListFacePersonsResult::ListFacePersonsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListFacePersonsResult::~ListFacePersonsResult()
{}

void ListFacePersonsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allFacePersonsNode = value["FacePersons"]["FacePerson"];
	for (auto valueFacePersonsFacePerson : allFacePersonsNode)
	{
		FacePerson facePersonsObject;
		if(!valueFacePersonsFacePerson["FacePersonId"].isNull())
			facePersonsObject.facePersonId = std::stol(valueFacePersonsFacePerson["FacePersonId"].asString());
		if(!valueFacePersonsFacePerson["FacePersonName"].isNull())
			facePersonsObject.facePersonName = valueFacePersonsFacePerson["FacePersonName"].asString();
		if(!valueFacePersonsFacePerson["ImageUrl"].isNull())
			facePersonsObject.imageUrl = valueFacePersonsFacePerson["ImageUrl"].asString();
		if(!valueFacePersonsFacePerson["ImageCount"].isNull())
			facePersonsObject.imageCount = std::stol(valueFacePersonsFacePerson["ImageCount"].asString());
		facePersons_.push_back(facePersonsObject);
	}
	if(!value["PageNumber"].isNull())
		pageNumber_ = std::stoi(value["PageNumber"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stol(value["TotalCount"].asString());

}

long ListFacePersonsResult::getTotalCount()const
{
	return totalCount_;
}

std::vector<ListFacePersonsResult::FacePerson> ListFacePersonsResult::getFacePersons()const
{
	return facePersons_;
}

int ListFacePersonsResult::getPageSize()const
{
	return pageSize_;
}

int ListFacePersonsResult::getPageNumber()const
{
	return pageNumber_;
}

