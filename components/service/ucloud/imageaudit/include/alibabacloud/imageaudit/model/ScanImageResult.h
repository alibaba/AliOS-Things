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

#ifndef ALIBABACLOUD_IMAGEAUDIT_MODEL_SCANIMAGERESULT_H_
#define ALIBABACLOUD_IMAGEAUDIT_MODEL_SCANIMAGERESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/imageaudit/ImageauditExport.h>

namespace AlibabaCloud
{
	namespace Imageaudit
	{
		namespace Model
		{
			class ALIBABACLOUD_IMAGEAUDIT_EXPORT ScanImageResult : public ServiceResult
			{
			public:
				struct Data
				{
					struct Result
					{
						struct SubResult
						{
							struct Frame
							{
								float rate;
								std::string uRL;
							};
							struct HintWordsInfo
							{
								std::string context;
							};
							struct ProgramCodeData
							{
								float x;
								float y;
								float height;
								float width;
							};
							struct LogoData
							{
								std::string type;
								float x;
								float y;
								float height;
								float width;
								std::string name;
							};
							struct SfaceData
							{
								struct Face
								{
									float rate;
									std::string id;
									std::string name;
								};
								float x;
								float y;
								float height;
								float width;
								std::vector<SfaceData::Face> faces;
							};
							std::vector<SubResult::Frame> frames;
							std::string suggestion;
							std::vector<SubResult::SfaceData> sfaceDataList;
							float rate;
							std::vector<std::string> oCRDataList;
							std::vector<SubResult::HintWordsInfo> hintWordsInfoList;
							std::string label;
							std::vector<SubResult::LogoData> logoDataList;
							std::string scene;
							std::vector<SubResult::ProgramCodeData> programCodeDataList;
						};
						std::string dataId;
						std::string taskId;
						std::string imageURL;
						std::vector<Result::SubResult> subResults;
					};
					std::vector<Result> results;
				};


				ScanImageResult();
				explicit ScanImageResult(const std::string &payload);
				~ScanImageResult();
				Data getData()const;

			protected:
				void parse(const std::string &payload);
			private:
				Data data_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMAGEAUDIT_MODEL_SCANIMAGERESULT_H_