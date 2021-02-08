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

#include <alibabacloud/alimt/model/GetTitleDiagnoseRequest.h>

using AlibabaCloud::Alimt::Model::GetTitleDiagnoseRequest;

GetTitleDiagnoseRequest::GetTitleDiagnoseRequest() :
	RpcServiceRequest("alimt", "2018-10-12", "GetTitleDiagnose")
{
	setMethod(HttpRequest::Method::Post);
}

GetTitleDiagnoseRequest::~GetTitleDiagnoseRequest()
{}

std::string GetTitleDiagnoseRequest::getLanguage()const
{
	return language_;
}

void GetTitleDiagnoseRequest::setLanguage(const std::string& language)
{
	language_ = language;
	setBodyParameter("Language", language);
}

std::string GetTitleDiagnoseRequest::getTitle()const
{
	return title_;
}

void GetTitleDiagnoseRequest::setTitle(const std::string& title)
{
	title_ = title;
	setBodyParameter("Title", title);
}

std::string GetTitleDiagnoseRequest::getPlatform()const
{
	return platform_;
}

void GetTitleDiagnoseRequest::setPlatform(const std::string& platform)
{
	platform_ = platform;
	setBodyParameter("Platform", platform);
}

std::string GetTitleDiagnoseRequest::getExtra()const
{
	return extra_;
}

void GetTitleDiagnoseRequest::setExtra(const std::string& extra)
{
	extra_ = extra;
	setBodyParameter("Extra", extra);
}

std::string GetTitleDiagnoseRequest::getCategoryId()const
{
	return categoryId_;
}

void GetTitleDiagnoseRequest::setCategoryId(const std::string& categoryId)
{
	categoryId_ = categoryId;
	setBodyParameter("CategoryId", categoryId);
}

