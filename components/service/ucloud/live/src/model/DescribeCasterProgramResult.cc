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

#include <alibabacloud/live/model/DescribeCasterProgramResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeCasterProgramResult::DescribeCasterProgramResult() :
	ServiceResult()
{}

DescribeCasterProgramResult::DescribeCasterProgramResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeCasterProgramResult::~DescribeCasterProgramResult()
{}

void DescribeCasterProgramResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allEpisodesNode = value["Episodes"]["Episode"];
	for (auto valueEpisodesEpisode : allEpisodesNode)
	{
		Episode episodesObject;
		if(!valueEpisodesEpisode["EpisodeId"].isNull())
			episodesObject.episodeId = valueEpisodesEpisode["EpisodeId"].asString();
		if(!valueEpisodesEpisode["EpisodeType"].isNull())
			episodesObject.episodeType = valueEpisodesEpisode["EpisodeType"].asString();
		if(!valueEpisodesEpisode["EpisodeName"].isNull())
			episodesObject.episodeName = valueEpisodesEpisode["EpisodeName"].asString();
		if(!valueEpisodesEpisode["ResourceId"].isNull())
			episodesObject.resourceId = valueEpisodesEpisode["ResourceId"].asString();
		if(!valueEpisodesEpisode["StartTime"].isNull())
			episodesObject.startTime = valueEpisodesEpisode["StartTime"].asString();
		if(!valueEpisodesEpisode["EndTime"].isNull())
			episodesObject.endTime = valueEpisodesEpisode["EndTime"].asString();
		if(!valueEpisodesEpisode["SwitchType"].isNull())
			episodesObject.switchType = valueEpisodesEpisode["SwitchType"].asString();
		if(!valueEpisodesEpisode["Status"].isNull())
			episodesObject.status = std::stoi(valueEpisodesEpisode["Status"].asString());
		auto allComponentIds = value["ComponentIds"]["ComponentId"];
		for (auto value : allComponentIds)
			episodesObject.componentIds.push_back(value.asString());
		episodes_.push_back(episodesObject);
	}
	if(!value["CasterId"].isNull())
		casterId_ = value["CasterId"].asString();
	if(!value["ProgramName"].isNull())
		programName_ = value["ProgramName"].asString();
	if(!value["ProgramEffect"].isNull())
		programEffect_ = std::stoi(value["ProgramEffect"].asString());
	if(!value["Total"].isNull())
		total_ = std::stoi(value["Total"].asString());

}

int DescribeCasterProgramResult::getProgramEffect()const
{
	return programEffect_;
}

int DescribeCasterProgramResult::getTotal()const
{
	return total_;
}

std::vector<DescribeCasterProgramResult::Episode> DescribeCasterProgramResult::getEpisodes()const
{
	return episodes_;
}

std::string DescribeCasterProgramResult::getCasterId()const
{
	return casterId_;
}

std::string DescribeCasterProgramResult::getProgramName()const
{
	return programName_;
}

