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

#include <alibabacloud/live/model/SetCasterSyncGroupRequest.h>

using AlibabaCloud::Live::Model::SetCasterSyncGroupRequest;

SetCasterSyncGroupRequest::SetCasterSyncGroupRequest() :
	RpcServiceRequest("live", "2016-11-01", "SetCasterSyncGroup")
{
	setMethod(HttpRequest::Method::Post);
}

SetCasterSyncGroupRequest::~SetCasterSyncGroupRequest()
{}

std::string SetCasterSyncGroupRequest::getCasterId()const
{
	return casterId_;
}

void SetCasterSyncGroupRequest::setCasterId(const std::string& casterId)
{
	casterId_ = casterId;
	setParameter("CasterId", casterId);
}

long SetCasterSyncGroupRequest::getOwnerId()const
{
	return ownerId_;
}

void SetCasterSyncGroupRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::vector<SetCasterSyncGroupRequest::SyncGroup> SetCasterSyncGroupRequest::getSyncGroup()const
{
	return syncGroup_;
}

void SetCasterSyncGroupRequest::setSyncGroup(const std::vector<SyncGroup>& syncGroup)
{
	syncGroup_ = syncGroup;
	for(int dep1 = 0; dep1!= syncGroup.size(); dep1++) {
		auto syncGroupObj = syncGroup.at(dep1);
		std::string syncGroupObjStr = "SyncGroup." + std::to_string(dep1 + 1);
		setParameter(syncGroupObjStr + ".Mode", std::to_string(syncGroupObj.mode));
		setParameter(syncGroupObjStr + ".SyncDelayThreshold", std::to_string(syncGroupObj.syncDelayThreshold));
		setParameter(syncGroupObjStr + ".HostResourceId", syncGroupObj.hostResourceId);
		for(int dep2 = 0; dep2!= syncGroupObj.resourceIds.size(); dep2++) {
			setParameter(syncGroupObjStr + ".ResourceIds."+ std::to_string(dep2), syncGroupObj.resourceIds.at(dep2));
		}
	}
}

