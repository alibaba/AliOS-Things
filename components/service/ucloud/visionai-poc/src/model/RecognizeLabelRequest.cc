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

#include <alibabacloud/visionai-poc/model/RecognizeLabelRequest.h>

using AlibabaCloud::Visionai_poc::Model::RecognizeLabelRequest;

RecognizeLabelRequest::RecognizeLabelRequest() :
	RpcServiceRequest("visionai-poc", "2020-04-08", "RecognizeLabel")
{
	setMethod(HttpRequest::Method::Post);
}

RecognizeLabelRequest::~RecognizeLabelRequest()
{}

std::string RecognizeLabelRequest::getMethod()const
{
	return method_;
}

void RecognizeLabelRequest::setMethod(const std::string& method)
{
	method_ = method;
	setBodyParameter("Method", method);
}

std::string RecognizeLabelRequest::getImageUrl()const
{
	return imageUrl_;
}

void RecognizeLabelRequest::setImageUrl(const std::string& imageUrl)
{
	imageUrl_ = imageUrl;
	setBodyParameter("ImageUrl", imageUrl);
}

std::string RecognizeLabelRequest::getUrl()const
{
	return url_;
}

void RecognizeLabelRequest::setUrl(const std::string& url)
{
	url_ = url;
	setBodyParameter("Url", url);
}

