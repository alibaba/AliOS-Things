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

#include <alibabacloud/alimt/model/TranslateRequest.h>

using AlibabaCloud::Alimt::Model::TranslateRequest;

TranslateRequest::TranslateRequest() :
	RpcServiceRequest("alimt", "2018-10-12", "Translate")
{
	setMethod(HttpRequest::Method::Post);
}

TranslateRequest::~TranslateRequest()
{}

std::string TranslateRequest::getSourceLanguage()const
{
	return sourceLanguage_;
}

void TranslateRequest::setSourceLanguage(const std::string& sourceLanguage)
{
	sourceLanguage_ = sourceLanguage;
	setBodyParameter("SourceLanguage", sourceLanguage);
}

std::string TranslateRequest::getSourceText()const
{
	return sourceText_;
}

void TranslateRequest::setSourceText(const std::string& sourceText)
{
	sourceText_ = sourceText;
	setBodyParameter("SourceText", sourceText);
}

std::string TranslateRequest::getFormatType()const
{
	return formatType_;
}

void TranslateRequest::setFormatType(const std::string& formatType)
{
	formatType_ = formatType;
	setBodyParameter("FormatType", formatType);
}

std::string TranslateRequest::getScene()const
{
	return scene_;
}

void TranslateRequest::setScene(const std::string& scene)
{
	scene_ = scene;
	setBodyParameter("Scene", scene);
}

std::string TranslateRequest::getTargetLanguage()const
{
	return targetLanguage_;
}

void TranslateRequest::setTargetLanguage(const std::string& targetLanguage)
{
	targetLanguage_ = targetLanguage;
	setBodyParameter("TargetLanguage", targetLanguage);
}

