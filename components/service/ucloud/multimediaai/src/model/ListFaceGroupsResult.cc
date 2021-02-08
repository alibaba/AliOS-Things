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

#include <alibabacloud/multimediaai/model/ListFaceGroupsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Multimediaai;
using namespace AlibabaCloud::Multimediaai::Model;

ListFaceGroupsResult::ListFaceGroupsResult() :
	ServiceResult()
{}

ListFaceGroupsResult::ListFaceGroupsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListFaceGroupsResult::~ListFaceGroupsResult()
{}

void ListFaceGroupsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allFaceGroupsNode = value["FaceGroups"]["FaceGroup"];
	for (auto valueFaceGroupsFaceGroup : allFaceGroupsNode)
	{
		FaceGroup faceGroupsObject;
		if(!valueFaceGroupsFaceGroup["FaceGroupId"].isNull())
			faceGroupsObject.faceGroupId = std::stol(valueFaceGroupsFaceGroup["FaceGroupId"].asString());
		if(!valueFaceGroupsFaceGroup["FaceGroupName"].isNull())
			faceGroupsObject.faceGroupName = valueFaceGroupsFaceGroup["FaceGroupName"].asString();
		if(!valueFaceGroupsFaceGroup["Description"].isNull())
			faceGroupsObject.description = valueFaceGroupsFaceGroup["Description"].asString();
		if(!valueFaceGroupsFaceGroup["PersonCount"].isNull())
			faceGroupsObject.personCount = std::stol(valueFaceGroupsFaceGroup["PersonCount"].asString());
		if(!valueFaceGroupsFaceGroup["ImageCount"].isNull())
			faceGroupsObject.imageCount = std::stol(valueFaceGroupsFaceGroup["ImageCount"].asString());
		auto allTemplatesNode = valueFaceGroupsFaceGroup["Templates"]["TemplatesItem"];
		for (auto valueFaceGroupsFaceGroupTemplatesTemplatesItem : allTemplatesNode)
		{
			FaceGroup::TemplatesItem templatesObject;
			if(!valueFaceGroupsFaceGroupTemplatesTemplatesItem["Id"].isNull())
				templatesObject.id = valueFaceGroupsFaceGroupTemplatesTemplatesItem["Id"].asString();
			if(!valueFaceGroupsFaceGroupTemplatesTemplatesItem["Name"].isNull())
				templatesObject.name = valueFaceGroupsFaceGroupTemplatesTemplatesItem["Name"].asString();
			faceGroupsObject.templates.push_back(templatesObject);
		}
		faceGroups_.push_back(faceGroupsObject);
	}
	if(!value["PageNumber"].isNull())
		pageNumber_ = std::stoi(value["PageNumber"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stol(value["TotalCount"].asString());

}

long ListFaceGroupsResult::getTotalCount()const
{
	return totalCount_;
}

int ListFaceGroupsResult::getPageSize()const
{
	return pageSize_;
}

int ListFaceGroupsResult::getPageNumber()const
{
	return pageNumber_;
}

std::vector<ListFaceGroupsResult::FaceGroup> ListFaceGroupsResult::getFaceGroups()const
{
	return faceGroups_;
}

