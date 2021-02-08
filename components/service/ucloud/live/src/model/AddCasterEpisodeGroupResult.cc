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

#include <alibabacloud/live/model/AddCasterEpisodeGroupResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

AddCasterEpisodeGroupResult::AddCasterEpisodeGroupResult() :
	ServiceResult()
{}

AddCasterEpisodeGroupResult::AddCasterEpisodeGroupResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

AddCasterEpisodeGroupResult::~AddCasterEpisodeGroupResult()
{}

void AddCasterEpisodeGroupResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allItemIds = value["ItemIds"]["ItemId"];
	for (const auto &item : allItemIds)
		itemIds_.push_back(item.asString());
	if(!value["ProgramId"].isNull())
		programId_ = value["ProgramId"].asString();

}

std::string AddCasterEpisodeGroupResult::getProgramId()const
{
	return programId_;
}

std::vector<std::string> AddCasterEpisodeGroupResult::getItemIds()const
{
	return itemIds_;
}

