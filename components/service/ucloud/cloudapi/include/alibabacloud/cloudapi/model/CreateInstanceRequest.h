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

#ifndef ALIBABACLOUD_CLOUDAPI_MODEL_CREATEINSTANCEREQUEST_H_
#define ALIBABACLOUD_CLOUDAPI_MODEL_CREATEINSTANCEREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/cloudapi/CloudAPIExport.h>

namespace AlibabaCloud
{
	namespace CloudAPI
	{
		namespace Model
		{
			class ALIBABACLOUD_CLOUDAPI_EXPORT CreateInstanceRequest : public RpcServiceRequest
			{

			public:
				CreateInstanceRequest();
				~CreateInstanceRequest();

				bool getAutoPay()const;
				void setAutoPay(bool autoPay);
				std::string getInstanceSpec()const;
				void setInstanceSpec(const std::string& instanceSpec);
				std::string getHttpsPolicy()const;
				void setHttpsPolicy(const std::string& httpsPolicy);
				std::string getToken()const;
				void setToken(const std::string& token);
				int getDuration()const;
				void setDuration(int duration);
				std::string getInstanceName()const;
				void setInstanceName(const std::string& instanceName);
				std::string getZoneId()const;
				void setZoneId(const std::string& zoneId);
				std::string getChargeType()const;
				void setChargeType(const std::string& chargeType);
				std::string getPricingCycle()const;
				void setPricingCycle(const std::string& pricingCycle);

            private:
				bool autoPay_;
				std::string instanceSpec_;
				std::string httpsPolicy_;
				std::string token_;
				int duration_;
				std::string instanceName_;
				std::string zoneId_;
				std::string chargeType_;
				std::string pricingCycle_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDAPI_MODEL_CREATEINSTANCEREQUEST_H_