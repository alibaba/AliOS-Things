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

#include <alibabacloud/imgsearch/model/SearchImageResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imgsearch;
using namespace AlibabaCloud::Imgsearch::Model;

SearchImageResult::SearchImageResult() :
	ServiceResult()
{}

SearchImageResult::SearchImageResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

SearchImageResult::~SearchImageResult()
{}

void SearchImageResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allMatchListNode = dataNode["MatchList"]["MatchListItem"];
	for (auto dataNodeMatchListMatchListItem : allMatchListNode)
	{
		Data::MatchListItem matchListItemObject;
		if(!dataNodeMatchListMatchListItem["DataId"].isNull())
			matchListItemObject.dataId = dataNodeMatchListMatchListItem["DataId"].asString();
		if(!dataNodeMatchListMatchListItem["ExtraData"].isNull())
			matchListItemObject.extraData = dataNodeMatchListMatchListItem["ExtraData"].asString();
		if(!dataNodeMatchListMatchListItem["EntityId"].isNull())
			matchListItemObject.entityId = dataNodeMatchListMatchListItem["EntityId"].asString();
		if(!dataNodeMatchListMatchListItem["ImageUrl"].isNull())
			matchListItemObject.imageUrl = dataNodeMatchListMatchListItem["ImageUrl"].asString();
		if(!dataNodeMatchListMatchListItem["Score"].isNull())
			matchListItemObject.score = std::stof(dataNodeMatchListMatchListItem["Score"].asString());
		data_.matchList.push_back(matchListItemObject);
	}

}

SearchImageResult::Data SearchImageResult::getData()const
{
	return data_;
}

