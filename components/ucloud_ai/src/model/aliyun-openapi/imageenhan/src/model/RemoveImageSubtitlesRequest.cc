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

#include <alibabacloud/imageenhan/model/RemoveImageSubtitlesRequest.h>

using AlibabaCloud::Imageenhan::Model::RemoveImageSubtitlesRequest;

RemoveImageSubtitlesRequest::RemoveImageSubtitlesRequest() :
	RpcServiceRequest("imageenhan", "2019-09-30", "RemoveImageSubtitles")
{
	setMethod(HttpRequest::Method::Post);
}

RemoveImageSubtitlesRequest::~RemoveImageSubtitlesRequest()
{}

float RemoveImageSubtitlesRequest::getBH()const
{
	return bH_;
}

void RemoveImageSubtitlesRequest::setBH(float bH)
{
	bH_ = bH;
	setBodyParameter("BH", std::to_string(bH));
}

float RemoveImageSubtitlesRequest::getBW()const
{
	return bW_;
}

void RemoveImageSubtitlesRequest::setBW(float bW)
{
	bW_ = bW;
	setBodyParameter("BW", std::to_string(bW));
}

float RemoveImageSubtitlesRequest::getBX()const
{
	return bX_;
}

void RemoveImageSubtitlesRequest::setBX(float bX)
{
	bX_ = bX;
	setBodyParameter("BX", std::to_string(bX));
}

std::string RemoveImageSubtitlesRequest::getImageURL()const
{
	return imageURL_;
}

void RemoveImageSubtitlesRequest::setImageURL(const std::string& imageURL)
{
	imageURL_ = imageURL;
	setBodyParameter("ImageURL", imageURL);
}

float RemoveImageSubtitlesRequest::getBY()const
{
	return bY_;
}

void RemoveImageSubtitlesRequest::setBY(float bY)
{
	bY_ = bY;
	setBodyParameter("BY", std::to_string(bY));
}

