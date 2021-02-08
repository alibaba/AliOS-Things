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

#include <alibabacloud/live/model/DescribeCasterScenesRequest.h>

using AlibabaCloud::Live::Model::DescribeCasterScenesRequest;

DescribeCasterScenesRequest::DescribeCasterScenesRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeCasterScenes")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeCasterScenesRequest::~DescribeCasterScenesRequest()
{}

std::string DescribeCasterScenesRequest::getCasterId()const
{
	return casterId_;
}

void DescribeCasterScenesRequest::setCasterId(const std::string& casterId)
{
	casterId_ = casterId;
	setParameter("CasterId", casterId);
}

long DescribeCasterScenesRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeCasterScenesRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeCasterScenesRequest::getSceneId()const
{
	return sceneId_;
}

void DescribeCasterScenesRequest::setSceneId(const std::string& sceneId)
{
	sceneId_ = sceneId;
	setParameter("SceneId", sceneId);
}

