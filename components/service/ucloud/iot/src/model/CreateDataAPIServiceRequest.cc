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

#include <alibabacloud/iot/model/CreateDataAPIServiceRequest.h>

using AlibabaCloud::Iot::Model::CreateDataAPIServiceRequest;

CreateDataAPIServiceRequest::CreateDataAPIServiceRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateDataAPIService")
{
	setMethod(HttpRequest::Method::Post);
}

CreateDataAPIServiceRequest::~CreateDataAPIServiceRequest()
{}

std::vector<CreateDataAPIServiceRequest::RequestParam> CreateDataAPIServiceRequest::getRequestParam()const
{
	return requestParam_;
}

void CreateDataAPIServiceRequest::setRequestParam(const std::vector<RequestParam>& requestParam)
{
	requestParam_ = requestParam;
	for(int dep1 = 0; dep1!= requestParam.size(); dep1++) {
		auto requestParamObj = requestParam.at(dep1);
		std::string requestParamObjStr = "RequestParam." + std::to_string(dep1 + 1);
		setParameter(requestParamObjStr + ".Name", requestParamObj.name);
		setParameter(requestParamObjStr + ".Type", requestParamObj.type);
		setParameter(requestParamObjStr + ".Desc", requestParamObj.desc);
		setParameter(requestParamObjStr + ".Example", requestParamObj.example);
		setParameter(requestParamObjStr + ".Required", requestParamObj.required ? "true" : "false");
	}
}

std::string CreateDataAPIServiceRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateDataAPIServiceRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setBodyParameter("IotInstanceId", iotInstanceId);
}

std::string CreateDataAPIServiceRequest::getApiPath()const
{
	return apiPath_;
}

void CreateDataAPIServiceRequest::setApiPath(const std::string& apiPath)
{
	apiPath_ = apiPath;
	setBodyParameter("ApiPath", apiPath);
}

std::string CreateDataAPIServiceRequest::getTemplateSql()const
{
	return templateSql_;
}

void CreateDataAPIServiceRequest::setTemplateSql(const std::string& templateSql)
{
	templateSql_ = templateSql;
	setBodyParameter("TemplateSql", templateSql);
}

std::vector<CreateDataAPIServiceRequest::ResponseParam> CreateDataAPIServiceRequest::getResponseParam()const
{
	return responseParam_;
}

void CreateDataAPIServiceRequest::setResponseParam(const std::vector<ResponseParam>& responseParam)
{
	responseParam_ = responseParam;
	for(int dep1 = 0; dep1!= responseParam.size(); dep1++) {
		auto responseParamObj = responseParam.at(dep1);
		std::string responseParamObjStr = "ResponseParam." + std::to_string(dep1 + 1);
		setParameter(responseParamObjStr + ".Name", responseParamObj.name);
		setParameter(responseParamObjStr + ".Type", responseParamObj.type);
		setParameter(responseParamObjStr + ".Desc", responseParamObj.desc);
		setParameter(responseParamObjStr + ".Example", responseParamObj.example);
		setParameter(responseParamObjStr + ".Required", responseParamObj.required ? "true" : "false");
	}
}

std::string CreateDataAPIServiceRequest::getOriginSql()const
{
	return originSql_;
}

void CreateDataAPIServiceRequest::setOriginSql(const std::string& originSql)
{
	originSql_ = originSql;
	setBodyParameter("OriginSql", originSql);
}

std::string CreateDataAPIServiceRequest::getDisplayName()const
{
	return displayName_;
}

void CreateDataAPIServiceRequest::setDisplayName(const std::string& displayName)
{
	displayName_ = displayName;
	setBodyParameter("DisplayName", displayName);
}

std::string CreateDataAPIServiceRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateDataAPIServiceRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CreateDataAPIServiceRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateDataAPIServiceRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string CreateDataAPIServiceRequest::getDesc()const
{
	return desc_;
}

void CreateDataAPIServiceRequest::setDesc(const std::string& desc)
{
	desc_ = desc;
	setBodyParameter("Desc", desc);
}

