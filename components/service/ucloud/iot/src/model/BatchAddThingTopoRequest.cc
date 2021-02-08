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

#include <alibabacloud/iot/model/BatchAddThingTopoRequest.h>

using AlibabaCloud::Iot::Model::BatchAddThingTopoRequest;

BatchAddThingTopoRequest::BatchAddThingTopoRequest() :
	RpcServiceRequest("iot", "2018-01-20", "BatchAddThingTopo")
{
	setMethod(HttpRequest::Method::Post);
}

BatchAddThingTopoRequest::~BatchAddThingTopoRequest()
{}

std::string BatchAddThingTopoRequest::getGwProductKey()const
{
	return gwProductKey_;
}

void BatchAddThingTopoRequest::setGwProductKey(const std::string& gwProductKey)
{
	gwProductKey_ = gwProductKey;
	setParameter("GwProductKey", gwProductKey);
}

std::string BatchAddThingTopoRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void BatchAddThingTopoRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string BatchAddThingTopoRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void BatchAddThingTopoRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string BatchAddThingTopoRequest::getExt()const
{
	return ext_;
}

void BatchAddThingTopoRequest::setExt(const std::string& ext)
{
	ext_ = ext;
	setParameter("Ext", ext);
}

std::vector<BatchAddThingTopoRequest::TopoAddItem> BatchAddThingTopoRequest::getTopoAddItem()const
{
	return topoAddItem_;
}

void BatchAddThingTopoRequest::setTopoAddItem(const std::vector<TopoAddItem>& topoAddItem)
{
	topoAddItem_ = topoAddItem;
	for(int dep1 = 0; dep1!= topoAddItem.size(); dep1++) {
		auto topoAddItemObj = topoAddItem.at(dep1);
		std::string topoAddItemObjStr = "TopoAddItem." + std::to_string(dep1 + 1);
		setParameter(topoAddItemObjStr + ".ClientId", topoAddItemObj.clientId);
		setParameter(topoAddItemObjStr + ".SignMethod", topoAddItemObj.signMethod);
		setParameter(topoAddItemObjStr + ".Sign", topoAddItemObj.sign);
		setParameter(topoAddItemObjStr + ".DeviceName", topoAddItemObj.deviceName);
		setParameter(topoAddItemObjStr + ".ProductKey", topoAddItemObj.productKey);
		setParameter(topoAddItemObjStr + ".Timestamp", topoAddItemObj.timestamp);
	}
}

std::string BatchAddThingTopoRequest::getGwDeviceName()const
{
	return gwDeviceName_;
}

void BatchAddThingTopoRequest::setGwDeviceName(const std::string& gwDeviceName)
{
	gwDeviceName_ = gwDeviceName;
	setParameter("GwDeviceName", gwDeviceName);
}

std::string BatchAddThingTopoRequest::getApiProduct()const
{
	return apiProduct_;
}

void BatchAddThingTopoRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string BatchAddThingTopoRequest::getApiRevision()const
{
	return apiRevision_;
}

void BatchAddThingTopoRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

