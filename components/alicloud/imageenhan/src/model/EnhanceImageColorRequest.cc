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

#include <alibabacloud/imageenhan/model/EnhanceImageColorRequest.h>

using AlibabaCloud::Imageenhan::Model::EnhanceImageColorRequest;

EnhanceImageColorRequest::EnhanceImageColorRequest() :
	RpcServiceRequest("imageenhan", "2019-09-30", "EnhanceImageColor")
{
	setMethod(HttpRequest::Method::Post);
}

EnhanceImageColorRequest::~EnhanceImageColorRequest()
{}

std::string EnhanceImageColorRequest::getMode()const
{
	return mode_;
}

void EnhanceImageColorRequest::setMode(const std::string& mode)
{
	mode_ = mode;
	setBodyParameter("Mode", mode);
}

std::string EnhanceImageColorRequest::getImageURL()const
{
	return imageURL_;
}

void EnhanceImageColorRequest::setImageURL(const std::string& imageURL)
{
	imageURL_ = imageURL;
	setBodyParameter("ImageURL", imageURL);
}

std::string EnhanceImageColorRequest::getOutputFormat()const
{
	return outputFormat_;
}

void EnhanceImageColorRequest::setOutputFormat(const std::string& outputFormat)
{
	outputFormat_ = outputFormat;
	setBodyParameter("OutputFormat", outputFormat);
}

