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

#include <alibabacloud/live/model/AddCasterLayoutRequest.h>

using AlibabaCloud::Live::Model::AddCasterLayoutRequest;

AddCasterLayoutRequest::AddCasterLayoutRequest() :
	RpcServiceRequest("live", "2016-11-01", "AddCasterLayout")
{
	setMethod(HttpRequest::Method::Post);
}

AddCasterLayoutRequest::~AddCasterLayoutRequest()
{}

std::vector<std::string> AddCasterLayoutRequest::getBlendList()const
{
	return blendList_;
}

void AddCasterLayoutRequest::setBlendList(const std::vector<std::string>& blendList)
{
	blendList_ = blendList;
	for(int dep1 = 0; dep1!= blendList.size(); dep1++) {
		setParameter("BlendList."+ std::to_string(dep1), blendList.at(dep1));
	}
}

std::string AddCasterLayoutRequest::getCasterId()const
{
	return casterId_;
}

void AddCasterLayoutRequest::setCasterId(const std::string& casterId)
{
	casterId_ = casterId;
	setParameter("CasterId", casterId);
}

long AddCasterLayoutRequest::getOwnerId()const
{
	return ownerId_;
}

void AddCasterLayoutRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::vector<AddCasterLayoutRequest::AudioLayer> AddCasterLayoutRequest::getAudioLayer()const
{
	return audioLayer_;
}

void AddCasterLayoutRequest::setAudioLayer(const std::vector<AudioLayer>& audioLayer)
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

std::vector<AddCasterLayoutRequest::VideoLayer> AddCasterLayoutRequest::getVideoLayer()const
{
	return videoLayer_;
}

void AddCasterLayoutRequest::setVideoLayer(const std::vector<VideoLayer>& videoLayer)
{
	videoLayer_ = videoLayer;
	for(int dep1 = 0; dep1!= videoLayer.size(); dep1++) {
		auto videoLayerObj = videoLayer.at(dep1);
		std::string videoLayerObjStr = "VideoLayer." + std::to_string(dep1 + 1);
		setParameter(videoLayerObjStr + ".FillMode", videoLayerObj.fillMode);
		setParameter(videoLayerObjStr + ".HeightNormalized", std::to_string(videoLayerObj.heightNormalized));
		setParameter(videoLayerObjStr + ".WidthNormalized", std::to_string(videoLayerObj.widthNormalized));
		setParameter(videoLayerObjStr + ".PositionRefer", videoLayerObj.positionRefer);
		for(int dep2 = 0; dep2!= videoLayerObj.positionNormalized.size(); dep2++) {
			setParameter(videoLayerObjStr + ".PositionNormalized."+ std::to_string(dep2), std::to_string(videoLayerObj.positionNormalized.at(dep2)));
		}
		setParameter(videoLayerObjStr + ".FixedDelayDuration", std::to_string(videoLayerObj.fixedDelayDuration));
	}
}

std::vector<std::string> AddCasterLayoutRequest::getMixList()const
{
	return mixList_;
}

void AddCasterLayoutRequest::setMixList(const std::vector<std::string>& mixList)
{
	mixList_ = mixList;
	for(int dep1 = 0; dep1!= mixList.size(); dep1++) {
		setParameter("MixList."+ std::to_string(dep1), mixList.at(dep1));
	}
}

