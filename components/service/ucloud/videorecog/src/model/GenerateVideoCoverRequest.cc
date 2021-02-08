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

#include <alibabacloud/videorecog/model/GenerateVideoCoverRequest.h>

using AlibabaCloud::Videorecog::Model::GenerateVideoCoverRequest;

GenerateVideoCoverRequest::GenerateVideoCoverRequest() :
	RpcServiceRequest("videorecog", "2020-03-20", "GenerateVideoCover")
{
	setMethod(HttpRequest::Method::Post);
}

GenerateVideoCoverRequest::~GenerateVideoCoverRequest()
{}

bool GenerateVideoCoverRequest::getIsGif()const
{
	return isGif_;
}

void GenerateVideoCoverRequest::setIsGif(bool isGif)
{
	isGif_ = isGif;
	setBodyParameter("IsGif", isGif ? "true" : "false");
}

bool GenerateVideoCoverRequest::getAsync()const
{
	return async_;
}

void GenerateVideoCoverRequest::setAsync(bool async)
{
	async_ = async;
	setBodyParameter("Async", async ? "true" : "false");
}

std::string GenerateVideoCoverRequest::getVideoUrl()const
{
	return videoUrl_;
}

void GenerateVideoCoverRequest::setVideoUrl(const std::string& videoUrl)
{
	videoUrl_ = videoUrl;
	setBodyParameter("VideoUrl", videoUrl);
}

