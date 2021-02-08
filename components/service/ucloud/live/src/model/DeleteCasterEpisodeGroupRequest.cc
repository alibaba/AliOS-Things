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

#include <alibabacloud/live/model/DeleteCasterEpisodeGroupRequest.h>

using AlibabaCloud::Live::Model::DeleteCasterEpisodeGroupRequest;

DeleteCasterEpisodeGroupRequest::DeleteCasterEpisodeGroupRequest() :
	RpcServiceRequest("live", "2016-11-01", "DeleteCasterEpisodeGroup")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteCasterEpisodeGroupRequest::~DeleteCasterEpisodeGroupRequest()
{}

std::string DeleteCasterEpisodeGroupRequest::getProgramId()const
{
	return programId_;
}

void DeleteCasterEpisodeGroupRequest::setProgramId(const std::string& programId)
{
	programId_ = programId;
	setParameter("ProgramId", programId);
}

long DeleteCasterEpisodeGroupRequest::getOwnerId()const
{
	return ownerId_;
}

void DeleteCasterEpisodeGroupRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

