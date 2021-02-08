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

#include <alibabacloud/iot/model/CopyThingModelRequest.h>

using AlibabaCloud::Iot::Model::CopyThingModelRequest;

CopyThingModelRequest::CopyThingModelRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CopyThingModel")
{
	setMethod(HttpRequest::Method::Post);
}

CopyThingModelRequest::~CopyThingModelRequest()
{}

std::string CopyThingModelRequest::getTargetProductKey()const
{
	return targetProductKey_;
}

void CopyThingModelRequest::setTargetProductKey(const std::string& targetProductKey)
{
	targetProductKey_ = targetProductKey;
	setParameter("TargetProductKey", targetProductKey);
}

std::string CopyThingModelRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CopyThingModelRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CopyThingModelRequest::getResourceGroupId()const
{
	return resourceGroupId_;
}

void CopyThingModelRequest::setResourceGroupId(const std::string& resourceGroupId)
{
	resourceGroupId_ = resourceGroupId;
	setParameter("ResourceGroupId", resourceGroupId);
}

std::string CopyThingModelRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CopyThingModelRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string CopyThingModelRequest::getSourceModelVersion()const
{
	return sourceModelVersion_;
}

void CopyThingModelRequest::setSourceModelVersion(const std::string& sourceModelVersion)
{
	sourceModelVersion_ = sourceModelVersion;
	setParameter("SourceModelVersion", sourceModelVersion);
}

std::string CopyThingModelRequest::getSourceProductKey()const
{
	return sourceProductKey_;
}

void CopyThingModelRequest::setSourceProductKey(const std::string& sourceProductKey)
{
	sourceProductKey_ = sourceProductKey;
	setParameter("SourceProductKey", sourceProductKey);
}

std::string CopyThingModelRequest::getApiProduct()const
{
	return apiProduct_;
}

void CopyThingModelRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CopyThingModelRequest::getApiRevision()const
{
	return apiRevision_;
}

void CopyThingModelRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

