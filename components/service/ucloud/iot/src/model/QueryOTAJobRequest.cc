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

#include <alibabacloud/iot/model/QueryOTAJobRequest.h>

using AlibabaCloud::Iot::Model::QueryOTAJobRequest;

QueryOTAJobRequest::QueryOTAJobRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryOTAJob")
{
	setMethod(HttpRequest::Method::Post);
}

QueryOTAJobRequest::~QueryOTAJobRequest()
{}

std::string QueryOTAJobRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryOTAJobRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryOTAJobRequest::getJobId()const
{
	return jobId_;
}

void QueryOTAJobRequest::setJobId(const std::string& jobId)
{
	jobId_ = jobId;
	setParameter("JobId", jobId);
}

std::string QueryOTAJobRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryOTAJobRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string QueryOTAJobRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryOTAJobRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryOTAJobRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryOTAJobRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

