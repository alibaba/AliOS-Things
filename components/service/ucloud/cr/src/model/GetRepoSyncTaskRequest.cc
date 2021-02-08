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

#include <alibabacloud/cr/model/GetRepoSyncTaskRequest.h>

using AlibabaCloud::Cr::Model::GetRepoSyncTaskRequest;

GetRepoSyncTaskRequest::GetRepoSyncTaskRequest() :
	RoaServiceRequest("cr", "2016-06-07")
{}

GetRepoSyncTaskRequest::~GetRepoSyncTaskRequest()
{}

std::string GetRepoSyncTaskRequest::getSyncTaskId()const
{
	return syncTaskId_;
}

void GetRepoSyncTaskRequest::setSyncTaskId(const std::string& syncTaskId)
{
	syncTaskId_ = syncTaskId;
	setCoreParameter("SyncTaskId", syncTaskId);
}

std::string GetRepoSyncTaskRequest::getRepoNamespace()const
{
	return repoNamespace_;
}

void GetRepoSyncTaskRequest::setRepoNamespace(const std::string& repoNamespace)
{
	repoNamespace_ = repoNamespace;
	setCoreParameter("RepoNamespace", repoNamespace);
}

std::string GetRepoSyncTaskRequest::getRepoName()const
{
	return repoName_;
}

void GetRepoSyncTaskRequest::setRepoName(const std::string& repoName)
{
	repoName_ = repoName;
	setCoreParameter("RepoName", repoName);
}

