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

#ifndef ALIBABACLOUD_IMM_MODEL_INDEXIMAGERESULT_H_
#define ALIBABACLOUD_IMM_MODEL_INDEXIMAGERESULT_H_

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
			class ALIBABACLOUD_IMM_EXPORT IndexImageResult : public ServiceResult
			{
			public:


				IndexImageResult();
				explicit IndexImageResult(const std::string &payload);
				~IndexImageResult();
				std::string getRemarksArrayB()const;
				std::string getModifyTime()const;
				std::string getRemarksC()const;
				std::string getRemarksD()const;
				std::string getCreateTime()const;
				std::string getExternalId()const;
				std::string getRemarksArrayA()const;
				std::string getSetId()const;
				std::string getImageUri()const;
				std::string getRemarksA()const;
				std::string getRemarksB()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string remarksArrayB_;
				std::string modifyTime_;
				std::string remarksC_;
				std::string remarksD_;
				std::string createTime_;
				std::string externalId_;
				std::string remarksArrayA_;
				std::string setId_;
				std::string imageUri_;
				std::string remarksA_;
				std::string remarksB_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_INDEXIMAGERESULT_H_