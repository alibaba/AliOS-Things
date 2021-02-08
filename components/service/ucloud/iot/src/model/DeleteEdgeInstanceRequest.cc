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

#include <alibabacloud/iot/model/DeleteEdgeInstanceRequest.h>

using AlibabaCloud::Iot::Model::DeleteEdgeInstanceRequest;

DeleteEdgeInstanceRequest::DeleteEdgeInstanceRequest() :
	RpcServiceRequest("iot", "2018-01-20", "DeleteEdgeInstance")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteEdgeInstanceRequest::~DeleteEdgeInstanceRequest()
{}

std::string DeleteEdgeInstanceRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DeleteEdgeInstanceRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DeleteEdgeInstanceRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void DeleteEdgeInstanceRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string DeleteEdgeInstanceRequest::getInstanceId()const
{
	return instanceId_;
}

void DeleteEdgeInstanceRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string DeleteEdgeInstanceRequest::getApiProduct()const
{
	return apiProduct_;
}

void DeleteEdgeInstanceRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string DeleteEdgeInstanceRequest::getApiRevision()const
{
	return apiRevision_;
}

void DeleteEdgeInstanceRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

