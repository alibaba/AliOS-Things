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

#include <alibabacloud/videosearch/model/AddDeletionVideoTaskRequest.h>

using AlibabaCloud::Videosearch::Model::AddDeletionVideoTaskRequest;

AddDeletionVideoTaskRequest::AddDeletionVideoTaskRequest() :
	RpcServiceRequest("videosearch", "2020-02-25", "AddDeletionVideoTask")
{
	setMethod(HttpRequest::Method::Post);
}

AddDeletionVideoTaskRequest::~AddDeletionVideoTaskRequest()
{}

std::string AddDeletionVideoTaskRequest::getClientToken()const
{
	return clientToken_;
}

void AddDeletionVideoTaskRequest::setClientToken(const std::string& clientToken)
{
	clientToken_ = clientToken;
	setParameter("ClientToken", clientToken);
}

std::string AddDeletionVideoTaskRequest::getVideoId()const
{
	return videoId_;
}

void AddDeletionVideoTaskRequest::setVideoId(const std::string& videoId)
{
	videoId_ = videoId;
	setBodyParameter("VideoId", videoId);
}

std::string AddDeletionVideoTaskRequest::getInstanceId()const
{
	return instanceId_;
}

void AddDeletionVideoTaskRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setBodyParameter("InstanceId", instanceId);
}

std::string AddDeletionVideoTaskRequest::getRegionId()const
{
	return regionId_;
}

void AddDeletionVideoTaskRequest::setRegionId(const std::string& regionId)
{
	regionId_ = regionId;
	setParameter("RegionId", regionId);
}

