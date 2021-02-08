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

#include <alibabacloud/alimt/model/GetTitleIntelligenceRequest.h>

using AlibabaCloud::Alimt::Model::GetTitleIntelligenceRequest;

GetTitleIntelligenceRequest::GetTitleIntelligenceRequest() :
	RpcServiceRequest("alimt", "2018-10-12", "GetTitleIntelligence")
{
	setMethod(HttpRequest::Method::Post);
}

GetTitleIntelligenceRequest::~GetTitleIntelligenceRequest()
{}

long GetTitleIntelligenceRequest::getCatLevelThreeId()const
{
	return catLevelThreeId_;
}

void GetTitleIntelligenceRequest::setCatLevelThreeId(long catLevelThreeId)
{
	catLevelThreeId_ = catLevelThreeId;
	setBodyParameter("CatLevelThreeId", std::to_string(catLevelThreeId));
}

long GetTitleIntelligenceRequest::getCatLevelTwoId()const
{
	return catLevelTwoId_;
}

void GetTitleIntelligenceRequest::setCatLevelTwoId(long catLevelTwoId)
{
	catLevelTwoId_ = catLevelTwoId;
	setBodyParameter("CatLevelTwoId", std::to_string(catLevelTwoId));
}

std::string GetTitleIntelligenceRequest::getKeywords()const
{
	return keywords_;
}

void GetTitleIntelligenceRequest::setKeywords(const std::string& keywords)
{
	keywords_ = keywords;
	setBodyParameter("Keywords", keywords);
}

std::string GetTitleIntelligenceRequest::getPlatform()const
{
	return platform_;
}

void GetTitleIntelligenceRequest::setPlatform(const std::string& platform)
{
	platform_ = platform;
	setBodyParameter("Platform", platform);
}

std::string GetTitleIntelligenceRequest::getExtra()const
{
	return extra_;
}

void GetTitleIntelligenceRequest::setExtra(const std::string& extra)
{
	extra_ = extra;
	setBodyParameter("Extra", extra);
}

