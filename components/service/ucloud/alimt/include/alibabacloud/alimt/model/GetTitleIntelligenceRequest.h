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

#ifndef ALIBABACLOUD_ALIMT_MODEL_GETTITLEINTELLIGENCEREQUEST_H_
#define ALIBABACLOUD_ALIMT_MODEL_GETTITLEINTELLIGENCEREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/alimt/AlimtExport.h>

namespace AlibabaCloud
{
	namespace Alimt
	{
		namespace Model
		{
			class ALIBABACLOUD_ALIMT_EXPORT GetTitleIntelligenceRequest : public RpcServiceRequest
			{

			public:
				GetTitleIntelligenceRequest();
				~GetTitleIntelligenceRequest();

				long getCatLevelThreeId()const;
				void setCatLevelThreeId(long catLevelThreeId);
				long getCatLevelTwoId()const;
				void setCatLevelTwoId(long catLevelTwoId);
				std::string getKeywords()const;
				void setKeywords(const std::string& keywords);
				std::string getPlatform()const;
				void setPlatform(const std::string& platform);
				std::string getExtra()const;
				void setExtra(const std::string& extra);

            private:
				long catLevelThreeId_;
				long catLevelTwoId_;
				std::string keywords_;
				std::string platform_;
				std::string extra_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_ALIMT_MODEL_GETTITLEINTELLIGENCEREQUEST_H_