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

#include <alibabacloud/alimt/model/GetDocTranslateTaskRequest.h>

using AlibabaCloud::Alimt::Model::GetDocTranslateTaskRequest;

GetDocTranslateTaskRequest::GetDocTranslateTaskRequest() :
	RpcServiceRequest("alimt", "2018-10-12", "GetDocTranslateTask")
{
	setMethod(HttpRequest::Method::Get);
}

GetDocTranslateTaskRequest::~GetDocTranslateTaskRequest()
{}

std::string GetDocTranslateTaskRequest::getTaskId()const
{
	return taskId_;
}

void GetDocTranslateTaskRequest::setTaskId(const std::string& taskId)
{
	taskId_ = taskId;
	setParameter("TaskId", taskId);
}

