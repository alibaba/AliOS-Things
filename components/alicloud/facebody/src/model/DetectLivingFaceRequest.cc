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

#include <alibabacloud/facebody/model/DetectLivingFaceRequest.h>

using AlibabaCloud::Facebody::Model::DetectLivingFaceRequest;

DetectLivingFaceRequest::DetectLivingFaceRequest() :
	RpcServiceRequest("facebody", "2019-12-30", "DetectLivingFace")
{
	setMethod(HttpRequest::Method::Post);
}

DetectLivingFaceRequest::~DetectLivingFaceRequest()
{}

std::vector<DetectLivingFaceRequest::Tasks> DetectLivingFaceRequest::getTasks()const
{
	return tasks_;
}

void DetectLivingFaceRequest::setTasks(const std::vector<Tasks>& tasks)
{
	tasks_ = tasks;
	for(int dep1 = 0; dep1!= tasks.size(); dep1++) {
		auto tasksObj = tasks.at(dep1);
		std::string tasksObjStr = "Tasks." + std::to_string(dep1 + 1);
		setParameter(tasksObjStr + ".ImageURL", tasksObj.imageURL);
	}
}

