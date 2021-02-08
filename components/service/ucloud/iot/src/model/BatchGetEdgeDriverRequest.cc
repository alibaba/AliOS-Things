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

#include <alibabacloud/iot/model/BatchGetEdgeDriverRequest.h>

using AlibabaCloud::Iot::Model::BatchGetEdgeDriverRequest;

BatchGetEdgeDriverRequest::BatchGetEdgeDriverRequest() :
	RpcServiceRequest("iot", "2018-01-20", "BatchGetEdgeDriver")
{
	setMethod(HttpRequest::Method::Post);
}

BatchGetEdgeDriverRequest::~BatchGetEdgeDriverRequest()
{}

std::vector<std::string> BatchGetEdgeDriverRequest::getDriverIds()const
{
	return driverIds_;
}

void BatchGetEdgeDriverRequest::setDriverIds(const std::vector<std::string>& driverIds)
{
	driverIds_ = driverIds;
	for(int dep1 = 0; dep1!= driverIds.size(); dep1++) {
		setParameter("DriverIds."+ std::to_string(dep1), driverIds.at(dep1));
	}
}

std::string BatchGetEdgeDriverRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void BatchGetEdgeDriverRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string BatchGetEdgeDriverRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void BatchGetEdgeDriverRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string BatchGetEdgeDriverRequest::getApiProduct()const
{
	return apiProduct_;
}

void BatchGetEdgeDriverRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string BatchGetEdgeDriverRequest::getApiRevision()const
{
	return apiRevision_;
}

void BatchGetEdgeDriverRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

