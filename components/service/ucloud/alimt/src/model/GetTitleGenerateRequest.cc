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

#include <alibabacloud/alimt/model/GetTitleGenerateRequest.h>

using AlibabaCloud::Alimt::Model::GetTitleGenerateRequest;

GetTitleGenerateRequest::GetTitleGenerateRequest() :
	RpcServiceRequest("alimt", "2018-10-12", "GetTitleGenerate")
{
	setMethod(HttpRequest::Method::Post);
}

GetTitleGenerateRequest::~GetTitleGenerateRequest()
{}

std::string GetTitleGenerateRequest::getLanguage()const
{
	return language_;
}

void GetTitleGenerateRequest::setLanguage(const std::string& language)
{
	language_ = language;
	setBodyParameter("Language", language);
}

std::string GetTitleGenerateRequest::getTitle()const
{
	return title_;
}

void GetTitleGenerateRequest::setTitle(const std::string& title)
{
	title_ = title;
	setBodyParameter("Title", title);
}

std::string GetTitleGenerateRequest::getPlatform()const
{
	return platform_;
}

void GetTitleGenerateRequest::setPlatform(const std::string& platform)
{
	platform_ = platform;
	setBodyParameter("Platform", platform);
}

std::string GetTitleGenerateRequest::getExtra()const
{
	return extra_;
}

void GetTitleGenerateRequest::setExtra(const std::string& extra)
{
	extra_ = extra;
	setBodyParameter("Extra", extra);
}

std::string GetTitleGenerateRequest::getAttributes()const
{
	return attributes_;
}

void GetTitleGenerateRequest::setAttributes(const std::string& attributes)
{
	attributes_ = attributes;
	setBodyParameter("Attributes", attributes);
}

std::string GetTitleGenerateRequest::getHotWords()const
{
	return hotWords_;
}

void GetTitleGenerateRequest::setHotWords(const std::string& hotWords)
{
	hotWords_ = hotWords;
	setBodyParameter("HotWords", hotWords);
}

std::string GetTitleGenerateRequest::getCategoryId()const
{
	return categoryId_;
}

void GetTitleGenerateRequest::setCategoryId(const std::string& categoryId)
{
	categoryId_ = categoryId;
	setBodyParameter("CategoryId", categoryId);
}

