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

#include <alibabacloud/iot/model/CreateProductTagsRequest.h>

using AlibabaCloud::Iot::Model::CreateProductTagsRequest;

CreateProductTagsRequest::CreateProductTagsRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateProductTags")
{
	setMethod(HttpRequest::Method::Post);
}

CreateProductTagsRequest::~CreateProductTagsRequest()
{}

std::string CreateProductTagsRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateProductTagsRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateProductTagsRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateProductTagsRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string CreateProductTagsRequest::getProductKey()const
{
	return productKey_;
}

void CreateProductTagsRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::vector<CreateProductTagsRequest::ProductTag> CreateProductTagsRequest::getProductTag()const
{
	return productTag_;
}

void CreateProductTagsRequest::setProductTag(const std::vector<ProductTag>& productTag)
{
	productTag_ = productTag;
	for(int dep1 = 0; dep1!= productTag.size(); dep1++) {
		auto productTagObj = productTag.at(dep1);
		std::string productTagObjStr = "ProductTag." + std::to_string(dep1 + 1);
		setParameter(productTagObjStr + ".TagValue", productTagObj.tagValue);
		setParameter(productTagObjStr + ".TagKey", productTagObj.tagKey);
	}
}

std::string CreateProductTagsRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateProductTagsRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CreateProductTagsRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateProductTagsRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

