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

#include <alibabacloud/imm/model/DeleteImageJobRequest.h>

using AlibabaCloud::Imm::Model::DeleteImageJobRequest;

DeleteImageJobRequest::DeleteImageJobRequest() :
	RpcServiceRequest("imm", "2017-09-06", "DeleteImageJob")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteImageJobRequest::~DeleteImageJobRequest()
{}

std::string DeleteImageJobRequest::getProject()const
{
	return project_;
}

void DeleteImageJobRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string DeleteImageJobRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DeleteImageJobRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DeleteImageJobRequest::getJobId()const
{
	return jobId_;
}

void DeleteImageJobRequest::setJobId(const std::string& jobId)
{
	jobId_ = jobId;
	setParameter("JobId", jobId);
}

std::string DeleteImageJobRequest::getJobType()const
{
	return jobType_;
}

void DeleteImageJobRequest::setJobType(const std::string& jobType)
{
	jobType_ = jobType;
	setParameter("JobType", jobType);
}

