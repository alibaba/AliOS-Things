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

#include <alibabacloud/alimt/model/TranslateECommerceRequest.h>

using AlibabaCloud::Alimt::Model::TranslateECommerceRequest;

TranslateECommerceRequest::TranslateECommerceRequest() :
	RpcServiceRequest("alimt", "2018-10-12", "TranslateECommerce")
{
	setMethod(HttpRequest::Method::Post);
}

TranslateECommerceRequest::~TranslateECommerceRequest()
{}

std::string TranslateECommerceRequest::getSourceLanguage()const
{
	return sourceLanguage_;
}

void TranslateECommerceRequest::setSourceLanguage(const std::string& sourceLanguage)
{
	sourceLanguage_ = sourceLanguage;
	setBodyParameter("SourceLanguage", sourceLanguage);
}

std::string TranslateECommerceRequest::getSourceText()const
{
	return sourceText_;
}

void TranslateECommerceRequest::setSourceText(const std::string& sourceText)
{
	sourceText_ = sourceText;
	setBodyParameter("SourceText", sourceText);
}

std::string TranslateECommerceRequest::getFormatType()const
{
	return formatType_;
}

void TranslateECommerceRequest::setFormatType(const std::string& formatType)
{
	formatType_ = formatType;
	setBodyParameter("FormatType", formatType);
}

std::string TranslateECommerceRequest::getScene()const
{
	return scene_;
}

void TranslateECommerceRequest::setScene(const std::string& scene)
{
	scene_ = scene;
	setBodyParameter("Scene", scene);
}

std::string TranslateECommerceRequest::getTargetLanguage()const
{
	return targetLanguage_;
}

void TranslateECommerceRequest::setTargetLanguage(const std::string& targetLanguage)
{
	targetLanguage_ = targetLanguage;
	setBodyParameter("TargetLanguage", targetLanguage);
}

