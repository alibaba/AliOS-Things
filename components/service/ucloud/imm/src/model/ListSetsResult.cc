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

#include <alibabacloud/imm/model/ListSetsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

ListSetsResult::ListSetsResult() :
	ServiceResult()
{}

ListSetsResult::ListSetsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListSetsResult::~ListSetsResult()
{}

void ListSetsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allSetsNode = value["Sets"]["SetsItem"];
	for (auto valueSetsSetsItem : allSetsNode)
	{
		SetsItem setsObject;
		if(!valueSetsSetsItem["SetId"].isNull())
			setsObject.setId = valueSetsSetsItem["SetId"].asString();
		if(!valueSetsSetsItem["SetName"].isNull())
			setsObject.setName = valueSetsSetsItem["SetName"].asString();
		if(!valueSetsSetsItem["CreateTime"].isNull())
			setsObject.createTime = valueSetsSetsItem["CreateTime"].asString();
		if(!valueSetsSetsItem["ModifyTime"].isNull())
			setsObject.modifyTime = valueSetsSetsItem["ModifyTime"].asString();
		if(!valueSetsSetsItem["FaceCount"].isNull())
			setsObject.faceCount = std::stoi(valueSetsSetsItem["FaceCount"].asString());
		if(!valueSetsSetsItem["ImageCount"].isNull())
			setsObject.imageCount = std::stoi(valueSetsSetsItem["ImageCount"].asString());
		if(!valueSetsSetsItem["VideoCount"].isNull())
			setsObject.videoCount = std::stoi(valueSetsSetsItem["VideoCount"].asString());
		if(!valueSetsSetsItem["VideoLength"].isNull())
			setsObject.videoLength = std::stoi(valueSetsSetsItem["VideoLength"].asString());
		sets_.push_back(setsObject);
	}
	if(!value["NextMarker"].isNull())
		nextMarker_ = value["NextMarker"].asString();

}

std::vector<ListSetsResult::SetsItem> ListSetsResult::getSets()const
{
	return sets_;
}

std::string ListSetsResult::getNextMarker()const
{
	return nextMarker_;
}

