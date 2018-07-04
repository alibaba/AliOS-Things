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

static void _setup_hal_suite(void)
{
    ADD_SUITE(HAL_OS);
}
#ifdef HTTP2_COMM_ENABLED
static void _setup_http2_suite(void)
{
    ADD_SUITE(HTTP2);
}
#endif
int main(int argc, char *argv[])
{
    _setup_hal_suite();
#ifdef HTTP2_COMM_ENABLED
    _setup_http2_suite();
#endif	
    cut_main(argc, argv);

    return 0;
}
