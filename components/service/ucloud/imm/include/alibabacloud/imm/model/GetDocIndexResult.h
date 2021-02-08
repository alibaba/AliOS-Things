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

#ifndef ALIBABACLOUD_IMM_MODEL_GETDOCINDEXRESULT_H_
#define ALIBABACLOUD_IMM_MODEL_GETDOCINDEXRESULT_H_

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
			class ALIBABACLOUD_IMM_EXPORT GetDocIndexResult : public ServiceResult
			{
			public:


				GetDocIndexResult();
				explicit GetDocIndexResult(const std::string &payload);
				~GetDocIndexResult();
				std::string getCustomKey4()const;
				std::string getUniqueId()const;
				std::string getCustomKey5()const;
				std::string getCustomKey2()const;
				std::string getCustomKey3()const;
				std::string getContentType()const;
				long getPageNum()const;
				std::string getCustomKey1()const;
				long getSize()const;
				std::string getIndexModifiedTime()const;
				std::string getIndexCreatedTime()const;
				std::string getCustomKey6()const;
				std::string getName()const;
				std::string getLastModified()const;
				std::string getSrcUri()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string customKey4_;
				std::string uniqueId_;
				std::string customKey5_;
				std::string customKey2_;
				std::string customKey3_;
				std::string contentType_;
				long pageNum_;
				std::string customKey1_;
				long size_;
				std::string indexModifiedTime_;
				std::string indexCreatedTime_;
				std::string customKey6_;
				std::string name_;
				std::string lastModified_;
				std::string srcUri_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_GETDOCINDEXRESULT_H_