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

#include <alibabacloud/alimt/model/CreateImageTranslateTaskRequest.h>

using AlibabaCloud::Alimt::Model::CreateImageTranslateTaskRequest;

CreateImageTranslateTaskRequest::CreateImageTranslateTaskRequest() :
	RpcServiceRequest("alimt", "2018-10-12", "CreateImageTranslateTask")
{
	setMethod(HttpRequest::Method::Post);
}

CreateImageTranslateTaskRequest::~CreateImageTranslateTaskRequest()
{}

std::string CreateImageTranslateTaskRequest::getSourceLanguage()const
{
	return sourceLanguage_;
}

void CreateImageTranslateTaskRequest::setSourceLanguage(const std::string& sourceLanguage)
{
	sourceLanguage_ = sourceLanguage;
	setBodyParameter("SourceLanguage", sourceLanguage);
}

std::string CreateImageTranslateTaskRequest::getClientToken()const
{
	return clientToken_;
}

void CreateImageTranslateTaskRequest::setClientToken(const std::string& clientToken)
{
	clientToken_ = clientToken;
	setBodyParameter("ClientToken", clientToken);
}

std::string CreateImageTranslateTaskRequest::getUrlList()const
{
	return urlList_;
}

void CreateImageTranslateTaskRequest::setUrlList(const std::string& urlList)
{
	urlList_ = urlList;
	setBodyParameter("UrlList", urlList);
}

std::string CreateImageTranslateTaskRequest::getExtra()const
{
	return extra_;
}

void CreateImageTranslateTaskRequest::setExtra(const std::string& extra)
{
	extra_ = extra;
	setBodyParameter("Extra", extra);
}

std::string CreateImageTranslateTaskRequest::getTargetLanguage()const
{
	return targetLanguage_;
}

void CreateImageTranslateTaskRequest::setTargetLanguage(const std::string& targetLanguage)
{
	targetLanguage_ = targetLanguage;
	setBodyParameter("TargetLanguage", targetLanguage);
}

