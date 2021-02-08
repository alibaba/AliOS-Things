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

#include <alibabacloud/iot/model/QueryEdgeInstanceGatewayRequest.h>

using AlibabaCloud::Iot::Model::QueryEdgeInstanceGatewayRequest;

QueryEdgeInstanceGatewayRequest::QueryEdgeInstanceGatewayRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryEdgeInstanceGateway")
{
	setMethod(HttpRequest::Method::Post);
}

QueryEdgeInstanceGatewayRequest::~QueryEdgeInstanceGatewayRequest()
{}

std::string QueryEdgeInstanceGatewayRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryEdgeInstanceGatewayRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryEdgeInstanceGatewayRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryEdgeInstanceGatewayRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string QueryEdgeInstanceGatewayRequest::getInstanceId()const
{
	return instanceId_;
}

void QueryEdgeInstanceGatewayRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string QueryEdgeInstanceGatewayRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryEdgeInstanceGatewayRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryEdgeInstanceGatewayRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryEdgeInstanceGatewayRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

