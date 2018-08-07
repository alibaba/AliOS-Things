/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#include "sdk-testsuites_internal.h"
#include "cut.h"

int main(int argc, char *argv[])
{
    ADD_SUITE(HAL_OS);
#ifdef SDK_ENHANCE
    ADD_SUITE(DM_TEST);
#endif
#ifdef HTTP2_COMM_ENABLED
    ADD_SUITE(HTTP2);
#endif	
    cut_main(argc, argv);

    return 0;
}
