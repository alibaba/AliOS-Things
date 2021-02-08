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

#include <alibabacloud/schedulerx2/model/GetJobInstanceRequest.h>

using AlibabaCloud::Schedulerx2::Model::GetJobInstanceRequest;

GetJobInstanceRequest::GetJobInstanceRequest() :
	RpcServiceRequest("schedulerx2", "2019-04-30", "GetJobInstance")
{
	setMethod(HttpRequest::Method::Get);
}

GetJobInstanceRequest::~GetJobInstanceRequest()
{}

long GetJobInstanceRequest::getJobId()const
{
	return jobId_;
}

void GetJobInstanceRequest::setJobId(long jobId)
{
	jobId_ = jobId;
	setParameter("JobId", std::to_string(jobId));
}

std::string GetJobInstanceRequest::getNamespaceSource()const
{
	return namespaceSource_;
}

void GetJobInstanceRequest::setNamespaceSource(const std::string& namespaceSource)
{
	namespaceSource_ = namespaceSource;
	setParameter("NamespaceSource", namespaceSource);
}

std::string GetJobInstanceRequest::getGroupId()const
{
	return groupId_;
}

void GetJobInstanceRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string GetJobInstanceRequest::get_Namespace()const
{
	return _namespace_;
}

void GetJobInstanceRequest::set_Namespace(const std::string& _namespace)
{
	_namespace_ = _namespace;
	setParameter("_Namespace", _namespace);
}

long GetJobInstanceRequest::getJobInstanceId()const
{
	return jobInstanceId_;
}

void GetJobInstanceRequest::setJobInstanceId(long jobInstanceId)
{
	jobInstanceId_ = jobInstanceId;
	setParameter("JobInstanceId", std::to_string(jobInstanceId));
}

