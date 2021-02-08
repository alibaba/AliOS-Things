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

#include <alibabacloud/iot/model/GetThingTemplateRequest.h>

using AlibabaCloud::Iot::Model::GetThingTemplateRequest;

GetThingTemplateRequest::GetThingTemplateRequest() :
	RpcServiceRequest("iot", "2018-01-20", "GetThingTemplate")
{
	setMethod(HttpRequest::Method::Post);
}

GetThingTemplateRequest::~GetThingTemplateRequest()
{}

std::string GetThingTemplateRequest::getCategoryKey()const
{
	return categoryKey_;
}

void GetThingTemplateRequest::setCategoryKey(const std::string& categoryKey)
{
	categoryKey_ = categoryKey;
	setParameter("CategoryKey", categoryKey);
}

std::string GetThingTemplateRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void GetThingTemplateRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string GetThingTemplateRequest::getResourceGroupId()const
{
	return resourceGroupId_;
}

void GetThingTemplateRequest::setResourceGroupId(const std::string& resourceGroupId)
{
	resourceGroupId_ = resourceGroupId;
	setParameter("ResourceGroupId", resourceGroupId);
}

std::string GetThingTemplateRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void GetThingTemplateRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string GetThingTemplateRequest::getApiProduct()const
{
	return apiProduct_;
}

void GetThingTemplateRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string GetThingTemplateRequest::getApiRevision()const
{
	return apiRevision_;
}

void GetThingTemplateRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

