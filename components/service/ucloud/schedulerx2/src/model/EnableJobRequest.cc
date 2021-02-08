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

#include <alibabacloud/schedulerx2/model/EnableJobRequest.h>

using AlibabaCloud::Schedulerx2::Model::EnableJobRequest;

EnableJobRequest::EnableJobRequest() :
	RpcServiceRequest("schedulerx2", "2019-04-30", "EnableJob")
{
	setMethod(HttpRequest::Method::Get);
}

EnableJobRequest::~EnableJobRequest()
{}

long EnableJobRequest::getJobId()const
{
	return jobId_;
}

void EnableJobRequest::setJobId(long jobId)
{
	jobId_ = jobId;
	setParameter("JobId", std::to_string(jobId));
}

std::string EnableJobRequest::getNamespaceSource()const
{
	return namespaceSource_;
}

void EnableJobRequest::setNamespaceSource(const std::string& namespaceSource)
{
	namespaceSource_ = namespaceSource;
	setParameter("NamespaceSource", namespaceSource);
}

std::string EnableJobRequest::getGroupId()const
{
	return groupId_;
}

void EnableJobRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string EnableJobRequest::get_Namespace()const
{
	return _namespace_;
}

void EnableJobRequest::set_Namespace(const std::string& _namespace)
{
	_namespace_ = _namespace;
	setParameter("_Namespace", _namespace);
}

