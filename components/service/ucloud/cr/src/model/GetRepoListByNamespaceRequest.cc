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

#include <alibabacloud/cr/model/GetRepoListByNamespaceRequest.h>

using AlibabaCloud::Cr::Model::GetRepoListByNamespaceRequest;

GetRepoListByNamespaceRequest::GetRepoListByNamespaceRequest() :
	RoaServiceRequest("cr", "2016-06-07")
{
	setResourcePath("/repos/[RepoNamespace]");
	setMethod(HttpRequest::Method::Get);
}

GetRepoListByNamespaceRequest::~GetRepoListByNamespaceRequest()
{}

std::string GetRepoListByNamespaceRequest::getRepoNamespace()const
{
	return repoNamespace_;
}

void GetRepoListByNamespaceRequest::setRepoNamespace(const std::string& repoNamespace)
{
	repoNamespace_ = repoNamespace;
	setParameter("RepoNamespace", repoNamespace);
}

int GetRepoListByNamespaceRequest::getPageSize()const
{
	return pageSize_;
}

void GetRepoListByNamespaceRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

int GetRepoListByNamespaceRequest::getPage()const
{
	return page_;
}

void GetRepoListByNamespaceRequest::setPage(int page)
{
	page_ = page;
	setParameter("Page", std::to_string(page));
}

std::string GetRepoListByNamespaceRequest::getStatus()const
{
	return status_;
}

void GetRepoListByNamespaceRequest::setStatus(const std::string& status)
{
	status_ = status;
	setParameter("Status", status);
}

