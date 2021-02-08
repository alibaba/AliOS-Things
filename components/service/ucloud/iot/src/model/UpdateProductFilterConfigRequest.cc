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

#include <alibabacloud/iot/model/UpdateProductFilterConfigRequest.h>

using AlibabaCloud::Iot::Model::UpdateProductFilterConfigRequest;

UpdateProductFilterConfigRequest::UpdateProductFilterConfigRequest() :
	RpcServiceRequest("iot", "2018-01-20", "UpdateProductFilterConfig")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateProductFilterConfigRequest::~UpdateProductFilterConfigRequest()
{}

bool UpdateProductFilterConfigRequest::getPropertyTimestampFilter()const
{
	return propertyTimestampFilter_;
}

void UpdateProductFilterConfigRequest::setPropertyTimestampFilter(bool propertyTimestampFilter)
{
	propertyTimestampFilter_ = propertyTimestampFilter;
	setParameter("PropertyTimestampFilter", propertyTimestampFilter ? "true" : "false");
}

std::string UpdateProductFilterConfigRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void UpdateProductFilterConfigRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string UpdateProductFilterConfigRequest::getProductKey()const
{
	return productKey_;
}

void UpdateProductFilterConfigRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string UpdateProductFilterConfigRequest::getApiProduct()const
{
	return apiProduct_;
}

void UpdateProductFilterConfigRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string UpdateProductFilterConfigRequest::getApiRevision()const
{
	return apiRevision_;
}

void UpdateProductFilterConfigRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

bool UpdateProductFilterConfigRequest::getPropertyValueFilter()const
{
	return propertyValueFilter_;
}

void UpdateProductFilterConfigRequest::setPropertyValueFilter(bool propertyValueFilter)
{
	propertyValueFilter_ = propertyValueFilter;
	setParameter("PropertyValueFilter", propertyValueFilter ? "true" : "false");
}

