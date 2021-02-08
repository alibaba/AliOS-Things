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

#include <alibabacloud/schedulerx2/model/DeleteWorkflowRequest.h>

using AlibabaCloud::Schedulerx2::Model::DeleteWorkflowRequest;

DeleteWorkflowRequest::DeleteWorkflowRequest() :
	RpcServiceRequest("schedulerx2", "2019-04-30", "DeleteWorkflow")
{
	setMethod(HttpRequest::Method::Get);
}

DeleteWorkflowRequest::~DeleteWorkflowRequest()
{}

std::string DeleteWorkflowRequest::getNamespaceSource()const
{
	return namespaceSource_;
}

void DeleteWorkflowRequest::setNamespaceSource(const std::string& namespaceSource)
{
	namespaceSource_ = namespaceSource;
	setParameter("NamespaceSource", namespaceSource);
}

std::string DeleteWorkflowRequest::getGroupId()const
{
	return groupId_;
}

void DeleteWorkflowRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string DeleteWorkflowRequest::get_Namespace()const
{
	return _namespace_;
}

void DeleteWorkflowRequest::set_Namespace(const std::string& _namespace)
{
	_namespace_ = _namespace;
	setParameter("_Namespace", _namespace);
}

long DeleteWorkflowRequest::getWorkflowId()const
{
	return workflowId_;
}

void DeleteWorkflowRequest::setWorkflowId(long workflowId)
{
	workflowId_ = workflowId;
	setParameter("WorkflowId", std::to_string(workflowId));
}

