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

#include <alibabacloud/alimt/model/CreateDocTranslateTaskRequest.h>

using AlibabaCloud::Alimt::Model::CreateDocTranslateTaskRequest;

CreateDocTranslateTaskRequest::CreateDocTranslateTaskRequest() :
	RpcServiceRequest("alimt", "2018-10-12", "CreateDocTranslateTask")
{
	setMethod(HttpRequest::Method::Post);
}

CreateDocTranslateTaskRequest::~CreateDocTranslateTaskRequest()
{}

std::string CreateDocTranslateTaskRequest::getSourceLanguage()const
{
	return sourceLanguage_;
}

void CreateDocTranslateTaskRequest::setSourceLanguage(const std::string& sourceLanguage)
{
	sourceLanguage_ = sourceLanguage;
	setBodyParameter("SourceLanguage", sourceLanguage);
}

std::string CreateDocTranslateTaskRequest::getClientToken()const
{
	return clientToken_;
}

void CreateDocTranslateTaskRequest::setClientToken(const std::string& clientToken)
{
	clientToken_ = clientToken;
	setBodyParameter("ClientToken", clientToken);
}

std::string CreateDocTranslateTaskRequest::getScene()const
{
	return scene_;
}

void CreateDocTranslateTaskRequest::setScene(const std::string& scene)
{
	scene_ = scene;
	setBodyParameter("Scene", scene);
}

std::string CreateDocTranslateTaskRequest::getFileUrl()const
{
	return fileUrl_;
}

void CreateDocTranslateTaskRequest::setFileUrl(const std::string& fileUrl)
{
	fileUrl_ = fileUrl;
	setBodyParameter("FileUrl", fileUrl);
}

std::string CreateDocTranslateTaskRequest::getTargetLanguage()const
{
	return targetLanguage_;
}

void CreateDocTranslateTaskRequest::setTargetLanguage(const std::string& targetLanguage)
{
	targetLanguage_ = targetLanguage;
	setBodyParameter("TargetLanguage", targetLanguage);
}

std::string CreateDocTranslateTaskRequest::getCallbackUrl()const
{
	return callbackUrl_;
}

void CreateDocTranslateTaskRequest::setCallbackUrl(const std::string& callbackUrl)
{
	callbackUrl_ = callbackUrl;
	setBodyParameter("CallbackUrl", callbackUrl);
}

