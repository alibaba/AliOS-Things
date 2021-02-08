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

#include <alibabacloud/iot/model/CreateEdgeDriverRequest.h>

using AlibabaCloud::Iot::Model::CreateEdgeDriverRequest;

CreateEdgeDriverRequest::CreateEdgeDriverRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateEdgeDriver")
{
	setMethod(HttpRequest::Method::Post);
}

CreateEdgeDriverRequest::~CreateEdgeDriverRequest()
{}

std::string CreateEdgeDriverRequest::getDriverProtocol()const
{
	return driverProtocol_;
}

void CreateEdgeDriverRequest::setDriverProtocol(const std::string& driverProtocol)
{
	driverProtocol_ = driverProtocol;
	setParameter("DriverProtocol", driverProtocol);
}

std::string CreateEdgeDriverRequest::getDriverName()const
{
	return driverName_;
}

void CreateEdgeDriverRequest::setDriverName(const std::string& driverName)
{
	driverName_ = driverName;
	setParameter("DriverName", driverName);
}

bool CreateEdgeDriverRequest::getIsBuiltIn()const
{
	return isBuiltIn_;
}

void CreateEdgeDriverRequest::setIsBuiltIn(bool isBuiltIn)
{
	isBuiltIn_ = isBuiltIn;
	setParameter("IsBuiltIn", isBuiltIn ? "true" : "false");
}

std::string CreateEdgeDriverRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateEdgeDriverRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateEdgeDriverRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateEdgeDriverRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string CreateEdgeDriverRequest::getRuntime()const
{
	return runtime_;
}

void CreateEdgeDriverRequest::setRuntime(const std::string& runtime)
{
	runtime_ = runtime;
	setParameter("Runtime", runtime);
}

std::string CreateEdgeDriverRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateEdgeDriverRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CreateEdgeDriverRequest::getCpuArch()const
{
	return cpuArch_;
}

void CreateEdgeDriverRequest::setCpuArch(const std::string& cpuArch)
{
	cpuArch_ = cpuArch;
	setParameter("CpuArch", cpuArch);
}

std::string CreateEdgeDriverRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateEdgeDriverRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

