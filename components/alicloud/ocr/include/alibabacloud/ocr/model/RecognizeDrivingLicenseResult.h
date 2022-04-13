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

#ifndef ALIBABACLOUD_OCR_MODEL_RECOGNIZEDRIVINGLICENSERESULT_H_
#define ALIBABACLOUD_OCR_MODEL_RECOGNIZEDRIVINGLICENSERESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/ocr/OcrExport.h>

namespace AlibabaCloud
{
	namespace Ocr
	{
		namespace Model
		{
			class ALIBABACLOUD_OCR_EXPORT RecognizeDrivingLicenseResult : public ServiceResult
			{
			public:
				struct Data
				{
					struct FaceResult
					{
						std::string plateNumber;
						std::string owner;
						std::string engineNumber;
						std::string address;
						std::string vehicleType;
						std::string model;
						std::string registerDate;
						std::string issueDate;
						std::string vin;
						std::string useCharacter;
					};
					struct BackResult
					{
						std::string approvedPassengerCapacity;
						std::string grossMass;
						std::string plateNumber;
						std::string energyType;
						std::string tractionMass;
						std::string approvedLoad;
						std::string inspectionRecord;
						std::string unladenMass;
						std::string fileNumber;
						std::string overallDimension;
					};
					FaceResult faceResult;
					BackResult backResult;
				};


				RecognizeDrivingLicenseResult();
				explicit RecognizeDrivingLicenseResult(const std::string &payload);
				~RecognizeDrivingLicenseResult();
				Data getData()const;

			protected:
				void parse(const std::string &payload);
			private:
				Data data_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_OCR_MODEL_RECOGNIZEDRIVINGLICENSERESULT_H_