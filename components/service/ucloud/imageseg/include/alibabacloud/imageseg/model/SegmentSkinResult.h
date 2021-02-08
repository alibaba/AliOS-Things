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

#ifndef ALIBABACLOUD_IMAGESEG_MODEL_SEGMENTSKINRESULT_H_
#define ALIBABACLOUD_IMAGESEG_MODEL_SEGMENTSKINRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/imageseg/ImagesegExport.h>

namespace AlibabaCloud
{
	namespace Imageseg
	{
		namespace Model
		{
			class ALIBABACLOUD_IMAGESEG_EXPORT SegmentSkinResult : public ServiceResult
			{
			public:
				struct Data
				{
					std::string uRL;
				};


				SegmentSkinResult();
				explicit SegmentSkinResult(const std::string &payload);
				~SegmentSkinResult();
				Data getData()const;

			protected:
				void parse(const std::string &payload);
			private:
				Data data_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMAGESEG_MODEL_SEGMENTSKINRESULT_H_