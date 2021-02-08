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

#include <alibabacloud/iot/model/DeleteProductTagsRequest.h>

using AlibabaCloud::Iot::Model::DeleteProductTagsRequest;

DeleteProductTagsRequest::DeleteProductTagsRequest() :
	RpcServiceRequest("iot", "2018-01-20", "DeleteProductTags")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteProductTagsRequest::~DeleteProductTagsRequest()
{}

std::string DeleteProductTagsRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DeleteProductTagsRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DeleteProductTagsRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void DeleteProductTagsRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::vector<std::string> DeleteProductTagsRequest::getProductTagKey()const
{
	return productTagKey_;
}

void DeleteProductTagsRequest::setProductTagKey(const std::vector<std::string>& productTagKey)
{
	productTagKey_ = productTagKey;
	for(int dep1 = 0; dep1!= productTagKey.size(); dep1++) {
		setParameter("ProductTagKey."+ std::to_string(dep1), productTagKey.at(dep1));
	}
}

std::string DeleteProductTagsRequest::getProductKey()const
{
	return productKey_;
}

void DeleteProductTagsRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string DeleteProductTagsRequest::getApiProduct()const
{
	return apiProduct_;
}

void DeleteProductTagsRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string DeleteProductTagsRequest::getApiRevision()const
{
	return apiRevision_;
}

void DeleteProductTagsRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

