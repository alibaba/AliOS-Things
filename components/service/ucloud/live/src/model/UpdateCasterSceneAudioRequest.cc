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

#include <alibabacloud/live/model/UpdateCasterSceneAudioRequest.h>

using AlibabaCloud::Live::Model::UpdateCasterSceneAudioRequest;

UpdateCasterSceneAudioRequest::UpdateCasterSceneAudioRequest() :
	RpcServiceRequest("live", "2016-11-01", "UpdateCasterSceneAudio")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateCasterSceneAudioRequest::~UpdateCasterSceneAudioRequest()
{}

std::string UpdateCasterSceneAudioRequest::getCasterId()const
{
	return casterId_;
}

void UpdateCasterSceneAudioRequest::setCasterId(const std::string& casterId)
{
	casterId_ = casterId;
	setParameter("CasterId", casterId);
}

long UpdateCasterSceneAudioRequest::getOwnerId()const
{
	return ownerId_;
}

void UpdateCasterSceneAudioRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::vector<UpdateCasterSceneAudioRequest::AudioLayer> UpdateCasterSceneAudioRequest::getAudioLayer()const
{
	return audioLayer_;
}

void UpdateCasterSceneAudioRequest::setAudioLayer(const std::vector<AudioLayer>& audioLayer)
{
	audioLayer_ = audioLayer;
	for(int dep1 = 0; dep1!= audioLayer.size(); dep1++) {
		auto audioLayerObj = audioLayer.at(dep1);
		std::string audioLayerObjStr = "AudioLayer." + std::to_string(dep1 + 1);
		setParameter(audioLayerObjStr + ".VolumeRate", std::to_string(audioLayerObj.volumeRate));
		setParameter(audioLayerObjStr + ".ValidChannel", audioLayerObj.validChannel);
		setParameter(audioLayerObjStr + ".FixedDelayDuration", std::to_string(audioLayerObj.fixedDelayDuration));
	}
}

std::string UpdateCasterSceneAudioRequest::getSceneId()const
{
	return sceneId_;
}

void UpdateCasterSceneAudioRequest::setSceneId(const std::string& sceneId)
{
	sceneId_ = sceneId;
	setParameter("SceneId", sceneId);
}

std::vector<std::string> UpdateCasterSceneAudioRequest::getMixList()const
{
	return mixList_;
}

void UpdateCasterSceneAudioRequest::setMixList(const std::vector<std::string>& mixList)
{
	mixList_ = mixList;
	for(int dep1 = 0; dep1!= mixList.size(); dep1++) {
		setParameter("MixList."+ std::to_string(dep1), mixList.at(dep1));
	}
}

int UpdateCasterSceneAudioRequest::getFollowEnable()const
{
	return followEnable_;
}

void UpdateCasterSceneAudioRequest::setFollowEnable(int followEnable)
{
	followEnable_ = followEnable;
	setParameter("FollowEnable", std::to_string(followEnable));
}

