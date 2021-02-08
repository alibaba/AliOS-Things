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

#include <alibabacloud/iot/model/DeleteEdgeDriverRequest.h>

using AlibabaCloud::Iot::Model::DeleteEdgeDriverRequest;

DeleteEdgeDriverRequest::DeleteEdgeDriverRequest() :
	RpcServiceRequest("iot", "2018-01-20", "DeleteEdgeDriver")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteEdgeDriverRequest::~DeleteEdgeDriverRequest()
{}

std::string DeleteEdgeDriverRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DeleteEdgeDriverRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DeleteEdgeDriverRequest::getDriverId()const
{
	return driverId_;
}

void DeleteEdgeDriverRequest::setDriverId(const std::string& driverId)
{
	driverId_ = driverId;
	setParameter("DriverId", driverId);
}

std::string DeleteEdgeDriverRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void DeleteEdgeDriverRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string DeleteEdgeDriverRequest::getApiProduct()const
{
	return apiProduct_;
}

void DeleteEdgeDriverRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string DeleteEdgeDriverRequest::getApiRevision()const
{
	return apiRevision_;
}

void DeleteEdgeDriverRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

