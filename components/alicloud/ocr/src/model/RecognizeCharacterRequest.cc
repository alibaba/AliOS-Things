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

#include <alibabacloud/ocr/model/RecognizeCharacterRequest.h>

using AlibabaCloud::Ocr::Model::RecognizeCharacterRequest;

RecognizeCharacterRequest::RecognizeCharacterRequest() :
	RpcServiceRequest("ocr", "2019-12-30", "RecognizeCharacter")
{
	setMethod(HttpRequest::Method::Post);
}

RecognizeCharacterRequest::~RecognizeCharacterRequest()
{}

int RecognizeCharacterRequest::getImageType()const
{
	return imageType_;
}

void RecognizeCharacterRequest::setImageType(int imageType)
{
	imageType_ = imageType;
	setBodyParameter("ImageType", std::to_string(imageType));
}

bool RecognizeCharacterRequest::getOutputProbability()const
{
	return outputProbability_;
}

void RecognizeCharacterRequest::setOutputProbability(bool outputProbability)
{
	outputProbability_ = outputProbability;
	setBodyParameter("OutputProbability", outputProbability ? "true" : "false");
}

std::string RecognizeCharacterRequest::getImageURL()const
{
	return imageURL_;
}

void RecognizeCharacterRequest::setImageURL(const std::string& imageURL)
{
	imageURL_ = imageURL;
	setBodyParameter("ImageURL", imageURL);
}

int RecognizeCharacterRequest::getMinHeight()const
{
	return minHeight_;
}

void RecognizeCharacterRequest::setMinHeight(int minHeight)
{
	minHeight_ = minHeight;
	setBodyParameter("MinHeight", std::to_string(minHeight));
}

