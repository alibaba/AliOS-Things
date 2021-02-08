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

#include <alibabacloud/videoenhan/model/ChangeVideoSizeRequest.h>

using AlibabaCloud::Videoenhan::Model::ChangeVideoSizeRequest;

ChangeVideoSizeRequest::ChangeVideoSizeRequest() :
	RpcServiceRequest("videoenhan", "2020-03-20", "ChangeVideoSize")
{
	setMethod(HttpRequest::Method::Post);
}

ChangeVideoSizeRequest::~ChangeVideoSizeRequest()
{}

int ChangeVideoSizeRequest::getHeight()const
{
	return height_;
}

void ChangeVideoSizeRequest::setHeight(int height)
{
	height_ = height;
	setBodyParameter("Height", std::to_string(height));
}

int ChangeVideoSizeRequest::getB()const
{
	return b_;
}

void ChangeVideoSizeRequest::setB(int b)
{
	b_ = b;
	setBodyParameter("B", std::to_string(b));
}

std::string ChangeVideoSizeRequest::getFillType()const
{
	return fillType_;
}

void ChangeVideoSizeRequest::setFillType(const std::string& fillType)
{
	fillType_ = fillType;
	setBodyParameter("FillType", fillType);
}

int ChangeVideoSizeRequest::getG()const
{
	return g_;
}

void ChangeVideoSizeRequest::setG(int g)
{
	g_ = g;
	setBodyParameter("G", std::to_string(g));
}

std::string ChangeVideoSizeRequest::getCropType()const
{
	return cropType_;
}

void ChangeVideoSizeRequest::setCropType(const std::string& cropType)
{
	cropType_ = cropType;
	setBodyParameter("CropType", cropType);
}

bool ChangeVideoSizeRequest::getAsync()const
{
	return async_;
}

void ChangeVideoSizeRequest::setAsync(bool async)
{
	async_ = async;
	setBodyParameter("Async", async ? "true" : "false");
}

int ChangeVideoSizeRequest::getR()const
{
	return r_;
}

void ChangeVideoSizeRequest::setR(int r)
{
	r_ = r;
	setBodyParameter("R", std::to_string(r));
}

std::string ChangeVideoSizeRequest::getVideoUrl()const
{
	return videoUrl_;
}

void ChangeVideoSizeRequest::setVideoUrl(const std::string& videoUrl)
{
	videoUrl_ = videoUrl;
	setBodyParameter("VideoUrl", videoUrl);
}

int ChangeVideoSizeRequest::getWidth()const
{
	return width_;
}

void ChangeVideoSizeRequest::setWidth(int width)
{
	width_ = width;
	setBodyParameter("Width", std::to_string(width));
}

float ChangeVideoSizeRequest::getTightness()const
{
	return tightness_;
}

void ChangeVideoSizeRequest::setTightness(float tightness)
{
	tightness_ = tightness;
	setBodyParameter("Tightness", std::to_string(tightness));
}

