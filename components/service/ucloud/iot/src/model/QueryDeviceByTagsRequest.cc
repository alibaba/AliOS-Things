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

#include <alibabacloud/iot/model/QueryDeviceByTagsRequest.h>

using AlibabaCloud::Iot::Model::QueryDeviceByTagsRequest;

QueryDeviceByTagsRequest::QueryDeviceByTagsRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryDeviceByTags")
{
	setMethod(HttpRequest::Method::Post);
}

QueryDeviceByTagsRequest::~QueryDeviceByTagsRequest()
{}

std::string QueryDeviceByTagsRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryDeviceByTagsRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryDeviceByTagsRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryDeviceByTagsRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QueryDeviceByTagsRequest::getPageSize()const
{
	return pageSize_;
}

void QueryDeviceByTagsRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::vector<QueryDeviceByTagsRequest::Tag> QueryDeviceByTagsRequest::getTag()const
{
	return tag_;
}

void QueryDeviceByTagsRequest::setTag(const std::vector<Tag>& tag)
{
	tag_ = tag;
	for(int dep1 = 0; dep1!= tag.size(); dep1++) {
		auto tagObj = tag.at(dep1);
		std::string tagObjStr = "Tag." + std::to_string(dep1 + 1);
		setParameter(tagObjStr + ".TagValue", tagObj.tagValue);
		setParameter(tagObjStr + ".TagKey", tagObj.tagKey);
	}
}

int QueryDeviceByTagsRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryDeviceByTagsRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string QueryDeviceByTagsRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryDeviceByTagsRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryDeviceByTagsRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryDeviceByTagsRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

