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

#include <alibabacloud/iot/model/CancelOTATaskByJobRequest.h>

using AlibabaCloud::Iot::Model::CancelOTATaskByJobRequest;

CancelOTATaskByJobRequest::CancelOTATaskByJobRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CancelOTATaskByJob")
{
	setMethod(HttpRequest::Method::Post);
}

CancelOTATaskByJobRequest::~CancelOTATaskByJobRequest()
{}

bool CancelOTATaskByJobRequest::getCancelScheduledTask()const
{
	return cancelScheduledTask_;
}

void CancelOTATaskByJobRequest::setCancelScheduledTask(bool cancelScheduledTask)
{
	cancelScheduledTask_ = cancelScheduledTask;
	setParameter("CancelScheduledTask", cancelScheduledTask ? "true" : "false");
}

std::string CancelOTATaskByJobRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CancelOTATaskByJobRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CancelOTATaskByJobRequest::getJobId()const
{
	return jobId_;
}

void CancelOTATaskByJobRequest::setJobId(const std::string& jobId)
{
	jobId_ = jobId;
	setParameter("JobId", jobId);
}

std::string CancelOTATaskByJobRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CancelOTATaskByJobRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

bool CancelOTATaskByJobRequest::getCancelQueuedTask()const
{
	return cancelQueuedTask_;
}

void CancelOTATaskByJobRequest::setCancelQueuedTask(bool cancelQueuedTask)
{
	cancelQueuedTask_ = cancelQueuedTask;
	setParameter("CancelQueuedTask", cancelQueuedTask ? "true" : "false");
}

bool CancelOTATaskByJobRequest::getCancelInProgressTask()const
{
	return cancelInProgressTask_;
}

void CancelOTATaskByJobRequest::setCancelInProgressTask(bool cancelInProgressTask)
{
	cancelInProgressTask_ = cancelInProgressTask;
	setParameter("CancelInProgressTask", cancelInProgressTask ? "true" : "false");
}

bool CancelOTATaskByJobRequest::getCancelNotifiedTask()const
{
	return cancelNotifiedTask_;
}

void CancelOTATaskByJobRequest::setCancelNotifiedTask(bool cancelNotifiedTask)
{
	cancelNotifiedTask_ = cancelNotifiedTask;
	setParameter("CancelNotifiedTask", cancelNotifiedTask ? "true" : "false");
}

std::string CancelOTATaskByJobRequest::getApiProduct()const
{
	return apiProduct_;
}

void CancelOTATaskByJobRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CancelOTATaskByJobRequest::getApiRevision()const
{
	return apiRevision_;
}

void CancelOTATaskByJobRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

