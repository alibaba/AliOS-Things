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

#include <alibabacloud/eas/model/GetTaskRequest.h>

using AlibabaCloud::Eas::Model::GetTaskRequest;

GetTaskRequest::GetTaskRequest() :
	RoaServiceRequest("eas", "2018-05-22")
{
	setResourcePath("/api/tasks/[region]/[task_name]");
	setMethod(HttpRequest::Method::Get);
}

GetTaskRequest::~GetTaskRequest()
{}

std::string GetTaskRequest::getTask_name()const
{
	return task_name_;
}

void GetTaskRequest::setTask_name(const std::string& task_name)
{
	task_name_ = task_name;
	setParameter("Task_name", task_name);
}

std::string GetTaskRequest::getRegion()const
{
	return region_;
}

void GetTaskRequest::setRegion(const std::string& region)
{
	region_ = region;
	setParameter("Region", region);
}

