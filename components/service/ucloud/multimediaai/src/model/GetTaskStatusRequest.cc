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

#include <alibabacloud/multimediaai/model/GetTaskStatusRequest.h>

using AlibabaCloud::Multimediaai::Model::GetTaskStatusRequest;

GetTaskStatusRequest::GetTaskStatusRequest() :
	RpcServiceRequest("multimediaai", "2019-08-10", "GetTaskStatus")
{
	setMethod(HttpRequest::Method::Post);
}

GetTaskStatusRequest::~GetTaskStatusRequest()
{}

long GetTaskStatusRequest::getTaskId()const
{
	return taskId_;
}

void GetTaskStatusRequest::setTaskId(long taskId)
{
	taskId_ = taskId;
	setParameter("TaskId", std::to_string(taskId));
}

