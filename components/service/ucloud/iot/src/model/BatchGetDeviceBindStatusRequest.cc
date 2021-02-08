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

#include <alibabacloud/iot/model/BatchGetDeviceBindStatusRequest.h>

using AlibabaCloud::Iot::Model::BatchGetDeviceBindStatusRequest;

BatchGetDeviceBindStatusRequest::BatchGetDeviceBindStatusRequest() :
	RpcServiceRequest("iot", "2018-01-20", "BatchGetDeviceBindStatus")
{
	setMethod(HttpRequest::Method::Post);
}

BatchGetDeviceBindStatusRequest::~BatchGetDeviceBindStatusRequest()
{}

std::string BatchGetDeviceBindStatusRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void BatchGetDeviceBindStatusRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::vector<std::string> BatchGetDeviceBindStatusRequest::getIotIds()const
{
	return iotIds_;
}

void BatchGetDeviceBindStatusRequest::setIotIds(const std::vector<std::string>& iotIds)
{
	iotIds_ = iotIds;
	for(int dep1 = 0; dep1!= iotIds.size(); dep1++) {
		setParameter("IotIds."+ std::to_string(dep1), iotIds.at(dep1));
	}
}

std::string BatchGetDeviceBindStatusRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void BatchGetDeviceBindStatusRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string BatchGetDeviceBindStatusRequest::getApiProduct()const
{
	return apiProduct_;
}

void BatchGetDeviceBindStatusRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string BatchGetDeviceBindStatusRequest::getApiRevision()const
{
	return apiRevision_;
}

void BatchGetDeviceBindStatusRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

