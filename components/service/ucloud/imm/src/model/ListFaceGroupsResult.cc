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

#include <alibabacloud/imm/model/ListFaceGroupsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

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
	auto allFaceGroupsNode = value["FaceGroups"]["FaceGroupsItem"];
	for (auto valueFaceGroupsFaceGroupsItem : allFaceGroupsNode)
	{
		FaceGroupsItem faceGroupsObject;
		if(!valueFaceGroupsFaceGroupsItem["GroupId"].isNull())
			faceGroupsObject.groupId = valueFaceGroupsFaceGroupsItem["GroupId"].asString();
		if(!valueFaceGroupsFaceGroupsItem["GroupName"].isNull())
			faceGroupsObject.groupName = valueFaceGroupsFaceGroupsItem["GroupName"].asString();
		if(!valueFaceGroupsFaceGroupsItem["FaceCount"].isNull())
			faceGroupsObject.faceCount = std::stoi(valueFaceGroupsFaceGroupsItem["FaceCount"].asString());
		if(!valueFaceGroupsFaceGroupsItem["CreateTime"].isNull())
			faceGroupsObject.createTime = valueFaceGroupsFaceGroupsItem["CreateTime"].asString();
		if(!valueFaceGroupsFaceGroupsItem["ModifyTime"].isNull())
			faceGroupsObject.modifyTime = valueFaceGroupsFaceGroupsItem["ModifyTime"].asString();
		if(!valueFaceGroupsFaceGroupsItem["RemarksA"].isNull())
			faceGroupsObject.remarksA = valueFaceGroupsFaceGroupsItem["RemarksA"].asString();
		if(!valueFaceGroupsFaceGroupsItem["RemarksB"].isNull())
			faceGroupsObject.remarksB = valueFaceGroupsFaceGroupsItem["RemarksB"].asString();
		if(!valueFaceGroupsFaceGroupsItem["RemarksC"].isNull())
			faceGroupsObject.remarksC = valueFaceGroupsFaceGroupsItem["RemarksC"].asString();
		if(!valueFaceGroupsFaceGroupsItem["RemarksD"].isNull())
			faceGroupsObject.remarksD = valueFaceGroupsFaceGroupsItem["RemarksD"].asString();
		if(!valueFaceGroupsFaceGroupsItem["RemarksArrayA"].isNull())
			faceGroupsObject.remarksArrayA = valueFaceGroupsFaceGroupsItem["RemarksArrayA"].asString();
		if(!valueFaceGroupsFaceGroupsItem["RemarksArrayB"].isNull())
			faceGroupsObject.remarksArrayB = valueFaceGroupsFaceGroupsItem["RemarksArrayB"].asString();
		if(!valueFaceGroupsFaceGroupsItem["ExternalId"].isNull())
			faceGroupsObject.externalId = valueFaceGroupsFaceGroupsItem["ExternalId"].asString();
		if(!valueFaceGroupsFaceGroupsItem["ImageCount"].isNull())
			faceGroupsObject.imageCount = std::stoi(valueFaceGroupsFaceGroupsItem["ImageCount"].asString());
		if(!valueFaceGroupsFaceGroupsItem["Gender"].isNull())
			faceGroupsObject.gender = valueFaceGroupsFaceGroupsItem["Gender"].asString();
		if(!valueFaceGroupsFaceGroupsItem["MaxAge"].isNull())
			faceGroupsObject.maxAge = std::stof(valueFaceGroupsFaceGroupsItem["MaxAge"].asString());
		if(!valueFaceGroupsFaceGroupsItem["AverageAge"].isNull())
			faceGroupsObject.averageAge = std::stof(valueFaceGroupsFaceGroupsItem["AverageAge"].asString());
		if(!valueFaceGroupsFaceGroupsItem["MinAge"].isNull())
			faceGroupsObject.minAge = std::stof(valueFaceGroupsFaceGroupsItem["MinAge"].asString());
		auto groupCoverFaceNode = value["GroupCoverFace"];
		if(!groupCoverFaceNode["FaceId"].isNull())
			faceGroupsObject.groupCoverFace.faceId = groupCoverFaceNode["FaceId"].asString();
		if(!groupCoverFaceNode["ImageUri"].isNull())
			faceGroupsObject.groupCoverFace.imageUri = groupCoverFaceNode["ImageUri"].asString();
		auto faceBoundaryNode = groupCoverFaceNode["FaceBoundary"];
		if(!faceBoundaryNode["Left"].isNull())
			faceGroupsObject.groupCoverFace.faceBoundary.left = std::stoi(faceBoundaryNode["Left"].asString());
		if(!faceBoundaryNode["Top"].isNull())
			faceGroupsObject.groupCoverFace.faceBoundary.top = std::stoi(faceBoundaryNode["Top"].asString());
		if(!faceBoundaryNode["Width"].isNull())
			faceGroupsObject.groupCoverFace.faceBoundary.width = std::stoi(faceBoundaryNode["Width"].asString());
		if(!faceBoundaryNode["Height"].isNull())
			faceGroupsObject.groupCoverFace.faceBoundary.height = std::stoi(faceBoundaryNode["Height"].asString());
		faceGroups_.push_back(faceGroupsObject);
	}
	if(!value["NextMarker"].isNull())
		nextMarker_ = value["NextMarker"].asString();

}

std::string ListFaceGroupsResult::getNextMarker()const
{
	return nextMarker_;
}

std::vector<ListFaceGroupsResult::FaceGroupsItem> ListFaceGroupsResult::getFaceGroups()const
{
	return faceGroups_;
}

