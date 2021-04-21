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

#include <alibabacloud/imagerecog/model/RecognizeImageColorRequest.h>

using AlibabaCloud::Imagerecog::Model::RecognizeImageColorRequest;

RecognizeImageColorRequest::RecognizeImageColorRequest() :
	RpcServiceRequest("imagerecog", "2019-09-30", "RecognizeImageColor")
{
	setMethod(HttpRequest::Method::Post);
}

RecognizeImageColorRequest::~RecognizeImageColorRequest()
{}

std::string RecognizeImageColorRequest::getUrl()const
{
	return url_;
}

void RecognizeImageColorRequest::setUrl(const std::string& url)
{
	url_ = url;
	setBodyParameter("Url", url);
}

int RecognizeImageColorRequest::getColorCount()const
{
	return colorCount_;
}

void RecognizeImageColorRequest::setColorCount(int colorCount)
{
	colorCount_ = colorCount;
	setBodyParameter("ColorCount", std::to_string(colorCount));
}

