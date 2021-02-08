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

#include <alibabacloud/videoenhan/model/SuperResolveVideoRequest.h>

using AlibabaCloud::Videoenhan::Model::SuperResolveVideoRequest;

SuperResolveVideoRequest::SuperResolveVideoRequest() :
	RpcServiceRequest("videoenhan", "2020-03-20", "SuperResolveVideo")
{
	setMethod(HttpRequest::Method::Post);
}

SuperResolveVideoRequest::~SuperResolveVideoRequest()
{}

int SuperResolveVideoRequest::getBitRate()const
{
	return bitRate_;
}

void SuperResolveVideoRequest::setBitRate(int bitRate)
{
	bitRate_ = bitRate;
	setBodyParameter("BitRate", std::to_string(bitRate));
}

bool SuperResolveVideoRequest::getAsync()const
{
	return async_;
}

void SuperResolveVideoRequest::setAsync(bool async)
{
	async_ = async;
	setBodyParameter("Async", async ? "true" : "false");
}

std::string SuperResolveVideoRequest::getVideoUrl()const
{
	return videoUrl_;
}

void SuperResolveVideoRequest::setVideoUrl(const std::string& videoUrl)
{
	videoUrl_ = videoUrl;
	setBodyParameter("VideoUrl", videoUrl);
}

