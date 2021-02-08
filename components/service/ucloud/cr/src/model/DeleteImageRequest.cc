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

#include <alibabacloud/cr/model/DeleteImageRequest.h>

using AlibabaCloud::Cr::Model::DeleteImageRequest;

DeleteImageRequest::DeleteImageRequest() :
	RoaServiceRequest("cr", "2016-06-07")
{
	setResourcePath("/repos/[RepoNamespace]/[RepoName]/tags/[Tag]");
	setMethod(HttpRequest::Method::Delete);
}

DeleteImageRequest::~DeleteImageRequest()
{}

std::string DeleteImageRequest::getRepoNamespace()const
{
	return repoNamespace_;
}

void DeleteImageRequest::setRepoNamespace(const std::string& repoNamespace)
{
	repoNamespace_ = repoNamespace;
	setParameter("RepoNamespace", repoNamespace);
}

std::string DeleteImageRequest::getRepoName()const
{
	return repoName_;
}

void DeleteImageRequest::setRepoName(const std::string& repoName)
{
	repoName_ = repoName;
	setParameter("RepoName", repoName);
}

std::string DeleteImageRequest::getTag()const
{
	return tag_;
}

void DeleteImageRequest::setTag(const std::string& tag)
{
	tag_ = tag;
	setParameter("Tag", tag);
}

