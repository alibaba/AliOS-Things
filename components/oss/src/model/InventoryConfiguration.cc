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

#include <alibabacloud/oss/model/InventoryConfiguration.h>
#include "utils/Utils.h"
#include <alibabacloud/oss/Types.h>
#include <sstream>

using namespace AlibabaCloud::OSS;

InventoryFilter::InventoryFilter()
{
}

InventoryFilter::InventoryFilter(const std::string& prefix) :
    prefix_(prefix)
{
}

InventorySSEOSS::InventorySSEOSS()
{
}

InventorySSEKMS::InventorySSEKMS()
{
}

InventorySSEKMS::InventorySSEKMS(const std::string& key) :
    keyId_(key)
{
}

InventoryEncryption::InventoryEncryption() :
    inventorySSEOSIsSet_(false),
    inventorySSEKMSIsSet_(false)
{
}

InventoryEncryption::InventoryEncryption(const InventorySSEOSS& value) :
    inventorySSEOSS_(value),
    inventorySSEOSIsSet_(true),
    inventorySSEKMSIsSet_(false)
{
}

InventoryEncryption::InventoryEncryption(const InventorySSEKMS& value) :
    inventorySSEOSIsSet_(false),
    inventorySSEKMS_(value),
    inventorySSEKMSIsSet_(true)
{
}

InventoryOSSBucketDestination::InventoryOSSBucketDestination():
    format_(InventoryFormat::NotSet)
{
}

InventoryConfiguration::InventoryConfiguration():
    isEnabled_(false),
    schedule_(InventoryFrequency::NotSet),
    includedObjectVersions_(InventoryIncludedObjectVersions::NotSet)
{
}
