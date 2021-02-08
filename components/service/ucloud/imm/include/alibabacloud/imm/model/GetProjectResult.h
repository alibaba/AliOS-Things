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

#ifndef ALIBABACLOUD_IMM_MODEL_GETPROJECTRESULT_H_
#define ALIBABACLOUD_IMM_MODEL_GETPROJECTRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/imm/ImmExport.h>

namespace AlibabaCloud
{
	namespace Imm
	{
		namespace Model
		{
			class ALIBABACLOUD_IMM_EXPORT GetProjectResult : public ServiceResult
			{
			public:


				GetProjectResult();
				explicit GetProjectResult(const std::string &payload);
				~GetProjectResult();
				std::string getProject()const;
				std::string getModifyTime()const;
				std::string getType()const;
				int getCU()const;
				std::string getServiceRole()const;
				std::string getEndpoint()const;
				std::string getCreateTime()const;
				std::string getRegionId()const;
				std::string getBillingType()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string project_;
				std::string modifyTime_;
				std::string type_;
				int cU_;
				std::string serviceRole_;
				std::string endpoint_;
				std::string createTime_;
				std::string regionId_;
				std::string billingType_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_GETPROJECTRESULT_H_