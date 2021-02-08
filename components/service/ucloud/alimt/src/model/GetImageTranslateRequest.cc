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

#include <alibabacloud/alimt/model/GetImageTranslateRequest.h>

using AlibabaCloud::Alimt::Model::GetImageTranslateRequest;

GetImageTranslateRequest::GetImageTranslateRequest() :
	RpcServiceRequest("alimt", "2018-10-12", "GetImageTranslate")
{
	setMethod(HttpRequest::Method::Post);
}

GetImageTranslateRequest::~GetImageTranslateRequest()
{}

std::string GetImageTranslateRequest::getSourceLanguage()const
{
	return sourceLanguage_;
}

void GetImageTranslateRequest::setSourceLanguage(const std::string& sourceLanguage)
{
	sourceLanguage_ = sourceLanguage;
	setBodyParameter("SourceLanguage", sourceLanguage);
}

std::string GetImageTranslateRequest::getUrl()const
{
	return url_;
}

void GetImageTranslateRequest::setUrl(const std::string& url)
{
	url_ = url;
	setBodyParameter("Url", url);
}

std::string GetImageTranslateRequest::getExtra()const
{
	return extra_;
}

void GetImageTranslateRequest::setExtra(const std::string& extra)
{
	extra_ = extra;
	setBodyParameter("Extra", extra);
}

std::string GetImageTranslateRequest::getTargetLanguage()const
{
	return targetLanguage_;
}

void GetImageTranslateRequest::setTargetLanguage(const std::string& targetLanguage)
{
	targetLanguage_ = targetLanguage;
	setBodyParameter("TargetLanguage", targetLanguage);
}

