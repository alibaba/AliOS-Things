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

#include <alibabacloud/videoenhan/model/AbstractFilmVideoRequest.h>

using AlibabaCloud::Videoenhan::Model::AbstractFilmVideoRequest;

AbstractFilmVideoRequest::AbstractFilmVideoRequest() :
	RpcServiceRequest("videoenhan", "2020-03-20", "AbstractFilmVideo")
{
	setMethod(HttpRequest::Method::Post);
}

AbstractFilmVideoRequest::~AbstractFilmVideoRequest()
{}

int AbstractFilmVideoRequest::getLength()const
{
	return length_;
}

void AbstractFilmVideoRequest::setLength(int length)
{
	length_ = length;
	setBodyParameter("Length", std::to_string(length));
}

bool AbstractFilmVideoRequest::getAsync()const
{
	return async_;
}

void AbstractFilmVideoRequest::setAsync(bool async)
{
	async_ = async;
	setBodyParameter("Async", async ? "true" : "false");
}

std::string AbstractFilmVideoRequest::getVideoUrl()const
{
	return videoUrl_;
}

void AbstractFilmVideoRequest::setVideoUrl(const std::string& videoUrl)
{
	videoUrl_ = videoUrl;
	setBodyParameter("VideoUrl", videoUrl);
}

