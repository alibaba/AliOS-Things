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

#include <alibabacloud/iot/model/CancelOTAStrategyByJobRequest.h>

using AlibabaCloud::Iot::Model::CancelOTAStrategyByJobRequest;

CancelOTAStrategyByJobRequest::CancelOTAStrategyByJobRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CancelOTAStrategyByJob")
{
	setMethod(HttpRequest::Method::Post);
}

CancelOTAStrategyByJobRequest::~CancelOTAStrategyByJobRequest()
{}

std::string CancelOTAStrategyByJobRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CancelOTAStrategyByJobRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CancelOTAStrategyByJobRequest::getJobId()const
{
	return jobId_;
}

void CancelOTAStrategyByJobRequest::setJobId(const std::string& jobId)
{
	jobId_ = jobId;
	setParameter("JobId", jobId);
}

std::string CancelOTAStrategyByJobRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CancelOTAStrategyByJobRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string CancelOTAStrategyByJobRequest::getApiProduct()const
{
	return apiProduct_;
}

void CancelOTAStrategyByJobRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CancelOTAStrategyByJobRequest::getApiRevision()const
{
	return apiRevision_;
}

void CancelOTAStrategyByJobRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

