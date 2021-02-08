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

#include <alibabacloud/iot/model/QueryDeviceStatisticsRequest.h>

using AlibabaCloud::Iot::Model::QueryDeviceStatisticsRequest;

QueryDeviceStatisticsRequest::QueryDeviceStatisticsRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryDeviceStatistics")
{
	setMethod(HttpRequest::Method::Post);
}

QueryDeviceStatisticsRequest::~QueryDeviceStatisticsRequest()
{}

std::string QueryDeviceStatisticsRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryDeviceStatisticsRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryDeviceStatisticsRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryDeviceStatisticsRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string QueryDeviceStatisticsRequest::getProductKey()const
{
	return productKey_;
}

void QueryDeviceStatisticsRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string QueryDeviceStatisticsRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryDeviceStatisticsRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryDeviceStatisticsRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryDeviceStatisticsRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

