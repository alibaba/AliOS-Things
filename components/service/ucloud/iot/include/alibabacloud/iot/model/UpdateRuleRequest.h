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

#ifndef ALIBABACLOUD_IOT_MODEL_UPDATERULEREQUEST_H_
#define ALIBABACLOUD_IOT_MODEL_UPDATERULEREQUEST_H_

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
			class ALIBABACLOUD_IOT_EXPORT UpdateRuleRequest : public RpcServiceRequest
			{

			public:
				UpdateRuleRequest();
				~UpdateRuleRequest();

				std::string getSelect()const;
				void setSelect(const std::string& select);
				std::string getRuleDesc()const;
				void setRuleDesc(const std::string& ruleDesc);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getShortTopic()const;
				void setShortTopic(const std::string& shortTopic);
				std::string getIotInstanceId()const;
				void setIotInstanceId(const std::string& iotInstanceId);
				std::string getWhere()const;
				void setWhere(const std::string& where);
				int getTopicType()const;
				void setTopicType(int topicType);
				std::string getProductKey()const;
				void setProductKey(const std::string& productKey);
				std::string getApiProduct()const;
				void setApiProduct(const std::string& apiProduct);
				std::string getName()const;
				void setName(const std::string& name);
				std::string getApiRevision()const;
				void setApiRevision(const std::string& apiRevision);
				long getRuleId()const;
				void setRuleId(long ruleId);

            private:
				std::string select_;
				std::string ruleDesc_;
				std::string accessKeyId_;
				std::string shortTopic_;
				std::string iotInstanceId_;
				std::string where_;
				int topicType_;
				std::string productKey_;
				std::string apiProduct_;
				std::string name_;
				std::string apiRevision_;
				long ruleId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IOT_MODEL_UPDATERULEREQUEST_H_