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

#include <alibabacloud/live/model/ModifyCasterProgramRequest.h>

using AlibabaCloud::Live::Model::ModifyCasterProgramRequest;

ModifyCasterProgramRequest::ModifyCasterProgramRequest() :
	RpcServiceRequest("live", "2016-11-01", "ModifyCasterProgram")
{
	setMethod(HttpRequest::Method::Post);
}

ModifyCasterProgramRequest::~ModifyCasterProgramRequest()
{}

std::vector<ModifyCasterProgramRequest::Episode> ModifyCasterProgramRequest::getEpisode()const
{
	return episode_;
}

void ModifyCasterProgramRequest::setEpisode(const std::vector<Episode>& episode)
{
	episode_ = episode;
	for(int dep1 = 0; dep1!= episode.size(); dep1++) {
		auto episodeObj = episode.at(dep1);
		std::string episodeObjStr = "Episode." + std::to_string(dep1 + 1);
		setParameter(episodeObjStr + ".EpisodeId", episodeObj.episodeId);
		setParameter(episodeObjStr + ".EpisodeType", episodeObj.episodeType);
		setParameter(episodeObjStr + ".EpisodeName", episodeObj.episodeName);
		setParameter(episodeObjStr + ".ResourceId", episodeObj.resourceId);
		for(int dep2 = 0; dep2!= episodeObj.componentId.size(); dep2++) {
			setParameter(episodeObjStr + ".ComponentId."+ std::to_string(dep2), episodeObj.componentId.at(dep2));
		}
		setParameter(episodeObjStr + ".StartTime", episodeObj.startTime);
		setParameter(episodeObjStr + ".EndTime", episodeObj.endTime);
		setParameter(episodeObjStr + ".SwitchType", episodeObj.switchType);
	}
}

std::string ModifyCasterProgramRequest::getCasterId()const
{
	return casterId_;
}

void ModifyCasterProgramRequest::setCasterId(const std::string& casterId)
{
	casterId_ = casterId;
	setParameter("CasterId", casterId);
}

long ModifyCasterProgramRequest::getOwnerId()const
{
	return ownerId_;
}

void ModifyCasterProgramRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

