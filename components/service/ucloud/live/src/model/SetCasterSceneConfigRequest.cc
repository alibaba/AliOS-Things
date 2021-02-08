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

#include <alibabacloud/live/model/SetCasterSceneConfigRequest.h>

using AlibabaCloud::Live::Model::SetCasterSceneConfigRequest;

SetCasterSceneConfigRequest::SetCasterSceneConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "SetCasterSceneConfig")
{
	setMethod(HttpRequest::Method::Post);
}

SetCasterSceneConfigRequest::~SetCasterSceneConfigRequest()
{}

std::string SetCasterSceneConfigRequest::getLayoutId()const
{
	return layoutId_;
}

void SetCasterSceneConfigRequest::setLayoutId(const std::string& layoutId)
{
	layoutId_ = layoutId;
	setParameter("LayoutId", layoutId);
}

std::vector<std::string> SetCasterSceneConfigRequest::getComponentId()const
{
	return componentId_;
}

void SetCasterSceneConfigRequest::setComponentId(const std::vector<std::string>& componentId)
{
	componentId_ = componentId;
	for(int dep1 = 0; dep1!= componentId.size(); dep1++) {
		setParameter("ComponentId."+ std::to_string(dep1), componentId.at(dep1));
	}
}

std::string SetCasterSceneConfigRequest::getCasterId()const
{
	return casterId_;
}

void SetCasterSceneConfigRequest::setCasterId(const std::string& casterId)
{
	casterId_ = casterId;
	setParameter("CasterId", casterId);
}

long SetCasterSceneConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void SetCasterSceneConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string SetCasterSceneConfigRequest::getSceneId()const
{
	return sceneId_;
}

void SetCasterSceneConfigRequest::setSceneId(const std::string& sceneId)
{
	sceneId_ = sceneId;
	setParameter("SceneId", sceneId);
}

