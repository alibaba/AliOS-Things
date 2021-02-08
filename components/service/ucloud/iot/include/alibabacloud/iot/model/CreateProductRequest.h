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

#ifndef ALIBABACLOUD_IOT_MODEL_CREATEPRODUCTREQUEST_H_
#define ALIBABACLOUD_IOT_MODEL_CREATEPRODUCTREQUEST_H_

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
			class ALIBABACLOUD_IOT_EXPORT CreateProductRequest : public RpcServiceRequest
			{

			public:
				CreateProductRequest();
				~CreateProductRequest();

				int getNodeType()const;
				void setNodeType(int nodeType);
				std::string getDescription()const;
				void setDescription(const std::string& description);
				std::string getCategoryKey()const;
				void setCategoryKey(const std::string& categoryKey);
				std::string getJoinPermissionId()const;
				void setJoinPermissionId(const std::string& joinPermissionId);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getAuthType()const;
				void setAuthType(const std::string& authType);
				std::string getResourceGroupId()const;
				void setResourceGroupId(const std::string& resourceGroupId);
				std::string getIotInstanceId()const;
				void setIotInstanceId(const std::string& iotInstanceId);
				std::string getProductName()const;
				void setProductName(const std::string& productName);
				std::string getAliyunCommodityCode()const;
				void setAliyunCommodityCode(const std::string& aliyunCommodityCode);
				bool getPublishAuto()const;
				void setPublishAuto(bool publishAuto);
				long getCategoryId()const;
				void setCategoryId(long categoryId);
				int getDataFormat()const;
				void setDataFormat(int dataFormat);
				bool getId2()const;
				void setId2(bool id2);
				std::string getNetType()const;
				void setNetType(const std::string& netType);
				std::string getApiProduct()const;
				void setApiProduct(const std::string& apiProduct);
				std::string getApiRevision()const;
				void setApiRevision(const std::string& apiRevision);
				std::string getProtocolType()const;
				void setProtocolType(const std::string& protocolType);

            private:
				int nodeType_;
				std::string description_;
				std::string categoryKey_;
				std::string joinPermissionId_;
				std::string accessKeyId_;
				std::string authType_;
				std::string resourceGroupId_;
				std::string iotInstanceId_;
				std::string productName_;
				std::string aliyunCommodityCode_;
				bool publishAuto_;
				long categoryId_;
				int dataFormat_;
				bool id2_;
				std::string netType_;
				std::string apiProduct_;
				std::string apiRevision_;
				std::string protocolType_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IOT_MODEL_CREATEPRODUCTREQUEST_H_