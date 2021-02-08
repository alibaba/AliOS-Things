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

#include <alibabacloud/iot/model/CreateOTAFirmwareRequest.h>

using AlibabaCloud::Iot::Model::CreateOTAFirmwareRequest;

CreateOTAFirmwareRequest::CreateOTAFirmwareRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateOTAFirmware")
{
	setMethod(HttpRequest::Method::Post);
}

CreateOTAFirmwareRequest::~CreateOTAFirmwareRequest()
{}

std::string CreateOTAFirmwareRequest::getSignMethod()const
{
	return signMethod_;
}

void CreateOTAFirmwareRequest::setSignMethod(const std::string& signMethod)
{
	signMethod_ = signMethod;
	setParameter("SignMethod", signMethod);
}

int CreateOTAFirmwareRequest::getType()const
{
	return type_;
}

void CreateOTAFirmwareRequest::setType(int type)
{
	type_ = type;
	setParameter("Type", std::to_string(type));
}

std::string CreateOTAFirmwareRequest::getFirmwareUrl()const
{
	return firmwareUrl_;
}

void CreateOTAFirmwareRequest::setFirmwareUrl(const std::string& firmwareUrl)
{
	firmwareUrl_ = firmwareUrl;
	setParameter("FirmwareUrl", firmwareUrl);
}

std::string CreateOTAFirmwareRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateOTAFirmwareRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateOTAFirmwareRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateOTAFirmwareRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string CreateOTAFirmwareRequest::getFirmwareDesc()const
{
	return firmwareDesc_;
}

void CreateOTAFirmwareRequest::setFirmwareDesc(const std::string& firmwareDesc)
{
	firmwareDesc_ = firmwareDesc;
	setParameter("FirmwareDesc", firmwareDesc);
}

std::string CreateOTAFirmwareRequest::getModuleName()const
{
	return moduleName_;
}

void CreateOTAFirmwareRequest::setModuleName(const std::string& moduleName)
{
	moduleName_ = moduleName;
	setParameter("ModuleName", moduleName);
}

std::string CreateOTAFirmwareRequest::getFirmwareSign()const
{
	return firmwareSign_;
}

void CreateOTAFirmwareRequest::setFirmwareSign(const std::string& firmwareSign)
{
	firmwareSign_ = firmwareSign;
	setParameter("FirmwareSign", firmwareSign);
}

int CreateOTAFirmwareRequest::getFirmwareSize()const
{
	return firmwareSize_;
}

void CreateOTAFirmwareRequest::setFirmwareSize(int firmwareSize)
{
	firmwareSize_ = firmwareSize;
	setParameter("FirmwareSize", std::to_string(firmwareSize));
}

std::string CreateOTAFirmwareRequest::getFirmwareName()const
{
	return firmwareName_;
}

void CreateOTAFirmwareRequest::setFirmwareName(const std::string& firmwareName)
{
	firmwareName_ = firmwareName;
	setParameter("FirmwareName", firmwareName);
}

std::string CreateOTAFirmwareRequest::getProductKey()const
{
	return productKey_;
}

void CreateOTAFirmwareRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string CreateOTAFirmwareRequest::getSrcVersion()const
{
	return srcVersion_;
}

void CreateOTAFirmwareRequest::setSrcVersion(const std::string& srcVersion)
{
	srcVersion_ = srcVersion;
	setParameter("SrcVersion", srcVersion);
}

std::string CreateOTAFirmwareRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateOTAFirmwareRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CreateOTAFirmwareRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateOTAFirmwareRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string CreateOTAFirmwareRequest::getDestVersion()const
{
	return destVersion_;
}

void CreateOTAFirmwareRequest::setDestVersion(const std::string& destVersion)
{
	destVersion_ = destVersion;
	setParameter("DestVersion", destVersion);
}

