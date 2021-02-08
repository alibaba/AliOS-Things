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

#include <alibabacloud/iot/model/QueryAppDeviceListRequest.h>

using AlibabaCloud::Iot::Model::QueryAppDeviceListRequest;

QueryAppDeviceListRequest::QueryAppDeviceListRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryAppDeviceList")
{
	setMethod(HttpRequest::Method::Post);
}

QueryAppDeviceListRequest::~QueryAppDeviceListRequest()
{}

int QueryAppDeviceListRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryAppDeviceListRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::vector<QueryAppDeviceListRequest::TagList> QueryAppDeviceListRequest::getTagList()const
{
	return tagList_;
}

void QueryAppDeviceListRequest::setTagList(const std::vector<TagList>& tagList)
{
	tagList_ = tagList;
	for(int dep1 = 0; dep1!= tagList.size(); dep1++) {
		auto tagListObj = tagList.at(dep1);
		std::string tagListObjStr = "TagList." + std::to_string(dep1 + 1);
		setParameter(tagListObjStr + ".TagName", tagListObj.tagName);
		setParameter(tagListObjStr + ".TagValue", tagListObj.tagValue);
	}
}

std::vector<std::string> QueryAppDeviceListRequest::getProductKeyList()const
{
	return productKeyList_;
}

void QueryAppDeviceListRequest::setProductKeyList(const std::vector<std::string>& productKeyList)
{
	productKeyList_ = productKeyList;
	for(int dep1 = 0; dep1!= productKeyList.size(); dep1++) {
		setParameter("ProductKeyList."+ std::to_string(dep1), productKeyList.at(dep1));
	}
}

std::vector<std::string> QueryAppDeviceListRequest::getCategoryKeyList()const
{
	return categoryKeyList_;
}

void QueryAppDeviceListRequest::setCategoryKeyList(const std::vector<std::string>& categoryKeyList)
{
	categoryKeyList_ = categoryKeyList;
	for(int dep1 = 0; dep1!= categoryKeyList.size(); dep1++) {
		setParameter("CategoryKeyList."+ std::to_string(dep1), categoryKeyList.at(dep1));
	}
}

std::string QueryAppDeviceListRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryAppDeviceListRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QueryAppDeviceListRequest::getPageSize()const
{
	return pageSize_;
}

void QueryAppDeviceListRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string QueryAppDeviceListRequest::getAppKey()const
{
	return appKey_;
}

void QueryAppDeviceListRequest::setAppKey(const std::string& appKey)
{
	appKey_ = appKey;
	setParameter("AppKey", appKey);
}

