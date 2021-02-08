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

#include <alibabacloud/iot/model/ListProductByTagsRequest.h>

using AlibabaCloud::Iot::Model::ListProductByTagsRequest;

ListProductByTagsRequest::ListProductByTagsRequest() :
	RpcServiceRequest("iot", "2018-01-20", "ListProductByTags")
{
	setMethod(HttpRequest::Method::Post);
}

ListProductByTagsRequest::~ListProductByTagsRequest()
{}

std::string ListProductByTagsRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ListProductByTagsRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ListProductByTagsRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void ListProductByTagsRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int ListProductByTagsRequest::getPageSize()const
{
	return pageSize_;
}

void ListProductByTagsRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

int ListProductByTagsRequest::getCurrentPage()const
{
	return currentPage_;
}

void ListProductByTagsRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::vector<ListProductByTagsRequest::ProductTag> ListProductByTagsRequest::getProductTag()const
{
	return productTag_;
}

void ListProductByTagsRequest::setProductTag(const std::vector<ProductTag>& productTag)
{
	productTag_ = productTag;
	for(int dep1 = 0; dep1!= productTag.size(); dep1++) {
		auto productTagObj = productTag.at(dep1);
		std::string productTagObjStr = "ProductTag." + std::to_string(dep1 + 1);
		setParameter(productTagObjStr + ".TagValue", productTagObj.tagValue);
		setParameter(productTagObjStr + ".TagKey", productTagObj.tagKey);
	}
}

std::string ListProductByTagsRequest::getApiProduct()const
{
	return apiProduct_;
}

void ListProductByTagsRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string ListProductByTagsRequest::getApiRevision()const
{
	return apiRevision_;
}

void ListProductByTagsRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

