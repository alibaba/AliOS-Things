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

#ifndef ALIBABACLOUD_NLP_AUTOML_MODEL_RUNCONTACTREVIEWRESULT_H_
#define ALIBABACLOUD_NLP_AUTOML_MODEL_RUNCONTACTREVIEWRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/nlp-automl/Nlp_automlExport.h>

namespace AlibabaCloud
{
	namespace Nlp_automl
	{
		namespace Model
		{
			class ALIBABACLOUD_NLP_AUTOML_EXPORT RunContactReviewResult : public ServiceResult
			{
			public:
				struct ContactContent
				{
					struct ReviewResult
					{
						std::string type;
						std::vector<std::string> value;
						std::string modificationSuggestion;
						std::string riskLevel;
						std::string reason;
						std::vector<std::string> startPosition;
						std::vector<std::string> endPosition;
					};
					struct StructureResult
					{
						std::vector<std::string> startPosition3;
						std::string type;
						std::vector<std::string> endPosition2;
						std::vector<std::string> value1;
						std::string name;
					};
					std::vector<StructureResult> structureResults;
					std::vector<ReviewResult> reviewResults;
				};


				RunContactReviewResult();
				explicit RunContactReviewResult(const std::string &payload);
				~RunContactReviewResult();
				std::string getRawContractContent()const;
				ContactContent getContactContent()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string rawContractContent_;
				ContactContent contactContent_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_NLP_AUTOML_MODEL_RUNCONTACTREVIEWRESULT_H_