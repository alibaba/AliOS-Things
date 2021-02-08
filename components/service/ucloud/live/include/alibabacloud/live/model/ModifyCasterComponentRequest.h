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

#ifndef ALIBABACLOUD_LIVE_MODEL_MODIFYCASTERCOMPONENTREQUEST_H_
#define ALIBABACLOUD_LIVE_MODEL_MODIFYCASTERCOMPONENTREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/live/LiveExport.h>

namespace AlibabaCloud
{
	namespace Live
	{
		namespace Model
		{
			class ALIBABACLOUD_LIVE_EXPORT ModifyCasterComponentRequest : public RpcServiceRequest
			{

			public:
				ModifyCasterComponentRequest();
				~ModifyCasterComponentRequest();

				std::string getImageLayerContent()const;
				void setImageLayerContent(const std::string& imageLayerContent);
				std::string getComponentName()const;
				void setComponentName(const std::string& componentName);
				std::string getComponentId()const;
				void setComponentId(const std::string& componentId);
				std::string getCasterId()const;
				void setCasterId(const std::string& casterId);
				std::string getComponentLayer()const;
				void setComponentLayer(const std::string& componentLayer);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				std::string getComponentType()const;
				void setComponentType(const std::string& componentType);
				std::string getEffect()const;
				void setEffect(const std::string& effect);
				std::string getCaptionLayerContent()const;
				void setCaptionLayerContent(const std::string& captionLayerContent);
				std::string getTextLayerContent()const;
				void setTextLayerContent(const std::string& textLayerContent);

            private:
				std::string imageLayerContent_;
				std::string componentName_;
				std::string componentId_;
				std::string casterId_;
				std::string componentLayer_;
				long ownerId_;
				std::string componentType_;
				std::string effect_;
				std::string captionLayerContent_;
				std::string textLayerContent_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_MODIFYCASTERCOMPONENTREQUEST_H_