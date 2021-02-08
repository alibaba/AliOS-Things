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

#ifndef ALIBABACLOUD_IMM_MODEL_CREATEDOCINDEXTASKREQUEST_H_
#define ALIBABACLOUD_IMM_MODEL_CREATEDOCINDEXTASKREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/imm/ImmExport.h>

namespace AlibabaCloud
{
	namespace Imm
	{
		namespace Model
		{
			class ALIBABACLOUD_IMM_EXPORT CreateDocIndexTaskRequest : public RpcServiceRequest
			{

			public:
				CreateDocIndexTaskRequest();
				~CreateDocIndexTaskRequest();

				std::string getProject()const;
				void setProject(const std::string& project);
				std::string getUniqueId()const;
				void setUniqueId(const std::string& uniqueId);
				std::string getCustomKey1()const;
				void setCustomKey1(const std::string& customKey1);
				std::string getSet()const;
				void setSet(const std::string& set);
				std::string getCustomKey5()const;
				void setCustomKey5(const std::string& customKey5);
				std::string getCustomKey4()const;
				void setCustomKey4(const std::string& customKey4);
				std::string getCustomKey3()const;
				void setCustomKey3(const std::string& customKey3);
				std::string getCustomKey2()const;
				void setCustomKey2(const std::string& customKey2);
				std::string getCustomKey6()const;
				void setCustomKey6(const std::string& customKey6);
				std::string getContentType()const;
				void setContentType(const std::string& contentType);
				std::string getName()const;
				void setName(const std::string& name);
				std::string getSrcUri()const;
				void setSrcUri(const std::string& srcUri);

            private:
				std::string project_;
				std::string uniqueId_;
				std::string customKey1_;
				std::string set_;
				std::string customKey5_;
				std::string customKey4_;
				std::string customKey3_;
				std::string customKey2_;
				std::string customKey6_;
				std::string contentType_;
				std::string name_;
				std::string srcUri_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_CREATEDOCINDEXTASKREQUEST_H_