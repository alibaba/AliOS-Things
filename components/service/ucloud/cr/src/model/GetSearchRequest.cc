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

#include <alibabacloud/cr/model/GetSearchRequest.h>

using AlibabaCloud::Cr::Model::GetSearchRequest;

GetSearchRequest::GetSearchRequest() :
	RoaServiceRequest("cr", "2016-06-07")
{}

GetSearchRequest::~GetSearchRequest()
{}

std::string GetSearchRequest::getOrigin()const
{
	return origin_;
}

void GetSearchRequest::setOrigin(const std::string& origin)
{
	origin_ = origin;
	setCoreParameter("Origin", origin);
}

int GetSearchRequest::getPageSize()const
{
	return pageSize_;
}

void GetSearchRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setCoreParameter("PageSize", std::to_string(pageSize));
}

int GetSearchRequest::getPage()const
{
	return page_;
}

void GetSearchRequest::setPage(int page)
{
	page_ = page;
	setCoreParameter("Page", std::to_string(page));
}

std::string GetSearchRequest::getKeyword()const
{
	return keyword_;
}

void GetSearchRequest::setKeyword(const std::string& keyword)
{
	keyword_ = keyword;
	setCoreParameter("Keyword", keyword);
}

