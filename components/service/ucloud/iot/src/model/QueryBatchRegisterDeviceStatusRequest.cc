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

#include <alibabacloud/iot/model/QueryBatchRegisterDeviceStatusRequest.h>

using AlibabaCloud::Iot::Model::QueryBatchRegisterDeviceStatusRequest;

QueryBatchRegisterDeviceStatusRequest::QueryBatchRegisterDeviceStatusRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryBatchRegisterDeviceStatus")
{
	setMethod(HttpRequest::Method::Post);
}

QueryBatchRegisterDeviceStatusRequest::~QueryBatchRegisterDeviceStatusRequest()
{}

std::string QueryBatchRegisterDeviceStatusRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryBatchRegisterDeviceStatusRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryBatchRegisterDeviceStatusRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryBatchRegisterDeviceStatusRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string QueryBatchRegisterDeviceStatusRequest::getProductKey()const
{
	return productKey_;
}

void QueryBatchRegisterDeviceStatusRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

long QueryBatchRegisterDeviceStatusRequest::getApplyId()const
{
	return applyId_;
}

void QueryBatchRegisterDeviceStatusRequest::setApplyId(long applyId)
{
	applyId_ = applyId;
	setParameter("ApplyId", std::to_string(applyId));
}

std::string QueryBatchRegisterDeviceStatusRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryBatchRegisterDeviceStatusRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryBatchRegisterDeviceStatusRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryBatchRegisterDeviceStatusRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

