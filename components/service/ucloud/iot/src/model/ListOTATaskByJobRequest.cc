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

#include <alibabacloud/iot/model/ListOTATaskByJobRequest.h>

using AlibabaCloud::Iot::Model::ListOTATaskByJobRequest;

ListOTATaskByJobRequest::ListOTATaskByJobRequest() :
	RpcServiceRequest("iot", "2018-01-20", "ListOTATaskByJob")
{
	setMethod(HttpRequest::Method::Post);
}

ListOTATaskByJobRequest::~ListOTATaskByJobRequest()
{}

std::string ListOTATaskByJobRequest::getJobId()const
{
	return jobId_;
}

void ListOTATaskByJobRequest::setJobId(const std::string& jobId)
{
	jobId_ = jobId;
	setParameter("JobId", jobId);
}

std::string ListOTATaskByJobRequest::getTaskStatus()const
{
	return taskStatus_;
}

void ListOTATaskByJobRequest::setTaskStatus(const std::string& taskStatus)
{
	taskStatus_ = taskStatus;
	setParameter("TaskStatus", taskStatus);
}

std::string ListOTATaskByJobRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void ListOTATaskByJobRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int ListOTATaskByJobRequest::getPageSize()const
{
	return pageSize_;
}

void ListOTATaskByJobRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

int ListOTATaskByJobRequest::getCurrentPage()const
{
	return currentPage_;
}

void ListOTATaskByJobRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string ListOTATaskByJobRequest::getApiProduct()const
{
	return apiProduct_;
}

void ListOTATaskByJobRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string ListOTATaskByJobRequest::getApiRevision()const
{
	return apiRevision_;
}

void ListOTATaskByJobRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

