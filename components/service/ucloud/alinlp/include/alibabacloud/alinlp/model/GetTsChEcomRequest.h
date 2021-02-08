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

#ifndef ALIBABACLOUD_ALINLP_MODEL_GETTSCHECOMREQUEST_H_
#define ALIBABACLOUD_ALINLP_MODEL_GETTSCHECOMREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/alinlp/AlinlpExport.h>

namespace AlibabaCloud
{
	namespace Alinlp
	{
		namespace Model
		{
			class ALIBABACLOUD_ALINLP_EXPORT GetTsChEcomRequest : public RpcServiceRequest
			{

			public:
				GetTsChEcomRequest();
				~GetTsChEcomRequest();

				std::string getType()const;
				void setType(const std::string& type);
				std::string getServiceCode()const;
				void setServiceCode(const std::string& serviceCode);
				std::string getOriginT()const;
				void setOriginT(const std::string& originT);
				std::string getOriginQ()const;
				void setOriginQ(const std::string& originQ);

            private:
				std::string type_;
				std::string serviceCode_;
				std::string originT_;
				std::string originQ_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_ALINLP_MODEL_GETTSCHECOMREQUEST_H_