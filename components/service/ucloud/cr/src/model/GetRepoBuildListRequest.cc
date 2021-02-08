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

#include <alibabacloud/cr/model/GetRepoBuildListRequest.h>

using AlibabaCloud::Cr::Model::GetRepoBuildListRequest;

GetRepoBuildListRequest::GetRepoBuildListRequest() :
	RoaServiceRequest("cr", "2016-06-07")
{
	setResourcePath("/repos/[RepoNamespace]/[RepoName]/build");
	setMethod(HttpRequest::Method::Get);
}

GetRepoBuildListRequest::~GetRepoBuildListRequest()
{}

std::string GetRepoBuildListRequest::getRepoNamespace()const
{
	return repoNamespace_;
}

void GetRepoBuildListRequest::setRepoNamespace(const std::string& repoNamespace)
{
	repoNamespace_ = repoNamespace;
	setParameter("RepoNamespace", repoNamespace);
}

std::string GetRepoBuildListRequest::getRepoName()const
{
	return repoName_;
}

void GetRepoBuildListRequest::setRepoName(const std::string& repoName)
{
	repoName_ = repoName;
	setParameter("RepoName", repoName);
}

int GetRepoBuildListRequest::getPageSize()const
{
	return pageSize_;
}

void GetRepoBuildListRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

int GetRepoBuildListRequest::getPage()const
{
	return page_;
}

void GetRepoBuildListRequest::setPage(int page)
{
	page_ = page;
	setParameter("Page", std::to_string(page));
}

