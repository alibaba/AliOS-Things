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

#include <alibabacloud/imageprocess/model/TranslateMedRequest.h>

using AlibabaCloud::Imageprocess::Model::TranslateMedRequest;

TranslateMedRequest::TranslateMedRequest() :
	RpcServiceRequest("imageprocess", "2020-03-20", "TranslateMed")
{
	setMethod(HttpRequest::Method::Post);
}

TranslateMedRequest::~TranslateMedRequest()
{}

std::string TranslateMedRequest::getFromLanguage()const
{
	return fromLanguage_;
}

void TranslateMedRequest::setFromLanguage(const std::string& fromLanguage)
{
	fromLanguage_ = fromLanguage;
	setBodyParameter("FromLanguage", fromLanguage);
}

std::string TranslateMedRequest::getToLanguage()const
{
	return toLanguage_;
}

void TranslateMedRequest::setToLanguage(const std::string& toLanguage)
{
	toLanguage_ = toLanguage;
	setBodyParameter("ToLanguage", toLanguage);
}

std::string TranslateMedRequest::getText()const
{
	return text_;
}

void TranslateMedRequest::setText(const std::string& text)
{
	text_ = text;
	setBodyParameter("Text", text);
}

