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

#include <alibabacloud/cr/model/SearchRepoRequest.h>

using AlibabaCloud::Cr::Model::SearchRepoRequest;

SearchRepoRequest::SearchRepoRequest() :
	RoaServiceRequest("cr", "2016-06-07")
{}

SearchRepoRequest::~SearchRepoRequest()
{}

std::string SearchRepoRequest::getOrigin()const
{
	return origin_;
}

void SearchRepoRequest::setOrigin(const std::string& origin)
{
	origin_ = origin;
	setCoreParameter("Origin", origin);
}

int SearchRepoRequest::getPageSize()const
{
	return pageSize_;
}

void SearchRepoRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setCoreParameter("PageSize", std::to_string(pageSize));
}

int SearchRepoRequest::getPage()const
{
	return page_;
}

void SearchRepoRequest::setPage(int page)
{
	page_ = page;
	setCoreParameter("Page", std::to_string(page));
}

std::string SearchRepoRequest::getKeyword()const
{
	return keyword_;
}

void SearchRepoRequest::setKeyword(const std::string& keyword)
{
	keyword_ = keyword;
	setCoreParameter("Keyword", keyword);
}

