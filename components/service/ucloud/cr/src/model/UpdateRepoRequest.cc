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

#include <alibabacloud/cr/model/UpdateRepoRequest.h>

using AlibabaCloud::Cr::Model::UpdateRepoRequest;

UpdateRepoRequest::UpdateRepoRequest() :
	RoaServiceRequest("cr", "2016-06-07")
{
	setResourcePath("/repos/[RepoNamespace]/[RepoName]");
	setMethod(HttpRequest::Method::Post);
}

UpdateRepoRequest::~UpdateRepoRequest()
{}

std::string UpdateRepoRequest::getRepoNamespace()const
{
	return repoNamespace_;
}

void UpdateRepoRequest::setRepoNamespace(const std::string& repoNamespace)
{
	repoNamespace_ = repoNamespace;
	setParameter("RepoNamespace", repoNamespace);
}

std::string UpdateRepoRequest::getRepoName()const
{
	return repoName_;
}

void UpdateRepoRequest::setRepoName(const std::string& repoName)
{
	repoName_ = repoName;
	setParameter("RepoName", repoName);
}

