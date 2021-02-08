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

#include <alibabacloud/live/model/ModifyCasterLayoutRequest.h>

using AlibabaCloud::Live::Model::ModifyCasterLayoutRequest;

ModifyCasterLayoutRequest::ModifyCasterLayoutRequest() :
	RpcServiceRequest("live", "2016-11-01", "ModifyCasterLayout")
{
	setMethod(HttpRequest::Method::Post);
}

ModifyCasterLayoutRequest::~ModifyCasterLayoutRequest()
{}

std::vector<std::string> ModifyCasterLayoutRequest::getBlendList()const
{
	return blendList_;
}

void ModifyCasterLayoutRequest::setBlendList(const std::vector<std::string>& blendList)
{
	blendList_ = blendList;
	for(int dep1 = 0; dep1!= blendList.size(); dep1++) {
		setParameter("BlendList."+ std::to_string(dep1), blendList.at(dep1));
	}
}

std::string ModifyCasterLayoutRequest::getLayoutId()const
{
	return layoutId_;
}

void ModifyCasterLayoutRequest::setLayoutId(const std::string& layoutId)
{
	layoutId_ = layoutId;
	setParameter("LayoutId", layoutId);
}

std::string ModifyCasterLayoutRequest::getCasterId()const
{
	return casterId_;
}

void ModifyCasterLayoutRequest::setCasterId(const std::string& casterId)
{
	casterId_ = casterId;
	setParameter("CasterId", casterId);
}

long ModifyCasterLayoutRequest::getOwnerId()const
{
	return ownerId_;
}

void ModifyCasterLayoutRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::vector<ModifyCasterLayoutRequest::AudioLayer> ModifyCasterLayoutRequest::getAudioLayer()const
{
	return audioLayer_;
}

void ModifyCasterLayoutRequest::setAudioLayer(const std::vector<AudioLayer>& audioLayer)
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

std::vector<ModifyCasterLayoutRequest::VideoLayer> ModifyCasterLayoutRequest::getVideoLayer()const
{
	return videoLayer_;
}

void ModifyCasterLayoutRequest::setVideoLayer(const std::vector<VideoLayer>& videoLayer)
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

std::vector<std::string> ModifyCasterLayoutRequest::getMixList()const
{
	return mixList_;
}

void ModifyCasterLayoutRequest::setMixList(const std::vector<std::string>& mixList)
{
	mixList_ = mixList;
	for(int dep1 = 0; dep1!= mixList.size(); dep1++) {
		setParameter("MixList."+ std::to_string(dep1), mixList.at(dep1));
	}
}

