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

#include <alibabacloud/videosearch/model/AddStorageVideoTaskRequest.h>

using AlibabaCloud::Videosearch::Model::AddStorageVideoTaskRequest;

AddStorageVideoTaskRequest::AddStorageVideoTaskRequest() :
	RpcServiceRequest("videosearch", "2020-02-25", "AddStorageVideoTask")
{
	setMethod(HttpRequest::Method::Post);
}

AddStorageVideoTaskRequest::~AddStorageVideoTaskRequest()
{}

std::string AddStorageVideoTaskRequest::getClientToken()const
{
	return clientToken_;
}

void AddStorageVideoTaskRequest::setClientToken(const std::string& clientToken)
{
	clientToken_ = clientToken;
	setParameter("ClientToken", clientToken);
}

std::string AddStorageVideoTaskRequest::getRegionId()const
{
	return regionId_;
}

void AddStorageVideoTaskRequest::setRegionId(const std::string& regionId)
{
	regionId_ = regionId;
	setParameter("RegionId", regionId);
}

std::string AddStorageVideoTaskRequest::getVideoTags()const
{
	return videoTags_;
}

void AddStorageVideoTaskRequest::setVideoTags(const std::string& videoTags)
{
	videoTags_ = videoTags;
	setBodyParameter("VideoTags", videoTags);
}

std::string AddStorageVideoTaskRequest::getVideoId()const
{
	return videoId_;
}

void AddStorageVideoTaskRequest::setVideoId(const std::string& videoId)
{
	videoId_ = videoId;
	setBodyParameter("VideoId", videoId);
}

std::string AddStorageVideoTaskRequest::getInstanceId()const
{
	return instanceId_;
}

void AddStorageVideoTaskRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setBodyParameter("InstanceId", instanceId);
}

std::string AddStorageVideoTaskRequest::getVideoUrl()const
{
	return videoUrl_;
}

void AddStorageVideoTaskRequest::setVideoUrl(const std::string& videoUrl)
{
	videoUrl_ = videoUrl;
	setBodyParameter("VideoUrl", videoUrl);
}

std::string AddStorageVideoTaskRequest::getCallbackUrl()const
{
	return callbackUrl_;
}

void AddStorageVideoTaskRequest::setCallbackUrl(const std::string& callbackUrl)
{
	callbackUrl_ = callbackUrl;
	setBodyParameter("CallbackUrl", callbackUrl);
}

