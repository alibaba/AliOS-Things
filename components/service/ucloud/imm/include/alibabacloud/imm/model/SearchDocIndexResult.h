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

#ifndef ALIBABACLOUD_IMM_MODEL_SEARCHDOCINDEXRESULT_H_
#define ALIBABACLOUD_IMM_MODEL_SEARCHDOCINDEXRESULT_H_

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
			class ALIBABACLOUD_IMM_EXPORT SearchDocIndexResult : public ServiceResult
			{
			public:
				struct DocInfosItem
				{
					std::string customKey4;
					std::string uniqueId;
					std::string customKey5;
					std::string customKey2;
					std::string customKey3;
					std::string contentType;
					int pageNum;
					std::string customKey1;
					int size;
					std::string customKey6;
					std::string name;
					std::string lastModified;
					std::string srcUri;
				};


				SearchDocIndexResult();
				explicit SearchDocIndexResult(const std::string &payload);
				~SearchDocIndexResult();
				std::vector<DocInfosItem> getDocInfos()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<DocInfosItem> docInfos_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_SEARCHDOCINDEXRESULT_H_