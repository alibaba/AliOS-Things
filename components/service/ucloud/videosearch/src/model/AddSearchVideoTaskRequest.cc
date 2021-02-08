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

#include <alibabacloud/videosearch/model/AddSearchVideoTaskRequest.h>

using AlibabaCloud::Videosearch::Model::AddSearchVideoTaskRequest;

AddSearchVideoTaskRequest::AddSearchVideoTaskRequest() :
	RpcServiceRequest("videosearch", "2020-02-25", "AddSearchVideoTask")
{
	setMethod(HttpRequest::Method::Post);
}

AddSearchVideoTaskRequest::~AddSearchVideoTaskRequest()
{}

std::string AddSearchVideoTaskRequest::getClientToken()const
{
	return clientToken_;
}

void AddSearchVideoTaskRequest::setClientToken(const std::string& clientToken)
{
	clientToken_ = clientToken;
	setParameter("ClientToken", clientToken);
}

int AddSearchVideoTaskRequest::getStorageType()const
{
	return storageType_;
}

void AddSearchVideoTaskRequest::setStorageType(int storageType)
{
	storageType_ = storageType;
	setBodyParameter("StorageType", std::to_string(storageType));
}

std::string AddSearchVideoTaskRequest::getRegionId()const
{
	return regionId_;
}

void AddSearchVideoTaskRequest::setRegionId(const std::string& regionId)
{
	regionId_ = regionId;
	setParameter("RegionId", regionId);
}

std::string AddSearchVideoTaskRequest::getVideoTags()const
{
	return videoTags_;
}

void AddSearchVideoTaskRequest::setVideoTags(const std::string& videoTags)
{
	videoTags_ = videoTags;
	setBodyParameter("VideoTags", videoTags);
}

int AddSearchVideoTaskRequest::getReturnVideoNumber()const
{
	return returnVideoNumber_;
}

void AddSearchVideoTaskRequest::setReturnVideoNumber(int returnVideoNumber)
{
	returnVideoNumber_ = returnVideoNumber;
	setBodyParameter("ReturnVideoNumber", std::to_string(returnVideoNumber));
}

std::string AddSearchVideoTaskRequest::getVideoId()const
{
	return videoId_;
}

void AddSearchVideoTaskRequest::setVideoId(const std::string& videoId)
{
	videoId_ = videoId;
	setBodyParameter("VideoId", videoId);
}

float AddSearchVideoTaskRequest::getReplaceStorageThreshold()const
{
	return replaceStorageThreshold_;
}

void AddSearchVideoTaskRequest::setReplaceStorageThreshold(float replaceStorageThreshold)
{
	replaceStorageThreshold_ = replaceStorageThreshold;
	setBodyParameter("ReplaceStorageThreshold", std::to_string(replaceStorageThreshold));
}

std::string AddSearchVideoTaskRequest::getInstanceId()const
{
	return instanceId_;
}

void AddSearchVideoTaskRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setBodyParameter("InstanceId", instanceId);
}

std::string AddSearchVideoTaskRequest::getVideoUrl()const
{
	return videoUrl_;
}

void AddSearchVideoTaskRequest::setVideoUrl(const std::string& videoUrl)
{
	videoUrl_ = videoUrl;
	setBodyParameter("VideoUrl", videoUrl);
}

std::string AddSearchVideoTaskRequest::getQueryTags()const
{
	return queryTags_;
}

void AddSearchVideoTaskRequest::setQueryTags(const std::string& queryTags)
{
	queryTags_ = queryTags;
	setBodyParameter("QueryTags", queryTags);
}

std::string AddSearchVideoTaskRequest::getCallbackUrl()const
{
	return callbackUrl_;
}

void AddSearchVideoTaskRequest::setCallbackUrl(const std::string& callbackUrl)
{
	callbackUrl_ = callbackUrl;
	setBodyParameter("CallbackUrl", callbackUrl);
}

