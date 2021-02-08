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

#include <alibabacloud/live/model/CopyCasterSceneConfigRequest.h>

using AlibabaCloud::Live::Model::CopyCasterSceneConfigRequest;

CopyCasterSceneConfigRequest::CopyCasterSceneConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "CopyCasterSceneConfig")
{
	setMethod(HttpRequest::Method::Post);
}

CopyCasterSceneConfigRequest::~CopyCasterSceneConfigRequest()
{}

std::string CopyCasterSceneConfigRequest::getFromSceneId()const
{
	return fromSceneId_;
}

void CopyCasterSceneConfigRequest::setFromSceneId(const std::string& fromSceneId)
{
	fromSceneId_ = fromSceneId;
	setParameter("FromSceneId", fromSceneId);
}

std::string CopyCasterSceneConfigRequest::getToSceneId()const
{
	return toSceneId_;
}

void CopyCasterSceneConfigRequest::setToSceneId(const std::string& toSceneId)
{
	toSceneId_ = toSceneId;
	setParameter("ToSceneId", toSceneId);
}

std::string CopyCasterSceneConfigRequest::getCasterId()const
{
	return casterId_;
}

void CopyCasterSceneConfigRequest::setCasterId(const std::string& casterId)
{
	casterId_ = casterId;
	setParameter("CasterId", casterId);
}

long CopyCasterSceneConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void CopyCasterSceneConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

