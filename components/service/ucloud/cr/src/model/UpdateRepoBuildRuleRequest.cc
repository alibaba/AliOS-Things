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

#include <alibabacloud/cr/model/UpdateRepoBuildRuleRequest.h>

using AlibabaCloud::Cr::Model::UpdateRepoBuildRuleRequest;

UpdateRepoBuildRuleRequest::UpdateRepoBuildRuleRequest() :
	RoaServiceRequest("cr", "2016-06-07")
{
	setResourcePath("/repos/[RepoNamespace]/[RepoName]/rules/[BuildRuleId]");
	setMethod(HttpRequest::Method::Post);
}

UpdateRepoBuildRuleRequest::~UpdateRepoBuildRuleRequest()
{}

long UpdateRepoBuildRuleRequest::getBuildRuleId()const
{
	return buildRuleId_;
}

void UpdateRepoBuildRuleRequest::setBuildRuleId(long buildRuleId)
{
	buildRuleId_ = buildRuleId;
	setParameter("BuildRuleId", std::to_string(buildRuleId));
}

std::string UpdateRepoBuildRuleRequest::getRepoNamespace()const
{
	return repoNamespace_;
}

void UpdateRepoBuildRuleRequest::setRepoNamespace(const std::string& repoNamespace)
{
	repoNamespace_ = repoNamespace;
	setParameter("RepoNamespace", repoNamespace);
}

std::string UpdateRepoBuildRuleRequest::getRepoName()const
{
	return repoName_;
}

void UpdateRepoBuildRuleRequest::setRepoName(const std::string& repoName)
{
	repoName_ = repoName;
	setParameter("RepoName", repoName);
}

