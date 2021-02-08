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

#include <alibabacloud/iot/model/DeleteThingModelRequest.h>

using AlibabaCloud::Iot::Model::DeleteThingModelRequest;

DeleteThingModelRequest::DeleteThingModelRequest() :
	RpcServiceRequest("iot", "2018-01-20", "DeleteThingModel")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteThingModelRequest::~DeleteThingModelRequest()
{}

std::string DeleteThingModelRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DeleteThingModelRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DeleteThingModelRequest::getResourceGroupId()const
{
	return resourceGroupId_;
}

void DeleteThingModelRequest::setResourceGroupId(const std::string& resourceGroupId)
{
	resourceGroupId_ = resourceGroupId;
	setParameter("ResourceGroupId", resourceGroupId);
}

std::vector<std::string> DeleteThingModelRequest::getPropertyIdentifier()const
{
	return propertyIdentifier_;
}

void DeleteThingModelRequest::setPropertyIdentifier(const std::vector<std::string>& propertyIdentifier)
{
	propertyIdentifier_ = propertyIdentifier;
	for(int dep1 = 0; dep1!= propertyIdentifier.size(); dep1++) {
		setParameter("PropertyIdentifier."+ std::to_string(dep1), propertyIdentifier.at(dep1));
	}
}

std::string DeleteThingModelRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void DeleteThingModelRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::vector<std::string> DeleteThingModelRequest::getServiceIdentifier()const
{
	return serviceIdentifier_;
}

void DeleteThingModelRequest::setServiceIdentifier(const std::vector<std::string>& serviceIdentifier)
{
	serviceIdentifier_ = serviceIdentifier;
	for(int dep1 = 0; dep1!= serviceIdentifier.size(); dep1++) {
		setParameter("ServiceIdentifier."+ std::to_string(dep1), serviceIdentifier.at(dep1));
	}
}

std::string DeleteThingModelRequest::getProductKey()const
{
	return productKey_;
}

void DeleteThingModelRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string DeleteThingModelRequest::getApiProduct()const
{
	return apiProduct_;
}

void DeleteThingModelRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string DeleteThingModelRequest::getApiRevision()const
{
	return apiRevision_;
}

void DeleteThingModelRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::vector<std::string> DeleteThingModelRequest::getEventIdentifier()const
{
	return eventIdentifier_;
}

void DeleteThingModelRequest::setEventIdentifier(const std::vector<std::string>& eventIdentifier)
{
	eventIdentifier_ = eventIdentifier;
	for(int dep1 = 0; dep1!= eventIdentifier.size(); dep1++) {
		setParameter("EventIdentifier."+ std::to_string(dep1), eventIdentifier.at(dep1));
	}
}

