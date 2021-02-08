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

#ifndef ALIBABACLOUD_IMM_MODEL_INDEXVIDEORESULT_H_
#define ALIBABACLOUD_IMM_MODEL_INDEXVIDEORESULT_H_

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
			class ALIBABACLOUD_IMM_EXPORT IndexVideoResult : public ServiceResult
			{
			public:


				IndexVideoResult();
				explicit IndexVideoResult(const std::string &payload);
				~IndexVideoResult();
				std::string getModifyTime()const;
				std::string getEndTime()const;
				bool getSaveType()const;
				std::string getCreateTime()const;
				std::string getExternalId()const;
				std::string getStartTime()const;
				std::string getVideoUri()const;
				std::string getGrabType()const;
				std::string getRemarksA()const;
				std::string getRemarksB()const;
				std::string getRemarksC()const;
				std::string getTgtUri()const;
				std::string getRemarksD()const;
				std::string getSetId()const;
				float getInterval()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string modifyTime_;
				std::string endTime_;
				bool saveType_;
				std::string createTime_;
				std::string externalId_;
				std::string startTime_;
				std::string videoUri_;
				std::string grabType_;
				std::string remarksA_;
				std::string remarksB_;
				std::string remarksC_;
				std::string tgtUri_;
				std::string remarksD_;
				std::string setId_;
				float interval_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_INDEXVIDEORESULT_H_