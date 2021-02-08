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

#include <alibabacloud/live/model/EffectCasterUrgentRequest.h>

using AlibabaCloud::Live::Model::EffectCasterUrgentRequest;

EffectCasterUrgentRequest::EffectCasterUrgentRequest() :
	RpcServiceRequest("live", "2016-11-01", "EffectCasterUrgent")
{
	setMethod(HttpRequest::Method::Post);
}

EffectCasterUrgentRequest::~EffectCasterUrgentRequest()
{}

std::string EffectCasterUrgentRequest::getCasterId()const
{
	return casterId_;
}

void EffectCasterUrgentRequest::setCasterId(const std::string& casterId)
{
	casterId_ = casterId;
	setParameter("CasterId", casterId);
}

long EffectCasterUrgentRequest::getOwnerId()const
{
	return ownerId_;
}

void EffectCasterUrgentRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string EffectCasterUrgentRequest::getSceneId()const
{
	return sceneId_;
}

void EffectCasterUrgentRequest::setSceneId(const std::string& sceneId)
{
	sceneId_ = sceneId;
	setParameter("SceneId", sceneId);
}

