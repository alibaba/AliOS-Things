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

#include <alibabacloud/imageaudit/model/ScanImageRequest.h>

using AlibabaCloud::Imageaudit::Model::ScanImageRequest;

ScanImageRequest::ScanImageRequest() :
	RpcServiceRequest("imageaudit", "2019-12-30", "ScanImage")
{
	setMethod(HttpRequest::Method::Post);
}

ScanImageRequest::~ScanImageRequest()
{}

std::vector<std::string> ScanImageRequest::getScene()const
{
	return scene_;
}

void ScanImageRequest::setScene(const std::vector<std::string>& scene)
{
	scene_ = scene;
	for(int dep1 = 0; dep1!= scene.size(); dep1++) {
		setBodyParameter("Scene."+ std::to_string(dep1), scene.at(dep1));
	}
}

std::vector<ScanImageRequest::Task> ScanImageRequest::getTask()const
{
	return task_;
}

void ScanImageRequest::setTask(const std::vector<Task>& task)
{
	task_ = task;
	for(int dep1 = 0; dep1!= task.size(); dep1++) {
		auto taskObj = task.at(dep1);
		std::string taskObjStr = "Task." + std::to_string(dep1 + 1);
		setParameter(taskObjStr + ".DataId", taskObj.dataId);
		setParameter(taskObjStr + ".ImageURL", taskObj.imageURL);
		setParameter(taskObjStr + ".MaxFrames", std::to_string(taskObj.maxFrames));
		setParameter(taskObjStr + ".Interval", std::to_string(taskObj.interval));
		setParameter(taskObjStr + ".ImageTimeMillisecond", std::to_string(taskObj.imageTimeMillisecond));
	}
}

