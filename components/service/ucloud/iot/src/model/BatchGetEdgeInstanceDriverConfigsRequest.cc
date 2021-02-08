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

#include <alibabacloud/iot/model/BatchGetEdgeInstanceDriverConfigsRequest.h>

using AlibabaCloud::Iot::Model::BatchGetEdgeInstanceDriverConfigsRequest;

BatchGetEdgeInstanceDriverConfigsRequest::BatchGetEdgeInstanceDriverConfigsRequest() :
	RpcServiceRequest("iot", "2018-01-20", "BatchGetEdgeInstanceDriverConfigs")
{
	setMethod(HttpRequest::Method::Post);
}

BatchGetEdgeInstanceDriverConfigsRequest::~BatchGetEdgeInstanceDriverConfigsRequest()
{}

std::vector<std::string> BatchGetEdgeInstanceDriverConfigsRequest::getDriverIds()const
{
	return driverIds_;
}

void BatchGetEdgeInstanceDriverConfigsRequest::setDriverIds(const std::vector<std::string>& driverIds)
{
	driverIds_ = driverIds;
	for(int dep1 = 0; dep1!= driverIds.size(); dep1++) {
		setParameter("DriverIds."+ std::to_string(dep1), driverIds.at(dep1));
	}
}

std::string BatchGetEdgeInstanceDriverConfigsRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void BatchGetEdgeInstanceDriverConfigsRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string BatchGetEdgeInstanceDriverConfigsRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void BatchGetEdgeInstanceDriverConfigsRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string BatchGetEdgeInstanceDriverConfigsRequest::getInstanceId()const
{
	return instanceId_;
}

void BatchGetEdgeInstanceDriverConfigsRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string BatchGetEdgeInstanceDriverConfigsRequest::getApiProduct()const
{
	return apiProduct_;
}

void BatchGetEdgeInstanceDriverConfigsRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string BatchGetEdgeInstanceDriverConfigsRequest::getApiRevision()const
{
	return apiRevision_;
}

void BatchGetEdgeInstanceDriverConfigsRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

