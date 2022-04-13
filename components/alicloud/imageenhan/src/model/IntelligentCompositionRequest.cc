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

#include <alibabacloud/imageenhan/model/IntelligentCompositionRequest.h>

using AlibabaCloud::Imageenhan::Model::IntelligentCompositionRequest;

IntelligentCompositionRequest::IntelligentCompositionRequest() :
	RpcServiceRequest("imageenhan", "2019-09-30", "IntelligentComposition")
{
	setMethod(HttpRequest::Method::Post);
}

IntelligentCompositionRequest::~IntelligentCompositionRequest()
{}

int IntelligentCompositionRequest::getNumBoxes()const
{
	return numBoxes_;
}

void IntelligentCompositionRequest::setNumBoxes(int numBoxes)
{
	numBoxes_ = numBoxes;
	setBodyParameter("NumBoxes", std::to_string(numBoxes));
}

std::string IntelligentCompositionRequest::getImageURL()const
{
	return imageURL_;
}

void IntelligentCompositionRequest::setImageURL(const std::string& imageURL)
{
	imageURL_ = imageURL;
	setBodyParameter("ImageURL", imageURL);
}

