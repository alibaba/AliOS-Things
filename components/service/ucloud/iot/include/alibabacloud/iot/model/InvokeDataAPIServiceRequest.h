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

#ifndef ALIBABACLOUD_IOT_MODEL_INVOKEDATAAPISERVICEREQUEST_H_
#define ALIBABACLOUD_IOT_MODEL_INVOKEDATAAPISERVICEREQUEST_H_

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
			class ALIBABACLOUD_IOT_EXPORT InvokeDataAPIServiceRequest : public RpcServiceRequest
			{
			public:
				struct Param
				{
					std::string paramType;
					std::vector<std::string> listParamValue;
					std::string listParamType;
					std::string paramName;
					std::string paramValue;
				};

			public:
				InvokeDataAPIServiceRequest();
				~InvokeDataAPIServiceRequest();

				std::vector<Param> getParam()const;
				void setParam(const std::vector<Param>& param);
				std::string getIotInstanceId()const;
				void setIotInstanceId(const std::string& iotInstanceId);
				std::string getApiSrn()const;
				void setApiSrn(const std::string& apiSrn);
				std::string getApiProduct()const;
				void setApiProduct(const std::string& apiProduct);
				std::string getApiRevision()const;
				void setApiRevision(const std::string& apiRevision);

            private:
				std::vector<Param> param_;
				std::string iotInstanceId_;
				std::string apiSrn_;
				std::string apiProduct_;
				std::string apiRevision_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IOT_MODEL_INVOKEDATAAPISERVICEREQUEST_H_