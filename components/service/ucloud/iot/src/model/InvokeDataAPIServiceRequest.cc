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

#include <alibabacloud/iot/model/InvokeDataAPIServiceRequest.h>

using AlibabaCloud::Iot::Model::InvokeDataAPIServiceRequest;

InvokeDataAPIServiceRequest::InvokeDataAPIServiceRequest() :
	RpcServiceRequest("iot", "2018-01-20", "InvokeDataAPIService")
{
	setMethod(HttpRequest::Method::Post);
}

InvokeDataAPIServiceRequest::~InvokeDataAPIServiceRequest()
{}

std::vector<InvokeDataAPIServiceRequest::Param> InvokeDataAPIServiceRequest::getParam()const
{
	return param_;
}

void InvokeDataAPIServiceRequest::setParam(const std::vector<Param>& param)
{
	param_ = param;
	for(int dep1 = 0; dep1!= param.size(); dep1++) {
		auto paramObj = param.at(dep1);
		std::string paramObjStr = "Param." + std::to_string(dep1 + 1);
		setParameter(paramObjStr + ".ParamType", paramObj.paramType);
		for(int dep2 = 0; dep2!= paramObj.listParamValue.size(); dep2++) {
			setParameter(paramObjStr + ".ListParamValue."+ std::to_string(dep2), paramObj.listParamValue.at(dep2));
		}
		setParameter(paramObjStr + ".ListParamType", paramObj.listParamType);
		setParameter(paramObjStr + ".ParamName", paramObj.paramName);
		setParameter(paramObjStr + ".ParamValue", paramObj.paramValue);
	}
}

std::string InvokeDataAPIServiceRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void InvokeDataAPIServiceRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setBodyParameter("IotInstanceId", iotInstanceId);
}

std::string InvokeDataAPIServiceRequest::getApiSrn()const
{
	return apiSrn_;
}

void InvokeDataAPIServiceRequest::setApiSrn(const std::string& apiSrn)
{
	apiSrn_ = apiSrn;
	setBodyParameter("ApiSrn", apiSrn);
}

std::string InvokeDataAPIServiceRequest::getApiProduct()const
{
	return apiProduct_;
}

void InvokeDataAPIServiceRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string InvokeDataAPIServiceRequest::getApiRevision()const
{
	return apiRevision_;
}

void InvokeDataAPIServiceRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

