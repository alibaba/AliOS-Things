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

#include <alibabacloud/iot/model/GetDataAPIServiceDetailRequest.h>

using AlibabaCloud::Iot::Model::GetDataAPIServiceDetailRequest;

GetDataAPIServiceDetailRequest::GetDataAPIServiceDetailRequest() :
	RpcServiceRequest("iot", "2018-01-20", "GetDataAPIServiceDetail")
{
	setMethod(HttpRequest::Method::Post);
}

GetDataAPIServiceDetailRequest::~GetDataAPIServiceDetailRequest()
{}

std::string GetDataAPIServiceDetailRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void GetDataAPIServiceDetailRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setBodyParameter("IotInstanceId", iotInstanceId);
}

std::string GetDataAPIServiceDetailRequest::getApiSrn()const
{
	return apiSrn_;
}

void GetDataAPIServiceDetailRequest::setApiSrn(const std::string& apiSrn)
{
	apiSrn_ = apiSrn;
	setBodyParameter("ApiSrn", apiSrn);
}

std::string GetDataAPIServiceDetailRequest::getApiProduct()const
{
	return apiProduct_;
}

void GetDataAPIServiceDetailRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string GetDataAPIServiceDetailRequest::getApiRevision()const
{
	return apiRevision_;
}

void GetDataAPIServiceDetailRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

