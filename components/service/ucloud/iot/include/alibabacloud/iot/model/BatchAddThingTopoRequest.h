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

#ifndef ALIBABACLOUD_IOT_MODEL_BATCHADDTHINGTOPOREQUEST_H_
#define ALIBABACLOUD_IOT_MODEL_BATCHADDTHINGTOPOREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/iot/IotExport.h>

namespace AlibabaCloud
{
	namespace Iot
	{
		namespace Model
		{
			class ALIBABACLOUD_IOT_EXPORT BatchAddThingTopoRequest : public RpcServiceRequest
			{
			public:
				struct TopoAddItem
				{
					std::string clientId;
					std::string signMethod;
					std::string sign;
					std::string deviceName;
					std::string productKey;
					std::string timestamp;
				};

			public:
				BatchAddThingTopoRequest();
				~BatchAddThingTopoRequest();

				std::string getGwProductKey()const;
				void setGwProductKey(const std::string& gwProductKey);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getIotInstanceId()const;
				void setIotInstanceId(const std::string& iotInstanceId);
				std::string getExt()const;
				void setExt(const std::string& ext);
				std::vector<TopoAddItem> getTopoAddItem()const;
				void setTopoAddItem(const std::vector<TopoAddItem>& topoAddItem);
				std::string getGwDeviceName()const;
				void setGwDeviceName(const std::string& gwDeviceName);
				std::string getApiProduct()const;
				void setApiProduct(const std::string& apiProduct);
				std::string getApiRevision()const;
				void setApiRevision(const std::string& apiRevision);

            private:
				std::string gwProductKey_;
				std::string accessKeyId_;
				std::string iotInstanceId_;
				std::string ext_;
				std::vector<TopoAddItem> topoAddItem_;
				std::string gwDeviceName_;
				std::string apiProduct_;
				std::string apiRevision_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IOT_MODEL_BATCHADDTHINGTOPOREQUEST_H_