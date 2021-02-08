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

#include <alibabacloud/videoseg/model/SegmentVideoBodyRequest.h>

using AlibabaCloud::Videoseg::Model::SegmentVideoBodyRequest;

SegmentVideoBodyRequest::SegmentVideoBodyRequest() :
	RpcServiceRequest("videoseg", "2020-03-20", "SegmentVideoBody")
{
	setMethod(HttpRequest::Method::Post);
}

SegmentVideoBodyRequest::~SegmentVideoBodyRequest()
{}

bool SegmentVideoBodyRequest::getAsync()const
{
	return async_;
}

void SegmentVideoBodyRequest::setAsync(bool async)
{
	async_ = async;
	setBodyParameter("Async", async ? "true" : "false");
}

std::string SegmentVideoBodyRequest::getVideoUrl()const
{
	return videoUrl_;
}

void SegmentVideoBodyRequest::setVideoUrl(const std::string& videoUrl)
{
	videoUrl_ = videoUrl;
	setBodyParameter("VideoUrl", videoUrl);
}

