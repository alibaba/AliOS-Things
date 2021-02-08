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

#ifndef ALIBABACLOUD_CONFIG_MODEL_PUTDELIVERYCHANNELREQUEST_H_
#define ALIBABACLOUD_CONFIG_MODEL_PUTDELIVERYCHANNELREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/config/ConfigExport.h>

namespace AlibabaCloud
{
	namespace Config
	{
		namespace Model
		{
			class ALIBABACLOUD_CONFIG_EXPORT PutDeliveryChannelRequest : public RpcServiceRequest
			{

			public:
				PutDeliveryChannelRequest();
				~PutDeliveryChannelRequest();

				std::string getClientToken()const;
				void setClientToken(const std::string& clientToken);
				std::string getDescription()const;
				void setDescription(const std::string& description);
				std::string getDeliveryChannelTargetArn()const;
				void setDeliveryChannelTargetArn(const std::string& deliveryChannelTargetArn);
				std::string getDeliveryChannelCondition()const;
				void setDeliveryChannelCondition(const std::string& deliveryChannelCondition);
				std::string getDeliveryChannelAssumeRoleArn()const;
				void setDeliveryChannelAssumeRoleArn(const std::string& deliveryChannelAssumeRoleArn);
				std::string getDeliveryChannelName()const;
				void setDeliveryChannelName(const std::string& deliveryChannelName);
				std::string getDeliveryChannelId()const;
				void setDeliveryChannelId(const std::string& deliveryChannelId);
				std::string getDeliveryChannelType()const;
				void setDeliveryChannelType(const std::string& deliveryChannelType);
				int getStatus()const;
				void setStatus(int status);

            private:
				std::string clientToken_;
				std::string description_;
				std::string deliveryChannelTargetArn_;
				std::string deliveryChannelCondition_;
				std::string deliveryChannelAssumeRoleArn_;
				std::string deliveryChannelName_;
				std::string deliveryChannelId_;
				std::string deliveryChannelType_;
				int status_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CONFIG_MODEL_PUTDELIVERYCHANNELREQUEST_H_