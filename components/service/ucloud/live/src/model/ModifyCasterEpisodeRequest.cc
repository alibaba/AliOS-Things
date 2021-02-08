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

#include <alibabacloud/live/model/ModifyCasterEpisodeRequest.h>

using AlibabaCloud::Live::Model::ModifyCasterEpisodeRequest;

ModifyCasterEpisodeRequest::ModifyCasterEpisodeRequest() :
	RpcServiceRequest("live", "2016-11-01", "ModifyCasterEpisode")
{
	setMethod(HttpRequest::Method::Post);
}

ModifyCasterEpisodeRequest::~ModifyCasterEpisodeRequest()
{}

std::string ModifyCasterEpisodeRequest::getEpisodeName()const
{
	return episodeName_;
}

void ModifyCasterEpisodeRequest::setEpisodeName(const std::string& episodeName)
{
	episodeName_ = episodeName;
	setParameter("EpisodeName", episodeName);
}

std::string ModifyCasterEpisodeRequest::getStartTime()const
{
	return startTime_;
}

void ModifyCasterEpisodeRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string ModifyCasterEpisodeRequest::getResourceId()const
{
	return resourceId_;
}

void ModifyCasterEpisodeRequest::setResourceId(const std::string& resourceId)
{
	resourceId_ = resourceId;
	setParameter("ResourceId", resourceId);
}

std::vector<std::string> ModifyCasterEpisodeRequest::getComponentId()const
{
	return componentId_;
}

void ModifyCasterEpisodeRequest::setComponentId(const std::vector<std::string>& componentId)
{
	componentId_ = componentId;
	for(int dep1 = 0; dep1!= componentId.size(); dep1++) {
		setParameter("ComponentId."+ std::to_string(dep1), componentId.at(dep1));
	}
}

std::string ModifyCasterEpisodeRequest::getCasterId()const
{
	return casterId_;
}

void ModifyCasterEpisodeRequest::setCasterId(const std::string& casterId)
{
	casterId_ = casterId;
	setParameter("CasterId", casterId);
}

std::string ModifyCasterEpisodeRequest::getEndTime()const
{
	return endTime_;
}

void ModifyCasterEpisodeRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long ModifyCasterEpisodeRequest::getOwnerId()const
{
	return ownerId_;
}

void ModifyCasterEpisodeRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string ModifyCasterEpisodeRequest::getEpisodeId()const
{
	return episodeId_;
}

void ModifyCasterEpisodeRequest::setEpisodeId(const std::string& episodeId)
{
	episodeId_ = episodeId;
	setParameter("EpisodeId", episodeId);
}

std::string ModifyCasterEpisodeRequest::getSwitchType()const
{
	return switchType_;
}

void ModifyCasterEpisodeRequest::setSwitchType(const std::string& switchType)
{
	switchType_ = switchType;
	setParameter("SwitchType", switchType);
}

