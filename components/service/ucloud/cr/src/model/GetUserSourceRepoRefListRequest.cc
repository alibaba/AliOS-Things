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

#include <alibabacloud/cr/model/GetUserSourceRepoRefListRequest.h>

using AlibabaCloud::Cr::Model::GetUserSourceRepoRefListRequest;

GetUserSourceRepoRefListRequest::GetUserSourceRepoRefListRequest() :
	RoaServiceRequest("cr", "2016-06-07")
{}

GetUserSourceRepoRefListRequest::~GetUserSourceRepoRefListRequest()
{}

long GetUserSourceRepoRefListRequest::getSourceAccountId()const
{
	return sourceAccountId_;
}

void GetUserSourceRepoRefListRequest::setSourceAccountId(long sourceAccountId)
{
	sourceAccountId_ = sourceAccountId;
	setCoreParameter("SourceAccountId", std::to_string(sourceAccountId));
}

std::string GetUserSourceRepoRefListRequest::getSourceRepoName()const
{
	return sourceRepoName_;
}

void GetUserSourceRepoRefListRequest::setSourceRepoName(const std::string& sourceRepoName)
{
	sourceRepoName_ = sourceRepoName;
	setCoreParameter("SourceRepoName", sourceRepoName);
}

std::string GetUserSourceRepoRefListRequest::getSourceRepoNamespace()const
{
	return sourceRepoNamespace_;
}

void GetUserSourceRepoRefListRequest::setSourceRepoNamespace(const std::string& sourceRepoNamespace)
{
	sourceRepoNamespace_ = sourceRepoNamespace;
	setCoreParameter("SourceRepoNamespace", sourceRepoNamespace);
}

