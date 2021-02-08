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

#include <alibabacloud/iot/model/CreateProductRequest.h>

using AlibabaCloud::Iot::Model::CreateProductRequest;

CreateProductRequest::CreateProductRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateProduct")
{
	setMethod(HttpRequest::Method::Post);
}

CreateProductRequest::~CreateProductRequest()
{}

int CreateProductRequest::getNodeType()const
{
	return nodeType_;
}

void CreateProductRequest::setNodeType(int nodeType)
{
	nodeType_ = nodeType;
	setParameter("NodeType", std::to_string(nodeType));
}

std::string CreateProductRequest::getDescription()const
{
	return description_;
}

void CreateProductRequest::setDescription(const std::string& description)
{
	description_ = description;
	setParameter("Description", description);
}

std::string CreateProductRequest::getCategoryKey()const
{
	return categoryKey_;
}

void CreateProductRequest::setCategoryKey(const std::string& categoryKey)
{
	categoryKey_ = categoryKey;
	setParameter("CategoryKey", categoryKey);
}

std::string CreateProductRequest::getJoinPermissionId()const
{
	return joinPermissionId_;
}

void CreateProductRequest::setJoinPermissionId(const std::string& joinPermissionId)
{
	joinPermissionId_ = joinPermissionId;
	setParameter("JoinPermissionId", joinPermissionId);
}

std::string CreateProductRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateProductRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateProductRequest::getAuthType()const
{
	return authType_;
}

void CreateProductRequest::setAuthType(const std::string& authType)
{
	authType_ = authType;
	setParameter("AuthType", authType);
}

std::string CreateProductRequest::getResourceGroupId()const
{
	return resourceGroupId_;
}

void CreateProductRequest::setResourceGroupId(const std::string& resourceGroupId)
{
	resourceGroupId_ = resourceGroupId;
	setParameter("ResourceGroupId", resourceGroupId);
}

std::string CreateProductRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateProductRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string CreateProductRequest::getProductName()const
{
	return productName_;
}

void CreateProductRequest::setProductName(const std::string& productName)
{
	productName_ = productName;
	setParameter("ProductName", productName);
}

std::string CreateProductRequest::getAliyunCommodityCode()const
{
	return aliyunCommodityCode_;
}

void CreateProductRequest::setAliyunCommodityCode(const std::string& aliyunCommodityCode)
{
	aliyunCommodityCode_ = aliyunCommodityCode;
	setParameter("AliyunCommodityCode", aliyunCommodityCode);
}

bool CreateProductRequest::getPublishAuto()const
{
	return publishAuto_;
}

void CreateProductRequest::setPublishAuto(bool publishAuto)
{
	publishAuto_ = publishAuto;
	setParameter("PublishAuto", publishAuto ? "true" : "false");
}

long CreateProductRequest::getCategoryId()const
{
	return categoryId_;
}

void CreateProductRequest::setCategoryId(long categoryId)
{
	categoryId_ = categoryId;
	setParameter("CategoryId", std::to_string(categoryId));
}

int CreateProductRequest::getDataFormat()const
{
	return dataFormat_;
}

void CreateProductRequest::setDataFormat(int dataFormat)
{
	dataFormat_ = dataFormat;
	setParameter("DataFormat", std::to_string(dataFormat));
}

bool CreateProductRequest::getId2()const
{
	return id2_;
}

void CreateProductRequest::setId2(bool id2)
{
	id2_ = id2;
	setParameter("Id2", id2 ? "true" : "false");
}

std::string CreateProductRequest::getNetType()const
{
	return netType_;
}

void CreateProductRequest::setNetType(const std::string& netType)
{
	netType_ = netType;
	setParameter("NetType", netType);
}

std::string CreateProductRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateProductRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CreateProductRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateProductRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string CreateProductRequest::getProtocolType()const
{
	return protocolType_;
}

void CreateProductRequest::setProtocolType(const std::string& protocolType)
{
	protocolType_ = protocolType;
	setParameter("ProtocolType", protocolType);
}

