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

#include <alibabacloud/cr/model/GetRepoSyncTaskListRequest.h>

using AlibabaCloud::Cr::Model::GetRepoSyncTaskListRequest;

GetRepoSyncTaskListRequest::GetRepoSyncTaskListRequest() :
	RoaServiceRequest("cr", "2016-06-07")
{}

GetRepoSyncTaskListRequest::~GetRepoSyncTaskListRequest()
{}

std::string GetRepoSyncTaskListRequest::getRepoNamespace()const
{
	return repoNamespace_;
}

void GetRepoSyncTaskListRequest::setRepoNamespace(const std::string& repoNamespace)
{
	repoNamespace_ = repoNamespace;
	setCoreParameter("RepoNamespace", repoNamespace);
}

std::string GetRepoSyncTaskListRequest::getRepoName()const
{
	return repoName_;
}

void GetRepoSyncTaskListRequest::setRepoName(const std::string& repoName)
{
	repoName_ = repoName;
	setCoreParameter("RepoName", repoName);
}

int GetRepoSyncTaskListRequest::getPageSize()const
{
	return pageSize_;
}

void GetRepoSyncTaskListRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setCoreParameter("PageSize", std::to_string(pageSize));
}

int GetRepoSyncTaskListRequest::getPage()const
{
	return page_;
}

void GetRepoSyncTaskListRequest::setPage(int page)
{
	page_ = page;
	setCoreParameter("Page", std::to_string(page));
}

