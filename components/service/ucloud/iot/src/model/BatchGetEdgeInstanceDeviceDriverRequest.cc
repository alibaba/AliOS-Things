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

#include <alibabacloud/iot/model/BatchGetEdgeInstanceDeviceDriverRequest.h>

using AlibabaCloud::Iot::Model::BatchGetEdgeInstanceDeviceDriverRequest;

BatchGetEdgeInstanceDeviceDriverRequest::BatchGetEdgeInstanceDeviceDriverRequest() :
	RpcServiceRequest("iot", "2018-01-20", "BatchGetEdgeInstanceDeviceDriver")
{
	setMethod(HttpRequest::Method::Post);
}

BatchGetEdgeInstanceDeviceDriverRequest::~BatchGetEdgeInstanceDeviceDriverRequest()
{}

std::string BatchGetEdgeInstanceDeviceDriverRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void BatchGetEdgeInstanceDeviceDriverRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::vector<std::string> BatchGetEdgeInstanceDeviceDriverRequest::getIotIds()const
{
	return iotIds_;
}

void BatchGetEdgeInstanceDeviceDriverRequest::setIotIds(const std::vector<std::string>& iotIds)
{
	iotIds_ = iotIds;
	for(int dep1 = 0; dep1!= iotIds.size(); dep1++) {
		setParameter("IotIds."+ std::to_string(dep1), iotIds.at(dep1));
	}
}

std::string BatchGetEdgeInstanceDeviceDriverRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void BatchGetEdgeInstanceDeviceDriverRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string BatchGetEdgeInstanceDeviceDriverRequest::getInstanceId()const
{
	return instanceId_;
}

void BatchGetEdgeInstanceDeviceDriverRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string BatchGetEdgeInstanceDeviceDriverRequest::getApiProduct()const
{
	return apiProduct_;
}

void BatchGetEdgeInstanceDeviceDriverRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string BatchGetEdgeInstanceDeviceDriverRequest::getApiRevision()const
{
	return apiRevision_;
}

void BatchGetEdgeInstanceDeviceDriverRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

