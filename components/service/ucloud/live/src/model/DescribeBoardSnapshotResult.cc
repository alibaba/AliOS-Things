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

#include <alibabacloud/live/model/DescribeBoardSnapshotResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeBoardSnapshotResult::DescribeBoardSnapshotResult() :
	ServiceResult()
{}

DescribeBoardSnapshotResult::DescribeBoardSnapshotResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeBoardSnapshotResult::~DescribeBoardSnapshotResult()
{}

void DescribeBoardSnapshotResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto snapshotNode = value["Snapshot"];
	auto boardNode = snapshotNode["Board"];
	if(!boardNode["BoardId"].isNull())
		snapshot_.board.boardId = boardNode["BoardId"].asString();
	if(!boardNode["AppUid"].isNull())
		snapshot_.board.appUid = boardNode["AppUid"].asString();
	if(!boardNode["EventTimestamp"].isNull())
		snapshot_.board.eventTimestamp = std::stol(boardNode["EventTimestamp"].asString());
	if(!boardNode["CreateTimestamp"].isNull())
		snapshot_.board.createTimestamp = std::stol(boardNode["CreateTimestamp"].asString());
	if(!boardNode["UpdateTimestamp"].isNull())
		snapshot_.board.updateTimestamp = std::stol(boardNode["UpdateTimestamp"].asString());
	auto allPagesNode = boardNode["Pages"]["Page"];
	for (auto boardNodePagesPage : allPagesNode)
	{
		Snapshot::Board::Page pageObject;
		if(!boardNodePagesPage["PageIndex"].isNull())
			pageObject.pageIndex = std::stoi(boardNodePagesPage["PageIndex"].asString());
		auto allElementsNode = allPagesNode["Elements"]["Element"];
		for (auto allPagesNodeElementsElement : allElementsNode)
		{
			Snapshot::Board::Page::Element elementsObject;
			if(!allPagesNodeElementsElement["ElementIndex"].isNull())
				elementsObject.elementIndex = allPagesNodeElementsElement["ElementIndex"].asString();
			if(!allPagesNodeElementsElement["OwnerId"].isNull())
				elementsObject.ownerId = allPagesNodeElementsElement["OwnerId"].asString();
			if(!allPagesNodeElementsElement["ElementType"].isNull())
				elementsObject.elementType = std::stoi(allPagesNodeElementsElement["ElementType"].asString());
			if(!allPagesNodeElementsElement["UpdateTimestamp"].isNull())
				elementsObject.updateTimestamp = std::stol(allPagesNodeElementsElement["UpdateTimestamp"].asString());
			if(!allPagesNodeElementsElement["Data"].isNull())
				elementsObject.data = allPagesNodeElementsElement["Data"].asString();
			pageObject.elements.push_back(elementsObject);
		}
		snapshot_.board.pages.push_back(pageObject);
	}
	auto allConfigsNode = boardNode["Configs"]["Config"];
	for (auto boardNodeConfigsConfig : allConfigsNode)
	{
		Snapshot::Board::Config configObject;
		if(!boardNodeConfigsConfig["AppUid"].isNull())
			configObject.appUid = boardNodeConfigsConfig["AppUid"].asString();
		if(!boardNodeConfigsConfig["Data"].isNull())
			configObject.data = boardNodeConfigsConfig["Data"].asString();
		snapshot_.board.configs.push_back(configObject);
	}

}

DescribeBoardSnapshotResult::Snapshot DescribeBoardSnapshotResult::getSnapshot()const
{
	return snapshot_;
}

