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

#include <alibabacloud/iot/model/UpdateProductTagsRequest.h>

using AlibabaCloud::Iot::Model::UpdateProductTagsRequest;

UpdateProductTagsRequest::UpdateProductTagsRequest() :
	RpcServiceRequest("iot", "2018-01-20", "UpdateProductTags")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateProductTagsRequest::~UpdateProductTagsRequest()
{}

std::string UpdateProductTagsRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void UpdateProductTagsRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string UpdateProductTagsRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void UpdateProductTagsRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string UpdateProductTagsRequest::getProductKey()const
{
	return productKey_;
}

void UpdateProductTagsRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::vector<UpdateProductTagsRequest::ProductTag> UpdateProductTagsRequest::getProductTag()const
{
	return productTag_;
}

void UpdateProductTagsRequest::setProductTag(const std::vector<ProductTag>& productTag)
{
	productTag_ = productTag;
	for(int dep1 = 0; dep1!= productTag.size(); dep1++) {
		auto productTagObj = productTag.at(dep1);
		std::string productTagObjStr = "ProductTag." + std::to_string(dep1 + 1);
		setParameter(productTagObjStr + ".TagValue", productTagObj.tagValue);
		setParameter(productTagObjStr + ".TagKey", productTagObj.tagKey);
	}
}

std::string UpdateProductTagsRequest::getApiProduct()const
{
	return apiProduct_;
}

void UpdateProductTagsRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string UpdateProductTagsRequest::getApiRevision()const
{
	return apiRevision_;
}

void UpdateProductTagsRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

