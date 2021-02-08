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

#ifndef ALIBABACLOUD_IVISION_MODEL_IMAGEPREDICTRESULT_H_
#define ALIBABACLOUD_IVISION_MODEL_IMAGEPREDICTRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/ivision/IvisionExport.h>

namespace AlibabaCloud
{
	namespace Ivision
	{
		namespace Model
		{
			class ALIBABACLOUD_IVISION_EXPORT ImagePredictResult : public ServiceResult
			{
			public:
				struct ImagePredict
				{
					std::string status;
					std::string message;
					std::string predictResult;
					std::string dataUrl;
					std::string predictTime;
					std::string predictId;
					std::string code;
					std::string modelId;
				};


				ImagePredictResult();
				explicit ImagePredictResult(const std::string &payload);
				~ImagePredictResult();
				ImagePredict getImagePredict()const;

			protected:
				void parse(const std::string &payload);
			private:
				ImagePredict imagePredict_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IVISION_MODEL_IMAGEPREDICTRESULT_H_