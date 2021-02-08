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

#ifndef ALIBABACLOUD_IMAGEENHAN_MODEL_IMAGEBLINDCHARACTERWATERMARKRESULT_H_
#define ALIBABACLOUD_IMAGEENHAN_MODEL_IMAGEBLINDCHARACTERWATERMARKRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/imageenhan/ImageenhanExport.h>

namespace AlibabaCloud
{
	namespace Imageenhan
	{
		namespace Model
		{
			class ALIBABACLOUD_IMAGEENHAN_EXPORT ImageBlindCharacterWatermarkResult : public ServiceResult
			{
			public:
				struct Data
				{
					std::string textImageURL;
					std::string watermarkImageURL;
				};


				ImageBlindCharacterWatermarkResult();
				explicit ImageBlindCharacterWatermarkResult(const std::string &payload);
				~ImageBlindCharacterWatermarkResult();
				Data getData()const;

			protected:
				void parse(const std::string &payload);
			private:
				Data data_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMAGEENHAN_MODEL_IMAGEBLINDCHARACTERWATERMARKRESULT_H_