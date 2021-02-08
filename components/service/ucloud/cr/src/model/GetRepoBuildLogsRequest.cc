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

#include <alibabacloud/cr/model/GetRepoBuildLogsRequest.h>

using AlibabaCloud::Cr::Model::GetRepoBuildLogsRequest;

GetRepoBuildLogsRequest::GetRepoBuildLogsRequest() :
	RoaServiceRequest("cr", "2016-06-07")
{}

GetRepoBuildLogsRequest::~GetRepoBuildLogsRequest()
{}

std::string GetRepoBuildLogsRequest::getBuildId()const
{
	return buildId_;
}

void GetRepoBuildLogsRequest::setBuildId(const std::string& buildId)
{
	buildId_ = buildId;
	setCoreParameter("BuildId", buildId);
}

std::string GetRepoBuildLogsRequest::getRepoNamespace()const
{
	return repoNamespace_;
}

void GetRepoBuildLogsRequest::setRepoNamespace(const std::string& repoNamespace)
{
	repoNamespace_ = repoNamespace;
	setCoreParameter("RepoNamespace", repoNamespace);
}

std::string GetRepoBuildLogsRequest::getRepoName()const
{
	return repoName_;
}

void GetRepoBuildLogsRequest::setRepoName(const std::string& repoName)
{
	repoName_ = repoName;
	setCoreParameter("RepoName", repoName);
}

