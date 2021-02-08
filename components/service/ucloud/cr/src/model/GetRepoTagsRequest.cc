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

#include <alibabacloud/cr/model/GetRepoTagsRequest.h>

using AlibabaCloud::Cr::Model::GetRepoTagsRequest;

GetRepoTagsRequest::GetRepoTagsRequest() :
	RoaServiceRequest("cr", "2016-06-07")
{
	setResourcePath("/repos/[RepoNamespace]/[RepoName]/tags");
	setMethod(HttpRequest::Method::Get);
}

GetRepoTagsRequest::~GetRepoTagsRequest()
{}

std::string GetRepoTagsRequest::getRepoNamespace()const
{
	return repoNamespace_;
}

void GetRepoTagsRequest::setRepoNamespace(const std::string& repoNamespace)
{
	repoNamespace_ = repoNamespace;
	setParameter("RepoNamespace", repoNamespace);
}

std::string GetRepoTagsRequest::getRepoName()const
{
	return repoName_;
}

void GetRepoTagsRequest::setRepoName(const std::string& repoName)
{
	repoName_ = repoName;
	setParameter("RepoName", repoName);
}

int GetRepoTagsRequest::getPageSize()const
{
	return pageSize_;
}

void GetRepoTagsRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

int GetRepoTagsRequest::getPage()const
{
	return page_;
}

void GetRepoTagsRequest::setPage(int page)
{
	page_ = page;
	setParameter("Page", std::to_string(page));
}

