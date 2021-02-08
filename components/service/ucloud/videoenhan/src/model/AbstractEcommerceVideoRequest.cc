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

#include <alibabacloud/videoenhan/model/AbstractEcommerceVideoRequest.h>

using AlibabaCloud::Videoenhan::Model::AbstractEcommerceVideoRequest;

AbstractEcommerceVideoRequest::AbstractEcommerceVideoRequest() :
	RpcServiceRequest("videoenhan", "2020-03-20", "AbstractEcommerceVideo")
{
	setMethod(HttpRequest::Method::Post);
}

AbstractEcommerceVideoRequest::~AbstractEcommerceVideoRequest()
{}

float AbstractEcommerceVideoRequest::getDuration()const
{
	return duration_;
}

void AbstractEcommerceVideoRequest::setDuration(float duration)
{
	duration_ = duration;
	setBodyParameter("Duration", std::to_string(duration));
}

bool AbstractEcommerceVideoRequest::getAsync()const
{
	return async_;
}

void AbstractEcommerceVideoRequest::setAsync(bool async)
{
	async_ = async;
	setBodyParameter("Async", async ? "true" : "false");
}

std::string AbstractEcommerceVideoRequest::getVideoUrl()const
{
	return videoUrl_;
}

void AbstractEcommerceVideoRequest::setVideoUrl(const std::string& videoUrl)
{
	videoUrl_ = videoUrl;
	setBodyParameter("VideoUrl", videoUrl);
}

int AbstractEcommerceVideoRequest::getWidth()const
{
	return width_;
}

void AbstractEcommerceVideoRequest::setWidth(int width)
{
	width_ = width;
	setBodyParameter("Width", std::to_string(width));
}

int AbstractEcommerceVideoRequest::getHeight()const
{
	return height_;
}

void AbstractEcommerceVideoRequest::setHeight(int height)
{
	height_ = height;
	setBodyParameter("Height", std::to_string(height));
}

